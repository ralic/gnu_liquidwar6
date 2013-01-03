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
#endif // HAVE_CONFIG_H

#if LW6_UNIX && HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#include "sys.h"
#include "sys-internal.h"

#define _CALLSTACK_SIZE 200
#define _BACKTRACE_FILE "backtrace.txt"

static char *
get_backtrace_file ()
{
  char *backtrace_file = NULL;
  char *user_dir = NULL;

  user_dir = lw6sys_get_default_user_dir ();
  if (user_dir)
    {
      backtrace_file = lw6sys_path_concat (user_dir, _BACKTRACE_FILE);
      LW6SYS_FREE (user_dir);
    }

  return backtrace_file;
}

/**
 * lw6sys_backtrace
 *
 * @skip: number of calls to skip
 *
 * Returns the current backtrace as a comma separated list.
 * This can typically be used for debugging purposes. Not available
 * on some platforms, including mingw32, it requires backtrace_symbols
 * to be defined. Note that this function calls internal string functions
 * so it makes usage of the sys module in many ways, therefore should
 * be used only in other modules, it can't be used for debugging of
 * internal memory functions for instance. To debug those, use
 * backtrace_symbols_fd directly (or maybe just gdb...). The skip
 * parameter allows you to skip caller's stack, 0 will display
 * everything but @lw6sys_backtrace itself.
 *
 * Return value: dynamically allocated string
 */
char *
lw6sys_backtrace (int skip)
{
  char *ret = NULL;
  char *backtrace_file = NULL;

#if LW6_UNIX && HAVE_EXECINFO_H
  void *callstack[_CALLSTACK_SIZE];
  char **symbols = NULL;
  int i = 0, n = 0;
  char *tmp = NULL;

  skip = lw6sys_imax (skip, 0);
  memset (callstack, 0, sizeof (void *) * _CALLSTACK_SIZE);
  n = backtrace (callstack, _CALLSTACK_SIZE);
  if (n > 1)
    {
      symbols = backtrace_symbols (callstack, n);
      if (symbols)
	{
	  /*
	   * We skip symbol 0 (which is just *this* function
	   */
	  if (n > skip + 1)
	    {
	      ret = lw6sys_str_copy (symbols[skip + 1]);
	    }
	  if (ret)
	    {
	      for (i = skip + 2; i < n; i++)
		{
		  tmp = lw6sys_new_sprintf ("%s, %s", ret, symbols[i]);
		  if (tmp)
		    {
		      LW6SYS_FREE (ret);
		      ret = tmp;
		      tmp = NULL;
		    }
		}
	    }
	  free (symbols);	// call to real free() and not LW6SYS_FREE
	}
    }
#else
  ret = lw6sys_new_sprintf (_x_ ("no backtrace"));
#endif

  if (ret)
    {
      backtrace_file = get_backtrace_file ();
      if (backtrace_file)
	{
	  /*
	   * On a dump, we would log stuff, but here no, we do
	   * not want to use complex internal functions.
	   */
	  lw6sys_write_file_content (backtrace_file, ret);
	  LW6SYS_FREE (backtrace_file);
	}
    }

  return ret;
}
