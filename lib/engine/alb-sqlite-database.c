/* alb-sqlite-database.c
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

#define G_LOG_DOMAIN "alb-sqlite-database"

#include "alb-sqlite-database.h"
#include "alb-database.h"
#include "alb-sqlite-helper.h"
#include "alb-debug.h"
#include <sqlite3.h>

struct _AlbSqliteDatabase
{
  GObject parent_instance;

  sqlite3 *db;
};

static void alb_sqlite_database_db_interface_init (AlbDatabaseInterface *iface);
G_DEFINE_TYPE_WITH_CODE (AlbSqliteDatabase, alb_sqlite_database, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ALB_TYPE_DATABASE, alb_sqlite_database_db_interface_init))

enum {
  PROP_0,
  PROP_DATABASE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

/* Internal methods */
static void alb_sqlite_database_create_tables (AlbSqliteDatabase *self);

/* Cleanup definitions */
G_DEFINE_AUTOPTR_CLEANUP_FUNC (sqlite3_stmt, sqlite3_finalize)

AlbSqliteDatabase *
alb_sqlite_database_new (char *filename)
{
  sqlite3 *db;
  g_autofree gchar *current_dir = g_get_current_dir ();
  ALB_TRACE_MSG ("Current Dir: %s\n", current_dir);
  int rc = sqlite3_open (filename, &db);
  if (rc != SQLITE_OK)
    g_error ("%s", sqlite3_errmsg (db));

  AlbSqliteDatabase *self = g_object_new (ALB_TYPE_SQLITE_DATABASE,
                                            "database", db,
                                            NULL);
  return self;
}

static void
alb_sqlite_database_finalize (GObject *object)
{
  AlbSqliteDatabase *self = (AlbSqliteDatabase *)object;

  ALB_TRACE_MSG ("Finalize SQLITE DB...");
  int rc = sqlite3_close (self->db);
  if (rc != SQLITE_OK)
    g_error ("%s", sqlite3_errmsg (self->db));
  self->db = NULL;

  G_OBJECT_CLASS (alb_sqlite_database_parent_class)->finalize (object);
}

