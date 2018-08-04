#include "engine/amsel-parser-rss.h"
#include "engine/amsel-parser.h"
#include "model/amsel-channel.h"
#include <libxml/parser.h>
#include "utils/amsel-date-parser.h"

typedef void (*process_node) (AmselParser *parser,
                              xmlNodePtr   node);

struct _AmselParserRss
{
  GObject parent_instance;
};

static void amsel_parser_iface_init (AmselParserInterface *iface);

G_DEFINE_TYPE_WITH_CODE (AmselParserRss, amsel_parser_rss, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (AMSEL_TYPE_PARSER,
                                                amsel_parser_iface_init))

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

AmselParserRss *
amsel_parser_rss_new (void)
{
  return g_object_new (AMSEL_TYPE_PARSER_RSS, NULL);
}

static void
amsel_parser_rss_finalize (GObject *object)
{
  AmselParserRss *self = (AmselParserRss *)object;

  G_OBJECT_CLASS (amsel_parser_rss_parent_class)->finalize (object);
}

static void
amsel_parser_rss_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  AmselParserRss *self = AMSEL_PARSER_RSS (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_parser_rss_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  AmselParserRss *self = AMSEL_PARSER_RSS (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_parser_rss_class_init (AmselParserRssClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_parser_rss_finalize;
  object_class->get_property = amsel_parser_rss_get_property;
  object_class->set_property = amsel_parser_rss_set_property;
}

static void
amsel_parser_rss_init (AmselParserRss *self)
{
}

#define HANDLE_CHANNEL_NODE(namestr, parsefunc) \
    (!xmlStrcmp (cur->name, BAD_CAST #namestr)) { \
      char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);\
      parsefunc (channel, str); \
      xmlFree (str);\
    }

#define HANDLE_ITEM_NODE(namestr, parsefunc) \
    (!xmlStrcmp (cur->name, BAD_CAST #namestr)) { \
      char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);\
      parsefunc (item, str); \
      xmlFree (str);\
    }

static void
amsel_parser_rss_parse_item (AmselParser *parser,
                             AmselEntry  *item,
                             xmlDocPtr    doc,
                             xmlNodePtr   cur)
{
  while (cur)
    {
      if HANDLE_ITEM_NODE(title, amsel_entry_set_title)
      else if HANDLE_ITEM_NODE(description, amsel_entry_set_content)
      else if (!xmlStrcmp (cur->name, BAD_CAST "pubDate"))
        {
          char *rfc822 = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
          g_autoptr (GDateTime) pubDate = amsel_date_parser_parse_RFC822 (rfc822);
          amsel_entry_set_updated_datetime (item, pubDate);
          xmlFree (rfc822);
        }
      else if HANDLE_ITEM_NODE(author, amsel_entry_set_author)

      cur = cur->next;
    }
}

static void
amsel_parser_rss_parse_image (AmselParser  *parser,
                              AmselChannel *channel,
                              xmlDocPtr     doc,
                              xmlNodePtr    cur)
{
  while (cur)
    {
      if HANDLE_CHANNEL_NODE (url, amsel_channel_set_icon)
      cur = cur->next;
    }
}

static void
amsel_parser_rss_parse_channel (AmselParser  *parser,
                                AmselChannel *channel,
                                xmlDocPtr     doc,
                                xmlNodePtr    cur)
{
  while (cur)
    {
      if HANDLE_CHANNEL_NODE(title, amsel_channel_set_title)
      else if HANDLE_CHANNEL_NODE(description, amsel_channel_set_description)
      else if HANDLE_CHANNEL_NODE(link, amsel_channel_set_source)

      if (!xmlStrcmp (cur->name, BAD_CAST "item"))
        {
          AmselEntry *item = amsel_entry_new ();
          amsel_parser_rss_parse_item (parser, item, doc, cur->xmlChildrenNode);
          amsel_channel_add_entry (channel, item);
        }
      if (!xmlStrcmp (cur->name, BAD_CAST "image"))
        {
          amsel_parser_rss_parse_image (parser, channel, doc, cur->xmlChildrenNode);
        }

      cur = cur->next;
    }
}

static void
amsel_parser_rss_internal_parse (AmselParser *parser,
                                 GPtrArray   *channels,
                                 xmlDocPtr    doc,
                                 xmlNodePtr   node)
{
  xmlNodePtr cur = node;

  if (cur && !xmlStrcmp (cur->name, BAD_CAST "rss")) {
    cur = cur->xmlChildrenNode;
  }

  while (cur)
    {
      if (!xmlStrcmp (cur->name, BAD_CAST "channel")) {
        AmselChannel *channel = amsel_channel_new ("");
        amsel_parser_rss_parse_channel (parser, channel, doc, cur->xmlChildrenNode);
        g_ptr_array_add (channels, channel);
      }
      cur = cur->next;
    }
}

GPtrArray *
amsel_parser_rss_parse (AmselParser  *parser,
                        AmselRequest *request)
{
  g_return_val_if_fail (AMSEL_IS_PARSER (parser), NULL);

  xmlDocPtr doc;
  GPtrArray *channels;

  channels = g_ptr_array_new_with_free_func (g_object_unref);

  doc = xmlParseMemory (amsel_request_get_data (request), amsel_request_get_size (request));

  xmlNodePtr root = xmlDocGetRootElement (doc);
  amsel_parser_rss_internal_parse (parser, channels, doc, root);
  xmlFreeDoc (doc);
  return channels;
}

static void
amsel_parser_iface_init (AmselParserInterface *iface)
{
  iface->parse = amsel_parser_rss_parse;
}
