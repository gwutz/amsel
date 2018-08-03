#pragma once

#include <glib-object.h>
#include "amsel-request.h"

G_BEGIN_DECLS

#define AMSEL_TYPE_ENGINE (amsel_engine_get_type())

G_DECLARE_FINAL_TYPE (AmselEngine, amsel_engine, AMSEL, ENGINE, GObject)

AmselEngine *amsel_engine_new (void);
gboolean     amsel_engine_validate (AmselEngine  *self,
                                    AmselRequest *request);
GPtrArray   *amsel_engine_parse    (AmselEngine  *self,
                                    AmselRequest *request);

G_END_DECLS
