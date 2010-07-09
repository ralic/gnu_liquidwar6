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

#include "p2p.h"
#include "p2p-internal.h"

#define _P2P_SUBDIR "p2p"
#define _CONSTS_FILE "p2p-const.xml"
#define _SQL_DIR "sql"
#define _CREATE_DATABASE_SQL_FILE "create-database.sql"

static void
_read_callback (void *callback_data, char *element, char *key, char *value)
{
  _lw6p2p_consts_t *consts;

  consts = (_lw6p2p_consts_t *) callback_data;

  if (!strcmp (element, "float"))
    {
      lw6cfg_read_xml_float (key, value, "sleep-delay", &consts->sleep_delay);
    }
}

static int
_load_consts (_lw6p2p_consts_t * consts, char *consts_file)
{
  int ret = 0;

  lw6sys_log (LW6SYS_LOG_INFO, _("reading \"%s\""), consts_file);
  ret =
    lw6cfg_read_key_value_xml_file (consts_file, _read_callback,
				    (void *) consts);

  return ret;
}

static char *
_read_query (char *sql_dir, char *query_file)
{
  char *ret = NULL;
  char *filename;

  filename = lw6sys_path_concat (sql_dir, query_file);
  if (filename)
    {
      lw6sys_log (LW6SYS_LOG_INFO, _("reading \"%s\""), filename);
      ret = lw6sys_read_file_content (filename);
      LW6SYS_FREE (filename);
    }

  return ret;
}

static int
_load_sql (_lw6p2p_sql_t * sql, char *sql_dir)
{
  int ret = 0;

  sql->create_database = _read_query (sql_dir, _CREATE_DATABASE_SQL_FILE);

  ret = (sql->create_database != NULL);

  return ret;
}

int
_lw6p2p_data_load (_lw6p2p_data_t * data, char *data_dir)
{
  int ret = 0;
  char *p2p_subdir = NULL;
  char *consts_file = NULL;
  char *sql_dir = NULL;

  p2p_subdir = lw6sys_path_concat (data_dir, _P2P_SUBDIR);
  if (p2p_subdir)
    {
      consts_file = lw6sys_path_concat (p2p_subdir, _CONSTS_FILE);
      sql_dir = lw6sys_path_concat (p2p_subdir, _SQL_DIR);
      if (consts_file && sql_dir)
	{
	  ret = _load_consts (&(data->consts), consts_file)
	    && _load_sql (&(data->sql), sql_dir);
	}
      if (consts_file)
	{
	  LW6SYS_FREE (consts_file);
	}
      if (sql_dir)
	{
	  LW6SYS_FREE (sql_dir);
	}
      LW6SYS_FREE (p2p_subdir);
    }

  return ret;
}

static int
_unload_consts (_lw6p2p_consts_t * consts)
{
  int ret = 1;

  memset (consts, 0, sizeof (_lw6p2p_consts_t));

  return ret;
}

static int
_unload_sql (_lw6p2p_sql_t * sql)
{
  int ret = 1;

  if (sql->create_database)
    {
      LW6SYS_FREE (sql->create_database);
    }

  return ret;
}

int
_lw6p2p_data_unload (_lw6p2p_data_t * data)
{
  int ret = 0;

  ret = _unload_consts (&(data->consts)) && _unload_sql (&(data->sql));

  return ret;
}