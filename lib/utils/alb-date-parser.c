/* alb-date-parser.c
 *
 * Copyright 2018 Günther Wagner <info@gunibert.de>
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#define G_LOG_DOMAIN "alb-date-parser"

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
