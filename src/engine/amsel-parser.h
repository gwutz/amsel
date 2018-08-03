#pragma once

#include <glib-object.h>
#include "amsel-request.h"

G_BEGIN_DECLS

#define AMSEL_TYPE_PARSER (amsel_parser_get_type ())

G_DECLARE_INTERFACE (AmselParser, amsel_parser, AMSEL, PARSER, GObject)

struct _AmselParserInterface
{
  GTypeInterface parent;

  GPtrArray * (*parse) (AmselParser  *self,
                        AmselRequest *request);
};

GPtrArray *amsel_parser_parse (AmselParser  *self,
                               AmselRequest *request);

G_END_DECLS
