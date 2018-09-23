#include "alb-engine.h"
#include "alb-validator.h"
// TODO: why do we implement this here? Should be agnostic!
#include "alb-validator-rss.h"
// TODO: why do we implement this here? Should be agnostic!
#include "alb-validator-atom.h"
#include "alb-parser.h"
#include "alb-parser-rss.h"
#include "alb-parser-atom.h"
#include "alb-cache.h"
// TODO: why do we implement this here? Should be agnostic!
#include "alb-sqlite-database.h"
#include "alb-debug.h"

/**
 * SECTION:alb-engine
 * @title: AlbEngine
 * @short_description: the main public api to handle libalb
 *
 *
 */

static GThread *main_thread;

struct _AlbEngine
{
  GObject parent_instance;
  GList *validators;
  GHashTable *parsers;

  AlbCache *cache;
};

G_DEFINE_TYPE (AlbEngine, alb_engine, G_TYPE_OBJECT)

AlbEngine *
alb_engine_new (void)
{
  return g_object_new (ALB_TYPE_ENGINE, NULL);
}

static void
alb_engine_finalize (GObject *object)
{
  AlbEngine *self = (AlbEngine *)object;

  g_list_free_full (self->validators, g_object_unref);
  g_clear_object (&self->cache);
  g_hash_table_unref (self->parsers);

  G_OBJECT_CLASS (alb_engine_parent_class)->finalize (object);
}

static void
alb_engine_class_init (AlbEngineClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = alb_engine_finalize;

  main_thread = g_thread_self ();
}

static void
alb_engine_init (AlbEngine *self)
{
  /* self->validators = g_list_append (self->validators, alb_validator_rss_new ()); */
  /* self->validators = g_list_append (self->validators, alb_validator_atom_new ()); */

  self->parsers = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_object_unref);
  g_hash_table_insert (self->parsers, GINT_TO_POINTER (ALB_REQUEST_TYPE_RSS), alb_parser_rss_new ());
  g_hash_table_insert (self->parsers, GINT_TO_POINTER (ALB_REQUEST_TYPE_ATOM), alb_parser_atom_new ());

  g_autoptr (AlbDatabase) database = ALB_DATABASE (alb_sqlite_database_new ("amsel.db"));
  self->cache = alb_cache_new (g_steal_pointer (&database));
}

gboolean
alb_engine_validate (AlbEngine  *self,
                       AlbRequest *request)
{
  g_return_val_if_fail (ALB_IS_ENGINE (self), FALSE);
  g_return_val_if_fail (request != NULL, FALSE);
  g_return_val_if_fail (alb_request_get_data (request) != NULL, FALSE);

  gboolean valid = FALSE;

  for (GList *cur = self->validators; cur; cur = g_list_next (cur))
    {
      AlbValidator *validator = ALB_VALIDATOR (cur->data);
      valid = alb_validator_validate (validator, alb_request_get_data (request));

      if (valid) {
        if (ALB_IS_VALIDATOR_RSS (validator)) {
          alb_request_set_request_type (request, ALB_REQUEST_TYPE_RSS);
        } else if (ALB_IS_VALIDATOR_ATOM (validator)) {
          alb_request_set_request_type (request, ALB_REQUEST_TYPE_ATOM);
        }
        break;
      }

    }

  return valid;
}

static void
alb_engine_parse_worker (GTask        *task,
                           gpointer      source_object,
                           gpointer      task_data,
                           GCancellable *cancellable)
{
  AlbEngine *self = ALB_ENGINE (source_object);
  AlbRequest *request = (AlbRequest *) task_data;
  GPtrArray *ret = g_ptr_array_new_with_free_func (NULL);

  AlbParser *parser = g_hash_table_lookup (self->parsers, GINT_TO_POINTER (alb_request_get_request_type (request)));
  if (parser == NULL) {
    g_task_return_boolean (task, FALSE);
    return;
  }

  ret = alb_parser_parse (ALB_PARSER (parser), request);

  g_task_return_pointer (task, ret, (GDestroyNotify) g_ptr_array_unref);
}

