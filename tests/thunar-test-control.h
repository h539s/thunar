#ifndef __THUNAR_TEST_CONTROL_H__
#define __THUNAR_TEST_CONTROL_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

void thunar_test_setup_window (GtkWindow *window);
void find_file_menu_and_click (GtkWidget *widget, gpointer data);
gboolean thunar_test_run (gpointer data);

G_END_DECLS

#endif /* !__THUNAR_TEST_CONTROL_H__ */
