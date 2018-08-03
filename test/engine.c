#include <glib.h>
#include "amsel-engine.h"
#include "amsel-request.h"
#include "amsel-config.h"
#include "amsel-channel.h"

void
test_validate_xml (void)
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
      AmselRequest *data = amsel_request_new (AMSEL_REQUEST_TYPE_XML, xml, length);
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
test_parse_xml (void)
{
  g_autoptr (AmselEngine) engine;
  AmselRequest *request;
  GPtrArray *channels;
  char *xml =
    "<rss>"
    " <channel>"
    "   <title>Planet Gnome</title>"
    "   <description>This is Planet Gnome</description>"
    "   <link>http://planet.gnome.org</link>"
    "   <item>"
    "     <title>Post 1</title>"
    "     <description>Post 1 Description</description>"
    "     <pubDate>Mon, 01 Jan 2018 21:19:18</pubDate>"
    "   </item>"
    " </channel>"
    "</rss>";

  request = amsel_request_new (AMSEL_REQUEST_TYPE_XML, xml, strlen (xml));

  engine = amsel_engine_new ();
  channels = amsel_engine_parse (engine, request);

  g_assert_nonnull (channels);
  g_assert_cmpint (1, ==, channels->len);
  for (int i = 0; i < channels->len; i++)
    {
      AmselChannel *c = g_ptr_array_index (channels, i);
      g_assert_true (AMSEL_IS_CHANNEL (c));

      g_assert_cmpstr (amsel_channel_get_title (c), ==, "Planet Gnome");
      g_assert_cmpstr (amsel_channel_get_description (c), ==, "This is Planet Gnome");
      g_assert_cmpstr (amsel_channel_get_source (c), ==, "http://planet.gnome.org");

      const GList *items = amsel_channel_get_entries (c);
      for (const GList *cur = items; cur != NULL; cur = g_list_next (cur))
        {
          AmselEntry *item = cur->data;
          g_assert_true (AMSEL_IS_ENTRY (item));
          g_assert_cmpstr (amsel_entry_get_title (item), ==, "Post 1");
          g_assert_cmpstr (amsel_entry_get_content (item), ==, "Post 1 Description");
          GDateTime *pubDate = amsel_entry_get_updated (item);
          g_assert_nonnull (pubDate);
          g_assert (g_date_time_to_unix (pubDate) == 1514837958);
        }
    }
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/validate/xml", test_validate_xml);
  g_test_add_func ("/parser/xml", test_parse_xml);

  return g_test_run ();
}
