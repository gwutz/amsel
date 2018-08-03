#include "amsel-engine.h"
#include "amsel-validator.h"
#include "amsel-validator-xml.h"
#include "amsel-parser.h"
#include "amsel-parser-rss.h"

struct _AmselEngine
{
  GObject parent_instance;
};

G_DEFINE_TYPE (AmselEngine, amsel_engine, G_TYPE_OBJECT)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

AmselEngine *
amsel_engine_new (void)
{
  return g_object_new (AMSEL_TYPE_ENGINE, NULL);
}

static void
amsel_engine_finalize (GObject *object)
{
  AmselEngine *self = (AmselEngine *)object;

  G_OBJECT_CLASS (amsel_engine_parent_class)->finalize (object);
}

static void
amsel_engine_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  AmselEngine *self = AMSEL_ENGINE (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_engine_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  AmselEngine *self = AMSEL_ENGINE (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
amsel_engine_class_init (AmselEngineClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_engine_finalize;
  object_class->get_property = amsel_engine_get_property;
  object_class->set_property = amsel_engine_set_property;
}

static void
amsel_engine_init (AmselEngine *self)
{
}

gboolean
amsel_engine_validate (AmselEngine  *self,
                       AmselRequest *request)
{
  g_return_val_if_fail (AMSEL_IS_ENGINE (self), FALSE);
  g_return_val_if_fail (request != NULL, FALSE);
  g_return_val_if_fail (amsel_request_get_data (request) != NULL, FALSE);

  g_autoptr (AmselValidator) validator;

  switch (amsel_request_get_type (request)) {
  case AMSEL_REQUEST_TYPE_XML:
    validator = AMSEL_VALIDATOR (amsel_validator_xml_new ());
    return amsel_validator_validate (validator, amsel_request_get_data (request));
    break;
  case AMSEL_REQUEST_TYPE_JSON:
    validator = AMSEL_VALIDATOR (amsel_validator_xml_new ());
    return amsel_validator_validate (validator, amsel_request_get_data (request));
    break;
  }
}

GPtrArray *
amsel_engine_parse (AmselEngine  *self,
                    AmselRequest *request)
{
  g_return_val_if_fail (AMSEL_IS_ENGINE (self), NULL);
  g_return_val_if_fail (request != NULL, NULL);
  g_return_val_if_fail (amsel_request_get_data (request) != NULL, NULL);

  AmselParserRss *parser = amsel_parser_rss_new ();

  switch (amsel_request_get_type (request)) {
  case AMSEL_REQUEST_TYPE_XML:
    return amsel_parser_parse (AMSEL_PARSER (parser), request);
    break;
  case AMSEL_REQUEST_TYPE_JSON:
    break;
  }

  return NULL;
}
