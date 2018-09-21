#include "amsel-parser-xml.h"

typedef struct
{
  GHashTable *channel_handler;
  GHashTable *item_handler;
} AmselParserXmlPrivate;

static void amsel_parser_xml_parser_iface_init (AmselParserInterface *iface);

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (AmselParserXml, amsel_parser_xml, G_TYPE_OBJECT,
                                  G_ADD_PRIVATE (AmselParserXml)
                                  G_IMPLEMENT_INTERFACE (AMSEL_TYPE_PARSER,
                                                         amsel_parser_xml_parser_iface_init))

static void
amsel_parser_xml_finalize (GObject *object)
{
  AmselParserXml *self = (AmselParserXml *)object;
  AmselParserXmlPrivate *priv = amsel_parser_xml_get_instance_private (self);

  g_clear_pointer (&priv->channel_handler, g_hash_table_unref);
  g_clear_pointer (&priv->item_handler, g_hash_table_unref);

  G_OBJECT_CLASS (amsel_parser_xml_parent_class)->finalize (object);
}

static void
amsel_parser_xml_class_init (AmselParserXmlClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_parser_xml_finalize;
}

static void
amsel_parser_xml_init (AmselParserXml *self)
{
  AmselParserXmlPrivate *priv = amsel_parser_xml_get_instance_private (self);

  priv->channel_handler = g_hash_table_new (g_str_hash, g_str_equal);
  priv->item_handler = g_hash_table_new (g_str_hash, g_str_equal);
}

static GPtrArray *
amsel_parser_xml_parse (AmselParser  *parser,
                        AmselRequest *request)
{
  g_return_val_if_fail (AMSEL_IS_PARSER (parser), NULL);
  AmselParserXml *self = AMSEL_PARSER_XML (parser);

  xmlDocPtr doc;
  GPtrArray *channels;

  doc = xmlParseMemory (amsel_request_get_data (request),
                        amsel_request_get_size (request));

  xmlNodePtr root = xmlDocGetRootElement (doc);

  channels = AMSEL_PARSER_XML_GET_CLASS (self)->parse_xml (self, amsel_request_get_url (request), doc, root);

  xmlFreeDoc (doc);

  return channels;
}

static void
amsel_parser_xml_parser_iface_init (AmselParserInterface *iface)
{
  iface->parse = amsel_parser_xml_parse;
}

void
amsel_parser_xml_add_channel_handler (AmselParserXml  *self,
                                      char            *name,
                                      process_channel  handler)
{
  g_return_if_fail (AMSEL_IS_PARSER_XML (self));
  AmselParserXmlPrivate *priv = amsel_parser_xml_get_instance_private (self);

  g_hash_table_insert (priv->channel_handler, name, handler);
}

void
amsel_parser_xml_add_item_handler (AmselParserXml *self,
                                   char           *name,
                                   process_item    handler)
{
  g_return_if_fail (AMSEL_IS_PARSER_XML (self));
  AmselParserXmlPrivate *priv = amsel_parser_xml_get_instance_private (self);

  g_hash_table_insert (priv->item_handler, name, handler);
}

void
amsel_parser_xml_handle_channel (AmselParserXml *self,
                                 const char     *name,
                                 AmselChannel   *channel,
                                 xmlDocPtr       doc,
                                 xmlNodePtr      node)
{
  AmselParserXmlPrivate *priv = amsel_parser_xml_get_instance_private (self);

  process_channel handler = g_hash_table_lookup (priv->channel_handler, name);
  if (handler)
    handler (AMSEL_PARSER (self), channel, doc, node);
}

void
amsel_parser_xml_handle_item (AmselParserXml *self,
                              const char     *name,
                              AmselEntry     *entry,
                              xmlDocPtr       doc,
                              xmlNodePtr      node)
{
  AmselParserXmlPrivate *priv = amsel_parser_xml_get_instance_private (self);

  process_item handler = g_hash_table_lookup (priv->item_handler, name);
  if (handler)
    handler (AMSEL_PARSER (self), entry, doc, node);
}
