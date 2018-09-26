/* alb-validator-atom.c
 *
 * Copyright 2018 Günther Wagner <info@gunibert.de>
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#define G_LOG_DOMAIN "alb-validator-atom"

#include "alb-validator-atom.h"
#include "alb-validator.h"
#include <libxml/relaxng.h>
#include <gio/gio.h>

#include "data/resources/amsel-resources.h"

struct _AlbValidatorAtom
{
  GObject parent_instance;
  xmlRelaxNGValidCtxtPtr validctxt;
  xmlRelaxNGPtr schema;
};

static void alb_validator_atom_iface_init (AlbValidatorInterface *iface);

G_DEFINE_TYPE_WITH_CODE (AlbValidatorAtom, alb_validator_atom, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ALB_TYPE_VALIDATOR,
                                                alb_validator_atom_iface_init))

AlbValidatorAtom *
alb_validator_atom_new (void)
{
  return g_object_new (ALB_TYPE_VALIDATOR_ATOM, NULL);
}

static void
alb_validator_atom_finalize (GObject *object)
{
  AlbValidatorAtom *self = (AlbValidatorAtom *)object;

  xmlRelaxNGFreeValidCtxt (self->validctxt);
  xmlRelaxNGFree (self->schema);

  G_OBJECT_CLASS (alb_validator_atom_parent_class)->finalize (object);
}

static void
alb_validator_atom_class_init (AlbValidatorAtomClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = alb_validator_atom_finalize;
}

static void
alb_validator_atom_init (AlbValidatorAtom *self)
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
alb_validator_atom_validate (AlbValidator *validator,
                               char           *data)
{
  g_return_val_if_fail (ALB_IS_VALIDATOR_ATOM (validator), FALSE);
  g_return_val_if_fail (data != NULL, FALSE);

  xmlDocPtr xmldoc;
  int result;

  AlbValidatorAtom *self = ALB_VALIDATOR_ATOM (validator);

  xmldoc = xmlParseMemory (data, strlen (data));

  result = xmlRelaxNGValidateDoc (self->validctxt, xmldoc);

  xmlFreeDoc (xmldoc);

  return result == 0 ? TRUE : FALSE;

  return FALSE;
}

static void
alb_validator_atom_iface_init (AlbValidatorInterface *iface)
{
  iface->validate = alb_validator_atom_validate;
}
