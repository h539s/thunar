#include "thunar-test-utils.h"
#include "tests/thunar-test-snapshots.h"
#include "tests/thunar-test-control.h"

gboolean
thunar_test_run (gpointer data)
{
  GtkWindow *window = GTK_WINDOW (data);

  if (!gtk_widget_get_visible (GTK_WIDGET (window)))
    return TRUE;

  if (gtk_widget_get_window (GTK_WIDGET (window)) == NULL)
    return TRUE;

  thunar_test_setup_window (window);

  take_snapshot_and_compare (window, "open");

  return FALSE;
}
