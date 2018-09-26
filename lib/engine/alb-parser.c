/* alb-parser.c
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

#define G_LOG_DOMAIN "alb-parser"

#include "alb-parser.h"

G_DEFINE_INTERFACE (AlbParser, alb_parser, G_TYPE_OBJECT)

static void
alb_parser_default_init (AlbParserInterface *iface)
{
  iface->parse = alb_parser_parse;
}

/**
 * alb_parser_parse:
 * @self: a #AlbParser
 * @request: a #AlbRequest
 *
 * Returns: (transfer full) (element-type AlbChannel):
 */
GPtrArray *
alb_parser_parse (AlbParser  *self,
                    AlbRequest *request)
{
  return ALB_PARSER_GET_IFACE (self)->parse (self, request);
}