static void
alb_sqlite_database_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  AlbSqliteDatabase *self = ALB_SQLITE_DATABASE (object);

  switch (prop_id)
    {
    case PROP_DATABASE:
      if (self->db != NULL)
        sqlite3_close (self->db);
      self->db = g_value_get_pointer (value);
      alb_sqlite_database_create_tables (self);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
alb_sqlite_database_class_init (AlbSqliteDatabaseClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = alb_sqlite_database_finalize;
  object_class->set_property = alb_sqlite_database_set_property;

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
alb_sqlite_database_init (AlbSqliteDatabase *self)
{
}

static void
alb_sqlite_database_create_tables (AlbSqliteDatabase *self)
{
  g_return_if_fail (ALB_IS_SQLITE_DATABASE (self));

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
               AlbChannel *channel)
{
  g_autoptr (sqlite3_stmt) stmt = NULL;
  int rc = sqlite3_prepare_v2 (db, "INSERT INTO channels VALUES (?, ?, ?)", -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    g_error ("3%s", sqlite3_errmsg (db));

  sqlite3_bind_text (stmt, 1, alb_channel_get_id (channel), -1, NULL);
  sqlite3_bind_text (stmt, 2, alb_channel_get_title (channel), -1, NULL);
  sqlite3_bind_text (stmt, 3, alb_channel_get_source (channel), -1, NULL);

  rc = sqlite3_step (stmt);
}

static void
_save_entry (sqlite3      *db,
             AlbChannel *channel,
             AlbEntry   *entry)
{
  g_return_if_fail (ALB_IS_CHANNEL (channel));
  g_return_if_fail (ALB_IS_ENTRY (entry));

  g_autoptr (sqlite3_stmt) stmt = NULL;
  int rc = sqlite3_prepare_v2 (db, "INSERT INTO entries VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    g_error ("%s", sqlite3_errmsg (db));

  GDateTime *updated = alb_entry_get_updated (entry);

  sqlite3_bind_text (stmt, 1, alb_entry_get_id (entry), -1, NULL);
  sqlite3_bind_text (stmt, 2, alb_entry_get_title (entry), -1, NULL);
  sqlite3_bind_text (stmt, 3, alb_entry_get_content (entry), -1, NULL);
  if (updated)
    sqlite3_bind_int64 (stmt, 4, g_date_time_to_unix (updated));
  sqlite3_bind_text (stmt, 5, alb_entry_get_link (entry), -1, NULL);
  sqlite3_bind_text (stmt, 6, alb_entry_get_link (entry), -1, NULL);
  sqlite3_bind_int (stmt, 7, alb_entry_get_read (entry));
  sqlite3_bind_text (stmt, 8, alb_entry_get_author (entry), -1, NULL);
  sqlite3_bind_text (stmt, 9, alb_channel_get_id (channel), -1, NULL);

  rc = sqlite3_step (stmt);
}

static void
alb_sqlite_database_save_entry (AlbDatabase  *db,
                                  AlbChannel   *channel,
                                  AlbEntry     *entry,
                                  GError        **error)
{
  g_return_if_fail (ALB_IS_DATABASE (db));

  AlbSqliteDatabase *self = ALB_SQLITE_DATABASE (db);
  _save_entry (self->db, channel, entry);
}

static void
alb_sqlite_database_set_read (AlbDatabase  *db,
                                AlbEntry     *entry,
                                GError        **error)
{
  g_return_if_fail (ALB_IS_DATABASE (db));

  AlbSqliteDatabase *self = ALB_SQLITE_DATABASE (db);
  gint rc;
  g_autoptr(sqlite3_stmt) stmt;

  rc = sqlite3_prepare_v2 (self->db, "UPDATE entries SET read=? WHERE id=?", -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    g_error ("%s", sqlite3_errmsg (self->db));

  sqlite3_bind_int (stmt, 1, alb_entry_get_read (entry));
  sqlite3_bind_text (stmt, 2, alb_entry_get_id (entry), -1, NULL);

  sqlite3_step (stmt);
}

static void
alb_sqlite_database_save_channel (AlbDatabase  *db,
                                    AlbChannel   *channel,
                                    GError        **error)
{
  g_return_if_fail (ALB_IS_DATABASE (db));

  ALB_TRACE_MSG ("Save channel with id %s", alb_channel_get_id (channel));

  AlbSqliteDatabase *self = ALB_SQLITE_DATABASE (db);

  alb_sqlite_begin_transaction (self->db);

  _save_channel (self->db, channel);
  GHashTable *entries = alb_channel_get_entries (channel);
  GList *entries_list = g_hash_table_get_values (entries);
  for (GList *cur = entries_list; cur; cur = g_list_next (cur))
    {
      _save_entry (self->db, channel, cur->data);
    }
  g_list_free (entries_list);

  alb_sqlite_end_transaction (self->db);
}

static void
alb_sqlite_database_get_entries_for_channel (AlbSqliteDatabase *self,
                                               AlbChannel        *channel)
{
  g_return_if_fail (ALB_IS_SQLITE_DATABASE (self));
  g_return_if_fail (ALB_IS_CHANNEL (channel));

  ALB_ENTER;

  g_autoptr (sqlite3_stmt) stmt = NULL;
  sqlite3_prepare_v2 (self->db,
    "SELECT id, title, content, updated, link, preview_image, read, author, channel_id "
    "FROM entries WHERE channel_id=?", -1, &stmt, NULL);
  sqlite3_bind_text (stmt, 1, alb_channel_get_id (channel), -1, NULL);

  while (sqlite3_step (stmt) != SQLITE_DONE)
    {
      int columns = sqlite3_column_count (stmt);
      AlbEntry* entry = alb_entry_new ();

      for (int i = 0; i < columns; i++)
        {
          const char *column_name = sqlite3_column_name (stmt, i);
          if (g_strcmp0 (column_name, "id") == 0)
            alb_entry_set_id (entry, (const char *) sqlite3_column_text (stmt, i));
          else if (g_strcmp0 (column_name, "title") == 0)
            alb_entry_set_title (entry, (const char *) sqlite3_column_text (stmt, i));
          else if (g_strcmp0 (column_name, "content") == 0)
            alb_entry_set_content (entry, (const char *) sqlite3_column_text (stmt, i));
          else if (g_strcmp0 (column_name, "updated") == 0) {
            g_autoptr (GDateTime) updated = g_date_time_new_from_unix_local (sqlite3_column_int64 (stmt, i));
            alb_entry_set_updated_datetime (entry, updated);
          }
          else if (g_strcmp0 (column_name, "link") == 0)
            alb_entry_set_link (entry, (const char *) sqlite3_column_text (stmt, i));
          else if (g_strcmp0 (column_name, "read") == 0)
            alb_entry_set_read (entry, sqlite3_column_int (stmt, i));
          else if (g_strcmp0 (column_name, "author") == 0)
            alb_entry_set_author (entry, (const char *) sqlite3_column_text (stmt, i));
        }

      alb_channel_add_entry (channel, entry);
    }
}

static GPtrArray *
alb_sqlite_database_get_channels (AlbDatabase *db)
{
  g_return_val_if_fail (ALB_IS_DATABASE (db), NULL);

  AlbSqliteDatabase *self = ALB_SQLITE_DATABASE (db);
  g_autoptr (sqlite3_stmt) stmt = NULL;
  g_autoptr (GPtrArray) channels = g_ptr_array_new ();

  int rc = sqlite3_prepare_v2 (self->db, "SELECT id, title, source FROM channels", -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    g_error ("%s", sqlite3_errmsg (self->db));

  while (sqlite3_step (stmt) != SQLITE_DONE)
    {
      AlbChannel *c = alb_channel_new ();
      int columns = sqlite3_column_count (stmt);
      for (int i = 0; i < columns; i++)
        {
          const char *column_name = sqlite3_column_name (stmt, i);
          if (g_strcmp0 (column_name, "id") == 0)
            alb_channel_set_id (c, (const char *) sqlite3_column_text (stmt, i));
          else if (g_strcmp0 (column_name, "title") == 0)
            alb_channel_set_title (c, (const char *) sqlite3_column_text (stmt, i));
          else if (g_strcmp0 (column_name, "source") == 0)
            alb_channel_set_source (c, (const char *) sqlite3_column_text (stmt, i));
        }
      alb_sqlite_database_get_entries_for_channel (self, c);
      g_ptr_array_add (channels, c);
    }

  return g_steal_pointer (&channels);
}

static void
alb_sqlite_database_db_interface_init (AlbDatabaseInterface *iface)
{
  iface->save_channel = alb_sqlite_database_save_channel;
  iface->save_entry = alb_sqlite_database_save_entry;
  iface->get_channels = alb_sqlite_database_get_channels;
  iface->set_read = alb_sqlite_database_set_read;
}
