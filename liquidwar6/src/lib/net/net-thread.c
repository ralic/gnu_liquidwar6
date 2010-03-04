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

#include "net.h"
#include "net-internal.h"

static void
free_thread (void *handler)
{
  /*
   * Join will free the handler himself, after calling
   * the callback_join func which will actually free
   * the data handler.
   */
  lw6sys_thread_join (handler);
}

int
_lw6net_thread_init ()
{
  int ret = 0;

  _lw6net_global_context->threads = lw6sys_assoc_new (&free_thread);

  if (_lw6net_global_context->threads)
    {
      ret = 1;
    }

  return ret;
}

void
_lw6net_thread_quit ()
{
  if (_lw6net_global_context->threads)
    {
      lw6sys_assoc_free (_lw6net_global_context->threads);
      _lw6net_global_context->threads = NULL;
    }
}

static char *
make_str_id (void *handler)
{
  char *ret = NULL;

  ret = lw6sys_new_sprintf ("net-%d", lw6sys_thread_get_id (handler));

  return ret;
}

int
_lw6net_thread_register (void *handler)
{
  int ret = 0;
  char *str_id;

  str_id = make_str_id (handler);
  if (str_id)
    {
      lw6sys_assoc_set (&(_lw6net_global_context->threads), str_id, handler);
      LW6SYS_FREE (str_id);
      ret = 1;
    }

  return ret;
}

int
_lw6net_thread_unregister (void *handler)
{
  int ret = 0;
  char *str_id;

  str_id = make_str_id (handler);
  if (str_id)
    {
      lw6sys_assoc_unset (_lw6net_global_context->threads, str_id);
      LW6SYS_FREE (str_id);
      ret = 1;
    }

  return ret;
}

int
_lw6net_thread_vacuum ()
{
  int ret = 0;
  lw6sys_list_t *keys;
  char *key;
  void *handler;

  if (_lw6net_global_context->threads)
    {
      keys = lw6sys_assoc_keys (_lw6net_global_context->threads);
      if (keys)
	{
	  ret = 1;
	  while (!lw6sys_list_is_empty (keys))
	    {
	      key = lw6sys_lifo_pop (&keys);
	      if (key)
		{
		  handler =
		    lw6sys_assoc_get (_lw6net_global_context->threads, key);
		  if (handler)
		    {
		      if (lw6sys_thread_is_callback_done (handler))
			{
			  /*
			   * This will call "join" automatically.
			   */
			  lw6sys_log (LW6SYS_LOG_INFO,
				      _("vacuum thread \"%s\""), key);
			  lw6sys_assoc_unset (_lw6net_global_context->threads,
					      key);
			}
		    }
		  LW6SYS_FREE (key);
		}
	    }
	  lw6sys_list_free (keys);
	}
    }

  return ret;
}
