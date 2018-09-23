#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "alb-entry.h"

G_BEGIN_DECLS

#define ALB_TYPE_CHANNEL (alb_channel_get_type())

G_DECLARE_FINAL_TYPE(AlbChannel, alb_channel, ALB, CHANNEL, GObject)

AlbChannel  *alb_channel_new             (void);
const gchar *alb_channel_get_plugin      (AlbChannel  *self);
void         alb_channel_set_title       (AlbChannel  *self,
                                          const gchar *title);
const gchar *alb_channel_get_title       (AlbChannel  *self);
void         alb_channel_set_description (AlbChannel  *self,
                                          const char  *description);
const char  *alb_channel_get_description (AlbChannel  *self);
void         alb_channel_set_id          (AlbChannel  *self,
                                          const gchar *id);
const gchar *alb_channel_get_id          (AlbChannel  *self);
void         alb_channel_set_source      (AlbChannel  *self,
                                          const gchar *source);
const gchar *alb_channel_get_source      (AlbChannel  *self);
void         alb_channel_set_icon        (AlbChannel  *self,
                                          const gchar *icon);
const gchar *alb_channel_get_icon        (AlbChannel  *self);
void         alb_channel_add_entry       (AlbChannel  *self,
                                          AlbEntry    *entry);
GHashTable  *alb_channel_get_entries     (AlbChannel  *self);
G_END_DECLS
