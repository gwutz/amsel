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

#include "amsel-entry.h"
#include "amsel-debug.h"

/**
 * SECTION:amsel-entry
 * @title: AmselEntry
 * @short_description: represents a feed item
 *
 *
 */

struct _AmselEntry
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

G_DEFINE_TYPE (AmselEntry, amsel_entry, G_TYPE_OBJECT)

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

AmselEntry *
amsel_entry_new (void)
{
  return g_object_new (AMSEL_TYPE_ENTRY, NULL);
}

static void
amsel_entry_finalize (GObject *object)
{
  AmselEntry *self = (AmselEntry *)object;

  g_clear_pointer (&self->id, g_free);
  g_clear_pointer (&self->title, g_free);
  g_clear_pointer (&self->content, g_free);
  g_clear_pointer (&self->updated, g_date_time_unref);
  g_clear_pointer (&self->link, g_free);
  g_clear_pointer (&self->preview_image, g_free);
  g_clear_pointer (&self->author, g_free);

  AM_TRACE_MSG ("%s", "Finalize Entry");

  G_OBJECT_CLASS (amsel_entry_parent_class)->finalize (object);
}

static void
amsel_entry_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  AmselEntry *self = AMSEL_ENTRY (object);

  switch (prop_id)
    {
    case PROP_ID:
      g_value_set_string (value, amsel_entry_get_id (self));
      break;
    case PROP_TITLE:
      g_value_set_string (value, amsel_entry_get_title (self));
      break;
    case PROP_CONTENT:
      g_value_set_string (value, amsel_entry_get_content (self));
      break;
    case PROP_UPDATED:
      g_value_set_boxed (value, amsel_entry_get_updated (self));
      break;
    case PROP_LINK:
      g_value_set_string (value, amsel_entry_get_link (self));
      break;
    case PROP_PREVIEW_IMAGE:
      g_value_set_string (value, amsel_entry_get_preview_image (self));
      break;
    case PROP_READ:
      g_value_set_boolean (value, amsel_entry_get_read (self));
      break;
    case PROP_AUTHOR:
      g_value_set_string (value, amsel_entry_get_author (self));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_entry_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  AmselEntry *self = AMSEL_ENTRY (object);

  switch (prop_id)
    {
    case PROP_ID:
      amsel_entry_set_id (self, g_value_get_string (value));
      break;
    case PROP_TITLE:
      amsel_entry_set_title (self, g_value_get_string (value));
      break;
    case PROP_CONTENT:
      amsel_entry_set_content (self, g_value_get_string (value));
      break;
    case PROP_UPDATED:
      amsel_entry_set_updated (self, g_value_get_string (value));
      break;
    case PROP_LINK:
      amsel_entry_set_link (self, g_value_get_string (value));
      break;
    case PROP_PREVIEW_IMAGE:
      amsel_entry_set_preview_image (self, g_value_get_string (value));
      break;
    case PROP_READ:
      amsel_entry_set_read (self, g_value_get_boolean (value));
      break;
    case PROP_AUTHOR:
      amsel_entry_set_author (self, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_entry_class_init (AmselEntryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_entry_finalize;
  object_class->get_property = amsel_entry_get_property;
  object_class->set_property = amsel_entry_set_property;

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
amsel_entry_init (AmselEntry *self)
{
}

void
amsel_entry_set_id (AmselEntry  *self,
                    const gchar *id)
{
  g_return_if_fail (AMSEL_IS_ENTRY (self));

  g_clear_pointer (&self->id, g_free);
  self->id = g_strdup (id);
}

const gchar *
amsel_entry_get_id (AmselEntry *self)
{
  g_return_val_if_fail (AMSEL_IS_ENTRY (self), NULL);

  return self->id;
}

void
amsel_entry_set_title (AmselEntry  *self,
                       const gchar *title)
{
  g_return_if_fail (AMSEL_IS_ENTRY (self));

  g_clear_pointer (&self->title, g_free);
  self->title = g_strdup (title);
}

const gchar *
amsel_entry_get_title (AmselEntry *self)
{
  g_return_val_if_fail (AMSEL_IS_ENTRY (self), NULL);

  return self->title;
}

void
amsel_entry_set_content (AmselEntry  *self,
                         const gchar *content)
{
  g_return_if_fail (AMSEL_IS_ENTRY (self));

  g_clear_pointer (&self->content, g_free);
  self->content = g_strdup (content);
}

const gchar *
amsel_entry_get_content (AmselEntry *self)
{
  g_return_val_if_fail (AMSEL_IS_ENTRY (self), NULL);

  return self->content;
}

void
amsel_entry_set_updated (AmselEntry  *self,
                         const gchar *updated)
{
  g_return_if_fail (AMSEL_IS_ENTRY (self));

  g_clear_pointer (&self->updated, g_date_time_unref);
  self->updated = g_date_time_new_from_iso8601 (updated, NULL);
}

void
amsel_entry_set_updated_datetime (AmselEntry *self,
                                  GDateTime  *updated)
{
  g_return_if_fail (AMSEL_IS_ENTRY (self));

  g_clear_pointer (&self->updated, g_date_time_unref);
  self->updated = g_date_time_ref (updated);
}

GDateTime *
amsel_entry_get_updated (AmselEntry *self)
{
  g_return_val_if_fail (AMSEL_IS_ENTRY (self), NULL);

  return self->updated;
}

// display
void
amsel_entry_set_preview_image (AmselEntry  *self,
                               const gchar *preview_image)
{
  g_return_if_fail (AMSEL_IS_ENTRY (self));

  g_clear_pointer (&self->preview_image, g_free);
  self->preview_image = g_strdup (preview_image);
}

const gchar *
amsel_entry_get_preview_image (AmselEntry *self)
{
  g_return_val_if_fail (AMSEL_IS_ENTRY (self), NULL);

  return self->preview_image;
}

void
amsel_entry_set_read (AmselEntry *self,
                      gboolean    read)
{
  g_return_if_fail (AMSEL_IS_ENTRY (self));

  self->read = read;
}

gboolean
amsel_entry_get_read (AmselEntry *self)
{
  g_return_val_if_fail (AMSEL_IS_ENTRY (self), FALSE);

  return self->read;
}

void
amsel_entry_set_link (AmselEntry  *self,
                      const gchar *link)
{
  g_return_if_fail (AMSEL_IS_ENTRY (self));

  g_clear_pointer (&self->link, g_free);
  self->link = g_strdup (link);
}

const gchar *
amsel_entry_get_link (AmselEntry *self)
{
  g_return_val_if_fail (AMSEL_IS_ENTRY (self), NULL);

  return self->link;
}

void
amsel_entry_set_author (AmselEntry  *self,
                        const gchar *author)
{
  g_return_if_fail (AMSEL_IS_ENTRY (self));

  g_clear_pointer (&self->author, g_free);
  self->author = g_strdup (author);
}

const gchar *
amsel_entry_get_author (AmselEntry *self)
{
  g_return_val_if_fail (AMSEL_IS_ENTRY (self), NULL);

  return self->author;
}

gint
amsel_entry_sort (AmselEntry *self,
                  AmselEntry *other,
                  gpointer    user_data)
{
  GDateTime *datetime_a = amsel_entry_get_updated (self);
  GDateTime *datetime_b = amsel_entry_get_updated (other);

  return -g_date_time_compare (datetime_a, datetime_b);
}
