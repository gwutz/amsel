#include <glib.h>
#include "amsel-config.h"
/* #include "amsel-engine.h" */
/* #include "amsel-channel.h" */
/* #include "amsel-cache.h" */
#include "amsel-memory-database.h"
#include "alb.h"

static void
test_add_items (void)
{
  g_autoptr (AlbEngine) engine = alb_engine_new ("amsel.db");
  char *xml;
  gsize length;
  GError *error = NULL;

  g_file_get_contents (SRCDIR"/test/testdata_cache/pl_gnome1.xml", &xml, &length, &error);
  AlbRequest *request = alb_request_new (xml, length, NULL);
  g_free (xml);

  GPtrArray *channels = alb_engine_parse (engine, request);
  alb_request_free (request);

  g_file_get_contents (SRCDIR"/test/testdata_cache/pl_gnome2.xml", &xml, &length, &error);
  request = alb_request_new (xml, length, NULL);
  g_free (xml);

  g_ptr_array_unref (channels);
  channels = alb_engine_parse (engine, request);
  alb_request_free (request);

  g_assert_cmpint (channels->len, ==, 1);
  GHashTable *entries = alb_channel_get_entries (g_ptr_array_index (channels, 0));

  g_assert_cmpint (g_hash_table_size (entries), ==, 42);
  g_ptr_array_unref (channels);

  /** test persistence of channel */
  channels = alb_engine_get_channels (engine);
  g_assert_nonnull (channels);
  g_assert_cmpint (channels->len, ==, 1);
  g_ptr_array_unref (channels);
}

void
test_add_channel_error (void)
{
  GError *error = NULL;
  AlbChannel *channel = alb_channel_new ();

  g_autoptr (AlbDatabase) database = ALB_DATABASE (amsel_memory_database_new ());
  AlbCache *cache = alb_cache_new (database);
  alb_cache_add_channel (cache, channel, &error);
  g_object_unref (channel);
  g_assert_error (error, ALB_CACHE_ERROR, ALB_CACHE_ERROR_NO_ID);
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
