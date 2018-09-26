/* alb-sqlite-helper.c
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

#define G_LOG_DOMAIN "alb-sqlite-helper"

#include "alb-sqlite-helper.h"

void
alb_sqlite_begin_transaction (sqlite3 *db)
{
  sqlite3_exec (db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
}

void
alb_sqlite_end_transaction (sqlite3 *db)
{
  sqlite3_exec (db, "COMMIT;", NULL, NULL, NULL);
}
