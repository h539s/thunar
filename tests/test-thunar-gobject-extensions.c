
#include <glib.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
#include "thunar/thunar-gobject-extensions.h"

static void
test_g_strescape (void)
{
  gchar *result;

  result = thunar_g_strescape ("hello world");
  g_assert_cmpstr (result, ==, "hello\\ world");
  g_free (result);

  result = thunar_g_strescape ("quote\"char");
  g_assert_cmpstr (result, ==, "quote\\\"char");
  g_free (result);
}

static void
test_g_utf8_normalize_for_search (void)
{
  gchar *result;

  result = thunar_g_utf8_normalize_for_search ("ÁéÍóÚ", TRUE, TRUE);
  g_assert_cmpstr (result, ==, "aeiou");
  g_free (result);

  result = thunar_g_utf8_normalize_for_search ("Thunar", FALSE, TRUE);
  g_assert_cmpstr (result, ==, "thunar");
  g_free (result);
}

static void
test_g_app_info_equal (void)
{
  GAppInfo *app1 = g_app_info_create_from_commandline ("true", "true-app", G_APP_INFO_CREATE_NONE, NULL);
  GAppInfo *app2 = g_app_info_create_from_commandline ("true", "true-app", G_APP_INFO_CREATE_NONE, NULL);

  if (app1 && app2)
    {
      g_assert_true (thunar_g_app_info_equal (app1, app2));
    }

  if (app1) g_object_unref (app1);
  if (app2) g_object_unref (app2);
}

int
main (int argc, char **argv)
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/thunar-gobject-extensions/test_g_strescape", test_g_strescape);
  g_test_add_func ("/thunar-gobject-extensions/test_g_utf8_normalize_for_search", test_g_utf8_normalize_for_search);
  g_test_add_func ("/thunar-gobject-extensions/test_g_app_info_equal", test_g_app_info_equal);

  return g_test_run ();
}
