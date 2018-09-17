#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "amsel-request.h"

G_BEGIN_DECLS

#define AMSEL_TYPE_PARSER (amsel_parser_get_type ())

G_DECLARE_INTERFACE (AmselParser, amsel_parser, AMSEL, PARSER, GObject)

/**
 * AmselParserInterface:
 * @parent: the parent interface
 * @parse: take the neutral input data and parse them to #AmselChannel data
 */
struct _AmselParserInterface
{
  GTypeInterface parent;

  GPtrArray * (*parse) (AmselParser  *self,
                        AmselRequest *request);
};

GPtrArray *amsel_parser_parse (AmselParser  *self,
                               AmselRequest *request);

G_END_DECLS
