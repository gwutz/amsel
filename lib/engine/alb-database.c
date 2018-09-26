/* alb-database.c
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

#define G_LOG_DOMAIN "alb-database"

#include "alb-database.h"

G_DEFINE_INTERFACE (AlbDatabase, alb_database, G_TYPE_OBJECT)

static void
alb_database_default_init (AlbDatabaseInterface *iface)
{
  iface->save_channel = alb_database_save_channel;
  iface->save_entry = alb_database_save_entry;
  iface->get_channels = alb_database_get_channels;
}

/**
 * alb_database_get_channels:
 * @self: a #AlbDatabase
 *
 * Returns: (element-type AlbChannel) (transfer full): returns the saved #AlbChannels
 */
GPtrArray *
alb_database_get_channels (AlbDatabase *self)
{
  return ALB_DATABASE_GET_IFACE (self)->get_channels (self);
}

void
alb_database_save_channel (AlbDatabase  *self,
                             AlbChannel   *channel,
                             GError        **error)
{
  ALB_DATABASE_GET_IFACE (self)->save_channel (self, channel, error);
}

void
alb_database_save_entry (AlbDatabase  *self,
                           AlbChannel   *channel,
                           AlbEntry     *entry,
                           GError        **error)
{
  ALB_DATABASE_GET_IFACE (self)->save_entry (self, channel, entry, error);
}

void
alb_database_set_read (AlbDatabase  *self,
                         AlbEntry     *entry,
                         GError        **error)
{
  ALB_DATABASE_GET_IFACE (self)->set_read (self, entry, error);
}


GQuark
alb_database_error_quark ()
{
  return g_quark_from_static_string ("amsel-database-error-quark");
}
