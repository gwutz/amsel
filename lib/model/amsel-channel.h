#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "amsel-entry.h"

G_BEGIN_DECLS

#define AMSEL_TYPE_CHANNEL (amsel_channel_get_type())

G_DECLARE_FINAL_TYPE(AmselChannel, amsel_channel, AMSEL, CHANNEL, GObject)

AmselChannel *amsel_channel_new             (void);
const gchar  *amsel_channel_get_plugin      (AmselChannel *self);
void          amsel_channel_set_title       (AmselChannel *self,
                                             const gchar  *title);
const gchar  *amsel_channel_get_title       (AmselChannel *self);
void          amsel_channel_set_description (AmselChannel *self,
                                             const char   *description);
const char   *amsel_channel_get_description (AmselChannel *self);
void          amsel_channel_set_id          (AmselChannel *self,
                                             const gchar  *id);
const gchar  *amsel_channel_get_id          (AmselChannel *self);
void          amsel_channel_set_source      (AmselChannel *self,
                                             const gchar  *source);
const gchar  *amsel_channel_get_source      (AmselChannel *self);
void          amsel_channel_set_icon        (AmselChannel *self,
                                             const gchar  *icon);
const gchar  *amsel_channel_get_icon        (AmselChannel *self);
void          amsel_channel_add_entry       (AmselChannel *self,
                                             AmselEntry   *entry);
GHashTable   *amsel_channel_get_entries     (AmselChannel *self);
G_END_DECLS
