#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "alb-channel.h"

G_BEGIN_DECLS

#define ALB_TYPE_DATABASE (alb_database_get_type ())

G_DECLARE_INTERFACE (AlbDatabase, alb_database, ALB, DATABASE, GObject)

/**
 * AlbDatabaseInterface
 * @parent: the parent interface
 * @save_channel: save channel in database
 * @get_channels: get all channels saved in database
 */
struct _AlbDatabaseInterface
{
  GTypeInterface parent;

  void       (*save_channel)          (AlbDatabase  *self,
                                       AlbChannel   *channel,
                                       GError        **error);
  void       (*save_entry)            (AlbDatabase  *self,
                                       AlbChannel   *channel,
                                       AlbEntry     *entry,
                                       GError        **error);
  void       (*set_read)              (AlbDatabase  *self,
                                       AlbEntry     *entry,
                                       GError        **error);
  GPtrArray *(*get_channels)          (AlbDatabase *self);
};

void       alb_database_save_channel (AlbDatabase  *self,
                                        AlbChannel   *channel,
                                        GError        **error);
void       alb_database_save_entry   (AlbDatabase  *self,
                                        AlbChannel   *channel,
                                        AlbEntry       *entry,
                                        GError        **error);
void       alb_database_set_read     (AlbDatabase  *self,
                                        AlbEntry       *entry,
                                        GError        **error);
GPtrArray *alb_database_get_channels (AlbDatabase  *self);

#define ALB_DATABASE_ERROR alb_database_error_quark ()

GQuark alb_database_error_quark ();

/**
 * AlbDatabaseError:
 * @ALB_DATABASE_ERROR_INSERT: insertion into database failed
 */
typedef enum
{
  ALB_DATABASE_ERROR_INSERT,
} AlbDatabaseError;

G_END_DECLS
