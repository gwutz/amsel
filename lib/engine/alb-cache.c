/* alb-cache.c
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

#define G_LOG_DOMAIN "alb-cache"

#include "alb-cache.h"
#include "alb-debug.h"

/**
 * SECTION:amsel-cache
 * @title: AlbCache
 * @short_description: in memory cache implementation for #AlbChannel
 *
 *
 */

GQuark
alb_cache_error_quark (void)
{
  return g_quark_from_static_string ("amsel-cache-error-quark");
}

struct _AlbCache
{
  GObject parent_instance;

  GHashTable *channels;
  AlbDatabase *database;
};

G_DEFINE_TYPE (AlbCache, alb_cache, G_TYPE_OBJECT)

enum {
  NEW_ENTRY,
  LAST_SIGNAL
};

static guint signals [LAST_SIGNAL] = {0};

enum
{
  PROP_0,
  PROP_DATABASE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

AlbCache *
alb_cache_new (AlbDatabase *database)
{
  return g_object_new (ALB_TYPE_CACHE, "database", database, NULL);
}

static void
alb_cache_constructed (GObject *object)
{
  AlbCache *self = ALB_CACHE (object);

  g_autoptr(GPtrArray) channels = alb_database_get_channels (self->database);
  if (channels) {
    for (int i = 0; i < channels->len; i++)
      {
        AlbChannel *c = ALB_CHANNEL (g_ptr_array_index (channels, i));
        const char *key = alb_channel_get_source (c);
        g_hash_table_insert (self->channels, (gchar *) key, c);
      }
  }

  G_OBJECT_CLASS (alb_cache_parent_class)->constructed (object);
}

static void
alb_cache_finalize (GObject *object)
{
  AlbCache *self = (AlbCache *)object;

  ALB_TRACE_MSG ("Finalize Cache...");

  g_hash_table_unref (self->channels);
  g_clear_object (&self->database);

  G_OBJECT_CLASS (alb_cache_parent_class)->finalize (object);
}

static void
alb_cache_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  AlbCache *self = ALB_CACHE (object);

