#include "alb-validator-rss.h"
#include "alb-validator.h"
#include <libxml/xmlschemas.h>
#include <gio/gio.h>

#include "data/resources/amsel-resources.h"

struct _AlbValidatorRss
{
  GObject parent_instance;
  xmlSchemaValidCtxtPtr validctxt;
  xmlSchemaPtr schema;
};

static void alb_validator_rss_iface_init (AlbValidatorInterface *iface);

G_DEFINE_TYPE_WITH_CODE (AlbValidatorRss, alb_validator_rss, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ALB_TYPE_VALIDATOR,
                                                alb_validator_rss_iface_init))

AlbValidatorRss *
alb_validator_rss_new (void)
{
  return g_object_new (ALB_TYPE_VALIDATOR_RSS, NULL);
}

static void
alb_validator_rss_finalize (GObject *object)
{
  AlbValidatorRss *self = (AlbValidatorRss *)object;

  xmlSchemaFreeValidCtxt (self->validctxt);
  xmlSchemaFree (self->schema);

  G_OBJECT_CLASS (alb_validator_rss_parent_class)->finalize (object);
}

static void
alb_validator_rss_class_init (AlbValidatorRssClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = alb_validator_rss_finalize;
}

static void
alb_validator_rss_init (AlbValidatorRss *self)
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
alb_validator_rss_validate (AlbValidator *validator,
                              char           *data)
{
  g_return_val_if_fail (ALB_IS_VALIDATOR_RSS (validator), FALSE);
  g_return_val_if_fail (data != NULL, FALSE);

  xmlDocPtr xmldoc;
  int result;

  AlbValidatorRss *self = ALB_VALIDATOR_RSS (validator);

  xmldoc = xmlParseMemory (data, strlen (data));

  result = xmlSchemaValidateDoc (self->validctxt, xmldoc);

  xmlFreeDoc (xmldoc);

  return result == 0 ? TRUE : FALSE;
}

static void
alb_validator_rss_iface_init (AlbValidatorInterface *iface)
{
  iface->validate = alb_validator_rss_validate;
}
