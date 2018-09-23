#include "alb-validator.h"

G_DEFINE_INTERFACE (AlbValidator, alb_validator, G_TYPE_OBJECT)

static void
alb_validator_default_init (AlbValidatorInterface *iface)
{
  iface->validate = alb_validator_validate;
}

gboolean
alb_validator_validate (AlbValidator *self,
                          char           *data)
{
  g_return_val_if_fail (ALB_IS_VALIDATOR (self), FALSE);

  return ALB_VALIDATOR_GET_IFACE (self)->validate (self, data);
}
