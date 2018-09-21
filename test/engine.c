#include <glib.h>
#include "amsel-config.h"
#include "alb.h"
#include <libxml/xmlerror.h>

void
test_validate_rss (void)
{
  GError *error = NULL;
  GDir *testdatadir = g_dir_open (SRCDIR"/test/testdata_rss", 0, &error);
  const char *name;
  char *xml;
  g_autoptr (AmselEngine) engine;

  engine = amsel_engine_new ();

  while ((name = g_dir_read_name (testdatadir)))
    {
      gboolean shouldfail = g_str_has_prefix (name, "fail");
      gsize length;

      g_autofree char *file = g_build_filename (SRCDIR, "/test/testdata_rss/", name, NULL);
      g_file_get_contents (file, &xml, &length, &error);
      g_debug ("Process testfile: %s", file);
      AmselRequest *data = amsel_request_new (xml, length, NULL);
      g_free (xml);

      gboolean validate = amsel_engine_validate (engine, data);
      amsel_request_free (data);

      if (shouldfail)
        g_assert_false (validate);
      else
        g_assert_true (validate);
    }

  g_dir_close (testdatadir);
}

void
test_validate_atom (void)
{
  GError *error = NULL;
  GDir *testdatadir = g_dir_open (SRCDIR"/test/testdata_atom", 0, &error);
  const char *name;
  char *xml;
  g_autoptr (AmselEngine) engine;

  engine = amsel_engine_new ();

  while ((name = g_dir_read_name (testdatadir)))
    {
      gboolean shouldfail = g_str_has_prefix (name, "fail");
      gsize length;

      g_autofree char *file = g_build_filename (SRCDIR, "/test/testdata_atom/", name, NULL);
      g_debug ("Process testfile: %s", file);
      g_file_get_contents (file, &xml, &length, &error);
      AmselRequest *data = amsel_request_new (xml, length, NULL);
      g_free (xml);

      gboolean validate = amsel_engine_validate (engine, data);
      amsel_request_free (data);

      if (shouldfail)
        g_assert_false (validate);
      else
        g_assert_true (validate);
    }

  g_dir_close (testdatadir);
}

void
test_parse_rss (void)
{
  GError *error = NULL;
  g_autoptr (AmselEngine) engine;
  AmselRequest *request;
  GPtrArray *channels;
  char *xml;
  gsize length;

  g_file_get_contents (SRCDIR"/test/testdata_rss/parse.xml", &xml, &length, &error);

  request = amsel_request_new (xml, strlen (xml), NULL);
  g_free (xml);

  engine = amsel_engine_new ();
  channels = amsel_engine_parse (engine, request);

  amsel_request_free (request);

  g_assert_nonnull (channels);
  g_assert_cmpint (1, ==, channels->len);
  for (int i = 0; i < channels->len; i++)
    {
      AmselChannel *c = g_ptr_array_index (channels, i);
      g_assert_true (AMSEL_IS_CHANNEL (c));

      g_assert_cmpstr (amsel_channel_get_title (c), ==, "Planet Gnome");
      g_assert_cmpstr (amsel_channel_get_description (c), ==, "This is Planet Gnome");
      g_assert_cmpstr (amsel_channel_get_source (c), ==, "http://planet.gnome.org");
      g_assert_cmpstr (amsel_channel_get_icon (c), ==, "http://planet.gnome.org/icon.png");

      GHashTable *items = amsel_channel_get_entries (c);
      g_assert_cmpint (g_hash_table_size (items), ==, 1);
      GHashTableIter iter;
      gpointer key, value;
      g_hash_table_iter_init (&iter, items);
      while (g_hash_table_iter_next (&iter, &key, &value))
        {
          AmselEntry *item = value;
          g_assert_true (AMSEL_IS_ENTRY (item));
          g_assert_cmpstr (amsel_entry_get_title (item), ==, "Post 1");
          g_assert_cmpstr (amsel_entry_get_content (item), ==, "Post 1 Description");
          GDateTime *pubDate = amsel_entry_get_updated (item);
          g_assert_nonnull (pubDate);
          g_assert (g_date_time_to_unix (pubDate) == 1514837958);
          g_assert_cmpstr (amsel_entry_get_author (item), ==, "Günther Wagner");
          g_assert_cmpstr (amsel_entry_get_id (item), ==, "unique identifier");
        }
    }
  g_ptr_array_unref (channels);

  /** test persistence of channel */
  channels = amsel_engine_get_channels (engine);
  g_assert_nonnull (channels);
  g_assert_cmpint (channels->len, ==, 1);
  g_ptr_array_unref (channels);
}

void
test_parse_atom (void)
{
  GError *error = NULL;
  g_autoptr (AmselEngine) engine;
  AmselRequest *request;
  GPtrArray *channels;
  g_autofree char *xml;
  gsize length;

  g_file_get_contents (SRCDIR"/test/testdata_atom/parse.xml", &xml, &length, &error);

  request = amsel_request_new (xml, strlen (xml), NULL);

  engine = amsel_engine_new ();
  channels = amsel_engine_parse (engine, request);

  amsel_request_free (request);

  g_assert_nonnull (channels);
  g_assert_cmpint (1, ==, channels->len);
  for (int i = 0; i < channels->len; i++)
    {
      AmselChannel *c = g_ptr_array_index (channels, i);
      g_assert_true (AMSEL_IS_CHANNEL (c));

      g_assert_cmpstr (amsel_channel_get_title (c), ==, "Planet Gnome");
      g_assert_cmpstr (amsel_channel_get_source (c), ==, "http://planet.gnome.org");

      GHashTable *items = amsel_channel_get_entries (c);
      g_assert_cmpint (g_hash_table_size (items), ==, 1);
      GHashTableIter iter;
      gpointer key, value;
      g_hash_table_iter_init (&iter, items);
      while (g_hash_table_iter_next (&iter, &key, &value))
        {
          AmselEntry *item = value;
          g_assert_true (AMSEL_IS_ENTRY (item));
          g_assert_cmpstr (amsel_entry_get_title (item), ==, "Post 1");
          g_assert_cmpstr (amsel_entry_get_content (item), ==, "Post 1 Content");
          GDateTime *pubDate = amsel_entry_get_updated (item);
          g_assert_nonnull (pubDate);
          g_assert (g_date_time_to_unix (pubDate) == 1533377473);
          g_assert_cmpstr (amsel_entry_get_author (item), ==, "Günther Wagner");
          g_assert_cmpstr (amsel_entry_get_id (item), ==, "Post 1 ID");
        }
    }
  g_ptr_array_unref (channels);

  /** test persistence of channel */
  channels = amsel_engine_get_channels (engine);
  g_assert_nonnull (channels);
  g_assert_cmpint (channels->len, ==, 1);
  g_ptr_array_unref (channels);
}

void
xml_error_func (void       *ctx,
                const char *msg,
                ...)
{
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  xmlGenericErrorFunc error_func = &(xml_error_func);
  initGenericErrorDefaultFunc (&error_func);

  /* g_test_add_func ("/validate/rss", test_validate_rss); */
  /* g_test_add_func ("/validate/atom", test_validate_atom); */
  g_test_add_func ("/parser/rss", test_parse_rss);
  g_test_add_func ("/parser/atom", test_parse_atom);

  return g_test_run ();
}
