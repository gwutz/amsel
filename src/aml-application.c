/* aml-application.c
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

#include "aml-application.h"
#include "aml-application-window.h"

struct _AmlApplication
{
  GtkApplication parent_instance;

  AmselEngine *engine;
};

G_DEFINE_TYPE (AmlApplication, aml_application, GTK_TYPE_APPLICATION)

AmlApplication *
aml_application_new (void)
{
  return g_object_new (AML_TYPE_APPLICATION,
                       "application-id", "org.gnome.Amsel",
                       "flags", G_APPLICATION_FLAGS_NONE,
                       NULL);
}

static void
aml_application_finalize (GObject *object)
{
  G_OBJECT_CLASS (aml_application_parent_class)->finalize (object);
}

static void
aml_application_activate (GApplication *app)
{
  GtkWindow *window = NULL;
  AmlApplication *self = AML_APPLICATION (app);
  GtkApplication *gtkapp = GTK_APPLICATION (app);

  window = gtk_application_get_active_window (gtkapp);
  if (window == NULL)
    {
      window = GTK_WINDOW (aml_application_window_new (self));
    }

  gtk_window_present (window);
}

static void
aml_application_class_init (AmlApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

  object_class->finalize = aml_application_finalize;

  app_class->activate = aml_application_activate;
}

static void
aml_application_init (AmlApplication *self)
{
  self->engine = amsel_engine_new ();
}

AmselEngine *
aml_application_get_engine (AmlApplication *self)
{
  g_return_val_if_fail (AML_IS_APPLICATION (self), NULL);

  return self->engine;
}
