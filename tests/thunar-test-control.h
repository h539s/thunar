#ifndef __THUNAR_TEST_CONTROL_H__
#define __THUNAR_TEST_CONTROL_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

void     thunar_test_setup_window (GtkWindow *window);
void     find_file_menu_and_click (GtkWidget *widget, gpointer data);
void     find_tree_view           (GtkWidget *widget, gpointer data);
gboolean thunar_run_test          (GtkWindow *window, gint step);
const gchar* thunar_test_get_name (void);

G_END_DECLS

#endif /* !__THUNAR_TEST_CONTROL_H__ */
