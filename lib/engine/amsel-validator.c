#include "amsel-validator.h"

G_DEFINE_INTERFACE (AmselValidator, amsel_validator, G_TYPE_OBJECT)

static void
amsel_validator_default_init (AmselValidatorInterface *iface)
{
  iface->validate = amsel_validator_validate;
}

gboolean
amsel_validator_validate (AmselValidator *self,
                          char           *data)
{
  g_return_val_if_fail (AMSEL_IS_VALIDATOR (self), FALSE);

  return AMSEL_VALIDATOR_GET_IFACE (self)->validate (self, data);
}
