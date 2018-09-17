#include "amsel-sqlite-helper.h"

void
amsel_sqlite_begin_transaction (sqlite3 *db)
{
  sqlite3_exec (db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
}

void
amsel_sqlite_end_transaction (sqlite3 *db)
{
  sqlite3_exec (db, "COMMIT;", NULL, NULL, NULL);
}
