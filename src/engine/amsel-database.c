#include "amsel-database.h"

G_DEFINE_INTERFACE (AmselDatabase, amsel_database, G_TYPE_OBJECT)

static void
amsel_database_default_init (AmselDatabaseInterface *iface)
{
  iface->save_channel = amsel_database_save_channel;
  iface->get_channels = amsel_database_get_channels;
}

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

GQuark
amsel_database_error_quark ()
{
  return g_quark_from_static_string ("amsel-database-error-quark");
}
