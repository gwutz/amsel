#include "alb-parser-rss.h"
#include "alb-parser.h"
#include "alb-channel.h"
#include <libxml/parser.h>
#include "alb-date-parser.h"
#include "alb-debug.h"

// Method announcement
static void alb_parser_rss_parse_item (AlbParser *parser,
                                         AlbEntry  *item,
                                         xmlDocPtr    doc,
                                         xmlNodePtr   cur);
// Method announcement

struct _AlbParserRss
{
  AlbParserXml parent_instance;
};

G_DEFINE_TYPE (AlbParserRss, alb_parser_rss, ALB_TYPE_PARSER_XML)

static void
_channel_title (AlbParser  *parser,
                AlbChannel *channel,
                xmlDocPtr     doc,
                xmlNodePtr    cur)
{
  AM_TRACE_MSG ("%s", "Parse Channel Title");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  alb_channel_set_title (channel, str);
  xmlFree (str);
}

static void
_channel_description (AlbParser  *parser,
                      AlbChannel *channel,
                      xmlDocPtr     doc,
                      xmlNodePtr    cur)
{
  AM_TRACE_MSG ("%s", "Parse Channel Description");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  alb_channel_set_description (channel, str);
  xmlFree (str);
}

static void
_channel_link (AlbParser  *parser,
               AlbChannel *channel,
               xmlDocPtr     doc,
               xmlNodePtr    cur)
{
  AM_TRACE_MSG ("%s", "Parse Channel Link");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  alb_channel_set_id (channel, str);
  xmlFree (str);
}

static void
_channel_item (AlbParser  *parser,
               AlbChannel *channel,
               xmlDocPtr     doc,
               xmlNodePtr    cur)
{
  AM_TRACE_MSG ("%s", "Parse Channel Item");

  AlbEntry *item = alb_entry_new ();
  alb_parser_rss_parse_item (parser, item, doc, cur->xmlChildrenNode);
  if (alb_entry_get_updated (item) == NULL) {
    g_autoptr(GDateTime) now = g_date_time_new_now_local ();
    alb_entry_set_updated_datetime (item, now);
  }
  alb_channel_add_entry (channel, item);
}

static void
_channel_image (AlbParser  *parser,
                AlbChannel *channel,
                xmlDocPtr     doc,
                xmlNodePtr    cur)
{
  AM_TRACE_MSG ("%s", "Parse Channel Image");

  cur = cur->xmlChildrenNode;

  while (cur)
    {
      if (!xmlStrcmp (cur->name, BAD_CAST "url"))
        {
          char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
          alb_channel_set_icon (channel, str);
          xmlFree (str);
        }
      cur = cur->next;
    }
}

static void
_item_title (AlbParser *parser,
             AlbEntry  *entry,
             xmlDocPtr    doc,
             xmlNodePtr   cur)
{
  AM_TRACE_MSG ("%s", "Parse Item Title");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  alb_entry_set_title (entry, str);
  xmlFree (str);
}

static void
_item_description (AlbParser *parser,
                   AlbEntry  *entry,
                   xmlDocPtr    doc,
                   xmlNodePtr   cur)
{
  AM_TRACE_MSG ("%s", "Parse Item Description");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  alb_entry_set_content (entry, str);
  xmlFree (str);
}

static void
_item_guid (AlbParser *parser,
            AlbEntry  *entry,
            xmlDocPtr    doc,
            xmlNodePtr   cur)
{
  AM_TRACE_MSG ("%s", "Parse Item GUID");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  alb_entry_set_id (entry, str);
  xmlFree (str);
}

static void
_item_pubDate (AlbParser *parser,
               AlbEntry  *entry,
               xmlDocPtr    doc,
               xmlNodePtr   cur)
{
  AM_TRACE_MSG ("%s", "Parse Item PubDate");
  char *rfc822 = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  g_autoptr (GDateTime) pubDate = alb_date_parser_parse_RFC822 (rfc822);
  alb_entry_set_updated_datetime (entry, pubDate);
  xmlFree (rfc822);
}

