#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_MEMORY_DATABASE (amsel_memory_database_get_type())

G_DECLARE_FINAL_TYPE (AmselMemoryDatabase, amsel_memory_database, AMSEL, MEMORY_DATABASE, GObject)

AmselMemoryDatabase *amsel_memory_database_new (void);
int amsel_memory_database_get_counter (AmselMemoryDatabase *self);
void amsel_memory_database_set_error (AmselMemoryDatabase *self,
                                      gboolean             set_error);

G_END_DECLS
