#include "amsel-sqlite-database.h"
#include "amsel-database.h"
#include <sqlite3.h>

struct _AmselSqliteDatabase
{
  GObject parent_instance;

  sqlite3 *db;
};

static void amsel_sqlite_database_db_interface_init (AmselDatabaseInterface *iface);
G_DEFINE_TYPE_WITH_CODE (AmselSqliteDatabase, amsel_sqlite_database, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (AMSEL_TYPE_DATABASE, amsel_sqlite_database_db_interface_init))

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

AmselSqliteDatabase *
amsel_sqlite_database_new (char *filename)
{
  AmselSqliteDatabase *self = g_object_new (AMSEL_TYPE_SQLITE_DATABASE, NULL);
  sqlite3_open (filename, &self->db);
  return self;
}

static void
amsel_sqlite_database_finalize (GObject *object)
{
  AmselSqliteDatabase *self = (AmselSqliteDatabase *)object;

  sqlite3_close (self->db);

  G_OBJECT_CLASS (amsel_sqlite_database_parent_class)->finalize (object);
}

static void
amsel_sqlite_database_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  AmselSqliteDatabase *self = AMSEL_SQLITE_DATABASE (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_sqlite_database_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  AmselSqliteDatabase *self = AMSEL_SQLITE_DATABASE (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_sqlite_database_class_init (AmselSqliteDatabaseClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_sqlite_database_finalize;
  object_class->get_property = amsel_sqlite_database_get_property;
  object_class->set_property = amsel_sqlite_database_set_property;
}

static void
amsel_sqlite_database_init (AmselSqliteDatabase *self)
{
}

static void
amsel_sqlite_database_save_channel (AmselDatabase  *db,
                                    AmselChannel   *channel,
                                    GError        **error)
{

}

static GPtrArray *
amsel_sqlite_database_get_channels (AmselDatabase *db)
{
  return NULL;
}

static void
amsel_sqlite_database_db_interface_init (AmselDatabaseInterface *iface)
{
  iface->save_channel = amsel_sqlite_database_save_channel;
  iface->get_channels = amsel_sqlite_database_get_channels;
}
