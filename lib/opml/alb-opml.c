/* alb-opml.c
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

#define G_LOG_DOMAIN "alb-opml"

#include "alb-opml.h"

struct _AlbOpml
{
  GObject parent_instance;

  GMarkupParser *parser;
  GMarkupParseContext *context;
  GList *urls;
};

G_DEFINE_TYPE (AlbOpml, alb_opml, G_TYPE_OBJECT)

AlbOpml *
alb_opml_new (void)
{
  return g_object_new (ALB_TYPE_OPML, NULL);
}

static void
alb_opml_finalize (GObject *object)
{
  AlbOpml *self = (AlbOpml *)object;

  g_clear_pointer (&self->context, g_markup_parse_context_unref);
  g_clear_pointer (&self->parser, g_free);

  G_OBJECT_CLASS (alb_opml_parent_class)->finalize (object);
}

static void
alb_opml_class_init (AlbOpmlClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = alb_opml_finalize;
}

static void
_start_element (GMarkupParseContext  *context,
                const gchar          *element_name,
                const gchar         **attribute_names,
                const gchar         **attribute_values,
                gpointer              user_data,
                GError              **error)
{
  AlbOpml *self = ALB_OPML (user_data);

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
alb_opml_init (AlbOpml *self)
{
  self->parser = g_new0 (GMarkupParser, 1);
  self->parser->start_element = _start_element;

  self->context = g_markup_parse_context_new (self->parser, G_MARKUP_TREAT_CDATA_AS_TEXT, self, NULL);
}

/**
 * alb_opml_parse:
 * @self: a #AlbOpml
 * @opml: the raw opml data
 *
 * parses provided opml data and returns urls
 *
 * Returns: (transfer full) (element-type gchar*): the list of urls found in opml
 */
GList *
alb_opml_parse (AlbOpml *self,
                  gchar     *opml)
{
  g_return_val_if_fail (ALB_IS_OPML (self), NULL);

  self->urls = NULL;

  g_markup_parse_context_parse (self->context, opml, -1, NULL);

  return self->urls;
}
