#include "tests/thunar-test-utils.h"
#include "tests/thunar-test-snapshots.h"
#include "tests/thunar-test-control.h"
#include "thunar/thunar-preferences.h"

static gboolean
thunar_test_file_menu_step2 (gpointer data)
{
  take_snapshot_and_compare (GTK_WINDOW (data), "file_menu");
  return FALSE;
}

gboolean
thunar_test_run (gpointer data)
{
  GtkWindow *window = GTK_WINDOW (data);
  GtkWidget *file_menu_item = NULL;
  GtkWidget *titlebar;

  if (!gtk_widget_get_visible (GTK_WIDGET (window)))
    return TRUE;

  if (gtk_widget_get_window (GTK_WIDGET (window)) == NULL)
    return TRUE;

  thunar_test_setup_window (window);

  /* Standardize the view: ensure the menubar is visible */
  g_object_set (thunar_preferences_get (), "last-menubar-visible", TRUE, NULL);

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

      /* Wait a bit for the menu to open and render */
      g_timeout_add (1000, thunar_test_file_menu_step2, window);
    }
  else
    {
      g_warning ("Thunar Test [file_menu]: Could not find File menu item");
      g_application_quit (g_application_get_default ());
    }

  return FALSE;
}
