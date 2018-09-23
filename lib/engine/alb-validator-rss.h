#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define ALB_TYPE_VALIDATOR_RSS (alb_validator_rss_get_type())

G_DECLARE_FINAL_TYPE (AlbValidatorRss, alb_validator_rss, ALB, VALIDATOR_RSS, GObject)

AlbValidatorRss *alb_validator_rss_new (void);

G_END_DECLS
