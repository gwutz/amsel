#include "alb-parser.h"

G_DEFINE_INTERFACE (AlbParser, alb_parser, G_TYPE_OBJECT)

static void
alb_parser_default_init (AlbParserInterface *iface)
{
  iface->parse = alb_parser_parse;
}

/**
 * alb_parser_parse:
 * @self: a #AlbParser
 * @request: a #AlbRequest
 *
 * Returns: (transfer full) (element-type AlbChannel):
 */
GPtrArray *
alb_parser_parse (AlbParser  *self,
                    AlbRequest *request)
{
  return ALB_PARSER_GET_IFACE (self)->parse (self, request);
}
