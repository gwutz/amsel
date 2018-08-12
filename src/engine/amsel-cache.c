#include "amsel-cache.h"
#include "amsel-debug.h"

GQuark
amsel_cache_error_quark (void)
{
  return g_quark_from_static_string ("amsel-cache-error-quark");
}

struct _AmselCache
{
  GObject parent_instance;

  GHashTable *channels;
  AmselDatabase *database;
};

G_DEFINE_TYPE (AmselCache, amsel_cache, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_DATABASE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

AmselCache *
amsel_cache_new (AmselDatabase *database)
{
  return g_object_new (AMSEL_TYPE_CACHE, "database", database, NULL);
}

static void
amsel_cache_finalize (GObject *object)
{
  AmselCache *self = (AmselCache *)object;

  AM_TRACE_MSG ("Finalize Cache...");

  g_hash_table_unref (self->channels);
  g_clear_object (&self->database);

  G_OBJECT_CLASS (amsel_cache_parent_class)->finalize (object);
}

static void
amsel_cache_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  AmselCache *self = AMSEL_CACHE (object);

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
amsel_cache_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  AmselCache *self = AMSEL_CACHE (object);

  switch (prop_id)
    {
    case PROP_DATABASE:
      g_clear_object (&self->database);
      self->database = g_value_dup_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_cache_class_init (AmselCacheClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_cache_finalize;
  object_class->get_property = amsel_cache_get_property;
  object_class->set_property = amsel_cache_set_property;

  properties [PROP_DATABASE] =
    g_param_spec_object ("database",
                         "Database",
                         "Database",
                         AMSEL_TYPE_DATABASE,
                         (G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (object_class, PROP_DATABASE,
                                   properties [PROP_DATABASE]);
}

static void
amsel_cache_init (AmselCache *self)
{
  self->channels = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_object_unref);
}

static void
amsel_cache_merge_channel_properties (AmselCache   *self,
                                      AmselChannel *existing,
                                      AmselChannel *new)
{
  if (!amsel_channel_get_icon (existing))
    {
      const char *icon = amsel_channel_get_icon (existing);
      if (icon)
        amsel_channel_set_icon (existing, icon);
    }

  if (!amsel_channel_get_title (existing))
    {
      const char *title = amsel_channel_get_title (existing);
      if (title)
        amsel_channel_set_title (existing, title);
    }

  if (!amsel_channel_get_description (existing))
    {
      const char *desc = amsel_channel_get_description (existing);
      if (desc)
        amsel_channel_set_description (existing, desc);
    }
}

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

  amsel_cache_merge_channel_properties (self, existing, new);
  GHashTable *entries = amsel_channel_get_entries (new);
  g_hash_table_iter_init (&iter, entries);

  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      AmselEntry *entry = value;
      amsel_cache_merge_entry (self, existing, entry);
    }
}

AmselChannel *
amsel_cache_add_channel (AmselCache    *self,
                         AmselChannel  *channel,
                         GError       **error)
{
  g_return_val_if_fail (AMSEL_IS_CACHE (self), channel);

  AM_TRACE_MSG ("%s", "Adding channel via Cache");

  GError *db_error = NULL;
  AmselChannel *existingChannel = NULL;
  const char *key = amsel_channel_get_source (channel);
  if (!key) {
    g_set_error (error,
                 AMSEL_CACHE_ERROR,
                 AMSEL_CACHE_ERROR_NO_SOURCE,
                 "No source for channel (title=%s). This should never been happen",
                 amsel_channel_get_title (channel));
    return NULL;
  }

  existingChannel = g_hash_table_lookup (self->channels, amsel_channel_get_source (channel));
  if (!existingChannel)
    {
      amsel_database_save_channel (self->database, channel, &db_error);
      if (db_error != NULL) {
        g_propagate_error (error, db_error);
        return NULL;
      }
      g_hash_table_insert (self->channels,
                           (char *) key,
                           g_object_ref(channel));
      return channel;
    }
  else
    {
      amsel_cache_merge_channels (self, existingChannel, channel);

      return existingChannel;
    }
}

GPtrArray *
amsel_cache_get_channels (AmselCache *self)
{
  g_return_val_if_fail (AMSEL_IS_CACHE (self), NULL);

  GPtrArray *ret = g_ptr_array_new ();

  GList *channel_list = g_hash_table_get_values (self->channels);
  for (GList *cur = channel_list; cur; cur = g_list_next (cur))
    {
      g_ptr_array_add (ret, cur->data);
    }
  g_list_free (channel_list);

  return ret;
}
