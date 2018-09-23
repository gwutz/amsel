#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define ALB_TYPE_VALIDATOR_ATOM (alb_validator_atom_get_type())

G_DECLARE_FINAL_TYPE (AlbValidatorAtom, alb_validator_atom, ALB, VALIDATOR_ATOM, GObject)

AlbValidatorAtom *alb_validator_atom_new (void);

G_END_DECLS
