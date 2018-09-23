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

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define ALB_TYPE_ENTRY (alb_entry_get_type())

G_DECLARE_FINAL_TYPE (AlbEntry, alb_entry, ALB, ENTRY, GObject)
AlbEntry    *alb_entry_new                  (void);
void         alb_entry_set_id               (AlbEntry    *self,
                                             const gchar *id);
const gchar *alb_entry_get_id               (AlbEntry    *self);
void         alb_entry_set_title            (AlbEntry    *self,
                                             const gchar *title);
const gchar *alb_entry_get_title            (AlbEntry    *self);
void         alb_entry_set_content          (AlbEntry    *self,
                                             const gchar *content);
const gchar *alb_entry_get_content          (AlbEntry    *self);
void         alb_entry_set_updated          (AlbEntry    *self,
                                             const gchar *updated);
void         alb_entry_set_updated_datetime (AlbEntry    *self,
                                             GDateTime   *updated);
GDateTime   *alb_entry_get_updated          (AlbEntry    *self);
void         alb_entry_set_link             (AlbEntry    *self,
                                             const gchar *link);
const gchar *alb_entry_get_link             (AlbEntry    *self);
void         alb_entry_set_author           (AlbEntry    *self,
                                             const gchar *author);
const gchar *alb_entry_get_author           (AlbEntry    *self);
void         alb_entry_set_preview_image    (AlbEntry    *self,
                                             const gchar *preview_image);
const gchar *alb_entry_get_preview_image    (AlbEntry    *self);
void         alb_entry_set_read             (AlbEntry    *self,
                                             gboolean     read);
gboolean     alb_entry_get_read             (AlbEntry    *self);
gint         alb_entry_sort                 (AlbEntry    *self,
                                             AlbEntry    *other,
                                             gpointer     user_data);

G_END_DECLS
