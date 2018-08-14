#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "amsel-channel.h"

G_BEGIN_DECLS

#define AMSEL_TYPE_DATABASE (amsel_database_get_type ())

G_DECLARE_INTERFACE (AmselDatabase, amsel_database, AMSEL, DATABASE, GObject)

struct _AmselDatabaseInterface
{
  GTypeInterface parent;

  void (*save_channel) (AmselDatabase  *self,
                        AmselChannel   *channel,
                        GError        **error);
  GPtrArray *(*get_channels) (AmselDatabase *self);
};

void       amsel_database_save_channel (AmselDatabase  *self,
                                        AmselChannel   *channel,
                                        GError        **error);
GPtrArray *amsel_database_get_channels (AmselDatabase *self);

#define AMSEL_DATABASE_ERROR amsel_database_error_quark ()

GQuark amsel_database_error_quark ();

typedef enum
{
  AMSEL_DATABASE_ERROR_INSERT,
} AmselDatabaseError;

G_END_DECLS
