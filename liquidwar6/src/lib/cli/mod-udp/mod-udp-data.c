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
#include "mod-udp-internal.h"

#define _UDP_SUBDIR "cli/udp"
#define _CONSTS_FILE "udp-const.xml"

static void
_read_callback (void *callback_data, char *element, char *key, char *value)
{
  _udp_consts_t *consts;

  consts = (_udp_consts_t *) callback_data;

  if (!strcmp (element, "int"))
    {
      lw6cfg_read_xml_int (key, value, "global-timeout",
			   &(consts->global_timeout));
      lw6cfg_read_xml_int (key, value, "broadcast-timeout",
			   &(consts->broadcast_timeout));
    }
}

static int
_load_consts (_udp_consts_t * consts, char *consts_file)
{
  int ret = 0;

  lw6sys_log (LW6SYS_LOG_INFO, _("reading \"%s\""), consts_file);

  ret =
    lw6cfg_read_key_value_xml_file (consts_file, _read_callback,
				    (void *) consts);

  return ret;
}

int
_mod_udp_load_data (_udp_data_t * udp_data, char *data_dir)
{
  int ret = 0;
  char *udp_subdir = NULL;
  char *consts_file = NULL;

  udp_subdir = lw6sys_path_concat (data_dir, _UDP_SUBDIR);
  if (udp_subdir)
    {
      consts_file = lw6sys_path_concat (udp_subdir, _CONSTS_FILE);
      if (consts_file)
	{
	  ret = _load_consts (&(udp_data->consts), consts_file);
	  LW6SYS_FREE (consts_file);
	}
      LW6SYS_FREE (udp_subdir);
    }

  return ret;
}

static int
_unload_consts (_udp_consts_t * consts)
{
  int ret = 1;

  memset (consts, 0, sizeof (_udp_consts_t));

  return ret;
}

void
_mod_udp_unload_data (_udp_data_t * udp_data)
{
  _unload_consts (&(udp_data->consts));
}