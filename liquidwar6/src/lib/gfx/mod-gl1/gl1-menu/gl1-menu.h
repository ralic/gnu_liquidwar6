/*
  Liquid War 6 is a unique multiplayer wargame.
  Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012  Christian Mauduit <ufoot@ufoot.org>

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

#ifndef LIQUIDWAR6GFX_MOD_GL1_MENU_H
#define LIQUIDWAR6GFX_MOD_GL1_MENU_H

#include <ltdl.h>

#include "../../../sys/sys.h"
#include "../../../map/map.h"
#include "../mod-gl1.h"
#include "../gl1-utils/gl1-utils.h"

typedef struct mod_gl1_menu_backend_s
{
  void *menu_context;
  char *backend_name;

  void *(*init) (mod_gl1_utils_context_t * utils_context);
  void (*quit) (mod_gl1_utils_context_t * utils_context, void *menu_context);
  void (*display_menu) (mod_gl1_utils_context_t * utils_context,
			void *menu_context, lw6gui_look_t * look,
			lw6gui_menu_t * menu);
  void (*display_progress) (mod_gl1_utils_context_t * utils_context,
			    void *menu_context, lw6gui_look_t * look,
			    float progress);
  void (*display_meta) (mod_gl1_utils_context_t * utils_context,
			void *menu_context, lw6gui_look_t * look,
			lw6gui_menu_t * menu);
}
mod_gl1_menu_backend_t;

/*
 * In api.c
 */
extern int mod_gl1_menu_init (mod_gl1_utils_context_t * utils_context,
			      mod_gl1_menu_backend_t * backend);
extern void mod_gl1_menu_quit (mod_gl1_utils_context_t * utils_context,
			       mod_gl1_menu_backend_t * backend);
extern void mod_gl1_menu_display_menu (mod_gl1_utils_context_t *
				       utils_context,
				       mod_gl1_menu_backend_t * backend,
				       lw6gui_look_t * look,
				       lw6gui_menu_t * menu);
extern void mod_gl1_menu_display_progress (mod_gl1_utils_context_t *
					   utils_context,
					   mod_gl1_menu_backend_t * backend,
					   lw6gui_look_t * look,
					   float progress);
extern void mod_gl1_menu_display_meta (mod_gl1_utils_context_t *
				       utils_context,
				       mod_gl1_menu_backend_t * backend,
				       lw6gui_look_t * look,
				       lw6gui_menu_t * menu);

/*
 * In backend.c
 */
extern mod_gl1_menu_backend_t
  * mod_gl1_menu_create_backend (mod_gl1_utils_context_t * utils_context,
				 char *name);
extern void mod_gl1_menu_destroy_backend (mod_gl1_utils_context_t *
					  utils_context,
					  mod_gl1_menu_backend_t * backend);

#endif