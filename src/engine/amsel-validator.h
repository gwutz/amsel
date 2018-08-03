#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_VALIDATOR (amsel_validator_get_type ())

G_DECLARE_INTERFACE (AmselValidator, amsel_validator, AMSEL, VALIDATOR, GObject)

struct _AmselValidatorInterface
{
  GTypeInterface parent;

  gboolean (*validate) (AmselValidator *self,
                        char           *data);
};

gboolean amsel_validator_validate (AmselValidator *self,
                                   char           *data);

G_END_DECLS
