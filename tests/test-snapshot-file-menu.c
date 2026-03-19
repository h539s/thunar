#include "tests/thunar-test-snapshots.h"
#include "tests/thunar-test-control.h"
#include "tests/thunar-job-control.h"
#include "thunar/thunar-preferences.h"

const gchar*
thunar_test_get_name (void)
{
  return "file_menu";
}

gboolean
thunar_run_test (GtkWindow *window, gint step)
{
  GtkWidget *file_menu_item = NULL;
  GtkWidget *titlebar;

  switch (step)
    {
    case 0:
      thunar_test_setup_window (window);
      /* Standardize the view: ensure the menubar is visible */
      g_object_set (thunar_preferences_get (), "last-menubar-visible", TRUE, NULL);
      return TRUE;

    case 1:
      gtk_container_foreach (GTK_CONTAINER (window), find_file_menu_and_click, &file_menu_item);

      if (file_menu_item == NULL)
        {
          titlebar = gtk_window_get_titlebar (window);
          if (titlebar)
            gtk_container_foreach (GTK_CONTAINER (titlebar), find_file_menu_and_click, &file_menu_item);
        }

      if (file_menu_item)
        {
          GtkWidget *menubar = gtk_widget_get_parent (file_menu_item);
          if (menubar)
            gtk_widget_show_all (menubar);

          gtk_window_present (window);
          gtk_widget_grab_focus (file_menu_item);

          g_print ("Thunar Test [file_menu]: Activating File menu\n");
          gtk_menu_item_select (GTK_MENU_ITEM (file_menu_item));
          gtk_menu_item_activate (GTK_MENU_ITEM (file_menu_item));

          return TRUE;
        }
      else
        {
          g_warning ("Thunar Test [file_menu]: Could not find File menu item");
          g_application_quit (g_application_get_default ());
        }
      return FALSE;

    case 2:
      /* Wait for the menu to open and render. */
      return FALSE; /* End of steps for this test */

    default:
      return FALSE;
    }
}
