#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib.h>
#include <sqlite3.h>

G_BEGIN_DECLS

void amsel_sqlite_begin_transaction (sqlite3 *db);
void amsel_sqlite_end_transaction (sqlite3 *db);

G_END_DECLS
