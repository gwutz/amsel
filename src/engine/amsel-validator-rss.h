#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_VALIDATOR_RSS (amsel_validator_rss_get_type())

G_DECLARE_FINAL_TYPE (AmselValidatorRss, amsel_validator_rss, AMSEL, VALIDATOR_RSS, GObject)

AmselValidatorRss *amsel_validator_rss_new (void);

G_END_DECLS
