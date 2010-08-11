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

#include "../cli.h"
#include "mod-udp.h"
#include "mod-udp-internal.h"

static void *
_init ()
{
  _udp_context_t *udp_context = _mod_udp_init ();

  return (void *) udp_context;
}

static void
_quit (void *cli_context)
{
  _udp_context_t *udp_context = (_udp_context_t *) cli_context;

  if (udp_context)
    {
      _mod_udp_quit (udp_context);
    }
}

static int
_process_oob (void *cli_context, lw6nod_info_t * node_info,
	      lw6cli_oob_data_t * oob_data)
{
  _udp_context_t *udp_context = (_udp_context_t *) cli_context;
  int ret = 0;

  if (udp_context)
    {
      ret = _mod_udp_process_oob (udp_context, node_info, oob_data);
    }

  return ret;
}

static lw6cli_connection_t *
_connect (void *cli_context, char *server_url, char *client_url,
	  char *password)
{
  _udp_context_t *udp_context = (_udp_context_t *) cli_context;
  lw6cli_connection_t *ret = NULL;

  if (udp_context)
    {
      ret = _mod_udp_connect (udp_context, server_url, client_url, password);
    }

  return ret;
}

static void
_close (void *cli_context, lw6cli_connection_t * connection)
{
  _udp_context_t *udp_context = (_udp_context_t *) cli_context;

  if (udp_context)
    {
      _mod_udp_close (udp_context, connection);
    }
}

static int
_send (void *cli_context, lw6cli_connection_t * connection, char *message)
{
  _udp_context_t *udp_context = (_udp_context_t *) cli_context;
  int ret = 0;

  if (udp_context)
    {
      ret = _mod_udp_send (udp_context, connection, message);
    }

  return ret;
}

static char *
_recv (void *cli_context, lw6cli_connection_t * connection)
{
  _udp_context_t *udp_context = (_udp_context_t *) cli_context;
  char *ret = NULL;

  if (udp_context)
    {
      ret = _mod_udp_recv (udp_context, connection);
    }

  return ret;
}

static int
_is_alive (void *cli_context, lw6cli_connection_t * connection)
{
  _udp_context_t *udp_context = (_udp_context_t *) cli_context;
  int ret = 0;

  if (udp_context)
    {
      ret = _mod_udp_is_alive (udp_context, connection);
    }

  return ret;
}

static char *
_repr (void *cli_context, lw6cli_connection_t * connection)
{
  _udp_context_t *udp_context = (_udp_context_t *) cli_context;
  char *ret = NULL;

  if (udp_context)
    {
      ret = _mod_udp_repr (udp_context, connection);
    }

  return ret;
}

static char *
_error (void *cli_context, lw6cli_connection_t * connection)
{
  _udp_context_t *udp_context = (_udp_context_t *) cli_context;
  char *ret = NULL;

  if (udp_context)
    {
      ret = _mod_udp_error (udp_context, connection);
    }

  return ret;
}

lw6sys_module_pedigree_t *
mod_udp_get_pedigree ()
{
  lw6sys_module_pedigree_t *module_pedigree = NULL;

  module_pedigree =
    (lw6sys_module_pedigree_t *)
    LW6SYS_CALLOC (sizeof (lw6sys_module_pedigree_t));
  if (module_pedigree)
    {
      module_pedigree->id = "udp";
      module_pedigree->category = "cli";
      module_pedigree->name = _("UDP");
      module_pedigree->readme =
	_
	("A simple network client which uses raw UDP/IP sockets. The fastest way to communicate.");
      module_pedigree->version = VERSION;
      module_pedigree->copyright =
	"Copyright (C)  2009  Christian Mauduit <ufoot@ufoot.org>";
      module_pedigree->license = "GPLv3+ (GNU GPL version 3 or later)";
      module_pedigree->date = __DATE__;
      module_pedigree->time = __TIME__;
    }

  return module_pedigree;
}

lw6cli_backend_t *
mod_udp_create_backend ()
{
  lw6cli_backend_t *backend;

  backend = LW6SYS_MALLOC (sizeof (lw6cli_backend_t));
  if (backend)
    {
      memset (backend, 0, sizeof (lw6cli_backend_t));

      backend->init = _init;
      backend->quit = _quit;
      backend->process_oob = _process_oob;
      backend->connect = _connect;
      backend->close = _close;
      backend->send = _send;
      backend->recv = _recv;
      backend->is_alive = _is_alive;
      backend->repr = _repr;
      backend->error = _error;
    }

  return backend;
}
