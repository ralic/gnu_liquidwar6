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

#include "../srv.h"
#include "mod-httpd-internal.h"

int
_mod_httpd_can_handle_tcp (_httpd_context_t * httpd_context,
			   lw6srv_tcp_accepter_t * tcp_accepter)
{
  int ret = 0;

  if (!strncmp
      (tcp_accepter->first_line, _MOD_HTTPD_PROTOCOL_GET_STRING,
       _MOD_HTTPD_PROTOCOL_GET_SIZE)
      || !strncmp (tcp_accepter->first_line, _MOD_HTTPD_PROTOCOL_POST_STRING,
		   _MOD_HTTPD_PROTOCOL_POST_SIZE)
      || !strncmp (tcp_accepter->first_line, _MOD_HTTPD_PROTOCOL_HEAD_STRING,
		   _MOD_HTTPD_PROTOCOL_HEAD_SIZE))
    {
      lw6sys_log (LW6SYS_LOG_NOTICE, _("recognized httpd protocol"));
      ret = 1;
    }

  return ret;
}

int
_mod_httpd_can_handle_udp (_httpd_context_t * httpd_context,
			   lw6srv_udp_buffer_t * udp_buffer)
{
  int ret = 0;

  /*
   * UDP is never handled by mod_httpd, return always false
   */

  return ret;
}

lw6srv_connection_t *
_mod_httpd_accept_tcp (_httpd_context_t * httpd_context,
		       lw6srv_tcp_accepter_t * tcp_accepter, char *password)
{
  lw6srv_connection_t *ret = NULL;

  // todo

  return ret;
}

lw6srv_connection_t *
_mod_httpd_new_udp (_httpd_context_t * httpd_context,
		    lw6srv_udp_buffer_t * udp_buffer, char *password)
{
  lw6srv_connection_t *ret = NULL;

  /*
   * UDP is never handled by mod_httpd, return always NULL
   */

  return ret;
}

int
_mod_httpd_is_associated_with_udp (_httpd_context_t * httpd_context,
				   lw6srv_connection_t * connection,
				   lw6srv_udp_buffer_t * udp_buffer)
{
  int ret = 0;

  /*
   * UDP is never handled by mod_httpd, return always false
   */

  return ret;
}

int
_mod_httpd_update_with_udp (_httpd_context_t * httpd_context,
			    lw6srv_connection_t * connection,
			    lw6srv_udp_buffer_t * udp_buffer)
{
  int ret = 0;

  /*
   * UDP is never handled by mod_httpd, return always false
   */

  return ret;
}
