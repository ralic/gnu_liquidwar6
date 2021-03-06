/*
  Liquid War 6 is a unique multiplayer wargame.
  Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015  Christian Mauduit <ufoot@ufoot.org>

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

#include <time.h>

#include "../../../gfx.h"
#include "gl1-flat.h"
#include "gl1-flat-internal.h"

/*
 * Low-level SDL initialisation.
 */
_mod_gl1_view_flat_context_t *
_mod_gl1_view_flat_init (lw6sys_context_t * sys_context, mod_gl1_utils_context_t * utils_context)
{
  _mod_gl1_view_flat_context_t *flat_context = NULL;

  lw6sys_log (sys_context, LW6SYS_LOG_INFO, _x_ ("init view/flat"));

  flat_context = (_mod_gl1_view_flat_context_t *) LW6SYS_CALLOC (sys_context, sizeof (_mod_gl1_view_flat_context_t));

  if (flat_context)
    {
      if (!_mod_gl1_view_flat_load_data (sys_context, utils_context, flat_context))
	{
	  LW6SYS_FREE (sys_context, flat_context);
	  flat_context = NULL;
	}
    }

  if (!flat_context)
    {
      lw6sys_log (sys_context, LW6SYS_LOG_WARNING, _x_ ("unable to init flat_context"));
    }

  return flat_context;
}

void *
mod_gl1_view_flat_init (lw6sys_context_t * sys_context, mod_gl1_utils_context_t * utils_context)
{
  return (void *) _mod_gl1_view_flat_init (sys_context, utils_context);
}

/*
 * Ends-up all SDL stuff.
 */
void
_mod_gl1_view_flat_quit (lw6sys_context_t * sys_context, mod_gl1_utils_context_t * utils_context, _mod_gl1_view_flat_context_t * flat_context)
{
  lw6sys_log (sys_context, LW6SYS_LOG_INFO, _x_ ("quit view/flat"));

  _mod_gl1_view_flat_game_context_clear (sys_context, utils_context, flat_context, &(flat_context->game_context));
  _mod_gl1_view_flat_cursors_context_clear (sys_context, utils_context, &(flat_context->cursors_context));

  LW6SYS_FREE (sys_context, flat_context);
}

void
mod_gl1_view_flat_quit (lw6sys_context_t * sys_context, mod_gl1_utils_context_t * utils_context, void *flat_context)
{
  _mod_gl1_view_flat_quit (sys_context, utils_context, (_mod_gl1_view_flat_context_t *) flat_context);
}
