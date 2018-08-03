#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define AMSEL_TYPE_VALIDATOR_XML (amsel_validator_xml_get_type())

G_DECLARE_FINAL_TYPE (AmselValidatorXml, amsel_validator_xml, AMSEL, VALIDATOR_XML, GObject)

AmselValidatorXml *amsel_validator_xml_new (void);

G_END_DECLS
