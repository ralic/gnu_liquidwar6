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

#include <string.h>

#include "ker.h"
#include "ker-internal.h"

void
_lw6ker_server_array_init (lw6ker_server_array_t * server_array)
{
  int i = 0;

  server_array->nb_servers = 0;
  for (i = 0; i < LW6MAP_MAX_NB_SERVERS; ++i)
    {
      _lw6ker_server_init (&(server_array->servers[i]));
    }
}

void
_lw6ker_server_array_update_checksum (lw6ker_server_array_t *
				      server_array, u_int32_t * checksum)
{
  int i = 0;

  lw6sys_checksum_update_int32 (checksum, server_array->nb_servers);
  for (i = 0; i < LW6MAP_MAX_NB_SERVERS; ++i)
    {
      _lw6ker_server_update_checksum (&(server_array->servers[i]), checksum);
    }
}

lw6ker_server_t *
_lw6ker_server_array_find_free (lw6ker_server_array_t * server_array)
{
  lw6ker_server_t *ret = NULL;
  int i;

  for (i = 0; i < LW6MAP_MAX_NB_SERVERS && !ret; ++i)
    {
      if (!server_array->servers[i].enabled)
	{
	  ret = &(server_array->servers[i]);
	}
    }

  if (!ret)
    {
      lw6sys_log (LW6SYS_LOG_DEBUG, _("unable to find free server"));
    }

  return ret;
}

void
_lw6ker_server_array_reset (lw6ker_server_array_t * server_array)
{
  int i;

  server_array->nb_servers = 0;
  for (i = 0; i < LW6MAP_MAX_NB_SERVERS; ++i)
    {
      _lw6ker_server_reset (&(server_array->servers[i]));
    }
}

lw6ker_server_t *
lw6ker_server_array_get (lw6ker_server_array_t *
			 server_array, u_int64_t server_id)
{
  lw6ker_server_t *ret = NULL;
  int i;

  for (i = 0; i < LW6MAP_MAX_NB_SERVERS; ++i)
    {
      if (server_array->servers[i].server_id == server_id)
	{
	  ret = &(server_array->servers[i]);
	}
    }

  return ret;
}

int
lw6ker_server_array_enable (lw6ker_server_array_t * server_array,
			    u_int64_t server_id)
{
  int ret = 0;
  lw6ker_server_t *server;

  server = lw6ker_server_array_get (server_array, server_id);
  if (!server)
    {
      server = _lw6ker_server_array_find_free (server_array);
      if (server)
	{
	  lw6ker_server_enable (server, server_id);
	  server_array->nb_servers++;
	  ret = 1;
	}
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_DEBUG,
		  _("server %x already exists, can't enable it twice"),
		  server_id);
    }

  return ret;
}

int
lw6ker_server_array_disable (lw6ker_server_array_t * server_array,
			     u_int64_t server_id)
{
  int ret = 0;
  lw6ker_server_t *server;

  server = lw6ker_server_array_get (server_array, server_id);
  if (server)
    {
      lw6ker_server_disable (server);
      server_array->nb_servers--;
      ret = 1;
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_DEBUG,
		  _("server %x already does not exist, can't disable it"),
		  server_id);
    }

  return ret;
}

int
lw6ker_server_array_sanity_check (lw6ker_server_array_t * server_array,
				  lw6map_rules_t * rules)
{
  int ret = 1;
  int i;
  int found = 0;

  for (i = 0; i < LW6MAP_MAX_NB_SERVERS; ++i)
    {
      ret = ret
	&& lw6ker_server_sanity_check (&(server_array->servers[i]), rules);
      if (server_array->servers[i].enabled)
	{
	  found++;
	}
    }
  if (found != server_array->nb_servers)
    {
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _
		  ("inconsistency in server_array, found %d servers but array reports %d"),
		  found, server_array->nb_servers);
      ret = 0;
    }

  return ret;
}
