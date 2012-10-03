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

#ifndef LIQUIDWAR6GFX_MOD_CACA_INTERNAL_H
#define LIQUIDWAR6GFX_MOD_CACA_INTERNAL_H

#include "../gfx-internal.h"
#include "mod-caca.h"

#include <caca.h>

typedef struct _mod_caca_path_s
{
  char *data_dir;
  char *debug_dir;
}
_mod_caca_path_t;

typedef struct _mod_caca_const_data_s
{
  int dummy;
}
_mod_caca_const_data_t;

typedef struct _mod_caca_context_s
{
  lw6gui_resize_callback_func_t resize_callback;
  lw6gui_video_mode_t video_mode;
  _mod_caca_path_t path;
  _mod_caca_const_data_t const_data;
}
_mod_caca_context_t;

/* mod-caca-const.c */
extern int _mod_caca_load_consts (_mod_caca_context_t * context);
extern void _mod_caca_unload_consts (_mod_caca_context_t * context);

/* mod-caca-mode.c */
extern int _mod_caca_set_video_mode (_mod_caca_context_t * caca_context,
				     lw6gui_video_mode_t * video_mode);
extern int _mod_caca_resize_video_mode (_mod_caca_context_t * caca_context,
					lw6gui_video_mode_t * video_mode);
extern int _mod_caca_get_video_mode (_mod_caca_context_t * caca_context,
				     lw6gui_video_mode_t * video_mode);
extern int _mod_caca_set_resize_callback (_mod_caca_context_t *
					  caca_context,
					  lw6gui_resize_callback_func_t
					  resize_callback);
extern void _mod_caca_call_resize_callback (_mod_caca_context_t *
					    caca_context);

/* mod-caca-path.c */
extern int _mod_caca_path_init (_mod_caca_path_t *
				path, int argc, const char *argv[]);
extern void _mod_caca_path_quit (_mod_caca_path_t * path);

/* mod-caca-repr.c */
extern char *_mod_caca_repr (_mod_caca_context_t * caca_context,
			     u_int32_t id);

/* mod-caca-resolution.c */
extern int _mod_caca_get_fullscreen_modes (_mod_caca_context_t *
					   caca_context,
					   lw6gui_fullscreen_modes_t * modes);
extern void _mod_caca_find_closest_resolution (_mod_caca_context_t *
					       caca_context,
					       int *closest_width,
					       int *closest_height,
					       int wished_width,
					       int wished_height);

/* mod-caca-setup.c */
extern _mod_caca_context_t *_mod_caca_init (int argc, const char *argv[],
					    lw6gui_video_mode_t *
					    video_mode,
					    lw6gui_resize_callback_func_t
					    resize_callback);
extern void _mod_caca_quit (_mod_caca_context_t * caca_context);

#endif // LIQUIDWAR6GFX_MOD_CACA_INTERNAL_H