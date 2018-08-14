#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_SQLITE_DATABASE (amsel_sqlite_database_get_type())

G_DECLARE_FINAL_TYPE (AmselSqliteDatabase, amsel_sqlite_database, AMSEL, SQLITE_DATABASE, GObject)

AmselSqliteDatabase *amsel_sqlite_database_new (char *filename);

G_END_DECLS
