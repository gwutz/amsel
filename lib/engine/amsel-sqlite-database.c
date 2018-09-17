#define G_LOG_DOMAIN "amsel-sqlite-database"
#include "amsel-sqlite-database.h"
#include "amsel-database.h"
#include "amsel-sqlite-helper.h"
#include "amsel-debug.h"
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
  PROP_DATABASE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

/* Internal methods */
static void amsel_sqlite_database_create_tables (AmselSqliteDatabase *self);

/* Cleanup definitions */
G_DEFINE_AUTOPTR_CLEANUP_FUNC (sqlite3_stmt, sqlite3_finalize)

AmselSqliteDatabase *
amsel_sqlite_database_new (char *filename)
{
  sqlite3 *db;
  g_print ("Current Dir: %s\n", g_get_current_dir ());
  int rc = sqlite3_open (filename, &db);
  if (rc != SQLITE_OK)
    g_error ("%s", sqlite3_errmsg (db));

  AmselSqliteDatabase *self = g_object_new (AMSEL_TYPE_SQLITE_DATABASE,
                                            "database", db,
                                            NULL);
  return self;
}

static void
amsel_sqlite_database_finalize (GObject *object)
{
  AmselSqliteDatabase *self = (AmselSqliteDatabase *)object;

  AM_TRACE_MSG ("Finalize SQLITE DB...");
  int rc = sqlite3_close (self->db);
  if (rc != SQLITE_OK)
    g_error ("%s", sqlite3_errmsg (self->db));
  self->db = NULL;

  G_OBJECT_CLASS (amsel_sqlite_database_parent_class)->finalize (object);
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
    case PROP_DATABASE:
      if (self->db != NULL)
        sqlite3_close (self->db);
      self->db = g_value_get_pointer (value);
      amsel_sqlite_database_create_tables (self);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_sqlite_database_class_init (AmselSqliteDatabaseClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_sqlite_database_finalize;
  object_class->set_property = amsel_sqlite_database_set_property;

  properties [PROP_DATABASE] =
    g_param_spec_pointer ("database",
                          "Database",
                          "Database",
                           (G_PARAM_WRITABLE |
                            G_PARAM_CONSTRUCT_ONLY |
                           G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (object_class, PROP_DATABASE,
                                   properties [PROP_DATABASE]);
}

static void
amsel_sqlite_database_init (AmselSqliteDatabase *self)
{
}

static void
amsel_sqlite_database_create_tables (AmselSqliteDatabase *self)
{
  g_return_if_fail (AMSEL_IS_SQLITE_DATABASE (self));

  g_autoptr (sqlite3_stmt) stmt = NULL;
  gchar *create_channel_table;
  gchar *create_entries_table;
  gint rc;

  create_channel_table =
    "CREATE TABLE IF NOT EXISTS channels ("
    "   id     text PRIMARY KEY NOT NULL,"
    "   title  text,"
    "   source text NOT NULL);";

  rc = sqlite3_prepare_v2 (self->db, create_channel_table, -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    g_error ("1%s", sqlite3_errmsg (self->db));
  rc = sqlite3_step (stmt);
  if (rc != SQLITE_DONE)
    g_error ("1%s", sqlite3_errmsg (self->db));
  sqlite3_finalize (stmt);

  create_entries_table =
    "CREATE TABLE IF NOT EXISTS entries ("
    "   id            text,"
    "   title         text,"
    "   content       text,"
    "   updated       bigint,"
    "   link          text,"
    "   preview_image text,"
    "   read          integer,"
    "   author        text,"
    "   channel_id    text,"
    "   PRIMARY KEY (id, channel_id),"
    "   CONSTRAINT fk_channel FOREIGN KEY(channel_id) REFERENCES channels(id) ON DELETE CASCADE)";

  sqlite3_prepare_v2 (self->db, create_entries_table, -1, &stmt, NULL);
  rc = sqlite3_step (stmt);
  if (rc != SQLITE_DONE)
    g_error ("2%s", sqlite3_errmsg (self->db));
}

static void
_save_channel (sqlite3      *db,
               AmselChannel *channel)
{
  g_autoptr (sqlite3_stmt) stmt = NULL;
  int rc = sqlite3_prepare_v2 (db, "INSERT INTO channels VALUES (?, ?, ?)", -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    g_error ("3%s", sqlite3_errmsg (db));

  sqlite3_bind_text (stmt, 1, amsel_channel_get_id (channel), -1, NULL);
  sqlite3_bind_text (stmt, 2, amsel_channel_get_title (channel), -1, NULL);
  sqlite3_bind_text (stmt, 3, amsel_channel_get_source (channel), -1, NULL);

  rc = sqlite3_step (stmt);
}

static void
_save_entry (sqlite3      *db,
             AmselChannel *channel,
             AmselEntry   *entry)
{
  g_return_if_fail (AMSEL_IS_CHANNEL (channel));
  g_return_if_fail (AMSEL_IS_ENTRY (entry));

  g_autoptr (sqlite3_stmt) stmt = NULL;
  int rc = sqlite3_prepare_v2 (db, "INSERT INTO entries VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    g_error ("%s", sqlite3_errmsg (db));

  GDateTime *updated = amsel_entry_get_updated (entry);

  sqlite3_bind_text (stmt, 1, amsel_entry_get_id (entry), -1, NULL);
  sqlite3_bind_text (stmt, 2, amsel_entry_get_title (entry), -1, NULL);
  sqlite3_bind_text (stmt, 3, amsel_entry_get_content (entry), -1, NULL);
  if (updated)
    sqlite3_bind_int64 (stmt, 4, g_date_time_to_unix (updated));
  sqlite3_bind_text (stmt, 5, amsel_entry_get_link (entry), -1, NULL);
  sqlite3_bind_text (stmt, 6, amsel_entry_get_link (entry), -1, NULL);
  sqlite3_bind_int (stmt, 7, amsel_entry_get_read (entry));
  sqlite3_bind_text (stmt, 8, amsel_entry_get_author (entry), -1, NULL);
  sqlite3_bind_text (stmt, 9, amsel_channel_get_id (channel), -1, NULL);

  rc = sqlite3_step (stmt);
}

static void
amsel_sqlite_database_save_entry (AmselDatabase  *db,
                                  AmselChannel   *channel,
                                  AmselEntry     *entry,
                                  GError        **error)
{
  g_return_if_fail (AMSEL_IS_DATABASE (db));

  AmselSqliteDatabase *self = AMSEL_SQLITE_DATABASE (db);
  _save_entry (self->db, channel, entry);
}

static void
amsel_sqlite_database_save_channel (AmselDatabase  *db,
                                    AmselChannel   *channel,
                                    GError        **error)
{
  g_return_if_fail (AMSEL_IS_DATABASE (db));

  AM_TRACE_MSG ("Save channel with id %s", amsel_channel_get_id (channel));

  AmselSqliteDatabase *self = AMSEL_SQLITE_DATABASE (db);

  amsel_sqlite_begin_transaction (self->db);

  _save_channel (self->db, channel);
  GHashTable *entries = amsel_channel_get_entries (channel);
  GList *entries_list = g_hash_table_get_values (entries);
  for (GList *cur = entries_list; cur; cur = g_list_next (cur))
    {
      _save_entry (self->db, channel, cur->data);
    }
  g_list_free (entries_list);

  amsel_sqlite_end_transaction (self->db);
}

static void
amsel_sqlite_database_get_entries_for_channel (AmselSqliteDatabase *self,
                                               AmselChannel        *channel)
{
  g_return_if_fail (AMSEL_IS_SQLITE_DATABASE (self));
  g_return_if_fail (AMSEL_IS_CHANNEL (channel));

  AM_ENTRY;

  g_autoptr (sqlite3_stmt) stmt = NULL;
  sqlite3_prepare_v2 (self->db,
    "SELECT id, title, content, updated, link, preview_image, read, author, channel_id "
    "FROM entries WHERE channel_id=?", -1, &stmt, NULL);
  sqlite3_bind_text (stmt, 1, amsel_channel_get_id (channel), -1, NULL);

  while (sqlite3_step (stmt) != SQLITE_DONE)
    {
      int columns = sqlite3_column_count (stmt);
      g_autoptr (AmselEntry) entry = amsel_entry_new ();

      for (int i = 0; i < columns; i++)
        {
          const char *column_name = sqlite3_column_name (stmt, i);
          if (g_strcmp0 (column_name, "id") == 0)
            amsel_entry_set_id (entry, (const char *) sqlite3_column_text (stmt, i));
          else if (g_strcmp0 (column_name, "title") == 0)
            amsel_entry_set_title (entry, (const char *) sqlite3_column_text (stmt, i));
          else if (g_strcmp0 (column_name, "content") == 0)
            amsel_entry_set_content (entry, (const char *) sqlite3_column_text (stmt, i));
          else if (g_strcmp0 (column_name, "updated") == 0) {
            g_autoptr (GDateTime) updated = g_date_time_new_from_unix_local (sqlite3_column_int64 (stmt, i));
            amsel_entry_set_updated_datetime (entry, updated);
          }
          else if (g_strcmp0 (column_name, "link") == 0)
            amsel_entry_set_link (entry, (const char *) sqlite3_column_text (stmt, i));
        }

      amsel_channel_add_entry (channel, entry);
    }
}

static GPtrArray *
amsel_sqlite_database_get_channels (AmselDatabase *db)
{
  g_return_val_if_fail (AMSEL_IS_DATABASE (db), NULL);

  AmselSqliteDatabase *self = AMSEL_SQLITE_DATABASE (db);
  g_autoptr (sqlite3_stmt) stmt = NULL;
  g_autoptr (GPtrArray) channels = g_ptr_array_new ();

  int rc = sqlite3_prepare_v2 (self->db, "SELECT id, title, source FROM channels", -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    g_error ("%s", sqlite3_errmsg (self->db));

  while (sqlite3_step (stmt) != SQLITE_DONE)
    {
      AmselChannel *c = amsel_channel_new ();
      int columns = sqlite3_column_count (stmt);
      for (int i = 0; i < columns; i++)
        {
          const char *column_name = sqlite3_column_name (stmt, i);
          if (g_strcmp0 (column_name, "id") == 0)
            amsel_channel_set_id (c, (const char *) sqlite3_column_text (stmt, i));
          else if (g_strcmp0 (column_name, "title") == 0)
            amsel_channel_set_title (c, (const char *) sqlite3_column_text (stmt, i));
          else if (g_strcmp0 (column_name, "source") == 0)
            amsel_channel_set_source (c, (const char *) sqlite3_column_text (stmt, i));
        }
      amsel_sqlite_database_get_entries_for_channel (self, c);
      g_ptr_array_add (channels, c);
    }

  return g_steal_pointer (&channels);
}

static void
amsel_sqlite_database_db_interface_init (AmselDatabaseInterface *iface)
{
  iface->save_channel = amsel_sqlite_database_save_channel;
  iface->save_entry = amsel_sqlite_database_save_entry;
  iface->get_channels = amsel_sqlite_database_get_channels;
}
