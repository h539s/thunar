#include "tests/thunar-test-snapshots.h"
#include "tests/thunar-test-control.h"

const gchar*
thunar_test_get_name (void)
{
  return "open";
}

gboolean
thunar_run_test (GtkWindow *window, gint step)
{
  if (step == 0)
    {
      thunar_test_setup_window (window);
      return TRUE;
    }

  return FALSE;
}
