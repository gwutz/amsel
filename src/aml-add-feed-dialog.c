/* aml-add-feed-dialog.c
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

#define G_LOG_DOMAIN "aml-add-feed-dialog"

#include "aml-add-feed-dialog.h"
#include <libsoup/soup.h>

struct _AmlAddFeedDialog
{
  GtkDialog parent_instance;

  GtkEntry *url_entry;
  GtkRevealer *validation;
};

G_DEFINE_TYPE (AmlAddFeedDialog, aml_add_feed_dialog, GTK_TYPE_DIALOG)

AmlAddFeedDialog *
aml_add_feed_dialog_new (GtkWindow *parent)
{
  return g_object_new (AML_TYPE_ADD_FEED_DIALOG,
                       "use-header-bar", TRUE,
                       "transient-for", parent,
                       NULL);
}

static void
aml_add_feed_dialog_finalize (GObject *object)
{
  G_OBJECT_CLASS (aml_add_feed_dialog_parent_class)->finalize (object);
}

static void
aml_add_feed_dialog_activated (GtkEntry *entry,
                               gpointer  user_data)
{
  g_return_if_fail (AML_IS_ADD_FEED_DIALOG (user_data));

  GtkDialog *dialog = GTK_DIALOG (user_data);
  gtk_dialog_response (dialog, GTK_RESPONSE_OK);
}

static void
aml_add_feed_dialog_class_init (AmlAddFeedDialogClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = aml_add_feed_dialog_finalize;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/Amsel/addfeed.ui");
  gtk_widget_class_bind_template_child (widget_class, AmlAddFeedDialog, url_entry);
  gtk_widget_class_bind_template_child (widget_class, AmlAddFeedDialog, validation);
  gtk_widget_class_bind_template_callback (widget_class, aml_add_feed_dialog_activated);
}

static void
aml_add_feed_dialog_validate (GtkEditable *editable,
                              gpointer  user_data)
{
  AmlAddFeedDialog *self = AML_ADD_FEED_DIALOG (user_data);
  g_autoptr (SoupURI) uri;

  uri = soup_uri_new (gtk_entry_get_text (GTK_ENTRY (editable)));
  if (uri == NULL)
    gtk_revealer_set_reveal_child (self->validation, TRUE);
  else
    gtk_revealer_set_reveal_child (self->validation, FALSE);
}

static void
aml_add_feed_dialog_init (AmlAddFeedDialog *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  g_signal_connect (self->url_entry, "changed", G_CALLBACK (aml_add_feed_dialog_validate), self);
}

gchar *
aml_add_feed_dialog_get_url (AmlAddFeedDialog *self)
{
  g_return_val_if_fail (AML_IS_ADD_FEED_DIALOG (self), NULL);

  return g_strdup (gtk_entry_get_text (self->url_entry));
}
