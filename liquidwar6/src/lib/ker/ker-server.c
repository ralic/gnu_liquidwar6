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
_lw6ker_server_init (lw6ker_server_t * server)
{
  memset (server, 0, sizeof (lw6ker_server_t));
}

void
_lw6ker_server_update_checksum (lw6ker_server_t * server,
				u_int32_t * checksum)
{
  lw6sys_checksum_update_int64 (checksum, server->server_id);
  lw6sys_checksum_update_int32 (checksum, server->enabled);
  lw6sys_checksum_update_int32 (checksum, server->last_command_round);
}

void
_lw6ker_server_reset (lw6ker_server_t * server)
{
  server->enabled = 0;
  server->last_command_round = 0;
}

int
lw6ker_server_enable (lw6ker_server_t * server, u_int64_t server_id)
{
  int ret = 0;

  if (!server->enabled)
    {
      server->server_id = server_id;
      _lw6ker_server_reset (server);
      server->enabled = 1;
      ret = 1;
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _("trying to enable server which is already enabled"));
    }

  return ret;
}

int
lw6ker_server_disable (lw6ker_server_t * server)
{
  int ret = 0;

  if (server->enabled)
    {
      server->server_id = 0;
      _lw6ker_server_reset (server);
      ret = 1;
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _("trying to disable server which is already disabled"));
    }

  return ret;
}

int
lw6ker_server_sanity_check (lw6ker_server_t * server, lw6map_rules_t * rules)
{
  int ret = 1;

  if ((server->enabled
       && (!lw6sys_check_id_64 (server->server_id)))
      || ((!server->enabled) && (server->server_id != 0)))
    {
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _
		  ("inconsistent enabled server values enabled=%d server_id=%"
		   LW6SYS_PRINTF_LL "x"), server->enabled, server->server_id);
      ret = 0;
    }

  return ret;
}
