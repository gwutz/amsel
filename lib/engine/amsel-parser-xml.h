#pragma once

#include <glib-object.h>
#include <libxml/parser.h>
#include "amsel-channel.h"
#include "amsel-entry.h"
#include "amsel-parser.h"

G_BEGIN_DECLS

typedef void (*process_channel) (AmselParser  *parser,
                                 AmselChannel *channel,
                                 xmlDocPtr     doc,
                                 xmlNodePtr    node);

typedef void (*process_item) (AmselParser *parser,
                              AmselEntry  *entry,
                              xmlDocPtr    doc,
                              xmlNodePtr   node);


#define AMSEL_TYPE_PARSER_XML (amsel_parser_xml_get_type())

G_DECLARE_DERIVABLE_TYPE (AmselParserXml, amsel_parser_xml, AMSEL, PARSER_XML, GObject)

/**
 * AmselParserXmlClass:
 * @parent_class: the parent class
 * @parse_xml: parse xml input data to #AmselChannel data
 */
struct _AmselParserXmlClass
{
  GObjectClass parent_class;

  GPtrArray * (*parse_xml) (AmselParserXml *xmlparser,
                            xmlDocPtr       doc,
                            xmlNodePtr      node);
};

AmselParserXml *amsel_parser_xml_new                 (void);
void            amsel_parser_xml_add_channel_handler (AmselParserXml  *self,
                                                      char            *name,
                                                      process_channel  handler);
void            amsel_parser_xml_add_item_handler    (AmselParserXml  *self,
                                                      char            *name,
                                                      process_item     handler);
void            amsel_parser_xml_handle_channel      (AmselParserXml  *self,
                                                      const char      *name,
                                                      AmselChannel    *channel,
                                                      xmlDocPtr        doc,
                                                      xmlNodePtr       node);
void            amsel_parser_xml_handle_item         (AmselParserXml  *self,
                                                      const char      *name,
                                                      AmselEntry      *entry,
                                                      xmlDocPtr        doc,
                                                      xmlNodePtr       node);

G_END_DECLS
