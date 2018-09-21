#include "amsel-request.h"
#include "amsel-debug.h"
#include <stdlib.h>

G_DEFINE_BOXED_TYPE (AmselRequest, amsel_request, amsel_request_copy, amsel_request_free)

/**
 * SECTION:amsel-request
 * @title: AmselRequest
 * @short_description: the abstracted input data
 *
 * #AmselRequest wraps the various possible input data in a data object. During
 * initialisation the #AmselRequest tries to determine the provided data flavor
 * like xml-rss, xml-atom or possible other formats the #AmselEngine can
 * process.
 */

struct _AmselRequest {
  AmselRequestType type;
  char *data;
  gsize size;

  gchar *url;
};

/* internal methods */
static AmselRequestType determine_type (const char *data,
                                        gsize       size);

/**
 * amsel_request_new:
 * @data: raw bytes
 * @size: the size of @data
 *
 * an #AmselRequest
 *
 * Returns: an #AmselRequest boxed type
 */
AmselRequest *
amsel_request_new (const char *data,
                   gsize       size,
                   gchar      *url)
{
  AmselRequest *request = malloc (sizeof (AmselRequest));
  request->type = determine_type (data, size);
  request->data = g_strdup (data);
  request->size = size;
  request->url = g_strdup (url);
  return request;
}

/**
 * amsel_request_copy:
 * @self: a #AmselRequest
 *
 * Makes a deep copy of a #AmselRequest.
 *
 * Returns: (transfer full): A newly created #AmselRequest with the same
 *   contents as @self
 */
AmselRequest *
amsel_request_copy (AmselRequest *self)
{
  AmselRequest *copy;

  g_return_val_if_fail (self, NULL);

  copy = amsel_request_new (self->data, self->size, self->url);

  return copy;
}

static AmselRequestType
determine_type (const char *data,
                gsize       size)
{
  gboolean isxml;
  const char *cur = data;

  // skip possible whitespace
  while (*cur == ' ')
    cur++;

  isxml = g_str_has_prefix (cur, "<?xml");

  if (isxml) {
    char *rss = strstr (cur, "<rss");
    if (rss != NULL) return AMSEL_REQUEST_TYPE_RSS;
    char *atom = strstr (cur, "<feed");
    if (atom != NULL) return AMSEL_REQUEST_TYPE_ATOM;
  }

  return AMSEL_REQUEST_TYPE_UNDECIDED;
}

/**
 * amsel_request_free:
 * @self: a #AmselRequest
 *
 * frees the #AmselRequest and all associated data
 */
void
amsel_request_free (AmselRequest *self)
{
  AM_TRACE_MSG ("Free request");
  free (self->data);
  free (self->url);
  free (self);
}

/**
 * amsel_request_get_data:
 * @self: a #AmselRequest
 *
 * getter for the wrapped data
 *
 * Returns: the wrapped data
 */
char *
amsel_request_get_data (AmselRequest *self)
{
  return self->data;
}

/**
 * amsel_request_get_size:
 * @self: a #AmselRequest
 *
 * getter for the size of the wrapped data
 *
 * Returns: the size of the wrapped data
 */
gsize
amsel_request_get_size (AmselRequest *self)
{
  return self->size;
}

/**
 * amsel_request_get_url:
 * @self: a #AmselRequest
 *
 * getter for the url of the wrapped data
 *
 * Returns: the url where we fetched this request
 */
char *
amsel_request_get_url (AmselRequest *self)
{
  return self->url;
}

/**
 * amsel_request_get_request_type:
 * @self: a #AmselRequest
 *
 * getter for type of the wrapped data
 *
 * Returns: the #AmselRequestType for the wrapped data
 */
AmselRequestType
amsel_request_get_request_type (AmselRequest *self)
{
  return self->type;
}

/**
 * amsel_request_set_request_type:
 * @self: a #AmselRequest
 * @type: the #AmselRequestType to set
 *
 */
void
amsel_request_set_request_type (AmselRequest     *self,
                                AmselRequestType  type)
{
  self->type = type;
}
