#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_REQUEST (amsel_request_get_type ())

typedef struct _AmselRequest AmselRequest;


/**
 * AmselRequestType:
 * @AMSEL_REQUEST_TYPE_UNDECIDED: undefined input data
 * @AMSEL_REQUEST_TYPE_RSS: rss input data
 * @AMSEL_REQUEST_TYPE_ATOM: atom input data
 */
typedef enum {
  AMSEL_REQUEST_TYPE_UNDECIDED,
  AMSEL_REQUEST_TYPE_RSS,
  AMSEL_REQUEST_TYPE_ATOM,
} AmselRequestType;

typedef struct _AmselRequest AmselRequest;

GType             amsel_request_get_type         (void) G_GNUC_CONST;
AmselRequest     *amsel_request_new              (const char       *data,
                                                  gsize             size,
                                                  gchar            *url);
AmselRequest     *amsel_request_copy             (AmselRequest     *self);
void              amsel_request_set_request_type (AmselRequest     *self,
                                                  AmselRequestType  type);
char             *amsel_request_get_data         (AmselRequest     *self);
gsize             amsel_request_get_size         (AmselRequest     *self);
char             *amsel_request_get_url          (AmselRequest     *self);
AmselRequestType  amsel_request_get_request_type (AmselRequest     *self);
void              amsel_request_free             (AmselRequest     *self);

G_END_DECLS
