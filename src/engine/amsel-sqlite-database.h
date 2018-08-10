#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_SQLITE_DATABASE (amsel_sqlite_database_get_type())

G_DECLARE_FINAL_TYPE (AmselSqliteDatabase, amsel_sqlite_database, AMSEL, SQLITE_DATABASE, GObject)

AmselSqliteDatabase *amsel_sqlite_database_new (char *filename);

G_END_DECLS
