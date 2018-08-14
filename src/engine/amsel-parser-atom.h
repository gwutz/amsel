#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_PARSER_ATOM (amsel_parser_atom_get_type())

G_DECLARE_FINAL_TYPE (AmselParserAtom, amsel_parser_atom, AMSEL, PARSER_ATOM, GObject)

AmselParserAtom *amsel_parser_atom_new (void);

G_END_DECLS
