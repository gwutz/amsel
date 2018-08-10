#include <glib.h>
#include "amsel-cache.h"
#include "amsel-memory-database.h"

void
test_init_database (void)
{
  GError *error = NULL;
  AmselChannel *channel = amsel_channel_new ("");
  g_autoptr (AmselMemoryDatabase) db = amsel_memory_database_new ();
  amsel_channel_set_source (channel, "custom source 1");

  g_autoptr (AmselCache) cache = amsel_cache_new (AMSEL_DATABASE (db));
  amsel_cache_add_channel (cache, channel, &error);
  g_assert_no_error (error);
  g_object_unref (channel);

  g_assert_cmpint (amsel_memory_database_get_counter (db), ==, 1);

  channel = amsel_channel_new ("");
  amsel_channel_set_source (channel, "custom source 2");
  amsel_memory_database_set_error (db, TRUE);
  amsel_cache_add_channel (cache, channel, &error);
  g_object_unref (channel);
  g_assert_error (error, AMSEL_DATABASE_ERROR, AMSEL_DATABASE_ERROR_INSERT);
  g_clear_error (&error);
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/database/init", test_init_database);

  return g_test_run ();
}
