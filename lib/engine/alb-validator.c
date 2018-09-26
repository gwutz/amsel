/* alb-validator.c
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

#define G_LOG_DOMAIN "alb-validator"

#include "alb-validator.h"

G_DEFINE_INTERFACE (AlbValidator, alb_validator, G_TYPE_OBJECT)

static void
alb_validator_default_init (AlbValidatorInterface *iface)
{
  iface->validate = alb_validator_validate;
}

gboolean
alb_validator_validate (AlbValidator *self,
                          char           *data)
{
  g_return_val_if_fail (ALB_IS_VALIDATOR (self), FALSE);

  return ALB_VALIDATOR_GET_IFACE (self)->validate (self, data);
}
