#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#define _XOPEN_SOURCE  600

#include <glib.h>

G_BEGIN_DECLS

GDateTime *alb_date_parser_parse_RFC822 (const char *rfc822);

G_END_DECLS
