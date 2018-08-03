#include "amsel-validator-xml.h"
#include "amsel-validator.h"
#include <libxml/relaxng.h>
#include <gio/gio.h>

struct _AmselValidatorXml
{
  GObject parent_instance;
  xmlRelaxNGValidCtxtPtr validctxt;
  xmlRelaxNGPtr schema;
};

static void amsel_validator_xml_iface_init (AmselValidatorInterface *iface);

G_DEFINE_TYPE_WITH_CODE (AmselValidatorXml, amsel_validator_xml, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (AMSEL_TYPE_VALIDATOR,
                                                amsel_validator_xml_iface_init))

AmselValidatorXml *
amsel_validator_xml_new (void)
{
  return g_object_new (AMSEL_TYPE_VALIDATOR_XML, NULL);
}

static void
amsel_validator_xml_finalize (GObject *object)
{
  AmselValidatorXml *self = (AmselValidatorXml *)object;

  xmlRelaxNGFreeValidCtxt (self->validctxt);
  xmlRelaxNGFree (self->schema);

  G_OBJECT_CLASS (amsel_validator_xml_parent_class)->finalize (object);
}

static void
amsel_validator_xml_class_init (AmselValidatorXmlClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_validator_xml_finalize;
}

static void
amsel_validator_xml_init (AmselValidatorXml *self)
{
  GError *error = NULL;
  GBytes *rngbytes;
  xmlRelaxNGParserCtxtPtr context;
  char *xml_rng;
  gsize size;

  g_resources_register (amsel_get_resource ());

  rngbytes = g_resources_lookup_data ("/org/gnome/Amsel/rss-relax-ng.xml",
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

gboolean
amsel_validator_xml_validate (AmselValidator *validator,
                              char           *data)
{
  xmlDocPtr xmldoc;
  int result;

  AmselValidatorXml *self = AMSEL_VALIDATOR_XML (validator);

  xmldoc = xmlParseMemory (data, strlen (data));

  result = xmlRelaxNGValidateDoc (self->validctxt, xmldoc);

  xmlFreeDoc (xmldoc);

  return result == 0 ? TRUE : FALSE;
}

static void
amsel_validator_xml_iface_init (AmselValidatorInterface *iface)
{
  iface->validate = amsel_validator_xml_validate;
}
