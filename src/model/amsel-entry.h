/* amsel-entry.h
 *
 * Copyright 2018 Guenther Wagner <info@gunibert.de>
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

G_BEGIN_DECLS

#define AMSEL_TYPE_ENTRY (amsel_entry_get_type())

G_DECLARE_FINAL_TYPE (AmselEntry, amsel_entry, AMSEL, ENTRY, GObject)

AmselEntry  *amsel_entry_new                  (void);
void         amsel_entry_set_id               (AmselEntry  *self,
                                               const gchar *id);
const gchar *amsel_entry_get_id               (AmselEntry  *self);
void         amsel_entry_set_title            (AmselEntry  *self,
                                               const gchar *title);
const gchar *amsel_entry_get_title            (AmselEntry  *self);
void         amsel_entry_set_content          (AmselEntry  *self,
                                               const gchar *content);
const gchar *amsel_entry_get_content          (AmselEntry  *self);
void         amsel_entry_set_updated          (AmselEntry  *self,
                                               const gchar *updated);
void         amsel_entry_set_updated_datetime (AmselEntry  *self,
                                               GDateTime   *updated);
GDateTime   *amsel_entry_get_updated          (AmselEntry  *self);
void         amsel_entry_set_link             (AmselEntry  *self,
                                               const gchar *link);
const gchar *amsel_entry_get_link             (AmselEntry  *self);
void         amsel_entry_set_author           (AmselEntry  *self,
                                               const gchar *author);
const gchar *amsel_entry_get_author           (AmselEntry  *self);

void         amsel_entry_set_preview_image    (AmselEntry  *self,
                                               const gchar *preview_image);
const gchar *amsel_entry_get_preview_image    (AmselEntry  *self);
void         amsel_entry_set_read             (AmselEntry  *self,
                                               gboolean     read);
gboolean     amsel_entry_get_read             (AmselEntry  *self);

G_END_DECLS
