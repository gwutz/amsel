#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_PARSER_RSS (amsel_parser_rss_get_type())

G_DECLARE_FINAL_TYPE (AmselParserRss, amsel_parser_rss, AMSEL, PARSER_RSS, GObject)

AmselParserRss *amsel_parser_rss_new (void);

G_END_DECLS
