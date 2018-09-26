/* alb-cache.h
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
#include "alb-channel.h"
#include "alb-database.h"

G_BEGIN_DECLS

#define ALB_TYPE_CACHE (alb_cache_get_type())

G_DECLARE_FINAL_TYPE (AlbCache, alb_cache, ALB, CACHE, GObject)

AlbCache   *alb_cache_new          (AlbDatabase  *database);
AlbChannel *alb_cache_add_channel  (AlbCache     *self,
                                        AlbChannel   *channel,
                                        GError        **error);
GPtrArray    *alb_cache_get_channels (AlbCache     *self);
void          alb_cache_mark_read    (AlbCache     *self,
                                        AlbEntry     *entry);

#define ALB_CACHE_ERROR alb_cache_error_quark ()

/**
 * AlbCacheError:
 * @ALB_CACHE_ERROR_NO_ID: Gets returned when there is no id defined
 * for #AlbChannel
 *
 * Errors in conjunction with #AlbCache
 *
 */
typedef enum
{
  ALB_CACHE_ERROR_NO_ID,
} AlbCacheError;

GQuark alb_cache_error_quark (void);

G_END_DECLS
