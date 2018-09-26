/* alb-request.h
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

G_BEGIN_DECLS

#define ALB_TYPE_REQUEST (alb_request_get_type ())

typedef struct _AlbRequest AlbRequest;


/**
 * AlbRequestType:
 * @ALB_REQUEST_TYPE_UNDECIDED: undefined input data
 * @ALB_REQUEST_TYPE_RSS: rss input data
 * @ALB_REQUEST_TYPE_ATOM: atom input data
 */
typedef enum {
  ALB_REQUEST_TYPE_UNDECIDED,
  ALB_REQUEST_TYPE_RSS,
  ALB_REQUEST_TYPE_ATOM,
} AlbRequestType;

typedef struct _AlbRequest AlbRequest;

GType             alb_request_get_type         (void) G_GNUC_CONST;
AlbRequest     *alb_request_new              (const char       *data,
                                                  gsize             size,
                                                  gchar            *url);
AlbRequest     *alb_request_copy             (AlbRequest     *self);
void              alb_request_set_request_type (AlbRequest     *self,
                                                  AlbRequestType  type);
char             *alb_request_get_data         (AlbRequest     *self);
gsize             alb_request_get_size         (AlbRequest     *self);
char             *alb_request_get_url          (AlbRequest     *self);
AlbRequestType  alb_request_get_request_type (AlbRequest     *self);
void              alb_request_free             (AlbRequest     *self);

G_END_DECLS
