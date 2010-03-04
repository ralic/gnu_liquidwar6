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

#ifndef LIQUIDWAR6GFX_MOD_GL_HUD_TACTICAL_H
#define LIQUIDWAR6GFX_MOD_GL_HUD_TACTICAL_H

#include "../../../gfx.h"
#include "../../gl-utils/gl-utils.h"
#include "../gl-hud.h"

extern mod_gl_hud_backend_t
  * mod_gl_hud_tactical_create_backend (mod_gl_utils_context_t *
					utils_context);

/*
 * In display.c
 */
extern void mod_gl_hud_tactical_display_hud (mod_gl_utils_context_t *
					     utils_context, void *hud_context,
					     lw6gui_look_t * look,
					     lw6ker_game_state_t *
					     game_state,
					     lw6pil_local_cursors_t *
					     local_cursors);
extern void mod_gl_hud_tactical_display_score (mod_gl_utils_context_t *
					       utils_context,
					       void *hud_context,
					       lw6gui_look_t * look,
					       lw6ker_game_state_t *
					       game_state,
					       lw6pil_local_cursors_t *
					       local_cursors);

/*
 * In setup.c
 */
extern void *mod_gl_hud_tactical_init (mod_gl_utils_context_t *
				       utils_context);
extern void mod_gl_hud_tactical_quit (mod_gl_utils_context_t * utils_context,
				      void *hud_context);

#endif
