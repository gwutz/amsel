#include <glib.h>
#include "alb.h"

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

    g_date_time_unref (parsedDate);
  }

  g_ptr_array_free (testdates, TRUE);
}

/**
 * What happens when i define a g_autoptr variable an redefine this?
 *
 * 2 Options:
 *   - nothing happens, leads to a memleak
 *   - the cleanup gets called twice
 *
 * lets see
 */
typedef struct {
  int placeholder;
} data;

void
data_cleanup (data *d) {
  g_print ("%s", "cleanup data");
  g_free (d);
}

G_DEFINE_AUTOPTR_CLEANUP_FUNC (data, data_cleanup)

void
test_autocleanup (void)
{
  g_autoptr (data) test = malloc (sizeof (data));
  test->placeholder = 1;

  g_free (test);
  test = malloc (sizeof (data));
  test->placeholder = 2;
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/dateparser/rfc822", test_parse_rfc822);
  g_test_add_func ("/toys/autocleanup", test_autocleanup);

  return g_test_run ();
}
