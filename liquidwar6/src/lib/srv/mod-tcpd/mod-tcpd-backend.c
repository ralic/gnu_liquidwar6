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
#include "mod-tcpd.h"
#include "mod-tcpd-internal.h"

/*
 * http://www.gnu.org/prep/standards/html_node/Dynamic-Plug_002dIn-Interfaces.html
 */
void
mod_tcpd_is_GPL_compatible ()
{
}

static void *
_init (int argc, char *argv[], lw6srv_listener_t * listener)
{
  _tcpd_context_t *tcpd_context = _mod_tcpd_init (argc, argv, listener);

  return (void *) tcpd_context;
}

static void
_quit (void *srv_context)
{
  _tcpd_context_t *tcpd_context = (_tcpd_context_t *) srv_context;

  if (tcpd_context)
    {
      _mod_tcpd_quit (tcpd_context);
    }
}

static int
_analyse_tcp (void *srv_context, lw6srv_tcp_accepter_t * tcp_accepter,
	      lw6nod_info_t * node_info,
	      u_int64_t * remote_id, char **remote_url)
{
  _tcpd_context_t *tcpd_context = (_tcpd_context_t *) srv_context;
  int ret = 0;

  if (tcpd_context)
    {
      ret =
	_mod_tcpd_analyse_tcp (tcpd_context, tcp_accepter, node_info,
			       remote_id, remote_url);
    }

  return ret;
}

static int
_analyse_udp (void *srv_context, lw6srv_udp_buffer_t * udp_buffer,
	      lw6nod_info_t * node_info,
	      u_int64_t * remote_id, char **remote_url)
{
  _tcpd_context_t *tcpd_context = (_tcpd_context_t *) srv_context;
  int ret = 0;

  if (tcpd_context)
    {
      ret =
	_mod_tcpd_analyse_udp (tcpd_context, udp_buffer, node_info, remote_id,
			       remote_url);
    }

  return ret;
}

static int
_process_oob (void *srv_context, lw6nod_info_t * node_info,
	      lw6srv_oob_data_t * oob_data)
{
  _tcpd_context_t *tcpd_context = (_tcpd_context_t *) srv_context;
  int ret = 0;

  if (tcpd_context)
    {
      ret = _mod_tcpd_process_oob (tcpd_context, node_info, oob_data);
    }

  return ret;
}

static lw6cnx_connection_t *
_open (void *srv_context, lw6srv_listener_t * listener, char *local_url,
       char *remote_url, char *remote_ip,
       int remote_port, char *password,
       u_int64_t local_id, u_int64_t remote_id, int dns_ok,
       int network_reliability, lw6cnx_recv_callback_t recv_callback_func,
       void *recv_callback_data)
{
  _tcpd_context_t *tcpd_context = (_tcpd_context_t *) srv_context;
  lw6cnx_connection_t *ret = NULL;

  if (tcpd_context)
    {
      ret =
	_mod_tcpd_open (tcpd_context, listener, local_url, remote_url,
			remote_ip, remote_port, password, local_id, remote_id,
			dns_ok, network_reliability, recv_callback_func,
			recv_callback_data);
    }

  return ret;
}

static int
_feed_with_tcp (void *srv_context, lw6cnx_connection_t * connection,
		lw6srv_tcp_accepter_t * tcp_accepter)
{
  _tcpd_context_t *tcpd_context = (_tcpd_context_t *) srv_context;
  int ret = 0;

  if (connection)
    {
      ret = _mod_tcpd_feed_with_tcp (tcpd_context, connection, tcp_accepter);
    }

  return ret;
}

static int
_feed_with_udp (void *srv_context, lw6cnx_connection_t * connection,
		lw6srv_udp_buffer_t * udp_buffer)
{
  _tcpd_context_t *tcpd_context = (_tcpd_context_t *) srv_context;
  int ret = 0;

  if (connection)
    {
      ret = _mod_tcpd_feed_with_udp (tcpd_context, connection, udp_buffer);
    }

  return ret;
}

static void
_close (void *srv_context, lw6cnx_connection_t * connection)
{
  _tcpd_context_t *tcpd_context = (_tcpd_context_t *) srv_context;
  if (tcpd_context)
    {
      _mod_tcpd_close (tcpd_context, connection);
    }
}

static int
_send (void *srv_context, lw6cnx_connection_t * connection,
       u_int32_t physical_ticket_sig, u_int32_t logical_ticket_sig,
       u_int64_t logical_from_id, u_int64_t logical_to_id, char *message)
{
  _tcpd_context_t *tcpd_context = (_tcpd_context_t *) srv_context;
  int ret = 0;

  if (tcpd_context)
    {
      ret =
	_mod_tcpd_send (tcpd_context, connection, physical_ticket_sig,
			logical_ticket_sig, logical_from_id, logical_to_id,
			message);
    }

  return ret;
}

static void
_poll (void *srv_context, lw6cnx_connection_t * connection)
{
  _tcpd_context_t *tcpd_context = (_tcpd_context_t *) srv_context;

  if (tcpd_context)
    {
      _mod_tcpd_poll (tcpd_context, connection);
    }
}

static char *
_repr (void *srv_context, lw6cnx_connection_t * connection)
{
  _tcpd_context_t *tcpd_context = (_tcpd_context_t *) srv_context;
  char *ret = NULL;

  if (tcpd_context)
    {
      ret = _mod_tcpd_repr (tcpd_context, connection);
    }

  return ret;
}

lw6sys_module_pedigree_t *
mod_tcpd_get_pedigree ()
{
  lw6sys_module_pedigree_t *module_pedigree = NULL;

  module_pedigree =
    (lw6sys_module_pedigree_t *)
    LW6SYS_CALLOC (sizeof (lw6sys_module_pedigree_t));
  if (module_pedigree)
    {
      module_pedigree->id = "tcpd";
      module_pedigree->category = "srv";
      module_pedigree->name = _x_ ("TCPD");
      module_pedigree->readme =
	_x_ ("Basic TCP/IP server, the most reliable way to communicate.");
      module_pedigree->version = VERSION;
      module_pedigree->copyright =
	"Copyright (C)  2010  Christian Mauduit <ufoot@ufoot.org>";
      module_pedigree->license = "GPLv3+ (GNU GPL version 3 or later)";
      module_pedigree->date = __DATE__;
      module_pedigree->time = __TIME__;
    }

  return module_pedigree;
}

lw6srv_backend_t *
mod_tcpd_create_backend ()
{
  lw6srv_backend_t *backend;

  backend = LW6SYS_MALLOC (sizeof (lw6srv_backend_t));
  if (backend)
    {
      memset (backend, 0, sizeof (lw6srv_backend_t));

      backend->init = _init;
      backend->quit = _quit;
      backend->analyse_tcp = _analyse_tcp;
      backend->analyse_udp = _analyse_udp;
      backend->process_oob = _process_oob;
      backend->open = _open;
      backend->feed_with_tcp = _feed_with_tcp;
      backend->feed_with_udp = _feed_with_udp;
      backend->close = _close;
      backend->send = _send;
      backend->poll = _poll;
      backend->repr = _repr;
    }

  return backend;
}
