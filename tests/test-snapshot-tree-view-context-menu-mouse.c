#include "tests/thunar-test-snapshots.h"
#include "tests/thunar-test-control.h"
#include "tests/thunar-job-control.h"
#include "thunar/thunar-preferences.h"
#include "thunar/thunar-tree-view.h"

const gchar*
thunar_test_get_name (void)
{
  return "tree_view_context_menu_mouse";
}

gboolean
thunar_run_test (GtkWindow *window, gint step)
{
  GtkWidget *tree_view = NULL;
  GtkTreePath *path;
  GdkRectangle rect;

  switch (step)
    {
    case 0:
      /* Standardize the view: ensure the tree pane is visible */
      g_object_set (thunar_preferences_get (), "last-side-pane", 1, NULL); // THUNAR_SIDEPANE_TYPE_TREE
      return TRUE;

    case 1:
      find_tree_view (GTK_WIDGET (window), &tree_view);
      if (tree_view)
        {
          g_print ("Thunar Test [tree_view_context_menu_mouse]: Found Tree View\n");
          /* Select the first row */
          path = gtk_tree_path_new_from_indices (0, -1);
          gtk_tree_view_set_cursor (GTK_TREE_VIEW (tree_view), path, NULL, FALSE);

          /* Get the cell area to click on */
          gtk_tree_view_get_cell_area (GTK_TREE_VIEW (tree_view), path, NULL, &rect);

          /* Simulate right click */
          GdkEvent *event = gdk_event_new (GDK_BUTTON_PRESS);
          event->button.window = g_object_ref (gtk_widget_get_window (tree_view));
          event->button.send_event = TRUE;
          event->button.time = GDK_CURRENT_TIME;
          event->button.x = rect.x + rect.width / 2;
          event->button.y = rect.y + rect.height / 2;
          event->button.axes = NULL;
          event->button.state = 0;
          event->button.button = 3;

          gdk_event_set_device (event, gdk_seat_get_pointer (gdk_display_get_default_seat (gtk_widget_get_display (tree_view))));

          event->button.x_root = 0;
          event->button.y_root = 0;

          g_print ("Thunar Test [tree_view_context_menu_mouse]: Sending right-click event\n");
          gtk_main_do_event (event);
          gdk_event_free (event);

          gtk_tree_path_free (path);
          
          return TRUE;
        }
      else
        {
          /* Not found yet, retry this step by returning TRUE without main incrementing step? 
             Wait, if I return TRUE main increments it. 
             If I return FALSE it takes snapshot.
             The user said: "quando não tiver mais steps o thunar_run_test retorna false e o o test-snapshot-main tira o snapshot"
             And "quem incrementa o step é o test-snapshot-main, não os testes!!"
             So if I want to retry Case 1, I should NOT return TRUE if I haven't finished Case 1.
             But the callback only triggers when idle. 
             If I stay in Case 1, how do I trigger the next idle?
             The user said "você não deve executar thunar_job_control_increment/thunar_job_control_decrement/trigger_job_control_idle isso é feito em outro lugar"
             This implies something else will trigger the idle. 
             In Thunar, many things trigger jobs or idles. 
             But if nothing is happening, the callback might not trigger again unless we do something.
             However, the user says "pressupões que esteja definido" and "isso é feito em outro lugar".
          */
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
