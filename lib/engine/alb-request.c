#include "alb-request.h"
#include "alb-debug.h"
#include <stdlib.h>

G_DEFINE_BOXED_TYPE (AlbRequest, alb_request, alb_request_copy, alb_request_free)

/**
 * SECTION:amsel-request
 * @title: AlbRequest
 * @short_description: the abstracted input data
 *
 * #AlbRequest wraps the various possible input data in a data object. During
 * initialisation the #AlbRequest tries to determine the provided data flavor
 * like xml-rss, xml-atom or possible other formats the #AmselEngine can
 * process.
 */

struct _AlbRequest {
  AlbRequestType type;
  char *data;
  gsize size;

  gchar *url;
};

/* internal methods */
static AlbRequestType determine_type (const char *data,
                                        gsize       size);

/**
 * alb_request_new:
 * @data: raw bytes
 * @size: the size of @data
 *
 * an #AlbRequest
 *
 * Returns: an #AlbRequest boxed type
 */
AlbRequest *
alb_request_new (const char *data,
                   gsize       size,
                   gchar      *url)
{
  AlbRequest *request = malloc (sizeof (AlbRequest));
  request->type = determine_type (data, size);
  request->data = g_strdup (data);
  request->size = size;
  request->url = g_strdup (url);
  return request;
}

/**
 * alb_request_copy:
 * @self: a #AlbRequest
 *
 * Makes a deep copy of a #AlbRequest.
 *
 * Returns: (transfer full): A newly created #AlbRequest with the same
 *   contents as @self
 */
AlbRequest *
alb_request_copy (AlbRequest *self)
{
  AlbRequest *copy;

  g_return_val_if_fail (self, NULL);

  copy = alb_request_new (self->data, self->size, self->url);

  return copy;
}

static AlbRequestType
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
    if (rss != NULL) return ALB_REQUEST_TYPE_RSS;
    char *atom = strstr (cur, "<feed");
    if (atom != NULL) return ALB_REQUEST_TYPE_ATOM;
  }

  return ALB_REQUEST_TYPE_UNDECIDED;
}

/**
 * alb_request_free:
 * @self: a #AlbRequest
 *
 * frees the #AlbRequest and all associated data
 */
void
alb_request_free (AlbRequest *self)
{
  AM_TRACE_MSG ("Free request");
  free (self->data);
  free (self->url);
  free (self);
}

/**
 * alb_request_get_data:
 * @self: a #AlbRequest
 *
 * getter for the wrapped data
 *
 * Returns: the wrapped data
 */
char *
alb_request_get_data (AlbRequest *self)
{
  return self->data;
}

/**
 * alb_request_get_size:
 * @self: a #AlbRequest
 *
 * getter for the size of the wrapped data
 *
 * Returns: the size of the wrapped data
 */
gsize
alb_request_get_size (AlbRequest *self)
{
  return self->size;
}

/**
 * alb_request_get_url:
 * @self: a #AlbRequest
 *
 * getter for the url of the wrapped data
 *
 * Returns: the url where we fetched this request
 */
char *
alb_request_get_url (AlbRequest *self)
{
  return self->url;
}

/**
 * alb_request_get_request_type:
 * @self: a #AlbRequest
 *
 * getter for type of the wrapped data
 *
 * Returns: the #AlbRequestType for the wrapped data
 */
AlbRequestType
alb_request_get_request_type (AlbRequest *self)
{
  return self->type;
}

/**
 * alb_request_set_request_type:
 * @self: a #AlbRequest
 * @type: the #AlbRequestType to set
 *
 */
void
alb_request_set_request_type (AlbRequest     *self,
                                AlbRequestType  type)
{
  self->type = type;
}
