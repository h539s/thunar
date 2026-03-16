#include "tests/thunar-test-utils.h"
#include "tests/thunar-test-snapshots.h"
#include "tests/thunar-test-control.h"

static gboolean
thunar_test_open_step2 (gpointer data)
{
  take_snapshot_and_compare (GTK_WINDOW (data), "open");
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

  /* Wait a bit for the window to maximize and render */
  g_timeout_add (1000, thunar_test_open_step2, window);

  return FALSE;
}
