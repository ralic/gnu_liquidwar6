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

#include "gl-hud.h"

static void
_warning (const char *func_name)
{
  lw6sys_log (LW6SYS_LOG_WARNING,
	      _("hud backend function \"%s\" is not defined"));
}

int
mod_gl_hud_init (mod_gl_utils_context_t * utils_context,
		 mod_gl_hud_backend_t * backend)
{
  if (backend->init)
    {
      backend->hud_context = backend->init (utils_context);
    }
  else
    {
      _warning (__FUNCTION__);
    }

  return backend->hud_context ? 1 : 0;
}

void
mod_gl_hud_quit (mod_gl_utils_context_t * utils_context,
		 mod_gl_hud_backend_t * backend)
{
  if (backend->quit)
    {
      backend->quit (utils_context, backend->hud_context);
    }
  else
    {
      _warning (__FUNCTION__);
    }
}

void
mod_gl_hud_display_hud (mod_gl_utils_context_t * utils_context,
			mod_gl_hud_backend_t * backend,
			lw6gui_look_t * look,
			lw6ker_game_state_t * game_state,
			lw6pil_local_cursors_t * local_cursors)
{
  if (backend->display_hud)
    {
      backend->display_hud (utils_context, backend->hud_context, look,
			    game_state, local_cursors);
    }
  else
    {
      _warning (__FUNCTION__);
    }
}

void
mod_gl_hud_display_score (mod_gl_utils_context_t * utils_context,
			  mod_gl_hud_backend_t * backend,
			  lw6gui_look_t * look,
			  lw6ker_game_state_t * game_state,
			  lw6pil_local_cursors_t * local_cursors)
{
  if (backend->display_score)
    {
      backend->display_score (utils_context, backend->hud_context, look,
			      game_state, local_cursors);
    }
  else
    {
      _warning (__FUNCTION__);
    }
}
