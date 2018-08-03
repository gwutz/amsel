#pragma once

#define _XOPEN_SOURCE  600

#include <glib.h>

G_BEGIN_DECLS

GDateTime *amsel_date_parser_parse_RFC822 (const char *rfc822);

G_END_DECLS
