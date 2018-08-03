#pragma once

#include <glib.h>

typedef enum {
  AMSEL_REQUEST_TYPE_XML,
  AMSEL_REQUEST_TYPE_JSON,
} AmselRequestType;

typedef struct _AmselRequest AmselRequest;

AmselRequest     *amsel_request_new      (AmselRequestType  type,
                                          char             *data,
                                          gsize             size);
char             *amsel_request_get_data (AmselRequest     *self);
gsize             amsel_request_get_size (AmselRequest     *self);
AmselRequestType  amsel_request_get_type (AmselRequest     *self);
void              amsel_request_free     (AmselRequest     *self);

