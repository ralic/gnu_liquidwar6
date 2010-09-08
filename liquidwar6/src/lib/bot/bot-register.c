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

#include "bot.h"
#ifdef LW6_ALLINONE
#include "mod-idiot/mod-idiot.h"
#include "mod-random/mod-random.h"
#endif

lw6sys_assoc_t *
lw6bot_get_backends (int argc, char *argv[])
{
  lw6sys_assoc_t *ret = NULL;

#ifdef LW6_ALLINONE
  lw6sys_module_pedigree_t *module_pedigree = NULL;

  ret = lw6sys_assoc_new (lw6sys_free_callback);
  if (ret)
    {
      module_pedigree = mod_idiot_get_pedigree ();
      if (module_pedigree)
	{
	  lw6sys_assoc_set (&ret, module_pedigree->id,
			    lw6sys_str_copy (module_pedigree->name));
	  LW6SYS_FREE (module_pedigree);
	}
      module_pedigree = mod_random_get_pedigree ();
      if (module_pedigree)
	{
	  lw6sys_assoc_set (&ret, module_pedigree->id,
			    lw6sys_str_copy (module_pedigree->name));
	  LW6SYS_FREE (module_pedigree);
	}
    }
#else
  ret = lw6dyn_list_backends (argc, argv, "bot");
#endif

  return ret;
}

lw6bot_backend_t *
lw6bot_create_backend (int argc, char *argv[], char *name)
{
  lw6bot_backend_t *backend = NULL;
#ifdef LW6_ALLINONE
  if (name && !strcmp (name, "idiot"))
    {
      backend = mod_idiot_create_backend ();
    }
  if (name && !strcmp (name, "random"))
    {
      backend = mod_random_create_backend ();
    }

  if (backend)
    {
      backend->argc = argc;
      backend->argv = argv;
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _x_ ("bot backend \"%s\" does not exist"), name);
    }
#else
  lw6dyn_dl_handle_t *backend_handle = NULL;

  backend_handle = lw6dyn_dlopen_backend (argc, argv, "bot", name);

  if (backend_handle)
    {
      char *init_func_name;
      lw6bot_backend_t *(*init_func) ();

      init_func_name =
	lw6sys_new_sprintf (LW6DYN_CREATE_BACKEND_FUNC_FORMAT, name);
      if (init_func_name)
	{
	  init_func = lw6dyn_dlsym (backend_handle, init_func_name);
	  if (init_func)
	    {
	      backend = init_func ();
	    }
	  else
	    {
	      lw6sys_log (LW6SYS_LOG_WARNING,
			  _x_
			  ("unable to find function \"%s\" in server backend \"%s\""),
			  init_func_name, name);
	    }

	  LW6SYS_FREE (init_func_name);
	}
    }

  if (backend && backend_handle)
    {
      backend->dl_handle = backend_handle;
      backend->argc = argc;
      backend->argv = argv;
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _x_ ("unable to open server backend \"%s\""), name);
    }
#endif

  if (backend)
    {
      static u_int32_t seq_id = 0;

      backend->id = 0;
      while (!(backend->id))
	{
	  backend->id = ++seq_id;
	}
    }

  return backend;
}

void
lw6bot_destroy_backend (lw6bot_backend_t * backend)
{
#ifndef LW6_ALLINONE
  lw6dyn_dlclose_backend (backend->dl_handle);
#endif
  LW6SYS_FREE (backend);
}
