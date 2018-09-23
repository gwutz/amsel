#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "amsel-channel.h"

G_BEGIN_DECLS

#define AMSEL_TYPE_DATABASE (amsel_database_get_type ())

G_DECLARE_INTERFACE (AmselDatabase, amsel_database, AMSEL, DATABASE, GObject)

/**
 * AmselDatabaseInterface
 * @parent: the parent interface
 * @save_channel: save channel in database
 * @get_channels: get all channels saved in database
 */
struct _AmselDatabaseInterface
{
  GTypeInterface parent;

  void       (*save_channel)          (AmselDatabase  *self,
                                       AmselChannel   *channel,
                                       GError        **error);
  void       (*save_entry)            (AmselDatabase  *self,
                                       AmselChannel   *channel,
                                       AmselEntry     *entry,
                                       GError        **error);
  void       (*set_read)              (AmselDatabase  *self,
                                       AmselEntry     *entry,
                                       GError        **error);
  GPtrArray *(*get_channels)          (AmselDatabase *self);
};

void       amsel_database_save_channel (AmselDatabase  *self,
                                        AmselChannel   *channel,
                                        GError        **error);
void       amsel_database_save_entry   (AmselDatabase  *self,
                                        AmselChannel   *channel,
                                        AmselEntry     *entry,
                                        GError        **error);
void       amsel_database_set_read     (AmselDatabase  *self,
                                        AmselEntry     *entry,
                                        GError        **error);
GPtrArray *amsel_database_get_channels (AmselDatabase  *self);

#define AMSEL_DATABASE_ERROR amsel_database_error_quark ()

GQuark amsel_database_error_quark ();

/**
 * AmselDatabaseError:
 * @AMSEL_DATABASE_ERROR_INSERT: insertion into database failed
 */
typedef enum
{
  AMSEL_DATABASE_ERROR_INSERT,
} AmselDatabaseError;

G_END_DECLS
