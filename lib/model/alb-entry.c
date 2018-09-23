/* amsel-entry.c
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
#define G_LOG_DOMAIN "amsel-entry"

#include "alb-entry.h"
#include "alb-debug.h"

/**
 * SECTION:amsel-entry
 * @title: AlbEntry
 * @short_description: represents a feed item
 *
 *
 */

struct _AlbEntry
{
  GObject parent_instance;

  gchar *id;
  gchar *title;
  gchar *content;
  GDateTime *updated;
  gchar *link;
  gchar *author;

  // display
  gchar *preview_image;
  gboolean read;
};

G_DEFINE_TYPE (AlbEntry, alb_entry, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_ID,
  PROP_TITLE,
  PROP_CONTENT,
  PROP_UPDATED,
  PROP_LINK,
  PROP_PREVIEW_IMAGE,
  PROP_READ,
  PROP_AUTHOR,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

AlbEntry *
alb_entry_new (void)
{
  return g_object_new (ALB_TYPE_ENTRY, NULL);
}

static void
alb_entry_finalize (GObject *object)
{
  AlbEntry *self = (AlbEntry *)object;

  g_clear_pointer (&self->id, g_free);
  g_clear_pointer (&self->title, g_free);
  g_clear_pointer (&self->content, g_free);
  g_clear_pointer (&self->updated, g_date_time_unref);
  g_clear_pointer (&self->link, g_free);
  g_clear_pointer (&self->preview_image, g_free);
  g_clear_pointer (&self->author, g_free);

  AM_TRACE_MSG ("%s", "Finalize Entry");

  G_OBJECT_CLASS (alb_entry_parent_class)->finalize (object);
}

static void
alb_entry_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  AlbEntry *self = ALB_ENTRY (object);

  switch (prop_id)
    {
    case PROP_ID:
      g_value_set_string (value, alb_entry_get_id (self));
      break;
    case PROP_TITLE:
      g_value_set_string (value, alb_entry_get_title (self));
      break;
    case PROP_CONTENT:
      g_value_set_string (value, alb_entry_get_content (self));
      break;
    case PROP_UPDATED:
      g_value_set_boxed (value, alb_entry_get_updated (self));
      break;
    case PROP_LINK:
      g_value_set_string (value, alb_entry_get_link (self));
      break;
    case PROP_PREVIEW_IMAGE:
      g_value_set_string (value, alb_entry_get_preview_image (self));
      break;
    case PROP_READ:
      g_value_set_boolean (value, alb_entry_get_read (self));
      break;
    case PROP_AUTHOR:
      g_value_set_string (value, alb_entry_get_author (self));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
alb_entry_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  AlbEntry *self = ALB_ENTRY (object);

  switch (prop_id)
    {
    case PROP_ID:
      alb_entry_set_id (self, g_value_get_string (value));
      break;
    case PROP_TITLE:
      alb_entry_set_title (self, g_value_get_string (value));
      break;
    case PROP_CONTENT:
      alb_entry_set_content (self, g_value_get_string (value));
      break;
    case PROP_UPDATED:
      alb_entry_set_updated (self, g_value_get_string (value));
      break;
    case PROP_LINK:
      alb_entry_set_link (self, g_value_get_string (value));
      break;
    case PROP_PREVIEW_IMAGE:
      alb_entry_set_preview_image (self, g_value_get_string (value));
      break;
    case PROP_READ:
      alb_entry_set_read (self, g_value_get_boolean (value));
      break;
    case PROP_AUTHOR:
      alb_entry_set_author (self, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
alb_entry_class_init (AlbEntryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = alb_entry_finalize;
  object_class->get_property = alb_entry_get_property;
  object_class->set_property = alb_entry_set_property;

  properties [PROP_ID] =
    g_param_spec_string ("id",
                         "Id",
                         "Id",
                         "",
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  properties [PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "Title",
                         "",
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  properties [PROP_CONTENT] =
    g_param_spec_string ("content",
                         "Content",
                         "Content",
                         "",
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  properties [PROP_UPDATED] =
    g_param_spec_boxed ("updated",
                        "Updated",
                        "Updated",
                        G_TYPE_DATE_TIME,
                        (G_PARAM_READWRITE |
                         G_PARAM_STATIC_STRINGS));

  properties [PROP_LINK] =
    g_param_spec_string ("link",
                         "Link",
                         "Link",
                         "",
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  properties [PROP_PREVIEW_IMAGE] =
    g_param_spec_string ("preview-image",
                         "PreviewImage",
                         "PreviewImage",
                         "",
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  properties [PROP_READ] =
    g_param_spec_boolean ("read",
                          "Read",
                          "Read",
                          FALSE,
                          (G_PARAM_READWRITE |
                           G_PARAM_STATIC_STRINGS));

  properties [PROP_AUTHOR] =
    g_param_spec_string ("author",
                         "Author",
                         "Author",
                         "",
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
alb_entry_init (AlbEntry *self)
{
}

void
alb_entry_set_id (AlbEntry  *self,
                    const gchar *id)
{
  g_return_if_fail (ALB_IS_ENTRY (self));

  g_clear_pointer (&self->id, g_free);
  self->id = g_strdup (id);
}

const gchar *
alb_entry_get_id (AlbEntry *self)
{
  g_return_val_if_fail (ALB_IS_ENTRY (self), NULL);

  return self->id;
}

void
alb_entry_set_title (AlbEntry  *self,
                       const gchar *title)
{
  g_return_if_fail (ALB_IS_ENTRY (self));

  g_clear_pointer (&self->title, g_free);
  self->title = g_strdup (title);
}

const gchar *
alb_entry_get_title (AlbEntry *self)
{
  g_return_val_if_fail (ALB_IS_ENTRY (self), NULL);

  return self->title;
}

void
alb_entry_set_content (AlbEntry  *self,
                         const gchar *content)
{
  g_return_if_fail (ALB_IS_ENTRY (self));

  g_clear_pointer (&self->content, g_free);
  self->content = g_strdup (content);
}

const gchar *
alb_entry_get_content (AlbEntry *self)
{
  g_return_val_if_fail (ALB_IS_ENTRY (self), NULL);

  return self->content;
}

void
alb_entry_set_updated (AlbEntry  *self,
                         const gchar *updated)
{
  g_return_if_fail (ALB_IS_ENTRY (self));

  g_clear_pointer (&self->updated, g_date_time_unref);
  self->updated = g_date_time_new_from_iso8601 (updated, NULL);
}

void
alb_entry_set_updated_datetime (AlbEntry *self,
                                  GDateTime  *updated)
{
  g_return_if_fail (ALB_IS_ENTRY (self));

  g_clear_pointer (&self->updated, g_date_time_unref);
  self->updated = g_date_time_ref (updated);
}

GDateTime *
alb_entry_get_updated (AlbEntry *self)
{
  g_return_val_if_fail (ALB_IS_ENTRY (self), NULL);

  return self->updated;
}

// display
void
alb_entry_set_preview_image (AlbEntry  *self,
                               const gchar *preview_image)
{
  g_return_if_fail (ALB_IS_ENTRY (self));

  g_clear_pointer (&self->preview_image, g_free);
  self->preview_image = g_strdup (preview_image);
}

const gchar *
alb_entry_get_preview_image (AlbEntry *self)
{
  g_return_val_if_fail (ALB_IS_ENTRY (self), NULL);

  return self->preview_image;
}

void
alb_entry_set_read (AlbEntry *self,
                      gboolean    read)
{
  g_return_if_fail (ALB_IS_ENTRY (self));

  self->read = read;
}

gboolean
alb_entry_get_read (AlbEntry *self)
{
  g_return_val_if_fail (ALB_IS_ENTRY (self), FALSE);

  return self->read;
}

void
alb_entry_set_link (AlbEntry  *self,
                      const gchar *link)
{
  g_return_if_fail (ALB_IS_ENTRY (self));

  g_clear_pointer (&self->link, g_free);
  self->link = g_strdup (link);
}

const gchar *
alb_entry_get_link (AlbEntry *self)
{
  g_return_val_if_fail (ALB_IS_ENTRY (self), NULL);

  return self->link;
}

void
alb_entry_set_author (AlbEntry  *self,
                        const gchar *author)
{
  g_return_if_fail (ALB_IS_ENTRY (self));

  g_clear_pointer (&self->author, g_free);
  self->author = g_strdup (author);
}

const gchar *
alb_entry_get_author (AlbEntry *self)
{
  g_return_val_if_fail (ALB_IS_ENTRY (self), NULL);

  return self->author;
}

gint
alb_entry_sort (AlbEntry *self,
                  AlbEntry *other,
                  gpointer    user_data)
{
  GDateTime *datetime_a = alb_entry_get_updated (self);
  GDateTime *datetime_b = alb_entry_get_updated (other);

  return -g_date_time_compare (datetime_a, datetime_b);
}
