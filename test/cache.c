#include <glib.h>
#include "amsel-config.h"
#include "amsel-engine.h"
#include "amsel-channel.h"
#include "amsel-cache.h"
#include "amsel-memory-database.h"

static void
test_add_items (void)
{
  g_autoptr (AmselEngine) engine = amsel_engine_new ();
  char *xml;
  gsize length;
  GError *error = NULL;

  g_file_get_contents (SRCDIR"/test/testdata_cache/pl_gnome1.xml", &xml, &length, &error);
  AmselRequest *request = amsel_request_new (xml, length);
  g_free (xml);

  GPtrArray *channels = amsel_engine_parse (engine, request);
  amsel_request_free (request);

  g_file_get_contents (SRCDIR"/test/testdata_cache/pl_gnome2.xml", &xml, &length, &error);
  request = amsel_request_new (xml, length);
  g_free (xml);

  g_ptr_array_unref (channels);
  channels = amsel_engine_parse (engine, request);
  amsel_request_free (request);

  g_assert_cmpint (channels->len, ==, 1);
  GHashTable *entries = amsel_channel_get_entries (g_ptr_array_index (channels, 0));

  g_assert_cmpint (g_hash_table_size (entries), ==, 42);
  g_ptr_array_unref (channels);

  /** test persistence of channel */
  channels = amsel_engine_get_channels (engine);
  g_assert_nonnull (channels);
  g_assert_cmpint (channels->len, ==, 1);
  g_ptr_array_unref (channels);
}

void
test_add_channel_error (void)
{
  GError *error = NULL;
  AmselChannel *channel = amsel_channel_new ("");

  g_autoptr (AmselDatabase) database = AMSEL_DATABASE (amsel_memory_database_new ());
  AmselCache *cache = amsel_cache_new (database);
  amsel_cache_add_channel (cache, channel, &error);
  g_object_unref (channel);
  g_assert_error (error, AMSEL_CACHE_ERROR, AMSEL_CACHE_ERROR_NO_SOURCE);
  g_clear_error (&error);
  g_object_unref (cache);
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/cache/items", test_add_items);
  g_test_add_func ("/cache/add_channel_error", test_add_channel_error);

  return g_test_run ();
}
