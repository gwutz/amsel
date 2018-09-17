#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_OPML (amsel_opml_get_type())

G_DECLARE_FINAL_TYPE (AmselOpml, amsel_opml, AMSEL, OPML, GObject)

AmselOpml *amsel_opml_new (void);
GList     *amsel_opml_parse (AmselOpml *self, gchar *opml);

G_END_DECLS
