#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>
#include "alb-parser-xml.h"

G_BEGIN_DECLS

#define ALB_TYPE_PARSER_RSS (alb_parser_rss_get_type())

G_DECLARE_FINAL_TYPE (AlbParserRss, alb_parser_rss, ALB, PARSER_RSS, AlbParserXml)

AlbParserRss *alb_parser_rss_new (void);

G_END_DECLS
