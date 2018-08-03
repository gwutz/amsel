#include <glib.h>
#include "amsel-date-parser.h"

void
test_parse_rfc822 (void)
{
  GPtrArray *testdates = g_ptr_array_new ();

  g_ptr_array_add (testdates, "Wed, 02 Oct 2002 08:07:37 EST");

  for (int i = 0; i < testdates->len; i++) {
    char *td = g_ptr_array_index (testdates, i);

    GDateTime *parsedDate = amsel_date_parser_parse_RFC822 (td);

    g_assert_nonnull (parsedDate);
    g_assert_cmpint (2, ==, g_date_time_get_day_of_month (parsedDate));
    g_assert_cmpint (10, ==, g_date_time_get_month (parsedDate));
    g_assert_cmpint (2002, ==, g_date_time_get_year (parsedDate));
    g_assert_cmpint (8, ==, g_date_time_get_hour (parsedDate));
    g_assert_cmpint (7, ==, g_date_time_get_minute (parsedDate));
    g_assert_cmpint (37, ==, g_date_time_get_second (parsedDate));
  }

}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/dateparser/rfc822", test_parse_rfc822);

  return g_test_run ();
}