/**
 * alb_engine_parse:
 * @self: the #AlbEngine
 * @request: the #AlbRequest
 *
 * Returns: (element-type AlbChannel) (transfer full): the #AlbChannel with the combined
 *          #AlbEntry's from #AlbCache
 */
GPtrArray *
alb_engine_parse (AlbEngine  *self,
                    AlbRequest *request)
{
  g_autoptr(GTask) task = NULL;

  g_return_val_if_fail (ALB_IS_ENGINE (self), NULL);

  task = g_task_new (self, NULL, NULL, NULL);
  g_task_set_source_tag (task, alb_engine_parse);
  g_task_set_task_data (task, request, (GDestroyNotify) alb_request_free);

  alb_engine_parse_worker (task, self, request, NULL);

  GPtrArray *ret = g_ptr_array_new_with_free_func (NULL);
  GPtrArray *channels = g_task_propagate_pointer (task, NULL);
  GError *error = NULL;

  for (int i = 0; i < channels->len; i++)
    {
      AlbChannel *channel = g_ptr_array_index (channels, i);
      AlbChannel *c = alb_cache_add_channel (self->cache, channel, &error);
      if (error != NULL) {
        g_warning ("%s", error->message);
        g_clear_error (&error);
        continue;
      }
      g_ptr_array_add (ret, c);
    }

  g_ptr_array_unref (channels);
  return ret;
}

/**
 * alb_engine_parse_async:
 * @self: an #AlbEngine
 * @cancellable: (nullable): a #GCancellable
 * @callback: a #GAsyncReadyCallback to execute upon completion
 * @user_data: closure data for @callback
 *
 */
void
alb_engine_parse_async (AlbEngine         *self,
                          AlbRequest        *request,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;

  g_return_if_fail (ALB_IS_ENGINE (self));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_source_tag (task, alb_engine_parse_async);
  g_task_set_task_data (task, request, (GDestroyNotify) alb_request_free);

  g_task_run_in_thread (task, alb_engine_parse_worker);
}

/**
 * alb_engine_parse_finish:
 * @self: an #AlbEngine
 * @result: a #GAsyncResult provided to callback
 * @error: a location for a #GError, or %NULL
 *
 * Returns:
 */
GPtrArray *
alb_engine_parse_finish (AlbEngine *self,
                           GAsyncResult *result,
                           GError **error)
{
  g_return_val_if_fail (ALB_IS_ENGINE (self), NULL);
  g_return_val_if_fail (G_IS_TASK (result), NULL);

  GPtrArray *ret = g_ptr_array_new_with_free_func (NULL);
  GPtrArray *channels = g_task_propagate_pointer (G_TASK (result), error);

  if (*error != NULL) {
    return NULL;
  }

  for (int i = 0; i < channels->len; i++)
    {
      AlbChannel *channel = g_ptr_array_index (channels, i);
      AlbChannel *c = alb_cache_add_channel (self->cache, channel, error);
      if (*error != NULL) {
        g_warning ("%s", (*error)->message);
        g_clear_error (error);
        continue;
      }
      g_ptr_array_add (ret, c);
    }

  g_ptr_array_unref (channels);
  return ret;
}

/**
 * alb_engine_get_channels:
 * @self: a #AlbEngine
 *
 *
 * Returns: (element-type AlbChannel) (transfer container):
 */
GPtrArray *
alb_engine_get_channels (AlbEngine  *self)
{
  g_return_val_if_fail (ALB_IS_ENGINE (self), NULL);

  return alb_cache_get_channels (self->cache);
}

void
alb_engine_connect_signal (AlbEngine *self,
                             GCallback    callback,
                             gpointer     user_data)
{
  g_signal_connect (self->cache, "new-entry", callback, user_data);
}

GThread *
alb_engine_get_main_thread (void)
{
  return main_thread;
}

void
alb_engine_mark_entry_read (AlbEngine *self,
                              AlbEntry  *entry)
{
  g_return_if_fail (ALB_IS_ENGINE (self));
  g_return_if_fail (ALB_IS_ENTRY (entry));

  alb_cache_mark_read (self->cache, entry);
}
