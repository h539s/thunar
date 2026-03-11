
#include "thunar/thunar-test-utils.h"
#include <gdk/gdk.h>
#include <gdk/gdktestutils.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>

#define SNAPSHOTS_DIR "test/snapshots"
#define ERROR_PREFIX "err_"

static gboolean
compare_pixbufs (GdkPixbuf *pb1, GdkPixbuf *pb2)
{
  gint width, height, rowstride1, rowstride2, n_channels;
  const guchar *pixels1, *pixels2;

  if (gdk_pixbuf_get_width (pb1) != gdk_pixbuf_get_width (pb2) ||
      gdk_pixbuf_get_height (pb1) != gdk_pixbuf_get_height (pb2) ||
      gdk_pixbuf_get_n_channels (pb1) != gdk_pixbuf_get_n_channels (pb2) ||
      gdk_pixbuf_get_has_alpha (pb1) != gdk_pixbuf_get_has_alpha (pb2))
    return FALSE;

  width = gdk_pixbuf_get_width (pb1);
  height = gdk_pixbuf_get_height (pb1);
  rowstride1 = gdk_pixbuf_get_rowstride (pb1);
  rowstride2 = gdk_pixbuf_get_rowstride (pb2);
  n_channels = gdk_pixbuf_get_n_channels (pb1);
  pixels1 = gdk_pixbuf_read_pixels (pb1);
  pixels2 = gdk_pixbuf_read_pixels (pb2);

  for (gint y = 0; y < height; y++)
    {
      const guchar *p1 = pixels1 + y * rowstride1;
      const guchar *p2 = pixels2 + y * rowstride2;
      if (memcmp (p1, p2, width * n_channels) != 0)
        return FALSE;
    }

  return TRUE;
}

static void
take_screenshot_and_compare (GtkWindow *window, const gchar *test_case)
{
  GdkWindow *gdk_window;
  GdkPixbuf *screenshot;
  GError    *error = NULL;
  gchar     *reference_path;
  gchar     *error_path;

  g_print ("Thunar Test [%s]: Taking screenshot...\n", test_case);

  gdk_window = gdk_get_default_root_window ();
  if (gdk_window == NULL)
    {
      g_warning ("Thunar Test [%s]: Failed to get Root Window", test_case);
      return;
    }

  screenshot = gdk_pixbuf_get_from_window (gdk_window, 0, 0,
                                           gdk_screen_get_width (gdk_screen_get_default ()),
                                           gdk_screen_get_height (gdk_screen_get_default ()));

  if (screenshot == NULL)
    {
      g_warning ("Thunar Test [%s]: Failed to capture screenshot", test_case);
      return;
    }

  if (!g_file_test (SNAPSHOTS_DIR, G_FILE_TEST_IS_DIR))
    g_mkdir_with_parents (SNAPSHOTS_DIR, 0755);

  reference_path = g_strdup_printf ("%s/%s.png", SNAPSHOTS_DIR, test_case);

  if (!g_file_test (reference_path, G_FILE_TEST_EXISTS))
    {
      g_print ("Thunar Test [%s]: Reference image not found. Creating reference...\n", test_case);
      if (!gdk_pixbuf_save (screenshot, reference_path, "png", &error, NULL))
        {
          g_warning ("Thunar Test [%s]: Failed to save reference image: %s", test_case, error->message);
          g_clear_error (&error);
        }
    }
  else
    {
      GdkPixbuf *reference = gdk_pixbuf_new_from_file (reference_path, &error);
      if (reference == NULL)
        {
          g_warning ("Thunar Test [%s]: Failed to load reference image: %s", test_case, error->message);
          g_clear_error (&error);
        }
      else
        {
          if (compare_pixbufs (screenshot, reference))
            {
              g_print ("Thunar Test [%s]: Screenshot matches reference. Success!\n", test_case);
            }
          else
            {
              error_path = g_strdup_printf ("%s/%s%s.png", SNAPSHOTS_DIR, ERROR_PREFIX, test_case);
              g_print ("Thunar Test [%s]: Screenshot DOES NOT match reference. Saving as %s\n", test_case, error_path);
              if (!gdk_pixbuf_save (screenshot, error_path, "png", &error, NULL))
                {
                  g_warning ("Thunar Test [%s]: Failed to save error image: %s", test_case, error->message);
                  g_clear_error (&error);
                }
              g_free (error_path);
              exit (1);
            }
          g_object_unref (reference);
        }
    }

  g_free (reference_path);
  g_object_unref (screenshot);

  g_print ("Thunar Test [%s]: Completed. Closing Thunar...\n", test_case);
  g_application_quit (g_application_get_default ());
}

static gboolean
thunar_test_file_menu_step2 (gpointer data)
{
  take_screenshot_and_compare (GTK_WINDOW (data), "file_menu");
  return FALSE;
}

static void
find_file_menu_and_click (GtkWidget *widget, gpointer data)
{
  GtkWidget **file_menu_item = (GtkWidget **) data;
  if (*file_menu_item != NULL)
    return;

  if (GTK_IS_MENU_ITEM (widget))
    {
      const gchar *label = gtk_menu_item_get_label (GTK_MENU_ITEM (widget));
         We can also check the action name if available. */
      if (label && (strstr (label, "File") || strstr (label, "Arquivo")))
        {
          *file_menu_item = widget;
          return;
        }
    }

  if (GTK_IS_CONTAINER (widget))
    gtk_container_foreach (GTK_CONTAINER (widget), find_file_menu_and_click, data);
}

gboolean
thunar_test_run (gpointer data)
{
  GtkWindow   *window = GTK_WINDOW (data);
  const gchar *test_case = g_getenv ("THUNAR_TEST_CASE");

  if (test_case == NULL)
    test_case = "open";

  if (!gtk_widget_get_visible (GTK_WIDGET (window)))
    return TRUE;

  if (gtk_widget_get_window (GTK_WIDGET (window)) == NULL)
    return TRUE;

  if (g_strcmp0 (test_case, "open") == 0)
    {
      take_screenshot_and_compare (window, "open");
      return FALSE;
    }
  else if (g_strcmp0 (test_case, "file_menu") == 0)
    {
      GtkWidget *file_menu_item = NULL;
      gtk_container_foreach (GTK_CONTAINER (window), find_file_menu_and_click, &file_menu_item);

      if (file_menu_item)
        {
          GtkAllocation alloc;
          gint x, y;
          gtk_widget_get_allocation (file_menu_item, &alloc);
          gdk_window_get_origin (gtk_widget_get_window (file_menu_item), &x, &y);

          g_print ("Thunar Test [file_menu]: Clicking File menu at %d, %d\n", x + alloc.x + alloc.width / 2, y + alloc.y + alloc.height / 2);
          gdk_test_simulate_button (gdk_get_default_root_window (),
                                    x + alloc.x + alloc.width / 2,
                                    y + alloc.y + alloc.height / 2,
                                    1,
                                    GDK_BUTTON_PRESS_MASK,
                                    GDK_BUTTON_PRESS);
          gdk_test_simulate_button (gdk_get_default_root_window (),
                                    x + alloc.x + alloc.width / 2,
                                    y + alloc.y + alloc.height / 2,
                                    1,
                                    GDK_BUTTON_RELEASE_MASK,
                                    GDK_BUTTON_RELEASE);

          /* Wait a bit for the menu to open and render */
          g_timeout_add (500, thunar_test_file_menu_step2, window);
        }
      else
        {
          g_warning ("Thunar Test [file_menu]: Could not find File menu item");
          g_application_quit (g_application_get_default ());
        }
      return FALSE;
    }

  return TRUE;
}
