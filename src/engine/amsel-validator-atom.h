#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_VALIDATOR_ATOM (amsel_validator_atom_get_type())

G_DECLARE_FINAL_TYPE (AmselValidatorAtom, amsel_validator_atom, AMSEL, VALIDATOR_ATOM, GObject)

AmselValidatorAtom *amsel_validator_atom_new (void);

G_END_DECLS
