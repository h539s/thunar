
#include "thunar/thunar-tree-view.h"
#include "thunar/thunar-file.h"
#include "thunar/thunar-preferences.h"
#include "thunar/thunar-gobject-extensions.h"

#include <gtk/gtk.h>
#include <glib/gstdio.h>

/* This test exercises thunar_tree_view_set_cursor by setting the current directory
 * of a ThunarTreeView and ensuring the cursor synchronization logic runs without crashing.
 * thunar_tree_view_set_cursor is an internal static function called via a timeout. */

static void
test_tree_view_set_cursor (void)
{
  GtkWidget   *view;
  ThunarFile  *file;
  GFile       *gfile;
  gchar       *tmp_dir;
  int          i;

  /* Initialize Thunar preferences in "failed" mode to avoid Xfconf dependency */
  thunar_preferences_xfconf_init_failed ();

  /* Create the tree view */
  view = thunar_tree_view_new ();
  g_object_ref_sink (view);

  /* Create a temporary directory */
  tmp_dir = g_dir_make_tmp ("thunar-test-tree-view-XXXXXX", NULL);

  if (tmp_dir) {
    gfile = g_file_new_for_path (tmp_dir);
    file = thunar_file_get (gfile, NULL);
    g_object_unref (gfile);

    if (file) {
      /* Set current directory. This triggers the asynchronous set_cursor timeout (10ms). */
      thunar_navigator_set_current_directory (THUNAR_NAVIGATOR (view), file, TRUE);
      g_object_unref (file);
    }
  }

  /* Process events to allow the timeout to fire and the logic to execute. */
  for (i = 0; i < 20; i++) {
    while (g_main_context_pending (NULL))
      g_main_context_iteration (NULL, FALSE);
    g_usleep (10000); 
  }

  /* Cleanup */
  g_object_unref (view);
  if (tmp_dir) {
    g_rmdir (tmp_dir);
    g_free (tmp_dir);
  }
}

int
main (int argc, char **argv)
{
  /* Initialize GTK with check to skip if no display is available */
  if (!gtk_init_check (&argc, &argv))
    return 0;

  /* Initialize Thunar transformations */
  thunar_g_initialize_transformations ();

  test_tree_view_set_cursor ();

  return 0;
}
