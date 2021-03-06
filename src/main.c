/* main.c
 *
 * Copyright 2018 Günther Wagner <info@gunibert.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define G_LOG_DOMAIN "main"

#include <glib.h>
#include "aml-application.h"
#include "alb.h"

gint
main (gint   argc,
      gchar *argv[])
{
  g_autoptr(AmlApplication) app;
  g_set_prgname ("amsel");
  g_set_application_name ("Amsel");

  alb_log_init ();

  app = aml_application_new ();

  return g_application_run (G_APPLICATION (app), argc, argv);
}
