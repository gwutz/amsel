#include <glib.h>
/* #include "amsel-cache.h" */
/* #include "amsel-sqlite-database.h" */
/* #include "amsel-debug.h" */
#include "amsel-memory-database.h"
#include "alb.h"

void
test_init_database (void)
{
  GError *error = NULL;
  AlbChannel *channel = alb_channel_new ();
  AmselMemoryDatabase* db = amsel_memory_database_new ();
  alb_channel_set_id (channel, "custom source 1");

  g_autoptr (AlbCache) cache = alb_cache_new (ALB_DATABASE (db));
  alb_cache_add_channel (cache, channel, &error);
  g_assert_no_error (error);
  g_object_unref (channel);

  g_assert_cmpint (amsel_memory_database_get_counter (db), ==, 1);

  channel = alb_channel_new ();
  alb_channel_set_id (channel, "custom source 2");
  amsel_memory_database_set_error (db, TRUE);
  alb_cache_add_channel (cache, channel, &error);
  g_object_unref (channel);
  g_assert_error (error, ALB_DATABASE_ERROR, ALB_DATABASE_ERROR_INSERT);
  g_clear_error (&error);
}

void
test_init_database2 (void)
{
  struct {
    char *id;
    char *title;
    char *source;
  } testdata[] ={
    {
      .id = "custom id 1",
      .title = "custom title 1",
      .source = "custom source 1"
    },{
      .id = "custom id 2",
      .title = "custom title 2",
      .source = "custom source 2"
    }
  };

  struct {
    char *id;
    char *title;
    char *content;
    GDateTime *updated;
    char *link;
  } testentries[] = {
      {
        .id = "id1",
        .title = "title1",
        .content = "content1",
        .updated = g_date_time_new_now_local (),
        .link = "link1",
      }, {
        .id = "id2",
        .title = "title2",
        .content = "content2",
        .updated = g_date_time_new_now_local (),
        .link = "link2",
      }
  };

  AlbSqliteDatabase* db = alb_sqlite_database_new (":memory:");
  g_autoptr (AlbCache) cache = alb_cache_new (ALB_DATABASE (db));


  for (int i = 0; i < sizeof (testdata)/sizeof(*testdata); i++) {
    g_autoptr (AlbChannel) channel = alb_channel_new ();
    alb_channel_set_source (channel, testdata[i].source);
    alb_channel_set_id (channel, testdata[i].id);
    alb_channel_set_title (channel, testdata[i].title);

    for (int j = 0; j < sizeof (testentries)/sizeof(*testentries); j++) {
      AlbEntry* entry = alb_entry_new ();
      alb_entry_set_id (entry, testentries[j].id);
      alb_entry_set_title (entry, testentries[j].title);
      alb_entry_set_content (entry, testentries[j].content);
      alb_entry_set_updated_datetime (entry, testentries[j].updated);
      alb_entry_set_link (entry, testentries[j].link);

      alb_channel_add_entry (channel, entry);
    }

    alb_cache_add_channel (cache, channel, NULL);
  }

  GPtrArray *channels = alb_database_get_channels (ALB_DATABASE (db));
  g_ptr_array_set_free_func (channels, g_object_unref);

  g_assert_nonnull (channels);
  g_assert_cmpint (channels->len, ==, 2);

  for (int i = 0; i < channels->len; i++)
    {
      AlbChannel *c = g_ptr_array_index (channels, i);

      g_assert_cmpstr (alb_channel_get_id (c), ==, testdata[i].id);
      g_assert_cmpstr (alb_channel_get_source (c), ==, testdata[i].source);
      g_assert_cmpstr (alb_channel_get_title (c), ==, testdata[i].title);

      GHashTable *entries = alb_channel_get_entries (c);
      g_assert_cmpint (g_hash_table_size (entries), ==, 2);
      for (int j = 0; j < sizeof (testentries)/sizeof(*testentries); j++) {
        AlbEntry *e = g_hash_table_lookup (entries, testentries[j].id);

        g_assert_cmpstr (alb_entry_get_title (e), ==, testentries[j].title);
        g_assert_cmpstr (alb_entry_get_content (e), ==, testentries[j].content);
        g_assert_cmpint (g_date_time_to_unix (alb_entry_get_updated (e)), ==, g_date_time_to_unix (testentries[j].updated));
        g_assert_cmpstr (alb_entry_get_link (e), ==, testentries[j].link);
      }
    }


  for (int j = 0; j < sizeof (testentries)/sizeof(*testentries); j++) {
    g_date_time_unref (testentries[j].updated);
  }

  g_ptr_array_unref (channels);
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/database/init", test_init_database);
  g_test_add_func ("/database/init2", test_init_database2);

  return g_test_run ();
}
