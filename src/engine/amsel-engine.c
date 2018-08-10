#include "amsel-engine.h"
#include "amsel-validator.h"
#include "amsel-validator-rss.h"
#include "amsel-validator-atom.h"
#include "amsel-parser.h"
#include "amsel-parser-rss.h"
#include "amsel-parser-atom.h"
#include "amsel-cache.h"
#include "amsel-sqlite-database.h"
#include "amsel-debug.h"

struct _AmselEngine
{
  GObject parent_instance;
  GList *validators;
  GHashTable *parsers;

  AmselCache *cache;
};

G_DEFINE_TYPE (AmselEngine, amsel_engine, G_TYPE_OBJECT)

AmselEngine *
amsel_engine_new (void)
{
  return g_object_new (AMSEL_TYPE_ENGINE, NULL);
}

static void
amsel_engine_finalize (GObject *object)
{
  AmselEngine *self = (AmselEngine *)object;

  g_list_free_full (self->validators, g_object_unref);
  g_clear_object (&self->cache);
  g_hash_table_unref (self->parsers);

  G_OBJECT_CLASS (amsel_engine_parent_class)->finalize (object);
}

static void
amsel_engine_class_init (AmselEngineClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_engine_finalize;
  /* object_class->get_property = amsel_engine_get_property; */
  /* object_class->set_property = amsel_engine_set_property; */
}

static void
amsel_engine_init (AmselEngine *self)
{
  self->validators = g_list_append (self->validators, amsel_validator_rss_new ());
  self->validators = g_list_append (self->validators, amsel_validator_atom_new ());

  self->parsers = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_object_unref);
  g_hash_table_insert (self->parsers, GINT_TO_POINTER (AMSEL_REQUEST_TYPE_RSS), amsel_parser_rss_new ());
  g_hash_table_insert (self->parsers, GINT_TO_POINTER (AMSEL_REQUEST_TYPE_ATOM), amsel_parser_atom_new ());

  g_autoptr (AmselDatabase) database = AMSEL_DATABASE (amsel_sqlite_database_new ("amsel.db"));
  self->cache = amsel_cache_new (database);
}

gboolean
amsel_engine_validate (AmselEngine  *self,
                       AmselRequest *request)
{
  g_return_val_if_fail (AMSEL_IS_ENGINE (self), FALSE);
  g_return_val_if_fail (request != NULL, FALSE);
  g_return_val_if_fail (amsel_request_get_data (request) != NULL, FALSE);

  gboolean valid = FALSE;

  for (GList *cur = self->validators; cur; cur = g_list_next (cur))
    {
      AmselValidator *validator = AMSEL_VALIDATOR (cur->data);
      valid = amsel_validator_validate (validator, amsel_request_get_data (request));

      if (valid) {
        if (AMSEL_IS_VALIDATOR_RSS (validator)) {
          amsel_request_set_type (request, AMSEL_REQUEST_TYPE_RSS);
        } else if (AMSEL_IS_VALIDATOR_ATOM (validator)) {
          amsel_request_set_type (request, AMSEL_REQUEST_TYPE_ATOM);
        }
        break;
      }

    }

  return valid;
}

/**
 * amsel_engine_parse:
 *
 * @self: the #AmselEngine
 * @request: the #AmselRequest
 *
 * Returns: the #AmselChannel with the combined #AmselEntry's from #AmselCache
 */
GPtrArray *
amsel_engine_parse (AmselEngine  *self,
                    AmselRequest *request)
{
  g_return_val_if_fail (AMSEL_IS_ENGINE (self), NULL);
  g_return_val_if_fail (request != NULL, NULL);
  g_return_val_if_fail (amsel_request_get_data (request) != NULL, NULL);

  GError *error = NULL;
  GPtrArray *channels;
  GPtrArray *ret = g_ptr_array_new_with_free_func (NULL);

  AmselParser *parser = g_hash_table_lookup (self->parsers, GINT_TO_POINTER (amsel_request_get_type (request)));
  if (parser == NULL) {
    return NULL;
  }

  channels = amsel_parser_parse (AMSEL_PARSER (parser), request);
  for (int i = 0; i < channels->len; i++)
    {
      AmselChannel *channel = g_ptr_array_index (channels, i);
      AmselChannel *c = amsel_cache_add_channel (self->cache, channel, &error);
      if (error != NULL) {
        g_warning ("%s", error->message);
        continue;
      }
      g_ptr_array_add (ret, c);
    }

  g_ptr_array_unref (channels);

  return ret;
}

GPtrArray *
amsel_engine_get_channels (AmselEngine  *self)
{
  g_return_val_if_fail (AMSEL_IS_ENGINE (self), NULL);

  return amsel_cache_get_channels (self->cache);
}
