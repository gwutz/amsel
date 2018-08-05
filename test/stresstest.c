#include <glib.h>
#include <libsoup/soup.h>
#include "amsel-engine.h"

typedef struct {
  AmselEngine *engine;
} AmselEngineFixture;

static void
test_feed (AmselEngineFixture *fixture, SoupMessage *msg)
{
  GPtrArray *channels;
  AmselRequest *request;

  request = amsel_request_new (msg->response_body->data, strlen (msg->response_body->data));
  channels = amsel_engine_parse (fixture->engine, request);
  g_assert_nonnull (channels);
  g_assert_cmpint (channels->len, >, 0);
}

static void
stresstest_setup (AmselEngineFixture *fixture,
                  gconstpointer       user_data)
{
  fixture->engine = amsel_engine_new ();
}

static void
stresstest_feeds (AmselEngineFixture *fixture,
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
stresstest_teardown (AmselEngineFixture *fixture,
                     gconstpointer       user_data)
{
  g_object_unref (fixture->engine);
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add ("/stresstest/feeds", AmselEngineFixture, NULL, stresstest_setup, stresstest_feeds, stresstest_teardown);

  return g_test_run ();
}
