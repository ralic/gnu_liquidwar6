/*
  Liquid War 6 is a unique multiplayer wargame.
  Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015  Christian Mauduit <ufoot@ufoot.org>

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

#include "../cli.h"
#include "mod-http-internal.h"

lw6cnx_connection_t *
_mod_http_open (lw6sys_context_t * sys_context, _mod_http_context_t * http_context, const char *local_url,
		const char *remote_url, const char *remote_ip,
		int remote_port, const char *password, u_int64_t local_id, u_int64_t remote_id, int dns_ok, int network_reliability)
{
  lw6cnx_connection_t *ret = NULL;
  _mod_http_specific_data_t *specific_data = NULL;

  lw6sys_log (sys_context, LW6SYS_LOG_DEBUG, _x_ ("_mod_http_open \"%s\""), remote_url);
  ret = lw6cnx_connection_new (sys_context, local_url, remote_url, remote_ip, remote_port, password, local_id, remote_id, dns_ok, network_reliability);
  if (ret)
    {
      ret->backend_specific_data = LW6SYS_CALLOC (sys_context, sizeof (_mod_http_specific_data_t));
      specific_data = (_mod_http_specific_data_t *) ret->backend_specific_data;
      if (specific_data)
	{
	  specific_data->query_threads = lw6sys_list_new (sys_context, _mod_http_query_thread_free_list_item);
	  if (specific_data->query_threads)
	    {
	      lw6sys_log (sys_context, LW6SYS_LOG_DEBUG, _x_ ("open http connection with \"%s\""), remote_url);
	    }
	  else
	    {
	      _mod_http_close (sys_context, http_context, ret);
	      ret = NULL;
	    }
	}
      else
	{
	  _mod_http_close (sys_context, http_context, ret);
	  ret = NULL;
	}
    }

  return ret;
}

void
_mod_http_close (lw6sys_context_t * sys_context, _mod_http_context_t * http_context, lw6cnx_connection_t * connection)
{
  _mod_http_specific_data_t *specific_data = (_mod_http_specific_data_t *) connection->backend_specific_data;

  if (specific_data)
    {
      if (specific_data->query_threads)
	{
	  lw6sys_list_free (sys_context, specific_data->query_threads);
	}
      LW6SYS_FREE (sys_context, specific_data);
    }
  lw6cnx_connection_free (sys_context, connection);
}

int
_mod_http_timeout_ok (lw6sys_context_t * sys_context, _mod_http_context_t * http_context, int64_t origin_timestamp)
{
  int ret = 0;
  int d = 0;

  /*
   * We check the time difference against the absolute delta, this
   * way should we have any inconsistency, time difference in the
   * wrong way because origin_timestamp is wrecked, then after
   * some time assumed to be reasonnable (depends on settings)
   * it will be over.
   */
  d = abs (lw6sys_get_timestamp (sys_context) - origin_timestamp);
  ret = (d < (http_context->data.consts.global_timeout * 1000));

  return ret;
}
