#pragma once

#include <glib.h>
#include <sqlite3.h>

G_BEGIN_DECLS

void amsel_sqlite_begin_transaction (sqlite3 *db);
void amsel_sqlite_end_transaction (sqlite3 *db);

G_END_DECLS
