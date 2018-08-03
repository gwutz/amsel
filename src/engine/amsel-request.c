#include "amsel-request.h"
#include <stdlib.h>

struct _AmselRequest {
  AmselRequestType type;
  char *data;
  gsize size;
};

AmselRequest *
amsel_request_new (AmselRequestType  type,
                   char             *data,
                   gsize             size)
{
  AmselRequest *request = malloc (sizeof (AmselRequest));
  request->type = type;
  request->data = g_strdup (data);
  request->size = size;
  return request;
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
