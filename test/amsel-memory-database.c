#include "amsel-memory-database.h"
#include "alb.h"
#include "amsel-debug.h"

struct _AmselMemoryDatabase
{
  GObject parent_instance;

  int counter;
  gboolean set_error;
};

static void amsel_memory_database_db_init (AmselDatabaseInterface *iface);
G_DEFINE_TYPE_WITH_CODE (AmselMemoryDatabase, amsel_memory_database, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (AMSEL_TYPE_DATABASE,
                                                amsel_memory_database_db_init))

AmselMemoryDatabase *
amsel_memory_database_new (void)
{
  return g_object_new (AMSEL_TYPE_MEMORY_DATABASE, NULL);
}

static void
amsel_memory_database_finalize (GObject *object)
{
  G_OBJECT_CLASS (amsel_memory_database_parent_class)->finalize (object);
}

static void
amsel_memory_database_class_init (AmselMemoryDatabaseClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_memory_database_finalize;
}

static void
amsel_memory_database_init (AmselMemoryDatabase *self)
{
  self->counter = 0;
  self->set_error = FALSE;
}

static void
amsel_memory_database_save_channel (AmselDatabase  *db,
                                    AmselChannel   *channel,
                                    GError        **error)
{
  AmselMemoryDatabase *self = AMSEL_MEMORY_DATABASE (db);

  if (self->set_error == TRUE) {
    g_set_error (error, AMSEL_DATABASE_ERROR, AMSEL_DATABASE_ERROR_INSERT, "Got an error");
  }

  self->counter++;
}

static GPtrArray *
amsel_memory_database_get_channels (AmselDatabase *db)
{
  return NULL;
}

static void
amsel_memory_database_db_init (AmselDatabaseInterface *iface)
{
  iface->get_channels = amsel_memory_database_get_channels;
  iface->save_channel = amsel_memory_database_save_channel;
}

int
amsel_memory_database_get_counter (AmselMemoryDatabase *self)
{
  return self->counter;
}

void
amsel_memory_database_set_error (AmselMemoryDatabase *self,
                                 gboolean             set_error)
{
  self->set_error = set_error;
}
