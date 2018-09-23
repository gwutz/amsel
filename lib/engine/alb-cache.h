#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "alb-channel.h"
#include "alb-database.h"

G_BEGIN_DECLS

#define ALB_TYPE_CACHE (alb_cache_get_type())

G_DECLARE_FINAL_TYPE (AlbCache, alb_cache, ALB, CACHE, GObject)

AlbCache   *alb_cache_new          (AlbDatabase  *database);
AlbChannel *alb_cache_add_channel  (AlbCache     *self,
                                        AlbChannel   *channel,
                                        GError        **error);
GPtrArray    *alb_cache_get_channels (AlbCache     *self);
void          alb_cache_mark_read    (AlbCache     *self,
                                        AlbEntry     *entry);

#define ALB_CACHE_ERROR alb_cache_error_quark ()

/**
 * AlbCacheError:
 * @ALB_CACHE_ERROR_NO_ID: Gets returned when there is no id defined
 * for #AlbChannel
 *
 * Errors in conjunction with #AlbCache
 *
 */
typedef enum
{
  ALB_CACHE_ERROR_NO_ID,
} AlbCacheError;

GQuark alb_cache_error_quark (void);

G_END_DECLS
