/* alb-parser-xml.h
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

#pragma once

#include <glib-object.h>
#include <libxml/parser.h>
#include "alb-channel.h"
#include "alb-entry.h"
#include "alb-parser.h"

G_BEGIN_DECLS

typedef void (*process_channel) (AlbParser  *parser,
                                 AlbChannel *channel,
                                 xmlDocPtr     doc,
                                 xmlNodePtr    node);

typedef void (*process_item) (AlbParser *parser,
                              AlbEntry  *entry,
                              xmlDocPtr    doc,
                              xmlNodePtr   node);


#define ALB_TYPE_PARSER_XML (alb_parser_xml_get_type())

G_DECLARE_DERIVABLE_TYPE (AlbParserXml, alb_parser_xml, ALB, PARSER_XML, GObject)

/**
 * AlbParserXmlClass:
 * @parent_class: the parent class
 * @parse_xml: parse xml input data to #AlbChannel data
 */
struct _AlbParserXmlClass
{
  GObjectClass parent_class;

  GPtrArray * (*parse_xml) (AlbParserXml *xmlparser,
                            gchar          *url,
                            xmlDocPtr       doc,
                            xmlNodePtr      node);
};

AlbParserXml *alb_parser_xml_new                 (void);
void          alb_parser_xml_add_channel_handler (AlbParserXml    *self,
                                                  char            *name,
                                                  process_channel  handler);
void          alb_parser_xml_add_item_handler    (AlbParserXml    *self,
                                                  char            *name,
                                                  process_item     handler);
void          alb_parser_xml_handle_channel      (AlbParserXml    *self,
                                                  const char      *name,
                                                  AlbChannel      *channel,
                                                  xmlDocPtr        doc,
                                                  xmlNodePtr       node);
void          alb_parser_xml_handle_item         (AlbParserXml    *self,
                                                  const char      *name,
                                                  AlbEntry        *entry,
                                                  xmlDocPtr        doc,
                                                  xmlNodePtr       node);

G_END_DECLS
