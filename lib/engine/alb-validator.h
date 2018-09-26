/* alb-validator.h
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

#pragma once

#if !defined (__ALB_H_INSIDE__) && !defined (ALB_COMPILATION)
#error "Only <alb.h> could be included directly"
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define ALB_TYPE_VALIDATOR (alb_validator_get_type ())

G_DECLARE_INTERFACE (AlbValidator, alb_validator, ALB, VALIDATOR, GObject)

/**
 * AlbValidatorInterface:
 * @parent: the parent interface
 * @validate: validates the provided data
 */
struct _AlbValidatorInterface
{
  GTypeInterface parent;

  gboolean (*validate) (AlbValidator *self,
                        char           *data);
};

gboolean alb_validator_validate (AlbValidator *self,
                                   char           *data);

G_END_DECLS
