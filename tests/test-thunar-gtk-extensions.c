
#include <gtk/gtk.h>
#include "thunar/thunar-gtk-extensions.h"

static void
test_gtk_editable_can_cut (void)
{
  GtkWidget *entry = gtk_entry_new ();
  
  g_assert_false (thunar_gtk_editable_can_cut (GTK_EDITABLE (entry)));
  g_assert_false (thunar_gtk_editable_can_copy (GTK_EDITABLE (entry)));
  
  gtk_entry_set_text (GTK_ENTRY (entry), "hello");
  gtk_editable_select_region (GTK_EDITABLE (entry), 0, -1);
  
  /* Selection present and editable, should be true */
  g_assert_true (thunar_gtk_editable_can_copy (GTK_EDITABLE (entry)));
  g_assert_true (thunar_gtk_editable_can_cut (GTK_EDITABLE (entry)));
  
  gtk_editable_set_editable (GTK_EDITABLE (entry), FALSE);
  g_assert_false (thunar_gtk_editable_can_cut (GTK_EDITABLE (entry)));
  g_assert_true (thunar_gtk_editable_can_copy (GTK_EDITABLE (entry)));
  
  gtk_widget_destroy (entry);
}

int
main (int argc, char **argv)
{
  if (!gtk_init_check (&argc, &argv))
    return 0;

  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/thunar-gtk-extensions/test_gtk_editable_can_cut", test_gtk_editable_can_cut);

  return g_test_run ();
}
