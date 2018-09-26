/* alb-parser-xml.c
 *
 * Copyright 2018 Günther Wagner <info@gunibert.de>
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#define G_LOG_DOMAIN "alb-parser-xml"

#include "alb-parser-xml.h"

typedef struct
{
  GHashTable *channel_handler;
  GHashTable *item_handler;
} AlbParserXmlPrivate;

static void alb_parser_xml_parser_iface_init (AlbParserInterface *iface);

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (AlbParserXml, alb_parser_xml, G_TYPE_OBJECT,
                                  G_ADD_PRIVATE (AlbParserXml)
                                  G_IMPLEMENT_INTERFACE (ALB_TYPE_PARSER,
                                                         alb_parser_xml_parser_iface_init))

static void
alb_parser_xml_finalize (GObject *object)
{
  AlbParserXml *self = (AlbParserXml *)object;
  AlbParserXmlPrivate *priv = alb_parser_xml_get_instance_private (self);

  g_clear_pointer (&priv->channel_handler, g_hash_table_unref);
  g_clear_pointer (&priv->item_handler, g_hash_table_unref);

  G_OBJECT_CLASS (alb_parser_xml_parent_class)->finalize (object);
}

static void
alb_parser_xml_class_init (AlbParserXmlClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = alb_parser_xml_finalize;
}

static void
alb_parser_xml_init (AlbParserXml *self)
{
  AlbParserXmlPrivate *priv = alb_parser_xml_get_instance_private (self);

  priv->channel_handler = g_hash_table_new (g_str_hash, g_str_equal);
  priv->item_handler = g_hash_table_new (g_str_hash, g_str_equal);
}

static GPtrArray *
alb_parser_xml_parse (AlbParser  *parser,
                        AlbRequest *request)
{
  g_return_val_if_fail (ALB_IS_PARSER (parser), NULL);
  AlbParserXml *self = ALB_PARSER_XML (parser);

  xmlDocPtr doc;
  GPtrArray *channels;

  doc = xmlParseMemory (alb_request_get_data (request),
                        alb_request_get_size (request));

  xmlNodePtr root = xmlDocGetRootElement (doc);

  channels = ALB_PARSER_XML_GET_CLASS (self)->parse_xml (self, alb_request_get_url (request), doc, root);

  xmlFreeDoc (doc);

  return channels;
}

static void
alb_parser_xml_parser_iface_init (AlbParserInterface *iface)
{
  iface->parse = alb_parser_xml_parse;
}

void
alb_parser_xml_add_channel_handler (AlbParserXml  *self,
                                      char            *name,
                                      process_channel  handler)
{
  g_return_if_fail (ALB_IS_PARSER_XML (self));
  AlbParserXmlPrivate *priv = alb_parser_xml_get_instance_private (self);

  g_hash_table_insert (priv->channel_handler, name, handler);
}

void
alb_parser_xml_add_item_handler (AlbParserXml *self,
                                   char           *name,
                                   process_item    handler)
{
  g_return_if_fail (ALB_IS_PARSER_XML (self));
  AlbParserXmlPrivate *priv = alb_parser_xml_get_instance_private (self);

  g_hash_table_insert (priv->item_handler, name, handler);
}

void
alb_parser_xml_handle_channel (AlbParserXml *self,
                                 const char     *name,
                                 AlbChannel   *channel,
                                 xmlDocPtr       doc,
                                 xmlNodePtr      node)
{
  AlbParserXmlPrivate *priv = alb_parser_xml_get_instance_private (self);

  process_channel handler = g_hash_table_lookup (priv->channel_handler, name);
  if (handler)
    handler (ALB_PARSER (self), channel, doc, node);
}

void
alb_parser_xml_handle_item (AlbParserXml *self,
                              const char     *name,
                              AlbEntry     *entry,
                              xmlDocPtr       doc,
                              xmlNodePtr      node)
{
  AlbParserXmlPrivate *priv = alb_parser_xml_get_instance_private (self);

  process_item handler = g_hash_table_lookup (priv->item_handler, name);
  if (handler)
    handler (ALB_PARSER (self), entry, doc, node);
}
