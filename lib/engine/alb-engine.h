#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include <gio/gio.h>
#include "alb-request.h"
#include "alb-entry.h"

G_BEGIN_DECLS

#define ALB_TYPE_ENGINE (alb_engine_get_type())
#define ALB_IS_MAIN_THREAD() (g_thread_self () == alb_engine_get_main_thread ())

G_DECLARE_FINAL_TYPE (AlbEngine, alb_engine, ALB, ENGINE, GObject)

AlbEngine *alb_engine_new             (void);
gboolean     alb_engine_validate        (AlbEngine          *self,
                                           AlbRequest         *request);
GPtrArray   *alb_engine_parse           (AlbEngine          *self,
                                           AlbRequest         *request);
void         alb_engine_parse_async     (AlbEngine          *self,
                                           AlbRequest         *request,
                                           GCancellable         *cancellable,
                                           GAsyncReadyCallback   callback,
                                           gpointer              user_data);
GPtrArray   *alb_engine_parse_finish    (AlbEngine          *self,
                                           GAsyncResult         *result,
                                           GError              **error);
GPtrArray   *alb_engine_get_channels    (AlbEngine          *self);
void         alb_engine_mark_entry_read (AlbEngine          *self,
                                           AlbEntry             *entry);
void         alb_engine_connect_signal  (AlbEngine          *self,
                                           GCallback             callback,
                                           gpointer              user_data);
GThread     *alb_engine_get_main_thread (void);

G_END_DECLS
