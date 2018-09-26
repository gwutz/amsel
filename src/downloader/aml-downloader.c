/* aml-downloader.c
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

#define G_LOG_DOMAIN "aml-downloader"

#include "aml-downloader.h"
#include <libsoup/soup.h>
#include "alb-debug.h"

struct _AmlDownloader
{
  GObject parent_instance;
  SoupSession *session;
};

G_DEFINE_TYPE (AmlDownloader, aml_downloader, G_TYPE_OBJECT)

AmlDownloader *
aml_downloader_new (void)
{
  return g_object_new (AML_TYPE_DOWNLOADER, NULL);
}

static void
aml_downloader_finalize (GObject *object)
{
  ALB_ENTER;
  AmlDownloader *self = AML_DOWNLOADER (object);

  g_clear_object (&self->session);

  G_OBJECT_CLASS (aml_downloader_parent_class)->finalize (object);
}

static void
aml_downloader_class_init (AmlDownloaderClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = aml_downloader_finalize;
}

static void
aml_downloader_init (AmlDownloader *self)
{
  self->session = soup_session_new ();
}

gchar *
aml_downloader_fetch (AmlDownloader *self,
                      gchar         *url)
{
  g_autoptr(SoupMessage) msg = NULL;
  gchar *data = NULL;

  g_return_val_if_fail (AML_IS_DOWNLOADER (self), NULL);

  msg = soup_message_new ("GET", url);
  soup_session_send_message (self->session, msg);

  data = g_strdup (msg->response_body->data);

  return data;
}

static void
aml_downloader_fetch_worker (GTask        *task,
                             gpointer      source_object,
                             gpointer      task_data,
                             GCancellable *cancellable)
{
  AmlDownloader *self = AML_DOWNLOADER (source_object);
  gchar *url = (gchar *) task_data;

  gchar *data = aml_downloader_fetch (self, url);
  g_task_return_pointer (task, data, g_free);
}

/**
 * aml_downloader_fetch_async:
 * @self: an #AmlDownloader
 * @url: the url to fetch
 * @cancellable: (nullable): a #GCancellable
 * @callback: a #GAsyncReadyCallback to execute upon completion
 * @user_data: closure data for @callback
 *
 */
void
aml_downloader_fetch_async (AmlDownloader       *self,
                            const gchar         *url,
                            GCancellable        *cancellable,
                            GAsyncReadyCallback  callback,
                            gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;

  g_return_if_fail (AML_IS_DOWNLOADER (self));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_source_tag (task, aml_downloader_fetch_async);
  g_task_set_task_data (task, (gchar *)url, NULL);

  g_task_run_in_thread (task, aml_downloader_fetch_worker);
}

/**
 * aml_downloader_fetch_finish:
 * @self: an #AmlDownloader
 * @result: a #GAsyncResult provided to callback
 * @error: a location for a #GError, or %NULL
 *
 * Returns:
 */
gchar *
aml_downloader_fetch_finish (AmlDownloader *self,
                             GAsyncResult *result,
                             GError **error)
{
  g_return_val_if_fail (AML_IS_DOWNLOADER (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (result), FALSE);

  return g_task_propagate_pointer (G_TASK (result), error);
}
