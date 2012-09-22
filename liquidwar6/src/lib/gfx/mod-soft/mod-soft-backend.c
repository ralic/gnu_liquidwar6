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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "../gfx.h"
#include "mod-soft.h"
#include "mod-soft-internal.h"

/*
 * http://www.gnu.org/prep/standards/html_node/Dynamic-Plug_002dIn-Interfaces.html
 */
void
mod_soft_is_GPL_compatible ()
{
}

static void *
_init (int argc, const char *argv[], lw6gui_video_mode_t * video_mode,
       lw6gui_resize_callback_func_t resize_callback)
{
  _mod_soft_context_t *mod_soft_context =
    _mod_soft_init (argc, argv, video_mode, resize_callback);

  return (void *) mod_soft_context;
}

static void
_quit (void *gfx_context)
{
  _mod_soft_context_t *mod_soft_context = (_mod_soft_context_t *) gfx_context;

  if (mod_soft_context)
    {
      _mod_soft_quit (mod_soft_context);
    }
}

static char *
_repr (void *gfx_context, u_int32_t id)
{
  char *ret = NULL;
  _mod_soft_context_t *mod_soft_context = (_mod_soft_context_t *) gfx_context;

  if (mod_soft_context)
    {
      ret = _mod_soft_repr (mod_soft_context, id);
    }

  return ret;
}

static int
_set_video_mode (void *gfx_context, lw6gui_video_mode_t * video_mode)
{
  int ret = 0;
  _mod_soft_context_t *mod_soft_context = (_mod_soft_context_t *) gfx_context;

  if (mod_soft_context)
    {
      ret = _mod_soft_set_video_mode (mod_soft_context, video_mode);
    }

  return ret;
}

static int
_get_video_mode (void *gfx_context, lw6gui_video_mode_t * video_mode)
{
  int ret = 0;
  _mod_soft_context_t *mod_soft_context = (_mod_soft_context_t *) gfx_context;

  if (mod_soft_context)
    {
      ret = _mod_soft_get_video_mode (mod_soft_context, video_mode);
    }

  return ret;
}

static int
_get_fullscreen_modes (void *gfx_context, lw6gui_fullscreen_modes_t * modes)
{
  int ret = 0;
  _mod_soft_context_t *mod_soft_context = (_mod_soft_context_t *) gfx_context;

  if (mod_soft_context)
    {
      ret = _mod_soft_get_fullscreen_modes (mod_soft_context, modes);
    }

  return ret;
}

static lw6gui_input_t *
_pump_events (void *gfx_context)
{
  lw6gui_input_t *ret = NULL;
  _mod_soft_context_t *mod_soft_context = (_mod_soft_context_t *) gfx_context;

  if (mod_soft_context)
    {
      _lw6gfx_sdl_timer_update (&(mod_soft_context->sdl_context));
      ret =
	_lw6gfx_sdl_pump_events (&(mod_soft_context->sdl_context), NULL,
				 NULL);
    }

  return ret;
}

static int
_display (void *gfx_context, int mask, lw6gui_look_t * look,
	  lw6map_level_t * level,
	  lw6ker_game_struct_t * game_struct,
	  lw6ker_game_state_t * game_state,
	  lw6pil_local_cursors_t * local_cursors,
	  lw6gui_menu_t * menu,
	  float progress,
	  float fps, float mps, char **log_list, int capture, int gfx_debug,
	  int debug_team_id, int debug_layer_id)
{
  int ret = 0;
  _mod_soft_context_t *mod_soft_context = (_mod_soft_context_t *) gfx_context;

  if (mod_soft_context)
    {
      // todo ...
      lw6sys_log (LW6SYS_LOG_WARNING, _x_ ("%s not implemented yet"),
		  __FUNCTION__);
    }

  return ret;
}

lw6sys_module_pedigree_t *
mod_soft_get_pedigree ()
{
  lw6sys_module_pedigree_t *module_pedigree = NULL;

  module_pedigree =
    (lw6sys_module_pedigree_t *)
    LW6SYS_CALLOC (sizeof (lw6sys_module_pedigree_t));
  if (module_pedigree)
    {
      module_pedigree->id = "soft";
      module_pedigree->category = "gfx";
      module_pedigree->name = _("Software / SDL");
      module_pedigree->readme =
	_x_ ("A plain and basic 2D only software renderer, based on SDL.");
      module_pedigree->version = VERSION;
      module_pedigree->copyright =
	"Copyright (C)  2012  Christian Mauduit <ufoot@ufoot.org>";
      module_pedigree->license = "GPLv3+ (GNU GPL version 3 or later)";
      module_pedigree->date = __DATE__;
      module_pedigree->time = __TIME__;
    }

  return module_pedigree;
}

lw6gfx_backend_t *
mod_soft_create_backend ()
{
  lw6gfx_backend_t *backend;

  backend = LW6SYS_CALLOC (sizeof (lw6gfx_backend_t));
  if (backend)
    {
      backend->init = _init;
      backend->quit = _quit;
      backend->repr = _repr;
      backend->set_video_mode = _set_video_mode;
      backend->get_video_mode = _get_video_mode;
      backend->get_fullscreen_modes = _get_fullscreen_modes;
      backend->pump_events = _pump_events;
      backend->display = _display;
    }

  return backend;
}