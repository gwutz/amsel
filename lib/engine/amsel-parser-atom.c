#include "amsel-parser-atom.h"
#include "amsel-parser.h"
#include <libxml/parser.h>
#include "amsel-channel.h"

struct _AmselParserAtom
{
  AmselParserXml parent_instance;
};

G_DEFINE_TYPE (AmselParserAtom, amsel_parser_atom, AMSEL_TYPE_PARSER_XML);

static void amsel_parser_atom_parse_item (AmselParser *parser,
                                          AmselEntry  *item,
                                          xmlDocPtr    doc,
                                          xmlNodePtr   cur);

static void
_channel_id (AmselParser  *parser,
             AmselChannel *channel,
             xmlDocPtr     doc,
             xmlNodePtr    cur)
{
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  amsel_channel_set_id (channel, str);
  xmlFree (str);
}

static void
_channel_title (AmselParser  *parser,
                AmselChannel *channel,
                xmlDocPtr     doc,
                xmlNodePtr    cur)
{
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  amsel_channel_set_title (channel, str);
  xmlFree (str);
}

static void
_channel_link (AmselParser  *parser,
               AmselChannel *channel,
               xmlDocPtr     doc,
               xmlNodePtr    cur)
{
  xmlChar *href = xmlGetProp (cur, BAD_CAST "href");
  if (href) {
    xmlChar *rel = xmlGetProp (cur, BAD_CAST "rel");
    if (xmlStrEqual (rel, BAD_CAST "self"))
      amsel_channel_set_source (channel, (const char *) href);
    xmlFree (href);
    xmlFree (rel);
  }
}

static void
_channel_entry (AmselParser  *parser,
                AmselChannel *channel,
                xmlDocPtr     doc,
                xmlNodePtr    cur)
{
  g_autoptr (AmselEntry) item = amsel_entry_new ();
  amsel_parser_atom_parse_item (parser, item, doc, cur->xmlChildrenNode);
  amsel_channel_add_entry (channel, item);
}

static void
_item_title (AmselParser *parser,
             AmselEntry  *item,
             xmlDocPtr    doc,
             xmlNodePtr   cur)
{
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  amsel_entry_set_title (item, str);
  xmlFree (str);
}

static void
_item_content (AmselParser *parser,
               AmselEntry  *item,
               xmlDocPtr    doc,
               xmlNodePtr   cur)
{
  xmlBufferPtr buffer = xmlBufferCreate ();
  xmlNodeDump (buffer, doc, cur->xmlChildrenNode, 0, 1);
  amsel_entry_set_content (item, (char *) buffer->content);
  xmlBufferFree (buffer);
}

static void
_item_id (AmselParser *parser,
          AmselEntry  *item,
          xmlDocPtr    doc,
          xmlNodePtr   cur)
{
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  amsel_entry_set_id (item, str);
  xmlFree (str);
}

static void
_item_updated (AmselParser *parser,
               AmselEntry  *item,
               xmlDocPtr    doc,
               xmlNodePtr   cur)
{
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  g_autoptr (GDateTime) updated = g_date_time_new_from_iso8601 (str, NULL);
  amsel_entry_set_updated_datetime (item, updated);
  xmlFree (str);
}

static void
_item_author (AmselParser *parser,
              AmselEntry  *item,
              xmlDocPtr    doc,
              xmlNodePtr   cur)
{
  cur = cur->xmlChildrenNode;
  while (cur) {
    if (!xmlStrcmp (cur->name, BAD_CAST "name")) {
      char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
      amsel_entry_set_author (item, str);
      xmlFree (str);
    }
    cur = cur->next;
  }
}

static void
amsel_parser_atom_parse_item (AmselParser *parser,
                              AmselEntry  *item,
                              xmlDocPtr    doc,
                              xmlNodePtr   cur)
{
  AmselParserXml *xmlparser = AMSEL_PARSER_XML (parser);
  while (cur)
    {
      amsel_parser_xml_handle_item (xmlparser, (const char *) cur->name, item, doc, cur);

      cur = cur->next;
    }
}

static void
amsel_parser_atom_parse_channel (AmselParser  *parser,
                                 AmselChannel *channel,
                                 xmlDocPtr     doc,
                                 xmlNodePtr    cur)
{
  AmselParserXml *xmlparser = AMSEL_PARSER_XML (parser);

  while (cur) {
    amsel_parser_xml_handle_channel (xmlparser, (const char *)cur->name, channel, doc, cur);

    cur = cur->next;
  }
}

static GPtrArray *
amsel_parser_atom_internal_parse (AmselParserXml *xmlparser,
                                  xmlDocPtr    doc,
                                  xmlNodePtr   node)
{
  AmselParser *parser = AMSEL_PARSER (xmlparser);
  GPtrArray *channels = g_ptr_array_new ();
  xmlNodePtr cur = node;

  while (cur)
    {
      AmselChannel *channel = amsel_channel_new ();
      amsel_parser_atom_parse_channel (parser, channel, doc, cur->xmlChildrenNode);
      g_ptr_array_add (channels, channel);
      cur = cur->next;
    }

  return channels;
}

AmselParserAtom *
amsel_parser_atom_new (void)
{
  return g_object_new (AMSEL_TYPE_PARSER_ATOM, NULL);
}

static void
amsel_parser_atom_finalize (GObject *object)
{
  /* AmselParserAtom *self = (AmselParserAtom *)object; */

  G_OBJECT_CLASS (amsel_parser_atom_parent_class)->finalize (object);
}

static void
amsel_parser_atom_class_init (AmselParserAtomClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  AmselParserXmlClass *xml_class = AMSEL_PARSER_XML_CLASS (klass);

  object_class->finalize = amsel_parser_atom_finalize;

  xml_class->parse_xml = amsel_parser_atom_internal_parse;
}

static void
amsel_parser_atom_init (AmselParserAtom *self)
{
  AmselParserXml *xmlparser = AMSEL_PARSER_XML (self);
  amsel_parser_xml_add_channel_handler (xmlparser, "id", _channel_id);
  amsel_parser_xml_add_channel_handler (xmlparser, "title", _channel_title);
  amsel_parser_xml_add_channel_handler (xmlparser, "link", _channel_link);
  amsel_parser_xml_add_channel_handler (xmlparser, "entry", _channel_entry);

  amsel_parser_xml_add_item_handler (xmlparser, "title", _item_title);
  amsel_parser_xml_add_item_handler (xmlparser, "content", _item_content);
  amsel_parser_xml_add_item_handler (xmlparser, "summary", _item_content);
  amsel_parser_xml_add_item_handler (xmlparser, "id", _item_id);
  amsel_parser_xml_add_item_handler (xmlparser, "updated", _item_updated);
  amsel_parser_xml_add_item_handler (xmlparser, "author", _item_author);
}
