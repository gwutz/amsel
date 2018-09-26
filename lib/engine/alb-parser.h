/* alb-parser.h
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

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "alb-request.h"

G_BEGIN_DECLS

#define ALB_TYPE_PARSER (alb_parser_get_type ())

G_DECLARE_INTERFACE (AlbParser, alb_parser, ALB, PARSER, GObject)

/**
 * AlbParserInterface:
 * @parent: the parent interface
 * @parse: take the neutral input data and parse them to #AmselChannel data
 */
struct _AlbParserInterface
{
  GTypeInterface parent;

  GPtrArray * (*parse) (AlbParser  *self,
                        AlbRequest *request);
};

GPtrArray *alb_parser_parse (AlbParser  *self,
                               AlbRequest *request);

G_END_DECLS
