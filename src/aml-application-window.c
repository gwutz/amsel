/* aml-application-window.c
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

#include "aml-application-window.h"
#include "downloader/aml-downloader.h"
#include "aml-add-feed-dialog.h"
#include <webkit2/webkit2.h>

struct _AmlApplicationWindow
{
  GtkApplicationWindow parent_instance;

  AmlDownloader *downloader;

  GtkStack *stack;
  GtkListBox *feedlist;
  GListStore *feedstore;
  WebKitWebView *entryview;
};

G_DEFINE_TYPE (AmlApplicationWindow, aml_application_window, GTK_TYPE_APPLICATION_WINDOW)

AmlApplicationWindow *
aml_application_window_new (AmlApplication *application)
{
  return g_object_new (AML_TYPE_APPLICATION_WINDOW,
                       "application", application,
                       NULL);
}

static void
aml_application_window_finalize (GObject *object)
{
  G_OBJECT_CLASS (aml_application_window_parent_class)->finalize (object);
}

static void
aml_application_window_entry_activated (GtkListBox    *box,
                                        GtkListBoxRow *row,
                                        gpointer       user_data)
{
  AmlApplicationWindow *self = AML_APPLICATION_WINDOW (user_data);
  AmselEntry *entry;

  gint index = gtk_list_box_row_get_index (row);
  entry = g_list_model_get_item (G_LIST_MODEL (self->feedstore), index);

  const gchar *content = amsel_entry_get_content (entry);
  webkit_web_view_load_html (self->entryview, content, NULL);
}

static void
aml_application_window_class_init (AmlApplicationWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = aml_application_window_finalize;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/Amsel/window.ui");
  gtk_widget_class_bind_template_child (widget_class, AmlApplicationWindow, stack);
  gtk_widget_class_bind_template_child (widget_class, AmlApplicationWindow, feedlist);
  gtk_widget_class_bind_template_child (widget_class, AmlApplicationWindow, entryview);
  gtk_widget_class_bind_template_callback (widget_class, aml_application_window_entry_activated);
}

static void
add_feed_ready (GObject      *source_object,
                GAsyncResult *res,
                gpointer      user_data)
{
  GError *error = NULL;
  AmselEngine *engine = AMSEL_ENGINE (source_object);
  AmlApplicationWindow *self = AML_APPLICATION_WINDOW (user_data);

  GPtrArray *channels = amsel_engine_parse_finish (engine, res, &error);
}

static void
data_fetched (GObject      *source_object,
              GAsyncResult *res,
              gpointer      user_data)
{
  gchar *raw;
  AmselRequest *request;
  AmselEngine *engine;
  AmlApplication *app;
  AmlApplicationWindow *self = AML_APPLICATION_WINDOW (user_data);

  app = AML_APPLICATION (gtk_window_get_application (GTK_WINDOW (self)));
  raw = aml_downloader_fetch_finish (AML_DOWNLOADER (source_object), res, NULL);
  request = amsel_request_new (raw, strlen (raw));

  if (amsel_request_get_request_type (request) == AMSEL_REQUEST_TYPE_UNDECIDED)
    {
      GtkWidget *message = gtk_message_dialog_new (GTK_WINDOW (self),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_WARNING,
                                                   GTK_BUTTONS_OK,
                                                   "Unknown input");
      gtk_dialog_run (GTK_DIALOG (message));
      gtk_widget_destroy (GTK_WIDGET (message));
      return;
      }
  engine = aml_application_get_engine (app);

  amsel_engine_parse_async (engine, request, NULL, add_feed_ready, self);
}

static void
add_feed (GSimpleAction *action,
          GVariant      *parameter,
          gpointer       user_data)
{
  AmlApplicationWindow *self = AML_APPLICATION_WINDOW (user_data);
  gchar *url;

  AmlAddFeedDialog *dialog = aml_add_feed_dialog_new (GTK_WINDOW (self));
  gint rc = gtk_dialog_run (GTK_DIALOG (dialog));
  switch (rc) {
  case GTK_RESPONSE_CANCEL:
    break;
  case GTK_RESPONSE_OK:
    url = aml_add_feed_dialog_get_url (dialog);

    aml_downloader_fetch_async (self->downloader, url, NULL, data_fetched, self);
    break;
  }
  gtk_widget_destroy (GTK_WIDGET (dialog));
}

static GtkWidget *
aml_application_window_create_row_channel (gpointer item,
                                           gpointer user_data)
{
  AmselEntry *e = AMSEL_ENTRY (item);
  GtkWidget *lbl = gtk_label_new (amsel_entry_get_title (e));
  return lbl;
}

static void
aml_application_window_init (AmlApplicationWindow *self)
{
  g_type_ensure (WEBKIT_TYPE_WEB_VIEW);
  gtk_widget_init_template (GTK_WIDGET (self));

  self->feedstore = g_list_store_new (AMSEL_TYPE_ENTRY);
  gtk_list_box_bind_model (self->feedlist, G_LIST_MODEL (self->feedstore), aml_application_window_create_row_channel, self, NULL);

  GApplication *app = g_application_get_default ();
  AmlApplication *aml = AML_APPLICATION (app);
  AmselEngine *engine = aml_application_get_engine (aml);
  self->downloader = aml_downloader_new ();

  GPtrArray *channels = amsel_engine_get_channels (engine);
  if (channels->len == 0) {
    gtk_stack_set_visible_child_name (self->stack, "empty");
  } else {
    gtk_stack_set_visible_child_name (self->stack, "feedview");
    for (int i = 0; i < channels->len; i++)
      {
        AmselChannel *c = AMSEL_CHANNEL (g_ptr_array_index (channels, i));
        GHashTable *tbl = amsel_channel_get_entries (c);
        GList *entries = g_hash_table_get_values (tbl);
        for (GList *cur = entries; cur != NULL; cur = g_list_next (cur))
          {
            g_list_store_append (self->feedstore, cur->data);
          }
      }
  }

  const GActionEntry win_entries[] = {
      { "add", add_feed },
  };

  g_action_map_add_action_entries (G_ACTION_MAP (self),
                                   win_entries,
                                   G_N_ELEMENTS (win_entries),
                                   self);
}
