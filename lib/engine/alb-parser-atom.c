#include "alb-parser-atom.h"
#include "alb-parser.h"
#include <libxml/parser.h>
#include "alb-channel.h"

struct _AlbParserAtom
{
  AlbParserXml parent_instance;
};

G_DEFINE_TYPE (AlbParserAtom, alb_parser_atom, ALB_TYPE_PARSER_XML);

static void alb_parser_atom_parse_item (AlbParser *parser,
                                          AlbEntry  *item,
                                          xmlDocPtr    doc,
                                          xmlNodePtr   cur);

static void
_channel_id (AlbParser  *parser,
             AlbChannel *channel,
             xmlDocPtr     doc,
             xmlNodePtr    cur)
{
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  alb_channel_set_id (channel, str);
  xmlFree (str);
}

static void
_channel_title (AlbParser  *parser,
                AlbChannel *channel,
                xmlDocPtr     doc,
                xmlNodePtr    cur)
{
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  alb_channel_set_title (channel, str);
  xmlFree (str);
}

static void
_channel_link (AlbParser  *parser,
               AlbChannel *channel,
               xmlDocPtr     doc,
               xmlNodePtr    cur)
{
  xmlChar *href = xmlGetProp (cur, BAD_CAST "href");
  if (href) {
    xmlChar *rel = xmlGetProp (cur, BAD_CAST "rel");
    if (xmlStrEqual (rel, BAD_CAST "self"))
      alb_channel_set_source (channel, (const char *) href);
    xmlFree (href);
    xmlFree (rel);
  }
}

static void
_channel_entry (AlbParser  *parser,
                AlbChannel *channel,
                xmlDocPtr     doc,
                xmlNodePtr    cur)
{
  AlbEntry *item = alb_entry_new ();
  alb_parser_atom_parse_item (parser, item, doc, cur->xmlChildrenNode);
  alb_channel_add_entry (channel, item);
}

static void
_item_title (AlbParser *parser,
             AlbEntry    *item,
             xmlDocPtr    doc,
             xmlNodePtr   cur)
{
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  alb_entry_set_title (item, str);
  xmlFree (str);
}

static void
_item_content (AlbParser *parser,
               AlbEntry  *item,
               xmlDocPtr    doc,
               xmlNodePtr   cur)
{
  xmlBufferPtr buffer = xmlBufferCreate ();
  xmlNodeDump (buffer, doc, cur->xmlChildrenNode, 0, 1);
  alb_entry_set_content (item, (char *) buffer->content);
  xmlBufferFree (buffer);
}

static void
_item_id (AlbParser *parser,
          AlbEntry    *item,
          xmlDocPtr    doc,
          xmlNodePtr   cur)
{
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  alb_entry_set_id (item, str);
  xmlFree (str);
}

static void
_item_updated (AlbParser *parser,
               AlbEntry    *item,
               xmlDocPtr    doc,
               xmlNodePtr   cur)
{
  char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
  g_autoptr (GDateTime) updated = g_date_time_new_from_iso8601 (str, NULL);
  alb_entry_set_updated_datetime (item, updated);
  xmlFree (str);
}

static void
_item_author (AlbParser *parser,
              AlbEntry  *item,
              xmlDocPtr    doc,
              xmlNodePtr   cur)
{
  cur = cur->xmlChildrenNode;
  while (cur) {
    if (!xmlStrcmp (cur->name, BAD_CAST "name")) {
      char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
      alb_entry_set_author (item, str);
      xmlFree (str);
    }
    cur = cur->next;
  }
}

static void
alb_parser_atom_parse_item (AlbParser *parser,
                              AlbEntry  *item,
                              xmlDocPtr    doc,
                              xmlNodePtr   cur)
{
  AlbParserXml *xmlparser = ALB_PARSER_XML (parser);
  while (cur)
    {
      alb_parser_xml_handle_item (xmlparser, (const char *) cur->name, item, doc, cur);

      cur = cur->next;
    }
}

static void
alb_parser_atom_parse_channel (AlbParser  *parser,
                                 AlbChannel *channel,
                                 xmlDocPtr     doc,
                                 xmlNodePtr    cur)
{
  AlbParserXml *xmlparser = ALB_PARSER_XML (parser);

  while (cur) {
    alb_parser_xml_handle_channel (xmlparser, (const char *)cur->name, channel, doc, cur);

    cur = cur->next;
  }
}

static GPtrArray *
alb_parser_atom_internal_parse (AlbParserXml *xmlparser,
                                  gchar          *url,
                                  xmlDocPtr       doc,
                                  xmlNodePtr      node)
{
  AlbParser *parser = ALB_PARSER (xmlparser);
  GPtrArray *channels = g_ptr_array_new ();
  xmlNodePtr cur = node;

  while (cur)
    {
      AlbChannel *channel = alb_channel_new ();
      alb_parser_atom_parse_channel (parser, channel, doc, cur->xmlChildrenNode);
      g_ptr_array_add (channels, channel);
      cur = cur->next;
    }

  return channels;
}

AlbParserAtom *
alb_parser_atom_new (void)
{
  return g_object_new (ALB_TYPE_PARSER_ATOM, NULL);
}

static void
alb_parser_atom_finalize (GObject *object)
{
  /* AlbParserAtom *self = (AlbParserAtom *)object; */

  G_OBJECT_CLASS (alb_parser_atom_parent_class)->finalize (object);
}

static void
alb_parser_atom_class_init (AlbParserAtomClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  AlbParserXmlClass *xml_class = ALB_PARSER_XML_CLASS (klass);

  object_class->finalize = alb_parser_atom_finalize;

  xml_class->parse_xml = alb_parser_atom_internal_parse;
}

static void
alb_parser_atom_init (AlbParserAtom *self)
{
  AlbParserXml *xmlparser = ALB_PARSER_XML (self);
  alb_parser_xml_add_channel_handler (xmlparser, "id", _channel_id);
  alb_parser_xml_add_channel_handler (xmlparser, "title", _channel_title);
  alb_parser_xml_add_channel_handler (xmlparser, "link", _channel_link);
  alb_parser_xml_add_channel_handler (xmlparser, "entry", _channel_entry);

  alb_parser_xml_add_item_handler (xmlparser, "title", _item_title);
  alb_parser_xml_add_item_handler (xmlparser, "content", _item_content);
  alb_parser_xml_add_item_handler (xmlparser, "summary", _item_content);
  alb_parser_xml_add_item_handler (xmlparser, "id", _item_id);
  alb_parser_xml_add_item_handler (xmlparser, "updated", _item_updated);
  alb_parser_xml_add_item_handler (xmlparser, "author", _item_author);
}
