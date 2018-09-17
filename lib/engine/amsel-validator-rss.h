#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_VALIDATOR_RSS (amsel_validator_rss_get_type())

G_DECLARE_FINAL_TYPE (AmselValidatorRss, amsel_validator_rss, AMSEL, VALIDATOR_RSS, GObject)

AmselValidatorRss *amsel_validator_rss_new (void);

G_END_DECLS
