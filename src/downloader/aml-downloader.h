/* aml-downloader.h
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

#pragma once

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define AML_TYPE_DOWNLOADER (aml_downloader_get_type())

G_DECLARE_FINAL_TYPE (AmlDownloader, aml_downloader, AML, DOWNLOADER, GObject)

AmlDownloader *aml_downloader_new          (void);
gchar         *aml_downloader_fetch        (AmlDownloader        *self,
                                            gchar                *url);
void           aml_downloader_fetch_async  (AmlDownloader        *self,
                                            const gchar          *url,
                                            GCancellable         *cancellable,
                                            GAsyncReadyCallback   callback,
                                            gpointer              user_data);
gchar         *aml_downloader_fetch_finish (AmlDownloader        *self,
                                            GAsyncResult         *result,
                                            GError              **error);
G_END_DECLS
