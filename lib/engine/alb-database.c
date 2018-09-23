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
