#include "tests/thunar-test-snapshots.h"
#include "tests/thunar-test-control.h"
#include "tests/thunar-job-control.h"
#include "thunar/thunar-preferences.h"
#include "thunar/thunar-tree-view.h"
#include <gdk/gdkkeysyms.h>

const gchar*
thunar_test_get_name (void)
{
  return "tree_view_context_menu_keyboard";
}

gboolean
thunar_run_test (GtkWindow *window, gint step)
{
  GtkWidget *tree_view = NULL;
  GtkTreePath *path;

  switch (step)
    {
    case 0:
      thunar_test_setup_window (window);
      /* Standardize the view: ensure the tree pane is visible */
      g_object_set (thunar_preferences_get (), "last-side-pane", 1, NULL); // THUNAR_SIDEPANE_TYPE_TREE
      return TRUE;

    case 1:
      find_tree_view (GTK_WIDGET (window), &tree_view);
      if (tree_view)
        {
          g_print ("Thunar Test [tree_view_context_menu_keyboard]: Found Tree View\n");
          /* Select the first row */
          path = gtk_tree_path_new_from_indices (0, -1);
          gtk_tree_view_set_cursor (GTK_TREE_VIEW (tree_view), path, NULL, FALSE);
          gtk_widget_grab_focus (tree_view);

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
          
          return TRUE;
        }
      else
        {
          return TRUE;
        }
      return FALSE;

    case 2:
      /* Let the menu open */
      return FALSE;

    default:
      return FALSE;
    }
}
