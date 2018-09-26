/* aml-entry-row.c
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

#define G_LOG_DOMAIN "aml-entry-row"

#include "aml-entry-row.h"

struct _AmlEntryRow
{
  GtkListBoxRow parent_instance;

  GtkLabel *lbl_title;
  GtkLabel *lbl_author;
};

G_DEFINE_TYPE (AmlEntryRow, aml_entry_row, GTK_TYPE_LIST_BOX_ROW)

GtkWidget *
aml_entry_row_new (void)
{
  return g_object_new (AML_TYPE_ENTRY_ROW, NULL);
}

static void
aml_entry_row_finalize (GObject *object)
{
  AmlEntryRow *self = (AmlEntryRow *)object;

  G_OBJECT_CLASS (aml_entry_row_parent_class)->finalize (object);
}

static void
aml_entry_row_class_init (AmlEntryRowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = aml_entry_row_finalize;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/Amsel/entry.ui");
  gtk_widget_class_bind_template_child (widget_class, AmlEntryRow, lbl_title);
  gtk_widget_class_bind_template_child (widget_class, AmlEntryRow, lbl_author);
}

static void
aml_entry_row_init (AmlEntryRow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

void
aml_entry_row_set_entry (AmlEntryRow *self,
                         AlbEntry    *entry)
{
  g_return_if_fail (AML_IS_ENTRY_ROW (self));
  g_return_if_fail (ALB_IS_ENTRY (entry));

  const gchar *str = alb_entry_get_title (entry);
  const gchar *format;
  if (alb_entry_get_read (entry))
    format = "%s";
  else
    format = "<b>\%s</b>";
  g_autofree gchar *markup;

  markup = g_markup_printf_escaped (format, str);
  gtk_label_set_markup (self->lbl_title, markup);

  gtk_label_set_text (self->lbl_author, alb_entry_get_author (entry));
}
