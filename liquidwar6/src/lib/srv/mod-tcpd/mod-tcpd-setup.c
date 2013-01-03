/*
  Liquid War 6 is a unique multiplayer wargame.
  Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013  Christian Mauduit <ufoot@ufoot.org>

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
#include "mod-tcpd-internal.h"

_mod_tcpd_context_t *
_mod_tcpd_init (int argc, const char *argv[],
		lw6cnx_properties_t * properties,
		lw6srv_listener_t * listener)
{
  _mod_tcpd_context_t *tcpd_context = NULL;
  char *data_dir = NULL;
  int ok = 0;

  lw6sys_log (LW6SYS_LOG_INFO, _x_ ("tcpd init"));

  tcpd_context =
    (_mod_tcpd_context_t *) LW6SYS_CALLOC (sizeof (_mod_tcpd_context_t));
  if (tcpd_context)
    {
      data_dir = lw6sys_get_data_dir (argc, argv);
      if (data_dir)
	{
	  if (_mod_tcpd_load_data (&(tcpd_context->data), data_dir))
	    {
	      properties->hint_timeout =
		tcpd_context->data.consts.error_timeout;
	      properties->ping_alter_base =
		tcpd_context->data.consts.ping_alter_base;
	      properties->ping_alter_percent =
		tcpd_context->data.consts.ping_alter_percent;
	      properties->reliable = 1;
	      properties->backend_id = MOD_TCPD_BACKEND_ID;
	      ok = 1;
	    }
	  LW6SYS_FREE (data_dir);
	}
      if (!ok)
	{
	  _mod_tcpd_quit (tcpd_context);
	  tcpd_context = NULL;
	}
    }

  if (!tcpd_context)
    {
      lw6sys_log (LW6SYS_LOG_ERROR, _("can't initialize mod_tcpd"));
    }

  return tcpd_context;
}

void
_mod_tcpd_quit (_mod_tcpd_context_t * tcpd_context)
{
  lw6sys_log (LW6SYS_LOG_INFO, _x_ ("tcpd quit"));
  _mod_tcpd_unload_data (&(tcpd_context->data));
  LW6SYS_FREE (tcpd_context);
}
