#include "amsel-channel.h"
#include "amsel-debug.h"

/**
 * SECTION:amsel-channel
 * @title: AmselChannel
 * @short_description: represents a feed
 *
 *
 */

struct _AmselChannel
{
  GObject parent_instance;

  char *title;
  char *description;
  char *id;
  char *source;
  char *icon;

  GHashTable *entries;
};

G_DEFINE_TYPE (AmselChannel, amsel_channel, G_TYPE_OBJECT)

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

AmselChannel *
amsel_channel_new (void)
{
  return g_object_new (AMSEL_TYPE_CHANNEL,
                       NULL);
}

static void
amsel_channel_finalize (GObject *object)
{
  AmselChannel *self = (AmselChannel *)object;

  g_clear_pointer (&self->title, g_free);
  g_clear_pointer (&self->description, g_free);
  g_clear_pointer (&self->id, g_free);
  g_clear_pointer (&self->source, g_free);
  g_clear_pointer (&self->icon, g_free);

  g_hash_table_unref (self->entries);

  AM_TRACE_MSG ("%s", "Finalize Channel");

  G_OBJECT_CLASS (amsel_channel_parent_class)->finalize (object);
}

static void
amsel_channel_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  AmselChannel *self = AMSEL_CHANNEL (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      g_value_set_string (value, amsel_channel_get_title (self));
      break;
    case PROP_DESCRIPTION:
      g_value_set_string (value, amsel_channel_get_description (self));
      break;
    case PROP_ID:
      g_value_set_string (value, amsel_channel_get_id (self));
      break;
    case PROP_SOURCE:
      g_value_set_string (value, amsel_channel_get_source (self));
      break;
    case PROP_ICON:
      g_value_set_string (value, amsel_channel_get_icon (self));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_channel_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  AmselChannel *self = AMSEL_CHANNEL (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      amsel_channel_set_title (self, g_value_get_string (value));
      break;
    case PROP_DESCRIPTION:
      amsel_channel_set_description (self, g_value_get_string (value));
      break;
    case PROP_ID:
      amsel_channel_set_id (self, g_value_get_string (value));
      break;
    case PROP_SOURCE:
      amsel_channel_set_source (self, g_value_get_string (value));
      break;
    case PROP_ICON:
      amsel_channel_set_icon (self, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_channel_class_init (AmselChannelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_channel_finalize;
  object_class->get_property = amsel_channel_get_property;
  object_class->set_property = amsel_channel_set_property;

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
amsel_channel_init (AmselChannel *self)
{
  self->entries = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_object_unref);
}

void
amsel_channel_set_title (AmselChannel *self,
                         const gchar  *title)
{
  g_return_if_fail (AMSEL_IS_CHANNEL (self));

  g_clear_pointer (&self->title, g_free);
  self->title = g_strdup (title);
}

const gchar *
amsel_channel_get_title (AmselChannel *self)
{
  g_return_val_if_fail (AMSEL_IS_CHANNEL (self), NULL);

  return self->title;
}

void
amsel_channel_set_description (AmselChannel *self,
                               const char   *description)
{
  g_return_if_fail (AMSEL_IS_CHANNEL (self));

  g_clear_pointer (&self->description, g_free);
  self->description = g_strdup (description);
}

const char *
amsel_channel_get_description (AmselChannel *self)
{
  g_return_val_if_fail (AMSEL_IS_CHANNEL (self), NULL);

  return self->description;
}

void
amsel_channel_set_id (AmselChannel *self,
                      const gchar  *id)
{
  g_return_if_fail (AMSEL_IS_CHANNEL (self));

  g_clear_pointer (&self->id, g_free);
  self->id = g_strdup (id);
}

const gchar *
amsel_channel_get_id (AmselChannel *self)
{
  g_return_val_if_fail (AMSEL_IS_CHANNEL (self), NULL);

  return self->id;
}


void
amsel_channel_set_source (AmselChannel *self,
                          const gchar  *source)
{
  g_return_if_fail (AMSEL_IS_CHANNEL (self));

  g_clear_pointer (&self->source, g_free);
  self->source = g_strdup (source);
}

/**
 * amsel_channel_get_source:
 * @self: a #AmselChannel
 *
 * Get the origin of that feed
 *
 * Returns: Returns the origin of that feed
 */
const gchar *
amsel_channel_get_source (AmselChannel *self)
{
  g_return_val_if_fail (AMSEL_IS_CHANNEL (self), NULL);

  return self->source;
}

/**
 * amsel_channel_set_icon:
 * @self: a #AmselChannel
 * @icon: the address of the icon
 *
 * Sets the icon representation of the channel
 */
void
amsel_channel_set_icon (AmselChannel *self,
                        const gchar  *icon)
{
  g_return_if_fail (AMSEL_IS_CHANNEL (self));

  g_clear_pointer (&self->icon, g_free);
  self->icon = g_strdup (icon);
}

/**
 * amsel_channel_get_icon:
 * @self: a #AmselChannel
 *
 * Get the icon representation for the feed (if none is available in the feed
 * returns the favicon)
 *
 * Returns: the icon of the feed
 */
const gchar *
amsel_channel_get_icon (AmselChannel *self)
{
  g_return_val_if_fail (AMSEL_IS_CHANNEL (self), NULL);

  return self->icon;
}

/**
 * amsel_channel_add_entry:
 * @self: the #AmselChannel
 * @entry: (transfer full): the #AmselEntry to add
 */
void
amsel_channel_add_entry (AmselChannel *self,
                         AmselEntry   *entry)
{
  g_return_if_fail (AMSEL_IS_CHANNEL (self));
  g_return_if_fail (AMSEL_IS_ENTRY (entry));

  g_hash_table_insert (self->entries, (char *)amsel_entry_get_id (entry), g_object_ref(entry));
}

/**
 * amsel_channel_get_entries:
 * @self: a #AmselChannel
 *
 * get all #AmselEntry objects from #AmselChannel
 *
 * Returns: (transfer none) (element-type utf8 AmselEntry): the #AmselEntry objects
 */
GHashTable *
amsel_channel_get_entries (AmselChannel *self)
{
  g_return_val_if_fail (AMSEL_IS_CHANNEL (self), NULL);

  return self->entries;
}
