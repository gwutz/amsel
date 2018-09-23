#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define ALB_TYPE_OPML (alb_opml_get_type())

G_DECLARE_FINAL_TYPE (AlbOpml, alb_opml, ALB, OPML, GObject)

AlbOpml *alb_opml_new (void);
GList     *alb_opml_parse (AlbOpml *self, gchar *opml);

G_END_DECLS
