/*
  Liquid War 6 is a unique multiplayer wargame.
  Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010  Christian Mauduit <ufoot@ufoot.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.


  Liquid War 6 homepage : http://www.gnu.org/software/liquidwar6/
  Contact author        : ufoot@ufoot.org
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ker.h"
#include "ker-internal.h"

void
_lw6ker_node_init (lw6ker_node_t * node)
{
  memset (node, 0, sizeof (lw6ker_node_t));
}

void
_lw6ker_node_update_checksum (lw6ker_node_t * node, u_int32_t * checksum)
{
  lw6sys_checksum_update_int64 (checksum, node->node_id);
  lw6sys_checksum_update_int32 (checksum, node->enabled);
  lw6sys_checksum_update_int32 (checksum, node->last_command_round);
}

void
_lw6ker_node_reset (lw6ker_node_t * node)
{
  node->enabled = 0;
  node->last_command_round = 0;
}

int
lw6ker_node_enable (lw6ker_node_t * node, u_int64_t node_id)
{
  int ret = 0;

  if (!node->enabled)
    {
      node->node_id = node_id;
      _lw6ker_node_reset (node);
      node->enabled = 1;
      ret = 1;
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _("trying to enable node which is already enabled"));
    }

  return ret;
}

int
lw6ker_node_disable (lw6ker_node_t * node)
{
  int ret = 0;

  if (node->enabled)
    {
      node->node_id = 0;
      _lw6ker_node_reset (node);
      ret = 1;
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _("trying to disable node which is already disabled"));
    }

  return ret;
}

int
lw6ker_node_sanity_check (lw6ker_node_t * node, lw6map_rules_t * rules)
{
  int ret = 1;

  if ((node->enabled
       && (!lw6sys_check_id_64 (node->node_id)))
      || ((!node->enabled) && (node->node_id != 0)))
    {
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _
		  ("inconsistent enabled node values enabled=%d node_id=%"
		   LW6SYS_PRINTF_LL "x"), node->enabled, node->node_id);
      ret = 0;
    }

  return ret;
}