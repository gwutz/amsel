#include "amsel-parser-atom.h"
#include "amsel-parser.h"

struct _AmselParserAtom
{
  GObject parent_instance;
};

static void amsel_parser_atom_iface_init (AmselParserInterface *iface);
G_DEFINE_TYPE_WITH_CODE (AmselParserAtom, amsel_parser_atom, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (AMSEL_TYPE_PARSER, amsel_parser_atom_iface_init))

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

AmselParserAtom *
amsel_parser_atom_new (void)
{
  return g_object_new (AMSEL_TYPE_PARSER_ATOM, NULL);
}

static void
amsel_parser_atom_finalize (GObject *object)
{
  AmselParserAtom *self = (AmselParserAtom *)object;

  G_OBJECT_CLASS (amsel_parser_atom_parent_class)->finalize (object);
}

static void
amsel_parser_atom_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  AmselParserAtom *self = AMSEL_PARSER_ATOM (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_parser_atom_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  AmselParserAtom *self = AMSEL_PARSER_ATOM (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_parser_atom_class_init (AmselParserAtomClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_parser_atom_finalize;
  object_class->get_property = amsel_parser_atom_get_property;
  object_class->set_property = amsel_parser_atom_set_property;
}

static void
amsel_parser_atom_init (AmselParserAtom *self)
{
}

GPtrArray *
amsel_parser_atom_parse (AmselParser  *parser,
                         AmselRequest *request)
{
  return NULL;
}

static void
amsel_parser_atom_iface_init (AmselParserInterface *iface)
{
  iface->parse = amsel_parser_atom_parse;
}
