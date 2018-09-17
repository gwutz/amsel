#include "amsel-opml.h"

struct _AmselOpml
{
  GObject parent_instance;

  GMarkupParser *parser;
  GMarkupParseContext *context;
  GList *urls;
};

G_DEFINE_TYPE (AmselOpml, amsel_opml, G_TYPE_OBJECT)

AmselOpml *
amsel_opml_new (void)
{
  return g_object_new (AMSEL_TYPE_OPML, NULL);
}

static void
amsel_opml_finalize (GObject *object)
{
  AmselOpml *self = (AmselOpml *)object;

  g_clear_pointer (&self->context, g_markup_parse_context_unref);
  g_clear_pointer (&self->parser, g_free);

  G_OBJECT_CLASS (amsel_opml_parent_class)->finalize (object);
}

static void
amsel_opml_class_init (AmselOpmlClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_opml_finalize;
}

static void
_start_element (GMarkupParseContext  *context,
                const gchar          *element_name,
                const gchar         **attribute_names,
                const gchar         **attribute_values,
                gpointer              user_data,
                GError              **error)
{
  AmselOpml *self = AMSEL_OPML (user_data);

  if (g_strcmp0 ("outline", element_name) == 0)
    {
      GHashTable *table = g_hash_table_new (g_str_hash, g_str_equal);
      while (*attribute_names != NULL) {
        const gchar *name = *attribute_names;
        const gchar *value = *attribute_values;
        g_hash_table_insert (table, (gchar *)name, (gchar *)value);
        attribute_names++;
        attribute_values++;
      }

      gchar *type = g_hash_table_lookup (table, "type");
      if (g_strcmp0 (type, "rss") == 0 ||
          g_strcmp0 (type, "atom") == 0)
        {
          gchar *url = g_hash_table_lookup (table, "xmlUrl");
          self->urls = g_list_append (self->urls, g_strdup (url));
        }
      else
        {
          gchar *url = g_hash_table_lookup (table, "xmlUrl");
          if (url != NULL)
            self->urls = g_list_append (self->urls, g_strdup (url));
        }
      g_hash_table_unref (table);
    }
}

static void
amsel_opml_init (AmselOpml *self)
{
  self->parser = g_new0 (GMarkupParser, 1);
  self->parser->start_element = _start_element;

  self->context = g_markup_parse_context_new (self->parser, G_MARKUP_TREAT_CDATA_AS_TEXT, self, NULL);
}

/**
 * amsel_opml_parse:
 * @self: a #AmselOpml
 * @opml: the raw opml data
 *
 * parses provided opml data and returns urls
 *
 * Returns: (transfer full) (element-type gchar*): the list of urls found in opml
 */
GList *
amsel_opml_parse (AmselOpml *self,
                  gchar     *opml)
{
  g_return_val_if_fail (AMSEL_IS_OPML (self), NULL);

  self->urls = NULL;

  g_markup_parse_context_parse (self->context, opml, -1, NULL);

  return self->urls;
}
