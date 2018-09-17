#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "amsel-channel.h"
#include "amsel-database.h"

G_BEGIN_DECLS

#define AMSEL_TYPE_CACHE (amsel_cache_get_type())

G_DECLARE_FINAL_TYPE (AmselCache, amsel_cache, AMSEL, CACHE, GObject)

AmselCache   *amsel_cache_new          (AmselDatabase *database);
AmselChannel *amsel_cache_add_channel  (AmselCache    *self,
                                        AmselChannel  *channel,
                                        GError       **error);
GPtrArray    *amsel_cache_get_channels (AmselCache   *self);

#define AMSEL_CACHE_ERROR amsel_cache_error_quark ()

/**
 * AmselCacheError:
 * @AMSEL_CACHE_ERROR_NO_SOURCE: Gets returned when there is no source defined
 * for #AmselChannel
 *
 * Errors in conjunction with #AmselCache
 *
 */
typedef enum
{
  AMSEL_CACHE_ERROR_NO_SOURCE,
} AmselCacheError;

GQuark amsel_cache_error_quark (void);

G_END_DECLS
