#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "alb-parser-xml.h"

G_BEGIN_DECLS

#define ALB_TYPE_PARSER_ATOM (alb_parser_atom_get_type())

G_DECLARE_FINAL_TYPE (AlbParserAtom, alb_parser_atom, ALB, PARSER_ATOM, AlbParserXml)

AlbParserAtom *alb_parser_atom_new (void);

G_END_DECLS
