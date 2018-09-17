#include "amsel-validator-atom.h"
#include "amsel-validator.h"
#include <libxml/relaxng.h>
#include <gio/gio.h>

#include "data/resources/amsel-resources.h"

struct _AmselValidatorAtom
{
  GObject parent_instance;
  xmlRelaxNGValidCtxtPtr validctxt;
  xmlRelaxNGPtr schema;
};

static void amsel_validator_atom_iface_init (AmselValidatorInterface *iface);

G_DEFINE_TYPE_WITH_CODE (AmselValidatorAtom, amsel_validator_atom, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (AMSEL_TYPE_VALIDATOR,
                                                amsel_validator_atom_iface_init))

AmselValidatorAtom *
amsel_validator_atom_new (void)
{
  return g_object_new (AMSEL_TYPE_VALIDATOR_ATOM, NULL);
}

static void
amsel_validator_atom_finalize (GObject *object)
{
  AmselValidatorAtom *self = (AmselValidatorAtom *)object;

  xmlRelaxNGFreeValidCtxt (self->validctxt);
  xmlRelaxNGFree (self->schema);

  G_OBJECT_CLASS (amsel_validator_atom_parent_class)->finalize (object);
}

static void
amsel_validator_atom_class_init (AmselValidatorAtomClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_validator_atom_finalize;
}

static void
amsel_validator_atom_init (AmselValidatorAtom *self)
{
  GError *error = NULL;
  GBytes *rngbytes;
  xmlRelaxNGParserCtxtPtr context;
  char *xml_rng;
  gsize size;

  g_resources_register (amsel_get_resource ());

  rngbytes = g_resources_lookup_data ("/org/gnome/Alb/atom-relax-ng.xml",
                                       G_RESOURCE_LOOKUP_FLAGS_NONE,
                                       &error);
  if (error != NULL) {
    g_error ("%s", error->message);
  }

  xml_rng = g_bytes_unref_to_data (rngbytes, &size);

  context = xmlRelaxNGNewMemParserCtxt (xml_rng, size);
  self->schema = xmlRelaxNGParse (context);
  self->validctxt = xmlRelaxNGNewValidCtxt (self->schema);

  g_free (xml_rng);
  xmlRelaxNGFreeParserCtxt (context);
}

static gboolean
amsel_validator_atom_validate (AmselValidator *validator,
                               char           *data)
{
  g_return_val_if_fail (AMSEL_IS_VALIDATOR_ATOM (validator), FALSE);
  g_return_val_if_fail (data != NULL, FALSE);

  xmlDocPtr xmldoc;
  int result;

  AmselValidatorAtom *self = AMSEL_VALIDATOR_ATOM (validator);

  xmldoc = xmlParseMemory (data, strlen (data));

  result = xmlRelaxNGValidateDoc (self->validctxt, xmldoc);

  xmlFreeDoc (xmldoc);

  return result == 0 ? TRUE : FALSE;

  return FALSE;
}

static void
amsel_validator_atom_iface_init (AmselValidatorInterface *iface)
{
  iface->validate = amsel_validator_atom_validate;
}
