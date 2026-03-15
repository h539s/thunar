#include "tests/thunar-test-snapshots.h"
#include <gdk/gdk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>

#define SNAPSHOTS_DIR "snapshots"
#define ERROR_PREFIX "err_"

static gchar *
get_snapshots_dir (void)
{
  const gchar *srcdir = g_getenv ("G_TEST_SRCDIR");
  if (srcdir != NULL)
    return g_build_filename (srcdir, SNAPSHOTS_DIR, NULL);
  return g_strdup (SNAPSHOTS_DIR);
}

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

void
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

  gchar *snapshots_dir = get_snapshots_dir ();
  if (!g_file_test (snapshots_dir, G_FILE_TEST_IS_DIR))
    g_mkdir_with_parents (snapshots_dir, 0755);

  reference_path = g_build_filename (snapshots_dir, g_strconcat (test_case, ".png", NULL), NULL);

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
              error_path = g_build_filename (snapshots_dir, g_strconcat (ERROR_PREFIX, test_case, ".png", NULL), NULL);
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
  g_free (snapshots_dir);
  g_object_unref (screenshot);

  g_print ("Thunar Test [%s]: Completed. Closing Thunar...\n", test_case);
  g_application_quit (g_application_get_default ());
}
