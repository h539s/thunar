
#include <glib.h>
#include <gio/gio.h>
#include "thunar/thunar-gio-extensions.h"

static void
test_g_file_is_root (void)
{
  GFile *root = g_file_new_for_uri ("file:///");
  GFile *other = g_file_new_for_uri ("file:///home");

  g_assert_true (thunar_g_file_is_root (root));
  g_assert_false (thunar_g_file_is_root (other));

  g_object_unref (root);
  g_object_unref (other);
}

static void
test_g_file_is_trashed (void)
{
  GFile *trashed = g_file_new_for_uri ("trash:///file.txt");
  GFile *local = g_file_new_for_uri ("file:///tmp/file.txt");

  g_assert_true (thunar_g_file_is_trashed (trashed));
  g_assert_false (thunar_g_file_is_trashed (local));

  g_object_unref (trashed);
  g_object_unref (local);
}

static void
test_g_file_get_location (void)
{
  GFile *file = g_file_new_for_path ("/tmp/file.txt");
  gchar *location;

  location = thunar_g_file_get_location (file);
  g_assert_cmpstr (location, ==, "/tmp/file.txt");
  g_free (location);

  g_object_unref (file);
}

int
main (int argc, char **argv)
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/thunar-gio-extensions/test_g_file_is_root", test_g_file_is_root);
  g_test_add_func ("/thunar-gio-extensions/test_g_file_is_trashed", test_g_file_is_trashed);
  g_test_add_func ("/thunar-gio-extensions/test_g_file_get_location", test_g_file_get_location);

  return g_test_run ();
}
