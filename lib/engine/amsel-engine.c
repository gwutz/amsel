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

/**
 * SECTION:amsel-engine
 * @title: AmselEngine
 * @short_description: the main public api to handle libalb
 *
 *
 */

static GThread *main_thread;

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

  main_thread = g_thread_self ();
}

static void
amsel_engine_init (AmselEngine *self)
{
  /* self->validators = g_list_append (self->validators, amsel_validator_rss_new ()); */
  /* self->validators = g_list_append (self->validators, amsel_validator_atom_new ()); */

  self->parsers = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_object_unref);
  g_hash_table_insert (self->parsers, GINT_TO_POINTER (AMSEL_REQUEST_TYPE_RSS), amsel_parser_rss_new ());
  g_hash_table_insert (self->parsers, GINT_TO_POINTER (AMSEL_REQUEST_TYPE_ATOM), amsel_parser_atom_new ());

  g_autoptr (AmselDatabase) database = AMSEL_DATABASE (amsel_sqlite_database_new ("amsel.db"));
  self->cache = amsel_cache_new (g_steal_pointer (&database));
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
          amsel_request_set_request_type (request, AMSEL_REQUEST_TYPE_RSS);
        } else if (AMSEL_IS_VALIDATOR_ATOM (validator)) {
          amsel_request_set_request_type (request, AMSEL_REQUEST_TYPE_ATOM);
        }
        break;
      }

    }

  return valid;
}

static void
amsel_engine_parse_worker (GTask        *task,
                           gpointer      source_object,
                           gpointer      task_data,
                           GCancellable *cancellable)
{
  AmselEngine *self = AMSEL_ENGINE (source_object);
  AmselRequest *request = (AmselRequest *) task_data;
  GPtrArray *ret = g_ptr_array_new_with_free_func (NULL);

  AmselParser *parser = g_hash_table_lookup (self->parsers, GINT_TO_POINTER (amsel_request_get_request_type (request)));
  if (parser == NULL) {
    g_task_return_boolean (task, FALSE);
    return;
  }

  ret = amsel_parser_parse (AMSEL_PARSER (parser), request);

  g_task_return_pointer (task, ret, (GDestroyNotify) g_ptr_array_unref);
}

/**
 * amsel_engine_parse:
 * @self: the #AmselEngine
 * @request: the #AmselRequest
 *
 * Returns: (element-type AmselChannel) (transfer full): the #AmselChannel with the combined
 *          #AmselEntry's from #AmselCache
 */
GPtrArray *
amsel_engine_parse (AmselEngine  *self,
                    AmselRequest *request)
{
  g_autoptr(GTask) task = NULL;

  g_return_val_if_fail (AMSEL_IS_ENGINE (self), NULL);

  task = g_task_new (self, NULL, NULL, NULL);
  g_task_set_source_tag (task, amsel_engine_parse);
  g_task_set_task_data (task, request, (GDestroyNotify) amsel_request_free);

  amsel_engine_parse_worker (task, self, request, NULL);
  return g_task_propagate_pointer (task, NULL);
}

/**
 * amsel_engine_parse_async:
 * @self: an #AmselEngine
 * @cancellable: (nullable): a #GCancellable
 * @callback: a #GAsyncReadyCallback to execute upon completion
 * @user_data: closure data for @callback
 *
 */
void
amsel_engine_parse_async (AmselEngine         *self,
                          AmselRequest        *request,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;

  g_return_if_fail (AMSEL_IS_ENGINE (self));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_source_tag (task, amsel_engine_parse_async);
  g_task_set_task_data (task, request, (GDestroyNotify) amsel_request_free);

  g_task_run_in_thread (task, amsel_engine_parse_worker);
}

/**
 * amsel_engine_parse_finish:
 * @self: an #AmselEngine
 * @result: a #GAsyncResult provided to callback
 * @error: a location for a #GError, or %NULL
 *
 * Returns:
 */
GPtrArray *
amsel_engine_parse_finish (AmselEngine *self,
                           GAsyncResult *result,
                           GError **error)
{
  g_return_val_if_fail (AMSEL_IS_ENGINE (self), NULL);
  g_return_val_if_fail (G_IS_TASK (result), NULL);

  GPtrArray *ret = g_ptr_array_new_with_free_func (NULL);
  GPtrArray *channels = g_task_propagate_pointer (G_TASK (result), error);

  if (*error != NULL) {
    return NULL;
  }

  for (int i = 0; i < channels->len; i++)
    {
      AmselChannel *channel = g_ptr_array_index (channels, i);
      AmselChannel *c = amsel_cache_add_channel (self->cache, channel, error);
      if (*error != NULL) {
        g_warning ("%s", (*error)->message);
        continue;
      }
      g_ptr_array_add (ret, c);
    }

  g_ptr_array_unref (channels);
  return ret;
}

/**
 * amsel_engine_get_channels:
 * @self: a #AmselEngine
 *
 *
 * Returns: (element-type AmselChannel) (transfer container):
 */
GPtrArray *
amsel_engine_get_channels (AmselEngine  *self)
{
  g_return_val_if_fail (AMSEL_IS_ENGINE (self), NULL);

  return amsel_cache_get_channels (self->cache);
}

void
amsel_engine_connect_signal (AmselEngine *self,
                             GCallback    callback,
                             gpointer     user_data)
{
  g_signal_connect (self->cache, "new-entry", callback, user_data);
}

GThread *
amsel_engine_get_main_thread (void)
{
  return main_thread;
}

void
amsel_engine_mark_entry_read (AmselEngine *self,
                              AmselEntry  *entry)
{
  g_return_if_fail (AMSEL_IS_ENGINE (self));
  g_return_if_fail (AMSEL_IS_ENTRY (entry));

  amsel_cache_mark_read (self->cache, entry);
}
