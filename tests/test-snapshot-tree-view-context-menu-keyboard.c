#include "tests/thunar-test-snapshots.h"
#include "tests/thunar-test-control.h"
#include "thunar/thunar-preferences.h"
#include "thunar/thunar-tree-view.h"
#include <gdk/gdkkeysyms.h>

static gboolean
thunar_test_tree_view_context_menu_keyboard_step3 (gpointer data)
{
  take_snapshot_and_compare (GTK_WINDOW (data), "tree_view_context_menu_keyboard");
  return FALSE;
}

static gboolean
thunar_test_tree_view_context_menu_keyboard_step2 (gpointer data)
{
  GtkWidget *tree_view = GTK_WIDGET (data);
  GtkTreePath *path;

  /* Select the first row */
  path = gtk_tree_path_new_from_indices (0, -1);
  gtk_tree_view_set_cursor (GTK_TREE_VIEW (tree_view), path, NULL, FALSE);
  gtk_widget_grab_focus (tree_view);

  /* Wait a bit for the menu to open and render. Schedule BEFORE triggering. */
  g_timeout_add (1000, thunar_test_tree_view_context_menu_keyboard_step3, gtk_widget_get_toplevel (tree_view));

  /* Simulate Menu key press */
  GdkEvent *event = gdk_event_new (GDK_KEY_PRESS);
  event->key.window = g_object_ref (gtk_widget_get_window (tree_view));
  event->key.send_event = TRUE;
  event->key.time = GDK_CURRENT_TIME;
  event->key.state = 0;
  event->key.keyval = GDK_KEY_Menu;
  event->key.length = 0;
  event->key.string = g_strdup ("");
  event->key.hardware_keycode = 0;
  event->key.group = 0;
  event->key.is_modifier = FALSE;

  gdk_event_set_device (event, gdk_seat_get_keyboard (gdk_display_get_default_seat (gtk_widget_get_display (tree_view))));

  g_print ("Thunar Test [tree_view_context_menu_keyboard]: Sending Menu key event\n");
  gtk_main_do_event (event);
  gdk_event_free (event);

  gtk_tree_path_free (path);
  return FALSE;
}

static gboolean
thunar_test_tree_view_context_menu_keyboard_find_tree_view (gpointer data)
{
  GtkWindow *window = GTK_WINDOW (data);
  GtkWidget *tree_view = NULL;

  find_tree_view (GTK_WIDGET (window), &tree_view);

  if (tree_view)
    {
      g_print ("Thunar Test [tree_view_context_menu_keyboard]: Found Tree View\n");
      /* Wait for the tree view to be populated */
      g_timeout_add (1000, thunar_test_tree_view_context_menu_keyboard_step2, tree_view);
    }
  else
    {
      static int attempts = 0;
      if (attempts++ < 10)
        {
          g_timeout_add (500, thunar_test_tree_view_context_menu_keyboard_find_tree_view, window);
        }
      else
        {
          g_warning ("Thunar Test [tree_view_context_menu_keyboard]: Could not find Tree View after several attempts");
          g_application_quit (g_application_get_default ());
        }
    }
  return FALSE;
}

gboolean
thunar_test_run (gpointer data)
{
  GtkWindow *window = GTK_WINDOW (data);

  if (!gtk_widget_get_visible (GTK_WIDGET (window)))
    return TRUE;

  if (gtk_widget_get_window (GTK_WIDGET (window)) == NULL)
    return TRUE;

  thunar_test_setup_window (window);

  /* Standardize the view: ensure the tree pane is visible */
  g_object_set (thunar_preferences_get (), "last-side-pane", 1, NULL); // THUNAR_SIDEPANE_TYPE_TREE

  g_timeout_add (500, thunar_test_tree_view_context_menu_keyboard_find_tree_view, window);

  return FALSE;
}
