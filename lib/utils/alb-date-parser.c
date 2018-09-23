#include "utils/alb-date-parser.h"
#include <time.h>

GDateTime *
alb_date_parser_parse_RFC822 (const char *rfc822)
{
  GDateTime *ret;
  struct tm tm;
  char *cur;

  memset (&tm, 0, sizeof (struct tm));

  cur = g_utf8_strchr (rfc822, -1, ',');
  if (cur)
    cur++;

  cur = strptime ((const char *) cur, " %d %b %Y %T", &tm);
  if (!cur) {
    return NULL;
  }

  ret = g_date_time_new_local (tm.tm_year + 1900,
                               tm.tm_mon + 1,
                               tm.tm_mday,
                               tm.tm_hour,
                               tm.tm_min,
                               tm.tm_sec);

  return ret;
}
