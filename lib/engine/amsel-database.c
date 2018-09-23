#include "amsel-database.h"

G_DEFINE_INTERFACE (AmselDatabase, amsel_database, G_TYPE_OBJECT)

static void
amsel_database_default_init (AmselDatabaseInterface *iface)
{
  iface->save_channel = amsel_database_save_channel;
  iface->save_entry = amsel_database_save_entry;
  iface->get_channels = amsel_database_get_channels;
}

/**
 * amsel_database_get_channels:
 * @self: a #AmselDatabase
 *
 * Returns: (element-type AmselChannel) (transfer full): returns the saved #AmselChannels
 */
GPtrArray *
amsel_database_get_channels (AmselDatabase *self)
{
  return AMSEL_DATABASE_GET_IFACE (self)->get_channels (self);
}

void
amsel_database_save_channel (AmselDatabase  *self,
                             AmselChannel   *channel,
                             GError        **error)
{
  AMSEL_DATABASE_GET_IFACE (self)->save_channel (self, channel, error);
}

void
amsel_database_save_entry (AmselDatabase  *self,
                           AmselChannel   *channel,
                           AmselEntry     *entry,
                           GError        **error)
{
  AMSEL_DATABASE_GET_IFACE (self)->save_entry (self, channel, entry, error);
}

void
amsel_database_set_read (AmselDatabase  *self,
                         AmselEntry     *entry,
                         GError        **error)
{
  AMSEL_DATABASE_GET_IFACE (self)->set_read (self, entry, error);
}


GQuark
amsel_database_error_quark ()
{
  return g_quark_from_static_string ("amsel-database-error-quark");
}
