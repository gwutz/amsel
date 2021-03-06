#include <glib.h>
#include <glib/gstdio.h>
#include <libsoup/soup.h>
#include "alb.h"

typedef struct {
  AlbEngine *engine;
} AlbEngineFixture;

static void
test_feed (AlbEngineFixture *fixture, SoupMessage *msg)
{
  GPtrArray *channels;
  AlbRequest *request;

  request = alb_request_new (msg->response_body->data, strlen (msg->response_body->data), soup_message_get_uri (msg)->path);
  channels = alb_engine_parse (fixture->engine, request);
  g_assert_nonnull (channels);
  g_assert_cmpint (channels->len, >, 0);
  alb_request_free (request);
}

static void
stresstest_setup (AlbEngineFixture *fixture,
                  gconstpointer       user_data)
{
  if (g_file_test ("amsel.db", G_FILE_TEST_EXISTS))
    {
      g_remove ("amsel.db");
    }
  fixture->engine = alb_engine_new ("amsel.db");
}

static void
stresstest_feeds (AlbEngineFixture *fixture,
                  gconstpointer       user_data)
{
  SoupSession *session = soup_session_new ();
  GPtrArray *urls = g_ptr_array_new ();

  SoupMessage *msg;

  g_ptr_array_add (urls, "http://planet.gnome.org/atom.xml");
  g_ptr_array_add (urls, "http://www.tagesschau.de/xml/rss2");
  g_ptr_array_add (urls, "https://medium.com/feed/elementaryos/");

  for (int i = 0; i < urls->len; i++)
    {
      char *url = g_ptr_array_index (urls, i);

      g_print ("Processing URL: %s\n", url);
      msg = soup_message_new ("GET", url);
      soup_session_send_message (session, msg);

      test_feed (fixture, msg);
      g_object_unref (msg);
    }
}

static void
stresstest_teardown (AlbEngineFixture *fixture,
                     gconstpointer       user_data)
{
  g_object_unref (fixture->engine);
  if (g_file_test ("amsel.db", G_FILE_TEST_EXISTS))
    {
      g_remove ("amsel.db");
    }
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  if (g_test_slow ())
    g_test_add ("/stresstest/feeds", AlbEngineFixture, NULL, stresstest_setup, stresstest_feeds, stresstest_teardown);

  return g_test_run ();
}
