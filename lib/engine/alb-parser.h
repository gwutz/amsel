#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "alb-request.h"

G_BEGIN_DECLS

#define ALB_TYPE_PARSER (alb_parser_get_type ())

G_DECLARE_INTERFACE (AlbParser, alb_parser, ALB, PARSER, GObject)

/**
 * AlbParserInterface:
 * @parent: the parent interface
 * @parse: take the neutral input data and parse them to #AmselChannel data
 */
struct _AlbParserInterface
{
  GTypeInterface parent;

  GPtrArray * (*parse) (AlbParser  *self,
                        AlbRequest *request);
};

GPtrArray *alb_parser_parse (AlbParser  *self,
                               AlbRequest *request);

G_END_DECLS
