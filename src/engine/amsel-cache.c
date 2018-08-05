#include "amsel-cache.h"

struct _AmselCache
{
  GObject parent_instance;

  GHashTable *channels;
};

G_DEFINE_TYPE (AmselCache, amsel_cache, G_TYPE_OBJECT)

AmselCache *
amsel_cache_new (void)
{
  return g_object_new (AMSEL_TYPE_CACHE, NULL);
}

static void
amsel_cache_finalize (GObject *object)
{
  AmselCache *self = (AmselCache *)object;

  g_hash_table_unref (self->channels);

  G_OBJECT_CLASS (amsel_cache_parent_class)->finalize (object);
}

static void
amsel_cache_class_init (AmselCacheClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_cache_finalize;
}

static void
amsel_cache_init (AmselCache *self)
{
  self->channels = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_object_unref);
}

/* static void */
/* amsel_cache_merge_channel_properties (AmselCache   *self, */
/*                                       AmselChannel *existing, */
/*                                       AmselChannel *new) */
/* { */
/*   if (!amsel_channel_get_icon (existing)) */
/*     { */
/*       const char *icon = amsel_channel_get_icon (existing); */
/*       if (icon) */
/*         amsel_channel_set_icon (existing, icon); */
/*     } */

/*   if (!amsel_channel_get_title (existing)) */
/*     { */
/*       const char *title = amsel_channel_get_title (existing); */
/*       if (title) */
/*         amsel_channel_set_title (existing, title); */
/*     } */

/*   if (!amsel_channel_get_description (existing)) */
/*     { */
/*       const char *desc = amsel_channel_get_description (existing); */
/*       if (desc) */
/*         amsel_channel_set_description (existing, desc); */
/*     } */
/* } */

static gboolean
amsel_cache_merge_entry (AmselCache   *self,
                         AmselChannel *channel,
                         AmselEntry   *entry)
{
  GHashTable *entries = amsel_channel_get_entries (channel);

  const char *id = amsel_entry_get_id (entry);
  gboolean found = g_hash_table_contains (entries, id);
  if (!found) {
    g_hash_table_insert (entries, (char *)id, g_object_ref (entry));
  }

  return found;
}


static void
amsel_cache_merge_channels (AmselCache   *self,
                            AmselChannel *existing,
                            AmselChannel *new)
{
  g_return_if_fail (AMSEL_IS_CACHE (self));
  GHashTableIter iter;
  gpointer key, value;

  /* amsel_cache_merge_channel_properties (self, existing, new); */
  GHashTable *entries = amsel_channel_get_entries (new);
  g_hash_table_iter_init (&iter, entries);

  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      AmselEntry *entry = value;
      amsel_cache_merge_entry (self, existing, entry);
    }
}

AmselChannel *
amsel_cache_add_channel (AmselCache   *self,
                         AmselChannel *channel)
{
  g_return_val_if_fail (AMSEL_IS_CACHE (self), channel);

  AmselChannel *existingChannel = NULL;
  const char *key = amsel_channel_get_source (channel);
  if (!key)
    g_error ("%s", "This should never happen - got no identifier for a channel");

  existingChannel = g_hash_table_lookup (self->channels, amsel_channel_get_source (channel));
  if (!existingChannel)
    {
      g_hash_table_insert (self->channels,
                           (char *) amsel_channel_get_source (channel),
                           channel);
      return channel;
    }
  else
    {
      amsel_cache_merge_channels (self, existingChannel, channel);
      g_object_unref (channel);

      return existingChannel;
    }
}
