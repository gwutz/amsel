#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "amsel-parser-xml.h"

G_BEGIN_DECLS

#define AMSEL_TYPE_PARSER_RSS (amsel_parser_rss_get_type())

G_DECLARE_FINAL_TYPE (AmselParserRss, amsel_parser_rss, AMSEL, PARSER_RSS, AmselParserXml)

AmselParserRss *amsel_parser_rss_new (void);

G_END_DECLS