#include "alb-sqlite-helper.h"

void
alb_sqlite_begin_transaction (sqlite3 *db)
{
  sqlite3_exec (db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
}

void
alb_sqlite_end_transaction (sqlite3 *db)
{
  sqlite3_exec (db, "COMMIT;", NULL, NULL, NULL);
}
