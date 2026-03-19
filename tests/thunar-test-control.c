#include "tests/thunar-test-control.h"
#include "tests/thunar-test-snapshots.h"
#include "thunar/thunar-preferences.h"
#include "thunar/thunar-tree-view.h"
#include <string.h>

void
thunar_test_setup_window (GtkWindow *window)
{
  gtk_window_unmaximize (window);
  gtk_window_resize (window, 800, 600);
  gtk_window_move (window, 0, 0);
}

void
find_file_menu_and_click (GtkWidget *widget, gpointer data)
{
  GtkWidget **file_menu_item = (GtkWidget **) data;
  if (*file_menu_item != NULL)
    return;

  if (GTK_IS_MENU_ITEM (widget))
    {
      const gchar *label = gtk_menu_item_get_label (GTK_MENU_ITEM (widget));
      /* We can also check the action name if available. */
      if (label && (strstr (label, "File") || strstr (label, "Arquivo")))
        {
          *file_menu_item = widget;
          return;
        }
    }

  if (GTK_IS_CONTAINER (widget))
    gtk_container_foreach (GTK_CONTAINER (widget), find_file_menu_and_click, data);
}

void
find_tree_view (GtkWidget *widget, gpointer data)
{
  GtkWidget **tree_view = (GtkWidget **) data;
  if (*tree_view != NULL)
    return;

  if (THUNAR_IS_TREE_VIEW (widget))
    {
      *tree_view = widget;
      return;
    }

  if (GTK_IS_CONTAINER (widget))
    gtk_container_foreach (GTK_CONTAINER (widget), find_tree_view, data);
}

