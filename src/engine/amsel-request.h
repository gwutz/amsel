#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib.h>

typedef enum {
  AMSEL_REQUEST_TYPE_UNDECIDED,
  AMSEL_REQUEST_TYPE_RSS,
  AMSEL_REQUEST_TYPE_ATOM,
} AmselRequestType;

typedef struct _AmselRequest AmselRequest;

AmselRequest     *amsel_request_new      (const char       *data,
                                          gsize             size);
void              amsel_request_set_type (AmselRequest     *self,
                                          AmselRequestType  type);
char             *amsel_request_get_data (AmselRequest     *self);
gsize             amsel_request_get_size (AmselRequest     *self);
AmselRequestType  amsel_request_get_type (AmselRequest     *self);
void              amsel_request_free     (AmselRequest     *self);

