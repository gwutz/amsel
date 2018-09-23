#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define ALB_TYPE_VALIDATOR (alb_validator_get_type ())

G_DECLARE_INTERFACE (AlbValidator, alb_validator, ALB, VALIDATOR, GObject)

/**
 * AlbValidatorInterface:
 * @parent: the parent interface
 * @validate: validates the provided data
 */
struct _AlbValidatorInterface
{
  GTypeInterface parent;

  gboolean (*validate) (AlbValidator *self,
                        char           *data);
};

gboolean alb_validator_validate (AlbValidator *self,
                                   char           *data);

G_END_DECLS
