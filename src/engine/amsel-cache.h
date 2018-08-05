#pragma once

#include <glib-object.h>
#include "model/amsel-channel.h"

G_BEGIN_DECLS

#define AMSEL_TYPE_CACHE (amsel_cache_get_type())

G_DECLARE_FINAL_TYPE (AmselCache, amsel_cache, AMSEL, CACHE, GObject)

AmselCache   *amsel_cache_new         (void);
AmselChannel *amsel_cache_add_channel (AmselCache   *self,
                                       AmselChannel *channel);

G_END_DECLS