  switch (prop_id)
    {
    case PROP_DATABASE:
      g_value_set_object (value, self->database);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
alb_cache_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  AlbCache *self = ALB_CACHE (object);

  switch (prop_id)
    {
    case PROP_DATABASE:
      g_clear_object (&self->database);
      self->database = g_value_get_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
alb_cache_class_init (AlbCacheClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = alb_cache_constructed;
  object_class->finalize = alb_cache_finalize;
  object_class->get_property = alb_cache_get_property;
  object_class->set_property = alb_cache_set_property;

  signals [NEW_ENTRY] =
    g_signal_new ("new-entry",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL,
                  NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE,
                  1,
                  ALB_TYPE_ENTRY);

  properties [PROP_DATABASE] =
    g_param_spec_object ("database",
                         "Database",
                         "Database",
                         ALB_TYPE_DATABASE,
                         (G_PARAM_READWRITE |
                          G_PARAM_CONSTRUCT |
                          G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (object_class, PROP_DATABASE,
                                   properties [PROP_DATABASE]);
}

static void
alb_cache_init (AlbCache *self)
{
  self->channels = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_object_unref);
}

static void
alb_cache_merge_channel_properties (AlbCache   *self,
                                      AlbChannel *existing,
                                      AlbChannel *new)
{
  if (!alb_channel_get_icon (existing))
    {
      const char *icon = alb_channel_get_icon (existing);
      if (icon)
        alb_channel_set_icon (existing, icon);
    }

  if (!alb_channel_get_title (existing))
    {
      const char *title = alb_channel_get_title (existing);
      if (title)
        alb_channel_set_title (existing, title);
    }

  if (!alb_channel_get_description (existing))
    {
      const char *desc = alb_channel_get_description (existing);
      if (desc)
        alb_channel_set_description (existing, desc);
    }
}

static gboolean
alb_cache_merge_entry (AlbCache   *self,
                         AlbChannel *channel,
                         AlbEntry   *entry)
{
  GHashTable *entries = alb_channel_get_entries (channel);

  const char *id = alb_entry_get_id (entry);
  gboolean found = g_hash_table_contains (entries, id);
  if (!found) {
    g_hash_table_insert (entries, (char *)id, g_object_ref (entry));
    alb_database_save_entry (self->database, channel, entry, NULL);
    g_signal_emit (self, signals[NEW_ENTRY], 0, entry);
  }

  return found;
}


static void
alb_cache_merge_channels (AlbCache   *self,
                            AlbChannel *existing,
                            AlbChannel *new)
{
  g_return_if_fail (ALB_IS_CACHE (self));
  GHashTableIter iter;
  gpointer key, value;

  alb_cache_merge_channel_properties (self, existing, new);
  GHashTable *entries = alb_channel_get_entries (new);
  g_hash_table_iter_init (&iter, entries);

  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      AlbEntry *entry = value;
      alb_cache_merge_entry (self, existing, entry);
    }
}

/**
 * alb_cache_add_channel:
 * @self: an #AlbCache
 * @channel: an #AlbChannel
 * @error: a #GError
 *
 * Adds the #AlbChannel to the provided database and saves it along in a
 * memory cache. If there is already a #AlbChannel with the same #AlbChannel::source
 * it merges the new items to the existing channel and returns the existing
 * #AlbChannel back to the caller.
 *
 * Returns: (transfer full): the new #AlbChannel or an existing one or %NULL
 */
AlbChannel *
alb_cache_add_channel (AlbCache    *self,
                         AlbChannel  *channel,
                         GError       **error)
{
  g_return_val_if_fail (ALB_IS_CACHE (self), channel);

  ALB_TRACE_MSG ("%s", "Adding channel via Cache");

  GError *db_error = NULL;
  AlbChannel *existingChannel = NULL;
  const char *key = alb_channel_get_id (channel);
  if (!key) {
    g_set_error (error,
                 ALB_CACHE_ERROR,
                 ALB_CACHE_ERROR_NO_ID,
                 "No id for channel (title=%s). This should never been happen",
                 alb_channel_get_title (channel));
    return NULL;
  }

  existingChannel = g_hash_table_lookup (self->channels, alb_channel_get_id (channel));
  if (!existingChannel)
    {
      alb_database_save_channel (self->database, channel, &db_error);
      if (db_error != NULL) {
        g_propagate_error (error, db_error);
        return NULL;
      }
      g_hash_table_insert (self->channels,
                           (char *) key,
                           g_object_ref(channel));

      // announce the new entries to possible listener
      GHashTableIter iter;
      gpointer key, value;
      GHashTable *entries = alb_channel_get_entries (channel);
      g_hash_table_iter_init (&iter, entries);
      while (g_hash_table_iter_next (&iter, &key, &value))
        {
          g_signal_emit (self, signals[NEW_ENTRY], 0, value);
        }

      return channel;
    }
  else
    {
      alb_cache_merge_channels (self, existingChannel, channel);

      g_object_unref (channel);
      return existingChannel;
    }
}

/**
 * alb_cache_get_channels:
 * @self: an #AlbCache
 *
 *
 * Returns: (transfer container) (element-type AlbChannel):
 */
GPtrArray *
alb_cache_get_channels (AlbCache *self)
{
  g_return_val_if_fail (ALB_IS_CACHE (self), NULL);

  GPtrArray *ret = g_ptr_array_new ();

  GList *channel_list = g_hash_table_get_values (self->channels);
  for (GList *cur = channel_list; cur; cur = g_list_next (cur))
    {
      g_ptr_array_add (ret, cur->data);
    }
  g_list_free (channel_list);

  return ret;
}

void
alb_cache_mark_read (AlbCache *self,
                       AlbEntry *entry)
{
  g_return_if_fail (ALB_IS_CACHE (self));

  ALB_TRACE_MSG ("mark '%s' as read", alb_entry_get_title (entry));

  alb_database_set_read (self->database, entry, NULL);
}
