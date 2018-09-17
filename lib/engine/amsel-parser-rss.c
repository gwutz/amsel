#include "amsel-parser-rss.h"
#include "amsel-parser.h"
#include "amsel-channel.h"
#include <libxml/parser.h>
#include "amsel-date-parser.h"
#include "amsel-debug.h"

// Method announcement
static void amsel_parser_rss_parse_item (AmselParser *parser,
                                         AmselEntry  *item,
                                         xmlDocPtr    doc,
                                         xmlNodePtr   cur);
// Method announcement

struct _AmselParserRss
{
  AmselParserXml parent_instance;
};

G_DEFINE_TYPE (AmselParserRss, amsel_parser_rss, AMSEL_TYPE_PARSER_XML)

static void
_channel_title (AmselParser  *parser,
                AmselChannel *channel,
                xmlDocPtr     doc,
                xmlNodePtr    cur)
{
  AM_TRACE_MSG ("%s", "Parse Channel Title");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  amsel_channel_set_title (channel, str);
  xmlFree (str);
}

static void
_channel_description (AmselParser  *parser,
                      AmselChannel *channel,
                      xmlDocPtr     doc,
                      xmlNodePtr    cur)
{
  AM_TRACE_MSG ("%s", "Parse Channel Description");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  amsel_channel_set_description (channel, str);
  xmlFree (str);
}

static void
_channel_link (AmselParser  *parser,
               AmselChannel *channel,
               xmlDocPtr     doc,
               xmlNodePtr    cur)
{
  AM_TRACE_MSG ("%s", "Parse Channel Link");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  amsel_channel_set_source (channel, str);
  amsel_channel_set_id (channel, str);
  xmlFree (str);
}

static void
_channel_item (AmselParser  *parser,
               AmselChannel *channel,
               xmlDocPtr     doc,
               xmlNodePtr    cur)
{
  AM_TRACE_MSG ("%s", "Parse Channel Item");

  g_autoptr (AmselEntry) item = amsel_entry_new ();
  amsel_parser_rss_parse_item (parser, item, doc, cur->xmlChildrenNode);
  amsel_channel_add_entry (channel, item);
}

static void
_channel_image (AmselParser  *parser,
                AmselChannel *channel,
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
          amsel_channel_set_icon (channel, str);
          xmlFree (str);
        }
      cur = cur->next;
    }
}

static void
_item_title (AmselParser *parser,
             AmselEntry  *entry,
             xmlDocPtr    doc,
             xmlNodePtr   cur)
{
  AM_TRACE_MSG ("%s", "Parse Item Title");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  amsel_entry_set_title (entry, str);
  xmlFree (str);
}

static void
_item_description (AmselParser *parser,
                   AmselEntry  *entry,
                   xmlDocPtr    doc,
                   xmlNodePtr   cur)
{
  AM_TRACE_MSG ("%s", "Parse Item Description");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  amsel_entry_set_content (entry, str);
  xmlFree (str);
}

static void
_item_guid (AmselParser *parser,
            AmselEntry  *entry,
            xmlDocPtr    doc,
            xmlNodePtr   cur)
{
  AM_TRACE_MSG ("%s", "Parse Item GUID");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  amsel_entry_set_id (entry, str);
  xmlFree (str);
}

static void
_item_pubDate (AmselParser *parser,
               AmselEntry  *entry,
               xmlDocPtr    doc,
               xmlNodePtr   cur)
{
  AM_TRACE_MSG ("%s", "Parse Item PubDate");
  char *rfc822 = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  g_autoptr (GDateTime) pubDate = amsel_date_parser_parse_RFC822 (rfc822);
  amsel_entry_set_updated_datetime (entry, pubDate);
  xmlFree (rfc822);
}

static void
_item_author (AmselParser *parser,
              AmselEntry  *entry,
              xmlDocPtr    doc,
              xmlNodePtr   cur)
{
  AM_TRACE_MSG ("%s", "Parse Item Author");
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  amsel_entry_set_author (entry, str);
  xmlFree (str);
}

static void
amsel_parser_rss_init (AmselParserRss *self)
{
  AmselParserXml *xmlparser = AMSEL_PARSER_XML (self);
  amsel_parser_xml_add_channel_handler (xmlparser, "title", _channel_title);
  amsel_parser_xml_add_channel_handler (xmlparser, "description", _channel_description);
  amsel_parser_xml_add_channel_handler (xmlparser, "link", _channel_link);
  amsel_parser_xml_add_channel_handler (xmlparser, "item", _channel_item);
  amsel_parser_xml_add_channel_handler (xmlparser, "image", _channel_image);

  amsel_parser_xml_add_item_handler (xmlparser, "title", _item_title);
  amsel_parser_xml_add_item_handler (xmlparser, "description", _item_description);
  amsel_parser_xml_add_item_handler (xmlparser, "guid", _item_guid);
  amsel_parser_xml_add_item_handler (xmlparser, "pubDate", _item_pubDate);
  amsel_parser_xml_add_item_handler (xmlparser, "author", _item_author);
}

static void
amsel_parser_rss_parse_item (AmselParser *parser,
                             AmselEntry  *item,
                             xmlDocPtr    doc,
                             xmlNodePtr   cur)
{
  AmselParserXml *xmlparser = AMSEL_PARSER_XML (parser);

  while (cur)
    {

      amsel_parser_xml_handle_item (xmlparser, (const char *)cur->name, item, doc, cur);

      cur = cur->next;
    }
}

static void
amsel_parser_rss_parse_channel (AmselParser  *parser,
                                AmselChannel *channel,
                                xmlDocPtr     doc,
                                xmlNodePtr    cur)
{
  AmselParserXml *xmlparser = AMSEL_PARSER_XML (parser);

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

      amsel_parser_xml_handle_channel (xmlparser, (const char*)cur->name, channel, doc, cur);

      cur = cur->next;
    }
}

static GPtrArray *
amsel_parser_rss_internal_parse (AmselParserXml *xmlparser,
                                 xmlDocPtr    doc,
                                 xmlNodePtr   node)
{
  AmselParser *parser = AMSEL_PARSER (xmlparser);
  GPtrArray *channels = g_ptr_array_new ();
  xmlNodePtr cur = node;

  if (cur && !xmlStrcmp (cur->name, BAD_CAST "rss")) {
    cur = cur->xmlChildrenNode;
  }

  while (cur)
    {
      if (!xmlStrcmp (cur->name, BAD_CAST "channel")) {
        AmselChannel *channel = amsel_channel_new ();
        amsel_parser_rss_parse_channel (parser, channel, doc, cur->xmlChildrenNode);
        g_ptr_array_add (channels, channel);
      }
      cur = cur->next;
    }

  return channels;
}

AmselParserRss *
amsel_parser_rss_new (void)
{
  return g_object_new (AMSEL_TYPE_PARSER_RSS, NULL);
}

static void
amsel_parser_rss_finalize (GObject *object)
{
  /* AmselParserRss *self = (AmselParserRss *)object; */

  G_OBJECT_CLASS (amsel_parser_rss_parent_class)->finalize (object);
}

static void
amsel_parser_rss_class_init (AmselParserRssClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  AmselParserXmlClass *xml_class = AMSEL_PARSER_XML_CLASS (klass);

  object_class->finalize = amsel_parser_rss_finalize;

  xml_class->parse_xml = amsel_parser_rss_internal_parse;
}

