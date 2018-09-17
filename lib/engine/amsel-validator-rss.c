#include "amsel-validator-rss.h"
#include "amsel-validator.h"
#include <libxml/xmlschemas.h>
#include <gio/gio.h>

#include "data/resources/amsel-resources.h"

struct _AmselValidatorRss
{
  GObject parent_instance;
  xmlSchemaValidCtxtPtr validctxt;
  xmlSchemaPtr schema;
};

static void amsel_validator_rss_iface_init (AmselValidatorInterface *iface);

G_DEFINE_TYPE_WITH_CODE (AmselValidatorRss, amsel_validator_rss, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (AMSEL_TYPE_VALIDATOR,
                                                amsel_validator_rss_iface_init))

AmselValidatorRss *
amsel_validator_rss_new (void)
{
  return g_object_new (AMSEL_TYPE_VALIDATOR_RSS, NULL);
}

static void
amsel_validator_rss_finalize (GObject *object)
{
  AmselValidatorRss *self = (AmselValidatorRss *)object;

  xmlSchemaFreeValidCtxt (self->validctxt);
  xmlSchemaFree (self->schema);

  G_OBJECT_CLASS (amsel_validator_rss_parent_class)->finalize (object);
}

static void
amsel_validator_rss_class_init (AmselValidatorRssClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_validator_rss_finalize;
}

static void
amsel_validator_rss_init (AmselValidatorRss *self)
{
  GError *error = NULL;
  GBytes *xsdbytes;
  xmlSchemaParserCtxtPtr context;
  char *xml_xsd;
  gsize size;

  g_resources_register (amsel_get_resource ());

  xsdbytes = g_resources_lookup_data ("/org/gnome/Alb/rss-schema.xsd",
                                       G_RESOURCE_LOOKUP_FLAGS_NONE,
                                       &error);
  if (error != NULL) {
    g_error ("%s", error->message);
  }

  xml_xsd = g_bytes_unref_to_data (xsdbytes, &size);

  context = xmlSchemaNewMemParserCtxt (xml_xsd, size);
  self->schema = xmlSchemaParse (context);
  self->validctxt = xmlSchemaNewValidCtxt (self->schema);

  g_free (xml_xsd);
  xmlSchemaFreeParserCtxt (context);
}

gboolean
amsel_validator_rss_validate (AmselValidator *validator,
                              char           *data)
{
  g_return_val_if_fail (AMSEL_IS_VALIDATOR_RSS (validator), FALSE);
  g_return_val_if_fail (data != NULL, FALSE);

  xmlDocPtr xmldoc;
  int result;

  AmselValidatorRss *self = AMSEL_VALIDATOR_RSS (validator);

  xmldoc = xmlParseMemory (data, strlen (data));

  result = xmlSchemaValidateDoc (self->validctxt, xmldoc);

  xmlFreeDoc (xmldoc);

  return result == 0 ? TRUE : FALSE;
}

static void
amsel_validator_rss_iface_init (AmselValidatorInterface *iface)
{
  iface->validate = amsel_validator_rss_validate;
}