static void
_item_author (AlbParser *parser,
              AlbEntry  *entry,
              xmlDocPtr    doc,
              xmlNodePtr   cur)
{
  AM_TRACE_MSG ("%s", "Parse Item Author");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  alb_entry_set_author (entry, str);
  xmlFree (str);
}

static void
alb_parser_rss_init (AlbParserRss *self)
{
  AlbParserXml *xmlparser = ALB_PARSER_XML (self);
  alb_parser_xml_add_channel_handler (xmlparser, "title", _channel_title);
  alb_parser_xml_add_channel_handler (xmlparser, "description", _channel_description);
  alb_parser_xml_add_channel_handler (xmlparser, "link", _channel_link);
  alb_parser_xml_add_channel_handler (xmlparser, "item", _channel_item);
  alb_parser_xml_add_channel_handler (xmlparser, "image", _channel_image);

  alb_parser_xml_add_item_handler (xmlparser, "title", _item_title);
  alb_parser_xml_add_item_handler (xmlparser, "description", _item_description);
  alb_parser_xml_add_item_handler (xmlparser, "guid", _item_guid);
  alb_parser_xml_add_item_handler (xmlparser, "pubDate", _item_pubDate);
  alb_parser_xml_add_item_handler (xmlparser, "author", _item_author);
}

static void
alb_parser_rss_parse_item (AlbParser *parser,
                             AlbEntry  *item,
                             xmlDocPtr    doc,
                             xmlNodePtr   cur)
{
  AlbParserXml *xmlparser = ALB_PARSER_XML (parser);

  while (cur)
    {

      alb_parser_xml_handle_item (xmlparser, (const char *)cur->name, item, doc, cur);

      cur = cur->next;
    }
}

static void
alb_parser_rss_parse_channel (AlbParser  *parser,
                                AlbChannel *channel,
                                xmlDocPtr     doc,
                                xmlNodePtr    cur)
{
  AlbParserXml *xmlparser = ALB_PARSER_XML (parser);

  while (cur)
    {
      if (cur->type != XML_ELEMENT_NODE || cur->name == NULL) {
        cur = cur->next;
        continue;
      }

      if (cur->ns) {
        cur = cur->next;
        continue;
      }

      alb_parser_xml_handle_channel (xmlparser, (const char*)cur->name, channel, doc, cur);

      cur = cur->next;
    }
}

static GPtrArray *
alb_parser_rss_internal_parse (AlbParserXml *xmlparser,
                                 gchar          *url,
                                 xmlDocPtr       doc,
                                 xmlNodePtr      node)
{
  AlbParser *parser = ALB_PARSER (xmlparser);
  GPtrArray *channels = g_ptr_array_new ();
  xmlNodePtr cur = node;

  if (cur && !xmlStrcmp (cur->name, BAD_CAST "rss")) {
    cur = cur->xmlChildrenNode;
  }

  while (cur)
    {
      if (!xmlStrcmp (cur->name, BAD_CAST "channel")) {
        AlbChannel *channel = alb_channel_new ();
        alb_channel_set_source (channel, url);
        alb_parser_rss_parse_channel (parser, channel, doc, cur->xmlChildrenNode);
        g_ptr_array_add (channels, channel);
      }
      cur = cur->next;
    }

  return channels;
}

AlbParserRss *
alb_parser_rss_new (void)
{
  return g_object_new (ALB_TYPE_PARSER_RSS, NULL);
}

static void
alb_parser_rss_finalize (GObject *object)
{
  /* AlbParserRss *self = (AlbParserRss *)object; */

  G_OBJECT_CLASS (alb_parser_rss_parent_class)->finalize (object);
}

static void
alb_parser_rss_class_init (AlbParserRssClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  AlbParserXmlClass *xml_class = ALB_PARSER_XML_CLASS (klass);

  object_class->finalize = alb_parser_rss_finalize;

  xml_class->parse_xml = alb_parser_rss_internal_parse;
}

