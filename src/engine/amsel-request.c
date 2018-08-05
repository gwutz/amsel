#include "amsel-request.h"
#include <stdlib.h>

struct _AmselRequest {
  AmselRequestType type;
  char *data;
  gsize size;
};

/* internal methods */
static AmselRequestType determine_type (const char *data,
                                        gsize       size);

AmselRequest *
amsel_request_new (const char *data,
                   gsize       size)
{
  AmselRequest *request = malloc (sizeof (AmselRequest));
  request->type = determine_type (data, size);
  request->data = g_strdup (data);
  request->size = size;
  return request;
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

void
amsel_request_free (AmselRequest *self)
{
  free (self->data);
  free (self);
}

char *
amsel_request_get_data (AmselRequest *self)
{
  return self->data;
}

gsize
amsel_request_get_size (AmselRequest *self)
{
  return self->size;
}

AmselRequestType
amsel_request_get_type (AmselRequest *self)
{
  return self->type;
}

void
amsel_request_set_type (AmselRequest     *self,
                        AmselRequestType  type)
{
  self->type = type;
}
