#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include <gio/gio.h>
#include "amsel-request.h"

G_BEGIN_DECLS

#define AMSEL_TYPE_ENGINE (amsel_engine_get_type())

G_DECLARE_FINAL_TYPE (AmselEngine, amsel_engine, AMSEL, ENGINE, GObject)

AmselEngine *amsel_engine_new          (void);
gboolean     amsel_engine_validate     (AmselEngine          *self,
                                        AmselRequest         *request);
GPtrArray   *amsel_engine_parse        (AmselEngine          *self,
                                        AmselRequest         *request);
void         amsel_engine_parse_async  (AmselEngine          *self,
                                        AmselRequest         *request,
                                        GCancellable         *cancellable,
                                        GAsyncReadyCallback   callback,
                                        gpointer              user_data);
GPtrArray   *amsel_engine_parse_finish (AmselEngine          *self,
                                        GAsyncResult         *result,
                                        GError              **error);
GPtrArray   *amsel_engine_get_channels (AmselEngine          *self);

G_END_DECLS