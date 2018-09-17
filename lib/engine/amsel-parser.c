#include "amsel-parser.h"

G_DEFINE_INTERFACE (AmselParser, amsel_parser, G_TYPE_OBJECT)

static void
amsel_parser_default_init (AmselParserInterface *iface)
{
  iface->parse = amsel_parser_parse;
}

/**
 * amsel_parser_parse:
 * @self: a #AmselParser
 * @request: a #AmselRequest
 *
 * Returns: (transfer full) (element-type AmselChannel):
 */
GPtrArray *
amsel_parser_parse (AmselParser  *self,
                    AmselRequest *request)
{
  return AMSEL_PARSER_GET_IFACE (self)->parse (self, request);
}
