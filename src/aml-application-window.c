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
#include "alb.h"
#include "amsel-debug.h"

struct _AmlApplicationWindow
{
  GtkApplicationWindow parent_instance;

  AmlDownloader *downloader;

  GtkStack *stack;
  GtkListBox *feedlist;
  GListStore *feedstore;
  WebKitWebView *entryview;
  GtkMenuButton *gears;
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
  AM_TRACE_MSG ("Finalize Application Window");
  AmlApplicationWindow *self = AML_APPLICATION_WINDOW (object);

  g_clear_object (&self->downloader);

  G_OBJECT_CLASS (aml_application_window_parent_class)->finalize (object);
}

static void
aml_application_window_entry_activated (GtkListBox    *box,
                                        GtkListBoxRow *row,
                                        gpointer       user_data)
{
  AmlApplicationWindow *self = AML_APPLICATION_WINDOW (user_data);
  AmlApplication *app = AML_APPLICATION (g_application_get_default ());
  AmselEngine *engine = aml_application_get_engine (app);
  AmselEntry *entry;

  gint index = gtk_list_box_row_get_index (row);
  entry = g_list_model_get_item (G_LIST_MODEL (self->feedstore), index);

  amsel_entry_set_read (entry, TRUE);
  amsel_engine_mark_entry_read (engine, entry);
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
  gtk_widget_class_bind_template_child (widget_class, AmlApplicationWindow, gears);
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

  g_autoptr (GPtrArray) channels = amsel_engine_parse_finish (engine, res, &error);
  if (channels->len > 0) {
    gtk_stack_set_visible_child_full (self->stack, "feedview", GTK_STACK_TRANSITION_TYPE_CROSSFADE);
  }
}

static void
data_fetched (GObject      *source_object,
              GAsyncResult *res,
              gpointer      user_data)
{
  g_autofree gchar *raw;
  AmselRequest *request;
  AmselEngine *engine;
  AmlApplication *app;
  AmlApplicationWindow *self = AML_APPLICATION_WINDOW (user_data);

  app = AML_APPLICATION (gtk_window_get_application (GTK_WINDOW (self)));
  raw = aml_downloader_fetch_finish (AML_DOWNLOADER (source_object), res, NULL);

  gchar *url = g_task_get_task_data (G_TASK (res));

  request = amsel_request_new (raw, strlen (raw), url);

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

static void
refresh_feeds (GSimpleAction *action,
               GVariant      *parameter,
               gpointer       user_data)
{
  AmlApplicationWindow *self = AML_APPLICATION_WINDOW (user_data);
  AmlApplication *app = AML_APPLICATION (g_application_get_default ());
  AmselEngine *engine = aml_application_get_engine (app);

  g_autoptr(GPtrArray) channels = amsel_engine_get_channels (engine);

  for (int i = 0; i < channels->len; i++)
    {
      AmselChannel *c = g_ptr_array_index (channels, i);
      const gchar *url = amsel_channel_get_source (c);

      aml_downloader_fetch_async (self->downloader, url, NULL, data_fetched, self);
    }
}

static void
show_about (GSimpleAction *action,
            GVariant      *parameter,
            gpointer       user_data)
{
  AmlApplicationWindow *self = AML_APPLICATION_WINDOW (user_data);
  const gchar *authors[] = {"Günther Wagner", NULL};

  gtk_show_about_dialog (GTK_WINDOW (self),
                         "title", "About Amsel",
                         "authors", authors,
                         "license-type", GTK_LICENSE_GPL_3_0,
                         "logo-icon-name", "org.gnome.Amsel",
                         "copyright", "© 2018 Günther Wagner, et al.",
                         NULL);
}

static GtkWidget *
aml_application_window_create_row_channel (gpointer item,
                                           gpointer user_data)
{
  AmselEntry *e = AMSEL_ENTRY (item);

  g_autoptr(GtkBuilder) builder = gtk_builder_new_from_resource ("/org/gnome/Amsel/entry.ui");
  GtkWidget *row = GTK_WIDGET (gtk_builder_get_object (builder, "row"));
  GtkWidget *lbl_title = GTK_WIDGET (gtk_builder_get_object (builder, "lbl_title"));
  GtkWidget *lbl_author = GTK_WIDGET (gtk_builder_get_object (builder, "lbl_author"));

  const gchar *str = amsel_entry_get_title (e);
  const gchar *format;
  if (amsel_entry_get_read (e))
    format = "%s";
  else
    format = "<b>\%s</b>";
  g_autofree gchar *markup;

  markup = g_markup_printf_escaped (format, str);
  gtk_label_set_markup (GTK_LABEL (lbl_title), markup);
  const gchar *author = amsel_entry_get_author (e);
  if (author != NULL)
    gtk_label_set_text (GTK_LABEL (lbl_author), author);
  else
    gtk_widget_hide (lbl_author);

  return g_object_ref (row);
}

static void
aml_application_window_new_entry_cb (AmselCache *cache,
                                     AmselEntry *entry,
                                     gpointer    user_data)
{
  AmlApplicationWindow *self = AML_APPLICATION_WINDOW (user_data);

  g_assert (AMSEL_IS_MAIN_THREAD ());
  g_list_store_insert_sorted (self->feedstore, entry, (GCompareDataFunc) amsel_entry_sort, NULL);
}

static void
aml_application_window_set_shortcuts (AmlApplicationWindow *self)
{
  g_return_if_fail (AML_IS_APPLICATION_WINDOW (self));

  GApplication *app = g_application_get_default ();
  g_autoptr(GtkBuilder) builder;

  const gchar *accel_add[] = {"<Control>a", NULL};
  gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.add", accel_add);

  const gchar *accel_refresh[] = {"<Control>r", NULL};
  gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.refresh", accel_refresh);

  const gchar *accel_shortcuts[] = {"<Control><Shift>question", NULL};
  gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.show-help-overlay", accel_shortcuts);

  builder = gtk_builder_new_from_resource ("/org/gnome/Amsel/shortcuts.ui");
  GtkShortcutsWindow *shortcuts_window = GTK_SHORTCUTS_WINDOW (gtk_builder_get_object (builder, "shortcuts"));
  gtk_application_window_set_help_overlay (GTK_APPLICATION_WINDOW (self), shortcuts_window);
}

static void
aml_application_window_set_gears_menu (AmlApplicationWindow *self)
{
  g_autoptr(GtkBuilder) builder = gtk_builder_new_from_resource ("/org/gnome/Amsel/gears_menu.ui");
  GMenuModel *menu = G_MENU_MODEL (gtk_builder_get_object (builder, "gears-menu"));
  gtk_menu_button_set_menu_model (self->gears, menu);
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
  amsel_engine_connect_signal (engine, G_CALLBACK (aml_application_window_new_entry_cb), self);
  self->downloader = aml_downloader_new ();

  g_autoptr(GPtrArray) channels = amsel_engine_get_channels (engine);
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
            g_list_store_insert_sorted (self->feedstore, cur->data, (GCompareDataFunc) amsel_entry_sort, NULL);
          }
        g_list_free (entries);
      }
  }

  const GActionEntry win_entries[] = {
      { "add", add_feed },
      { "refresh", refresh_feeds },
      { "about", show_about },
  };

  g_action_map_add_action_entries (G_ACTION_MAP (self),
                                   win_entries,
                                   G_N_ELEMENTS (win_entries),
                                   self);
  aml_application_window_set_shortcuts (self);
  aml_application_window_set_gears_menu (self);
}
