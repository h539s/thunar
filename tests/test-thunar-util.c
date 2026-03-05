
#include <glib.h>
#include <time.h>
#include "thunar/thunar-util.h"

static void
test_util_str_get_extension (void)
{
  g_assert_cmpstr (thunar_util_str_get_extension ("file.txt"), ==, ".txt");
  g_assert_cmpstr (thunar_util_str_get_extension ("file.tar.gz"), ==, ".tar.gz");
  g_assert_cmpstr (thunar_util_str_get_extension ("file.desktop.in.in"), ==, ".desktop.in.in");
  g_assert_null (thunar_util_str_get_extension ("file"));
  g_assert_null (thunar_util_str_get_extension (".hidden"));
}

static void
test_util_strjoin_list (void)
{
  GList *list = NULL;
  gchar *result;

  list = g_list_append (list, "first");
  list = g_list_append (list, NULL);
  list = g_list_append (list, "second");
  list = g_list_append (list, "");
  list = g_list_append (list, "third");

  result = thunar_util_strjoin_list (list, ", ");
  g_assert_cmpstr (result, ==, "first, second, third");
  g_free (result);

  g_list_free (list);
}

static void
test_util_split_search_query (void)
{
  gchar **terms;
  GError *error = NULL;

  terms = thunar_util_split_search_query ("word1   word2", &error);
  g_assert_no_error (error);
  g_assert_nonnull (terms);
  g_assert_cmpstr (terms[0], ==, "word1");
  g_assert_cmpstr (terms[1], ==, "word2");
  g_assert_null (terms[2]);

  g_strfreev (terms);
}

static void
test_util_search_terms_match (void)
{
  gchar *terms[] = { "word1", "word2", NULL };

  g_assert_true (thunar_util_search_terms_match (terms, "this is word1 and word2"));
  g_assert_false (thunar_util_search_terms_match (terms, "this is word1 only"));
}

static void
test_util_time_from_rfc3339 (void)
{
  time_t t;
  struct tm *tm;
  struct tm expected_tm;

  const gchar *date_str = "2023-10-27T10:30:00";

  t = thunar_util_time_from_rfc3339 (date_str);
  g_assert_cmpint (t, !=, 0);

  memset (&expected_tm, 0, sizeof (expected_tm));
  expected_tm.tm_year = 2023 - 1900;
  expected_tm.tm_mon = 10 - 1;
  expected_tm.tm_mday = 27;
  expected_tm.tm_hour = 10;
  expected_tm.tm_min = 30;
  expected_tm.tm_sec = 0;
  expected_tm.tm_isdst = -1; /* Let mktime determine DST */

  g_assert_cmpint (t, ==, mktime (&expected_tm));

  tm = localtime (&t);
  g_assert_cmpint (tm->tm_year + 1900, ==, 2023);
  g_assert_cmpint (tm->tm_mon + 1, ==, 10);
  g_assert_cmpint (tm->tm_mday, ==, 27);
  g_assert_cmpint (tm->tm_hour, ==, 10);
  g_assert_cmpint (tm->tm_min, ==, 30);
  g_assert_cmpint (tm->tm_sec, ==, 00);
}

static void
test_util_accel_path_to_id (void)
{
  gchar *id;

  id = thunar_util_accel_path_to_id ("<Actions>/ThunarWindow/reload");
  g_assert_cmpstr (id, ==, "reload");
  g_free (id);

  id = thunar_util_accel_path_to_id ("simple/path");
  g_assert_cmpstr (id, ==, "path");
  g_free (id);

  g_assert_null (thunar_util_accel_path_to_id (NULL));
}

int
main (int argc, char **argv)
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/thunar-util/test_util_str_get_extension", test_util_str_get_extension);
  g_test_add_func ("/thunar-util/test_util_strjoin_list", test_util_strjoin_list);
  g_test_add_func ("/thunar-util/test_util_split_search_query", test_util_split_search_query);
  g_test_add_func ("/thunar-util/test_util_search_terms_match", test_util_search_terms_match);
  g_test_add_func ("/thunar-util/test_util_time_from_rfc3339", test_util_time_from_rfc3339);
  g_test_add_func ("/thunar-util/test_util_accel_path_to_id", test_util_accel_path_to_id);

  return g_test_run ();
}
