/* alb-channel.c
 *
 * Copyright 2018 Günther Wagner <info@gunibert.de>
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#define G_LOG_DOMAIN "alb-channel"

#include "alb-channel.h"
#include "alb-debug.h"

/**
 * SECTION:amsel-channel
 * @title: AlbChannel
 * @short_description: represents a feed
 *
 *
 */

struct _AlbChannel
{
  GObject parent_instance;

  char *title;
  char *description;
  char *id;
  char *source;
  char *icon;

  GHashTable *entries;
};

G_DEFINE_TYPE (AlbChannel, alb_channel, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_TITLE,
  PROP_DESCRIPTION,
  PROP_ID,
  PROP_SOURCE,
  PROP_ICON,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

AlbChannel *
alb_channel_new (void)
{
  return g_object_new (ALB_TYPE_CHANNEL,
                       NULL);
}

static void
alb_channel_finalize (GObject *object)
{
  AlbChannel *self = (AlbChannel *)object;

  g_clear_pointer (&self->title, g_free);
  g_clear_pointer (&self->description, g_free);
  g_clear_pointer (&self->id, g_free);
  g_clear_pointer (&self->source, g_free);
  g_clear_pointer (&self->icon, g_free);

  g_hash_table_unref (self->entries);

  ALB_TRACE_MSG ("%s", "Finalize Channel");

  G_OBJECT_CLASS (alb_channel_parent_class)->finalize (object);
}

static void
alb_channel_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  AlbChannel *self = ALB_CHANNEL (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      g_value_set_string (value, alb_channel_get_title (self));
      break;
    case PROP_DESCRIPTION:
      g_value_set_string (value, alb_channel_get_description (self));
      break;
    case PROP_ID:
      g_value_set_string (value, alb_channel_get_id (self));
      break;
    case PROP_SOURCE:
      g_value_set_string (value, alb_channel_get_source (self));
      break;
    case PROP_ICON:
      g_value_set_string (value, alb_channel_get_icon (self));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
alb_channel_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  AlbChannel *self = ALB_CHANNEL (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      alb_channel_set_title (self, g_value_get_string (value));
      break;
    case PROP_DESCRIPTION:
      alb_channel_set_description (self, g_value_get_string (value));
      break;
    case PROP_ID:
      alb_channel_set_id (self, g_value_get_string (value));
      break;
    case PROP_SOURCE:
      alb_channel_set_source (self, g_value_get_string (value));
      break;
    case PROP_ICON:
      alb_channel_set_icon (self, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
alb_channel_class_init (AlbChannelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = alb_channel_finalize;
  object_class->get_property = alb_channel_get_property;
  object_class->set_property = alb_channel_set_property;

  properties [PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "Title",
                         "",
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  properties [PROP_DESCRIPTION] =
    g_param_spec_string ("description",
                         "Description",
                         "Description",
                         "",
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  properties [PROP_ID] =
    g_param_spec_string ("id",
                         "Id",
                         "Id",
                         "",
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  properties [PROP_SOURCE] =
    g_param_spec_string ("source",
                         "Source",
                         "Source",
                         "",
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  properties [PROP_ICON] =
    g_param_spec_string ("icon",
                         "Icon",
                         "Icon",
                         "",
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
alb_channel_init (AlbChannel *self)
{
  self->entries = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_object_unref);
}

void
alb_channel_set_title (AlbChannel *self,
                         const gchar  *title)
{
  g_return_if_fail (ALB_IS_CHANNEL (self));

  g_clear_pointer (&self->title, g_free);
  self->title = g_strdup (title);
}

const gchar *
alb_channel_get_title (AlbChannel *self)
{
  g_return_val_if_fail (ALB_IS_CHANNEL (self), NULL);

  return self->title;
}

void
alb_channel_set_description (AlbChannel *self,
                               const char   *description)
{
  g_return_if_fail (ALB_IS_CHANNEL (self));

  g_clear_pointer (&self->description, g_free);
  self->description = g_strdup (description);
}

const char *
alb_channel_get_description (AlbChannel *self)
{
  g_return_val_if_fail (ALB_IS_CHANNEL (self), NULL);

  return self->description;
}

void
alb_channel_set_id (AlbChannel *self,
                      const gchar  *id)
{
  g_return_if_fail (ALB_IS_CHANNEL (self));

  g_clear_pointer (&self->id, g_free);
  self->id = g_strdup (id);
}

const gchar *
alb_channel_get_id (AlbChannel *self)
{
  g_return_val_if_fail (ALB_IS_CHANNEL (self), NULL);

  return self->id;
}


void
alb_channel_set_source (AlbChannel *self,
                          const gchar  *source)
{
  g_return_if_fail (ALB_IS_CHANNEL (self));

  g_clear_pointer (&self->source, g_free);
  self->source = g_strdup (source);
}

/**
 * alb_channel_get_source:
 * @self: a #AlbChannel
 *
 * Get the origin of that feed
 *
 * Returns: Returns the origin of that feed
 */
const gchar *
alb_channel_get_source (AlbChannel *self)
{
  g_return_val_if_fail (ALB_IS_CHANNEL (self), NULL);

  return self->source;
}

/**
 * alb_channel_set_icon:
 * @self: a #AlbChannel
 * @icon: the address of the icon
 *
 * Sets the icon representation of the channel
 */
void
alb_channel_set_icon (AlbChannel *self,
                        const gchar  *icon)
{
  g_return_if_fail (ALB_IS_CHANNEL (self));

  g_clear_pointer (&self->icon, g_free);
  self->icon = g_strdup (icon);
}

/**
 * alb_channel_get_icon:
 * @self: a #AlbChannel
 *
 * Get the icon representation for the feed (if none is available in the feed
 * returns the favicon)
 *
 * Returns: the icon of the feed
 */
const gchar *
alb_channel_get_icon (AlbChannel *self)
{
  g_return_val_if_fail (ALB_IS_CHANNEL (self), NULL);

  return self->icon;
}

/**
 * alb_channel_add_entry:
 * @self: the #AlbChannel
 * @entry: (transfer full): the #AmselEntry to add
 */
void
alb_channel_add_entry (AlbChannel *self,
                         AlbEntry     *entry)
{
  g_return_if_fail (ALB_IS_CHANNEL (self));
  g_return_if_fail (ALB_IS_ENTRY (entry));

  g_hash_table_insert (self->entries, (char *)alb_entry_get_id (entry), entry);
}

/**
 * alb_channel_get_entries:
 * @self: a #AlbChannel
 *
 * get all #AlbEntry objects from #AlbChannel
 *
 * Returns: (transfer none) (element-type utf8 AlbEntry): the #AlbEntry objects
 */
GHashTable *
alb_channel_get_entries (AlbChannel *self)
{
  g_return_val_if_fail (ALB_IS_CHANNEL (self), NULL);

  return self->entries;
}
