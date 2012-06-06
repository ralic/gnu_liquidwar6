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
#endif

#include <math.h>

#include "liquidwar6.h"

/*
 * Helper function, creates a 0 terminated string
 * from a Guile string. A very common task.
 * Returned pointer must be freed.
 */
static char *
to_0str (SCM string)
{
  char *c_string = NULL;
  int length = 0;
  int c_length = 0;

  if (SCM_NFALSEP (string))
    {
      SCM_ASSERT (scm_is_string (string), string, SCM_ARG1, __FUNCTION__);
      /*
       * See the comment in sys/sys-str.c to see why we use
       * 2 trailing '\0' at the end of the string.
       */
      length = scm_c_string_length (string);
    }

  /*
   * We use a double size + 3 extra bytes to make double-triple
   * sure it's zero terminated, you never know, if it's interpreted
   * in UTF-16, who knows, I just don't want to take any risk,
   * string handling is awfull enough...
   */
  c_length = length * 2 * sizeof (char);
  c_string = (char *) LW6SYS_CALLOC (c_length + 3);
  if (c_string)
    {
      if (length > 0)
	{
	  scm_to_locale_stringbuf (string, c_string, c_length);
	}
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _x_
		  ("unable to convert a guile SCM string to a standard C \"'\\0' terminated\" string"));
    }

  return c_string;
}

static SCM
to_scm_str_list (lw6sys_list_t * c_list)
{
  SCM ret = SCM_EOL;
  lw6sys_list_t *c_item = NULL;

  for (c_item = c_list; c_item; c_item = lw6sys_list_next (c_item))
    {
      if (c_item->data)
	{
	  ret =
	    scm_cons (scm_from_locale_string ((char *) c_item->data), ret);
	}
    }

  ret = scm_reverse (ret);

  return ret;
}

static lw6sys_assoc_t *
to_sys_str_assoc (SCM assoc)
{
  lw6sys_assoc_t *c_assoc = NULL;

  c_assoc = lw6sys_assoc_new (lw6sys_free_callback);
  if (c_assoc)
    {
      if (SCM_CONSP (assoc))
	{
	  int i, n;
	  char *key, *value;
	  SCM item;

	  n = scm_to_int (scm_length (assoc));
	  for (i = 0; i < n; ++i)
	    {
	      item = scm_list_ref (assoc, scm_from_int (i));
	      if (SCM_CONSP (item))
		{
		  key = to_0str (scm_car (item));
		  value = to_0str (scm_cdr (item));
		  if (key && value)
		    {
		      lw6sys_assoc_set (&c_assoc, key, (void *) value);
		      LW6SYS_FREE (key);
		      // value must not be freed now
		    }
		}
	    }
	}
    }

  return c_assoc;
}

static lw6sys_list_t *
to_sys_str_list (SCM list)
{
  lw6sys_list_t *c_list = NULL;

  c_list = lw6sys_list_new (lw6sys_free_callback);
  if (c_list)
    {
      if (SCM_CONSP (list))
	{
	  int i, n;
	  char *c_item;
	  SCM item;

	  n = scm_to_int (scm_length (list));
	  for (i = n - 1; i >= 0; --i)
	    {
	      item = scm_list_ref (list, scm_from_int (i));
	      c_item = to_0str (item);
	      if (c_item)
		{
		  lw6sys_list_push_front (&c_list, c_item);
		  // do not free c_item, list will do it
		}
	    }
	}
    }

  return c_list;
}

static int
prepare_update_param_bootstrap (lw6dsp_param_t * c_param, SCM param)
{
  int ret = 0;
  SCM value;

  lw6dsp_param_zero (c_param);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_SPLASH),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_SPLASH);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_SPLASH;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_BACKGROUND),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_BACKGROUND);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_BACKGROUND;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_PREVIEW),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_PREVIEW);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_PREVIEW;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_MAP),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_MAP);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_MAP;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_FIGHTERS),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_FIGHTERS);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_FIGHTERS;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_CURSORS),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_CURSORS);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_CURSORS;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_HUD),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_HUD);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_HUD;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_SCORE),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_SCORE);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_SCORE;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_MENU),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_MENU);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_MENU;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_META),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_META);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_META;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_PROGRESS),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_PROGRESS);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_PROGRESS;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_LOG),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_LOG);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_LOG;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_FPS),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_FPS);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_FPS;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_MPS),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_MPS);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_MPS;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_URL),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_URL);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_URL;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_MOUSE),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_MOUSE);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_MOUSE;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DISPLAY_DEBUG_ZONES),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_DISPLAY_DEBUG_ZONES);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_DEBUG_ZONES;
    }

  value =
    scm_hash_ref (param,
		  scm_from_locale_string (LW6DEF_DISPLAY_DEBUG_GRADIENT),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn,
	      LW6DEF_DISPLAY_DEBUG_GRADIENT);
  if (SCM_NFALSEP (value))
    {
      c_param->misc.mask |= LW6GUI_DISPLAY_DEBUG_GRADIENT;
    }

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_TARGET_FPS),
		  SCM_BOOL_F);
  SCM_ASSERT (scm_is_integer (value), value, SCM_ARGn, LW6DEF_TARGET_FPS);
  c_param->misc.target_fps = scm_to_int (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_GFX_CPU_USAGE),
		  SCM_BOOL_F);
  SCM_ASSERT (scm_is_number (value), value, SCM_ARGn, LW6DEF_GFX_CPU_USAGE);
  c_param->misc.gfx_cpu_usage = scm_to_double (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DIRTY_READ),
		  SCM_BOOL_F);
  SCM_ASSERT (scm_is_integer (value), value, SCM_ARGn, LW6DEF_DIRTY_READ);
  c_param->misc.dirty_read = scm_to_int (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_CAPTURE), SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_CAPTURE);
  c_param->misc.capture = SCM_NFALSEP (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_GFX_DEBUG),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_GFX_DEBUG);
  c_param->misc.gfx_debug = SCM_NFALSEP (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DEBUG_TEAM_ID),
		  SCM_BOOL_F);
  SCM_ASSERT (scm_is_integer (value), value, SCM_ARGn, LW6DEF_DEBUG_TEAM_ID);
  c_param->misc.debug_team_id = scm_to_int (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DEBUG_LAYER_ID),
		  SCM_BOOL_F);
  SCM_ASSERT (scm_is_integer (value), value, SCM_ARGn, LW6DEF_DEBUG_LAYER_ID);
  c_param->misc.debug_layer_id = scm_to_int (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_REPEAT_DELAY),
		  SCM_BOOL_F);
  SCM_ASSERT (scm_is_integer (value), value, SCM_ARGn, LW6DEF_REPEAT_DELAY);
  c_param->misc.repeat_settings.delay = scm_to_int (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_REPEAT_INTERVAL),
		  SCM_BOOL_F);
  SCM_ASSERT (scm_is_integer (value), value, SCM_ARGn,
	      LW6DEF_REPEAT_INTERVAL);
  c_param->misc.repeat_settings.interval = scm_to_int (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_DOUBLE_CLICK_DELAY),
		  SCM_BOOL_F);
  SCM_ASSERT (scm_is_integer (value), value, SCM_ARGn,
	      LW6DEF_DOUBLE_CLICK_DELAY);
  c_param->misc.repeat_settings.double_click_delay = scm_to_int (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_LOG_TIMEOUT),
		  SCM_BOOL_F);
  SCM_ASSERT (scm_is_integer (value), value, SCM_ARGn, LW6DEF_LOG_TIMEOUT);
  c_param->misc.log_timeout = scm_to_int (value);

  /*
   * Progress taken from C globals
   */
  c_param->misc.progress = &(lw6_global.progress);

  /*
   * Video mode
   */
  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_WIDTH), SCM_BOOL_F);
  SCM_ASSERT (scm_is_integer (value), value, SCM_ARGn, LW6DEF_WIDTH);
  c_param->video_mode.width = scm_to_int (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_HEIGHT), SCM_BOOL_F);
  SCM_ASSERT (scm_is_integer (value), value, SCM_ARGn, LW6DEF_HEIGHT);
  c_param->video_mode.height = scm_to_int (value);

  value =
    scm_hash_ref (param, scm_from_locale_string (LW6DEF_FULLSCREEN),
		  SCM_BOOL_F);
  SCM_ASSERT (SCM_BOOLP (value), value, SCM_ARGn, LW6DEF_FULLSCREEN);
  c_param->video_mode.fullscreen = SCM_NFALSEP (value);

  /*
   * Pointers on complex data types
   */
  value = scm_hash_ref (param, scm_from_locale_string ("look"), SCM_BOOL_F);
  if (SCM_SMOB_PREDICATE (lw6_global.smob_types.look, value))
    {
      c_param->look = lw6_scm_to_look (value);
    }

  value = scm_hash_ref (param, scm_from_locale_string ("menu"), SCM_BOOL_F);
  if (SCM_SMOB_PREDICATE (lw6_global.smob_types.menu, value))
    {
      c_param->menu = lw6_scm_to_menu (value);
    }

  ret = 1;

  return ret;
}

static int
prepare_update_param (SCM dsp, lw6dsp_param_t * c_param, SCM param)
{
  int ret = 0;
  SCM value;
  lw6_dsp_smob_t *dsp_smob = NULL;

  dsp_smob = (lw6_dsp_smob_t *) SCM_SMOB_DATA (dsp);
  if (dsp_smob)
    {
      prepare_update_param_bootstrap (c_param, param);

      value =
	scm_hash_ref (param, scm_from_locale_string ("level"), SCM_BOOL_F);
      if (SCM_SMOB_PREDICATE (lw6_global.smob_types.map, value))
	{
	  /*
	   * It's really (really) important to do this so that level
	   * is marked when Guile garbage collects
	   */
	  dsp_smob->level = value;
	  c_param->level = lw6_scm_to_map (value);
	}
      else
	{
	  dsp_smob->level = SCM_BOOL_F;
	  c_param->level = NULL;
	}

      value =
	scm_hash_ref (param, scm_from_locale_string ("game-struct"),
		      SCM_BOOL_F);
      if (SCM_SMOB_PREDICATE (lw6_global.smob_types.game_struct, value))
	{
	  /*
	   * It's really (really) important to do this so that game_struct
	   * is marked when Guile garbage collects
	   */
	  dsp_smob->game_struct = value;
	  c_param->game_struct = lw6_scm_to_game_struct (value);
	}
      else
	{
	  dsp_smob->game_struct = SCM_BOOL_F;
	  c_param->game_struct = NULL;
	}

      value =
	scm_hash_ref (param, scm_from_locale_string ("game-state"),
		      SCM_BOOL_F);
      if (SCM_SMOB_PREDICATE (lw6_global.smob_types.game_state, value))
	{
	  /*
	   * It's really (really) important to do this so that game_state
	   * is marked when Guile garbage collects
	   */
	  dsp_smob->game_state = value;
	  c_param->game_state = lw6_scm_to_game_state (value);
	}
      else
	{
	  dsp_smob->game_state = SCM_BOOL_F;
	  c_param->game_state = NULL;
	}

      value =
	scm_hash_ref (param, scm_from_locale_string ("pilot"), SCM_BOOL_F);
      if (SCM_SMOB_PREDICATE (lw6_global.smob_types.pilot, value))
	{
	  /*
	   * It's really (really) important to do this so that game_state
	   * is marked when Guile garbage collects
	   */
	  dsp_smob->pilot = value;
	  c_param->pilot = lw6_scm_to_pilot (value);
	}
      else
	{
	  dsp_smob->pilot = SCM_BOOL_F;
	  c_param->pilot = NULL;
	}

      ret = 1;
    }

  return ret;
}

static SCM
mouse_get_state (lw6dsp_backend_t * c_dsp)
{
  SCM ret = SCM_BOOL_F;

  ret = scm_cons
    (scm_cons
     (scm_from_locale_string ("screen-pos-x"),
      scm_from_int (c_dsp->input->mouse.screen_pointer.pos_x)),
     (scm_cons
      (scm_cons
       (scm_from_locale_string ("screen-pos-y"),
	scm_from_int (c_dsp->input->mouse.screen_pointer.pos_y)),
       (scm_cons
	(scm_cons
	 (scm_from_locale_string ("screen-speed-x"),
	  scm_from_int (c_dsp->input->mouse.screen_pointer.speed_x)),
	 (scm_cons
	  (scm_cons
	   (scm_from_locale_string ("screen-speed-y"),
	    scm_from_int (c_dsp->input->mouse.screen_pointer.speed_y)),
	   (scm_cons
	    (scm_cons
	     (scm_from_locale_string ("map-pos-x"),
	      scm_from_int (c_dsp->input->mouse.map_pointer.pos_x)),
	     (scm_cons
	      (scm_cons
	       (scm_from_locale_string ("map-pos-y"),
		scm_from_int (c_dsp->input->mouse.map_pointer.pos_y)),
	       scm_cons
	       (scm_cons
		(scm_from_locale_string ("map-speed-x"),
		 scm_from_int (c_dsp->input->mouse.map_pointer.speed_x)),
		(scm_cons
		 (scm_cons
		  (scm_from_locale_string ("map-speed-y"),
		   scm_from_int (c_dsp->input->mouse.map_pointer.speed_y)),
		  (scm_cons
		   (scm_cons
		    (scm_from_locale_string ("menu-position"),
		     scm_from_int (c_dsp->input->mouse.menu_position)),
		    (scm_cons
		     (scm_cons
		      (scm_from_locale_string ("menu-scroll"),
		       scm_from_int (c_dsp->input->mouse.menu_scroll)),
		      (scm_cons
		       (scm_cons
			(scm_from_locale_string ("menu-esc"),
			 c_dsp->input->
			 mouse.menu_esc ? SCM_BOOL_T : SCM_BOOL_F),
			SCM_LIST0))))))))))))))))))));

  return ret;
}

/*
 * For a GNU gettext-like behavior of scheme code.
 */
static SCM
_scm_gettext (SCM string)
{
  char *c_string;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (string), string, SCM_ARG1, __FUNCTION__);

  c_string = to_0str (string);
  if (c_string)
    {
      ret = scm_from_locale_string (gettext (c_string));
      LW6SYS_FREE (c_string);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In liquidwar6sys
 */

/*
 * In bazooka.c
 */
static SCM
_scm_lw6sys_set_memory_bazooka_size (SCM size)
{
  SCM ret = SCM_BOOL_F;
  int c_size;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_integer (size), size, SCM_ARG1, __FUNCTION__);

  c_size = scm_to_int (size);

  ret = lw6sys_set_memory_bazooka_size (c_size) ? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_memory_bazooka_size ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_int (lw6sys_get_memory_bazooka_size ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_set_memory_bazooka_eraser (SCM state)
{
  SCM ret = SCM_BOOL_F;
  int c_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_bool (state), state, SCM_ARG1, __FUNCTION__);

  c_state = scm_to_bool (state);

  ret = lw6sys_set_memory_bazooka_eraser (c_state) ? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_memory_bazooka_eraser ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = lw6sys_get_memory_bazooka_eraser ()? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * build.c
 */
static SCM
_scm_lw6sys_build_get_package_tarname ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_package_tarname ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_package_name ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_package_name ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_package_string ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_package_string ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_package_id ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_package_id ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_version ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_version ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_codename ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_codename ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_stamp ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_stamp ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_md5sum ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_md5sum ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_copyright ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_copyright ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_license ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_license ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_home_url ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_home_url ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_bugs_url ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_bugs_url ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_configure_args ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_configure_args ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_gcc_version ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_gcc_version ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_cflags ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_cflags ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_ldflags ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_ldflags ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_hostname ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_hostname ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_date ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_date ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_time ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_time ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_host_cpu ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_host_cpu ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_endianness ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_endianness ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_pointer_size ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_int (lw6sys_build_get_pointer_size ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_is_x86 ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_int (lw6sys_build_is_x86 ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_host_os ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_host_os ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_is_gnu ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = lw6sys_build_is_gnu ()? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_is_unix ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = lw6sys_build_is_unix ()? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_is_ms_windows ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = lw6sys_build_is_ms_windows ()? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_is_mac_os_x ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = lw6sys_build_is_mac_os_x ()? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_is_gp2x ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = lw6sys_build_is_gp2x ()? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_top_srcdir ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_top_srcdir ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_prefix ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_prefix ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_datadir ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_datadir ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_libdir ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_libdir ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_includedir ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_includedir ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_localedir ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_localedir ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_docdir ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_docdir ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_console ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_console ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_gtk ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_gtk ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_mod_gl ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_mod_gl ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_mod_csound ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_mod_csound ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_mod_ogg ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_mod_ogg ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_mod_http ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_mod_http ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_openmp ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_openmp ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_optimize ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_optimize ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_allinone ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_allinone ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_fullstatic ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_fullstatic ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_paranoid ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_paranoid ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_gprof ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_gprof ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_instrument ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_instrument ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_profiler ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_profiler ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_gcov ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_gcov ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_enable_valgrind ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6sys_build_get_enable_valgrind ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_build_get_bin_id ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_int (lw6sys_build_get_bin_id ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * in debug.c
 */

static SCM
_scm_lw6sys_debug_get ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = lw6sys_debug_get ()? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_debug_set (SCM mode)
{
  int c_mode;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_bool (mode), mode, SCM_ARG1, __FUNCTION__);

  c_mode = scm_to_bool (mode);

  lw6sys_debug_set (c_mode);

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

/*
 * In dump.c
 */

static SCM
_scm_lw6sys_dump (SCM content)
{
  char *c_content;
  char *user_dir = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (content), content, SCM_ARG1, __FUNCTION__);

  c_content = to_0str (content);
  if (c_content)
    {
      user_dir =
	lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
      if (user_dir)
	{
	  lw6sys_dump (user_dir, c_content);
	  LW6SYS_FREE (user_dir);
	}
      LW6SYS_FREE (c_content);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6sys_dump_clear ()
{
  char *user_dir = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  user_dir = lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
  if (user_dir)
    {
      lw6sys_dump_clear (user_dir);
      LW6SYS_FREE (user_dir);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

/*
 * In env.c
 */
static SCM
_scm_lw6sys_get_username ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_username ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_hostname ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_hostname ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_getenv (SCM key)
{
  SCM ret = SCM_BOOL_F;
  char *c_key = NULL;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

  c_key = to_0str (key);
  if (c_key)
    {
      buf = lw6sys_getenv (c_key);
      if (buf)
	{
	  ret = scm_from_locale_string (buf);
	  LW6SYS_FREE (buf);
	}
      LW6SYS_FREE (c_key);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_getenv_prefixed (SCM keyword)
{
  SCM ret = SCM_BOOL_F;
  char *c_keyword = NULL;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (keyword), keyword, SCM_ARG1, __FUNCTION__);

  c_keyword = to_0str (keyword);
  if (c_keyword)
    {
      buf = lw6sys_getenv_prefixed (c_keyword);
      if (buf)
	{
	  ret = scm_from_locale_string (buf);
	  LW6SYS_FREE (buf);
	}
      LW6SYS_FREE (c_keyword);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In id.c
 */
static SCM
_scm_lw6sys_generate_id_16 ()
{
  SCM ret = SCM_BOOL_F;
  char *c_ret = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_ret = lw6sys_id_ltoa (lw6sys_generate_id_16 ());
  if (c_ret)
    {
      ret = scm_from_locale_string (c_ret);
      LW6SYS_FREE (c_ret);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_generate_id_32 ()
{
  SCM ret = SCM_BOOL_F;
  char *c_ret = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_ret = lw6sys_id_ltoa (lw6sys_generate_id_32 ());
  if (c_ret)
    {
      ret = scm_from_locale_string (c_ret);
      LW6SYS_FREE (c_ret);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_generate_id_64 ()
{
  SCM ret = SCM_BOOL_F;
  char *c_ret = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_ret = lw6sys_id_ltoa (lw6sys_generate_id_64 ());
  if (c_ret)
    {
      ret = scm_from_locale_string (c_ret);
      LW6SYS_FREE (c_ret);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In log.c
 */
static SCM
_scm_lw6sys_log (SCM level, SCM message)
{
  int c_level;
  char *c_message;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_integer (level), level, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (message), message, SCM_ARG2, __FUNCTION__);

  c_level = scm_to_int (level);
  c_message = to_0str (message);
  if (c_message)
    {
      lw6sys_log (c_level, __FILE__, __LINE__, "%s", c_message);
      LW6SYS_FREE (c_message);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6sys_log_get_level ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_int (lw6sys_log_get_level ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_log_set_level (SCM level)
{
  int c_level;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_integer (level), level, SCM_ARG1, __FUNCTION__);

  c_level = scm_to_int (level);
  lw6sys_log_set_level (c_level);

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

/*
 * In mem.c
 */
static SCM
_scm_lw6sys_megabytes_available ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_int (lw6sys_megabytes_available ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In openmp.c
 */
static SCM
_scm_lw6sys_openmp_get_num_procs ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_int (lw6sys_openmp_get_num_procs ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In option.c
 */
static SCM
_scm_lw6sys_get_default_user_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_default_user_dir ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_default_config_file ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_default_config_file ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_default_log_file ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_default_log_file ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_default_prefix ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_default_prefix ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_default_mod_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_default_mod_dir ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_default_data_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_default_data_dir ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_default_music_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_default_music_dir ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_default_music_path ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_default_music_path ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_default_map_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_default_map_dir ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_default_map_path ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_default_map_path ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_default_script_file ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_default_script_file ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_cwd ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_cwd ();
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_run_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_run_dir (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_user_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_user_dir (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_config_file ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_config_file (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_log_file ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_log_file (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_prefix ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_prefix (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_mod_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_mod_dir (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_data_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_data_dir (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_music_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_music_dir (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_music_path ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_music_path (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_map_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_map_dir (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_map_path ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_map_path (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_script_file ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6sys_get_script_file (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In path.c
 */
static SCM
_scm_lw6sys_path_concat (SCM path1, SCM path2)
{
  SCM ret = SCM_BOOL_F;
  char *c_path1 = NULL;
  char *c_path2 = NULL;
  char *c_ret = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (path1), path1, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (path2), path2, SCM_ARG2, __FUNCTION__);

  c_path1 = to_0str (path1);
  if (c_path1)
    {
      c_path2 = to_0str (path2);
      if (c_path2)
	{
	  c_ret = lw6sys_path_concat (c_path1, c_path2);
	  if (c_ret)
	    {
	      ret = scm_from_locale_string (c_ret);
	      LW6SYS_FREE (c_ret);
	    }
	  LW6SYS_FREE (c_path2);
	}
      LW6SYS_FREE (c_path1);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_path_file_only (SCM path)
{
  SCM ret = SCM_BOOL_F;
  char *c_path = NULL;
  char *c_ret = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (path), path, SCM_ARG1, __FUNCTION__);

  c_path = to_0str (path);
  if (c_path)
    {
      c_ret = lw6sys_path_file_only (c_path);
      if (c_ret)
	{
	  ret = scm_from_locale_string (c_ret);
	  LW6SYS_FREE (c_ret);
	}
      LW6SYS_FREE (c_path);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_path_parent (SCM path)
{
  SCM ret = SCM_BOOL_F;
  char *c_path = NULL;
  char *c_ret = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (path), path, SCM_ARG1, __FUNCTION__);

  c_path = to_0str (path);
  if (c_path)
    {
      c_ret = lw6sys_path_parent (c_path);
      if (c_ret)
	{
	  ret = scm_from_locale_string (c_ret);
	  LW6SYS_FREE (c_ret);
	}
      LW6SYS_FREE (c_path);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_path_split (SCM path)
{
  SCM ret = SCM_BOOL_F;
  char *c_path = NULL;
  lw6sys_list_t *c_ret = NULL;
  lw6sys_list_t *tmp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (path), path, SCM_ARG1, __FUNCTION__);

  c_path = to_0str (path);
  if (c_path)
    {
      ret = SCM_LIST0;
      c_ret = lw6sys_path_split (c_path);
      if (c_ret)
	{
	  for (tmp = c_ret; tmp != NULL;
	       tmp = (lw6sys_list_t *) tmp->next_item)
	    {
	      if (tmp->data)
		{
		  ret =
		    scm_cons (scm_from_locale_string ((char *) tmp->data),
			      ret);
		}
	    }
	  lw6sys_list_free (c_ret);
	  ret = scm_reverse (ret);
	}
      LW6SYS_FREE (c_path);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In signal.c
 */
static SCM
_scm_lw6sys_signal_custom (SCM trap_errors)
{
  int c_trap_errors;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_BOOLP (trap_errors), trap_errors, SCM_ARG1, __FUNCTION__);

  c_trap_errors = SCM_NFALSEP (trap_errors);
  lw6sys_signal_custom (c_trap_errors);

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6sys_signal_default ()
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  lw6sys_signal_default ();

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6sys_signal_send_quit ()
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  lw6sys_signal_send_quit ();

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6sys_signal_poll_quit ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = lw6sys_signal_poll_quit ()? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In time.c
 */
static SCM
_scm_lw6sys_get_timestamp ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_long_long (lw6sys_get_timestamp ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_uptime ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_long_long (lw6sys_get_uptime ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_get_cycle ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_int (lw6sys_get_cycle ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6sys_sleep (SCM seconds)
{
  float c_seconds;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_number (seconds), seconds, SCM_ARG1, __FUNCTION__);

  c_seconds = scm_to_double (seconds);

  lw6sys_sleep (c_seconds);

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6sys_delay (SCM msec)
{
  int c_msec;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_number (msec), msec, SCM_ARG1, __FUNCTION__);

  c_msec = scm_to_int (msec);

  lw6sys_delay (c_msec);

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6sys_idle ()
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  lw6sys_idle ();

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6sys_snooze ()
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  lw6sys_snooze ();

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

/*
 * In url.c
 */
static SCM
_scm_lw6sys_url_canonize (SCM url)
{
  char *c_url = NULL;
  char *c_ret = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (url), url, SCM_ARG1, __FUNCTION__);

  c_url = to_0str (url);
  if (c_url)
    {
      c_ret = lw6sys_url_canonize (c_url);
      if (c_ret)
	{
	  ret = scm_from_locale_string (c_ret);
	  LW6SYS_FREE (c_ret);
	}
      LW6SYS_FREE (c_url);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * lw6hlp
 */

static SCM
_scm_lw6hlp_about (SCM key)
{
  char *c_key;
  const char *c_ret;
  SCM ret = SCM_BOOL_F;

  SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

  c_key = to_0str (key);
  if (c_key)
    {
      c_ret = lw6hlp_about (NULL, NULL, NULL, NULL, c_key);
      if (c_ret)
	{
	  ret = scm_from_locale_string (c_ret);
	  // no need to free c_ret
	}
      LW6SYS_FREE (c_key);
    }

  return ret;
}

static SCM
_scm_lw6hlp_get_default_value (SCM key)
{
  char *c_key;
  const char *c_ret;
  SCM ret = SCM_BOOL_F;

  SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

  c_key = to_0str (key);
  if (c_key)
    {
      c_ret = lw6hlp_get_default_value (c_key);
      if (c_ret)
	{
	  ret = scm_from_locale_string (c_ret);
	  // no need to free c_ret
	}
      LW6SYS_FREE (c_key);
    }

  return ret;
}

static SCM
_scm_lw6hlp_list_quick ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_quick ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_doc ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_doc ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_show ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_show ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_path ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_path ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_players ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_players ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_input ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_input ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_graphics ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_graphics ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_sound ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_sound ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_network ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_network ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_map ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_map ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_map_rules ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_map_rules ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_map_hints ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_map_hints ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_map_style ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_map_style ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_map_teams ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_map_teams ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_funcs ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_funcs ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_hooks ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_hooks ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_advanced ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_advanced ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_aliases ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_aliases ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_team_colors ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_team_colors ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list_weapons ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list_weapons ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6hlp_list ()
{
  lw6sys_list_t *c_list = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_list = lw6hlp_list ();
  if (c_list)
    {
      ret = to_scm_str_list (c_list);
      lw6sys_list_free (c_list);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * lw6cfg
 */
static SCM
_scm_lw6cfg_defaults ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (lw6_global.cfg_context != NULL)
    {
      if (lw6cfg_defaults (lw6_global.cfg_context))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6cfg_load (SCM filename)
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (lw6_global.cfg_context != NULL)
    {
      char *c_filename;

      SCM_ASSERT (scm_is_string (filename), filename, SCM_ARG1, __FUNCTION__);

      c_filename = to_0str (filename);
      if (c_filename)
	{
	  if (lw6cfg_load (lw6_global.cfg_context, c_filename))
	    {
	      ret = SCM_BOOL_T;
	    }
	  else
	    {
	      ret = SCM_BOOL_F;
	    }
	  LW6SYS_FREE (c_filename);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In option.c
 */
static SCM
_scm_lw6cfg_option_exists (SCM key)
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (lw6_global.cfg_context != NULL)
    {
      char *c_key;

      SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

      c_key = to_0str (key);
      if (c_key)
	{
	  if (lw6cfg_option_exists (lw6_global.cfg_context, c_key))
	    {
	      ret = SCM_BOOL_T;
	    }
	  else
	    {
	      ret = SCM_BOOL_F;
	    }
	  LW6SYS_FREE (c_key);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6cfg_get_option (SCM key)
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (lw6_global.cfg_context != NULL)
    {
      char *c_key;

      SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

      c_key = to_0str (key);
      if (c_key)
	{
	  char *value = NULL;

	  value = lw6cfg_get_option (lw6_global.cfg_context, c_key);
	  if (value)
	    {
	      ret = scm_from_locale_string (value);
	      LW6SYS_FREE (value);
	    }

	  LW6SYS_FREE (c_key);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6cfg_set_option (SCM key, SCM value)
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (lw6_global.cfg_context != NULL)
    {
      char *c_key;
      char *c_value;

      SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);
      SCM_ASSERT (scm_is_string (value), value, SCM_ARG2, __FUNCTION__);

      c_key = to_0str (key);
      if (c_key)
	{
	  c_value = to_0str (value);
	  if (c_value)
	    {
	      lw6cfg_set_option (lw6_global.cfg_context, c_key, c_value);

	      LW6SYS_FREE (c_value);
	    }
	  LW6SYS_FREE (c_key);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

/*
 * In save.c
 */
static SCM
_scm_lw6cfg_save (SCM filename)
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (lw6_global.cfg_context != NULL)
    {
      char *c_filename;

      SCM_ASSERT (scm_is_string (filename), filename, SCM_ARG1, __FUNCTION__);

      c_filename = to_0str (filename);
      if (c_filename)
	{
	  if (lw6cfg_save (lw6_global.cfg_context, c_filename))
	    {
	      ret = SCM_BOOL_T;
	    }
	  else
	    {
	      ret = SCM_BOOL_F;
	    }
	  LW6SYS_FREE (c_filename);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In setup.c
 */
static SCM
_scm_lw6cfg_init ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  lw6_global.cfg_context = lw6cfg_init (lw6_global.argc, lw6_global.argv);
  lw6_global.cfg_initialized = lw6_global.cfg_context ? 1 : 0;
  ret = lw6_global.cfg_context ? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6cfg_quit ()
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (lw6_global.cfg_context != NULL)
    {
      lw6cfg_quit (lw6_global.cfg_context);
    }

  lw6_global.cfg_context = NULL;
  lw6_global.cfg_initialized = 0;

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

/*
 * Unified
 */

static SCM
_scm_lw6cfg_unified_get_user_dir ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6cfg_unified_get_log_file ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6cfg_unified_get_log_file (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6cfg_unified_get_music_path ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6cfg_unified_get_music_path (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6cfg_unified_get_map_path ()
{
  SCM ret = SCM_BOOL_F;
  char *buf = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  buf = lw6cfg_unified_get_map_path (lw6_global.argc, lw6_global.argv);
  if (buf)
    {
      ret = scm_from_locale_string (buf);
      LW6SYS_FREE (buf);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}



/*
 * In liquidwar6opt
 */

/*
 * In liquidwar6gui
 */
static SCM
_scm_lw6gui_menu_new (SCM title, SCM help, SCM popup, SCM esc, SCM enable_esc)
{
  SCM ret = SCM_BOOL_F;

  char *c_title = NULL;
  char *c_help = NULL;
  char *c_popup = NULL;
  char *c_esc = NULL;
  int c_enable_esc = 0;
  lw6gui_menu_t *c_menu = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (SCM_NFALSEP (title))
    {
      SCM_ASSERT (scm_is_string (title), title, SCM_ARG1, __FUNCTION__);
    }
  if (SCM_NFALSEP (help))
    {
      SCM_ASSERT (scm_is_string (help), help, SCM_ARG2, __FUNCTION__);
    }
  if (SCM_NFALSEP (popup))
    {
      SCM_ASSERT (scm_is_string (popup), popup, SCM_ARG3, __FUNCTION__);
    }
  SCM_ASSERT (scm_is_string (esc), esc, SCM_ARG4, __FUNCTION__);
  SCM_ASSERT (SCM_BOOLP (enable_esc), enable_esc, SCM_ARG5, __FUNCTION__);

  if (SCM_NFALSEP (title))
    {
      c_title = to_0str (title);
    }
  else
    {
      c_title = lw6sys_str_copy (LW6SYS_STR_EMPTY);
    }
  if (c_title)
    {
      if (SCM_NFALSEP (help))
	{
	  c_help = to_0str (help);
	}
      else
	{
	  c_help = lw6sys_str_copy (LW6SYS_STR_EMPTY);
	}
      if (c_help)
	{
	  if (SCM_NFALSEP (popup))
	    {
	      c_popup = to_0str (popup);
	    }
	  else
	    {
	      c_popup = lw6sys_str_copy (LW6SYS_STR_EMPTY);
	    }
	  if (c_popup)
	    {
	      c_esc = to_0str (esc);
	      if (c_esc)
		{
		  c_enable_esc = SCM_NFALSEP (enable_esc);

		  c_menu =
		    lw6gui_menu_new (c_title, c_help, c_popup, c_esc,
				     c_enable_esc);
		  if (c_menu)
		    {
		      ret = lw6_make_scm_menu (c_menu);
		    }
		  LW6SYS_FREE (c_esc);
		}
	      LW6SYS_FREE (c_popup);
	    }
	  LW6SYS_FREE (c_help);
	}
      LW6SYS_FREE (c_title);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_menu_append (SCM menu, SCM menuitem)
{
  char *c_label;
  char *c_tooltip;
  int c_value;
  int c_enabled;
  int c_selected;
  int c_colored;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.menu,
	       menu), menu, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_CONSP (menuitem)
	      || menuitem == SCM_EOL, menuitem, SCM_ARG2, __FUNCTION__);

  c_label =
    to_0str (scm_assoc_ref (menuitem, scm_from_locale_string ("label")));
  if (c_label)
    {
      c_tooltip =
	to_0str (scm_assoc_ref
		 (menuitem, scm_from_locale_string ("tooltip")));
      if (c_tooltip)
	{
	  lw6gui_menu_t *c_menu;

	  c_value =
	    scm_to_int (scm_assoc_ref
			(menuitem, scm_from_locale_string ("value")));
	  c_enabled =
	    scm_to_bool (scm_assoc_ref
			 (menuitem, scm_from_locale_string ("enabled")));
	  c_selected =
	    scm_to_bool (scm_assoc_ref
			 (menuitem, scm_from_locale_string ("selected")));
	  c_colored =
	    scm_to_bool (scm_assoc_ref
			 (menuitem, scm_from_locale_string ("colored")));

	  c_menu = lw6_scm_to_menu (menu);

	  ret =
	    scm_from_int (lw6gui_menu_append_for_id_use
			  (c_menu, c_label, c_tooltip, c_value, c_enabled,
			   c_selected, c_colored, lw6sys_get_timestamp ()));

	  LW6SYS_FREE (c_tooltip);
	}
      LW6SYS_FREE (c_label);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_menu_sync (SCM menu, SCM menuitem)
{
  int c_id;
  char *c_label;
  char *c_tooltip;
  int c_value;
  int c_enabled;
  int c_selected;
  int c_colored;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.menu,
	       menu), menu, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_CONSP (menuitem)
	      || menuitem == SCM_EOL, menuitem, SCM_ARG2, __FUNCTION__);

  c_label =
    to_0str (scm_assoc_ref (menuitem, scm_from_locale_string ("label")));
  if (c_label)
    {
      c_tooltip =
	to_0str (scm_assoc_ref
		 (menuitem, scm_from_locale_string ("tooltip")));
      if (c_tooltip)
	{
	  lw6gui_menu_t *c_menu;

	  c_id =
	    scm_to_int (scm_assoc_ref
			(menuitem, scm_from_locale_string ("id")));
	  c_value =
	    scm_to_int (scm_assoc_ref
			(menuitem, scm_from_locale_string ("value")));
	  c_enabled =
	    scm_to_bool (scm_assoc_ref
			 (menuitem, scm_from_locale_string ("enabled")));
	  c_selected =
	    scm_to_bool (scm_assoc_ref
			 (menuitem, scm_from_locale_string ("selected")));
	  c_colored =
	    scm_to_bool (scm_assoc_ref
			 (menuitem, scm_from_locale_string ("colored")));

	  c_menu = lw6_scm_to_menu (menu);

	  lw6gui_menu_sync_using_id
	    (c_menu, c_id, c_label, c_tooltip, c_value, c_enabled, c_selected,
	     c_colored, lw6sys_get_timestamp ());

	  LW6SYS_FREE (c_tooltip);
	}
      LW6SYS_FREE (c_label);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_menu_select (SCM menu, SCM position, SCM allow_scroll)
{
  lw6gui_menu_t *c_menu;
  int c_position;
  int c_allow_scroll;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.menu,
	       menu), menu, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (position), position, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (SCM_BOOLP (allow_scroll), allow_scroll, SCM_ARG3, __FUNCTION__);

  c_menu = lw6_scm_to_menu (menu);
  c_position = scm_to_int (position);
  c_allow_scroll = SCM_NFALSEP (allow_scroll);

  ret =
    lw6gui_menu_select (c_menu, c_position, c_allow_scroll,
			lw6sys_get_timestamp ())? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_menu_select_esc (SCM menu, SCM state)
{
  lw6gui_menu_t *c_menu;
  int c_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.menu,
	       menu), menu, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_BOOLP (state), state, SCM_ARG2, __FUNCTION__);

  c_menu = lw6_scm_to_menu (menu);
  c_state = SCM_NFALSEP (state);

  lw6gui_menu_select_esc (c_menu, c_state, lw6sys_get_timestamp ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6gui_menu_enable_esc (SCM menu, SCM state)
{
  lw6gui_menu_t *c_menu;
  int c_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.menu,
	       menu), menu, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_BOOLP (state), state, SCM_ARG2, __FUNCTION__);

  c_menu = lw6_scm_to_menu (menu);
  c_state = SCM_NFALSEP (state);

  lw6gui_menu_enable_esc (c_menu, c_state, lw6sys_get_timestamp ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6gui_menu_scroll_up (SCM menu)
{
  lw6gui_menu_t *c_menu;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.menu,
	       menu), menu, SCM_ARG1, __FUNCTION__);

  c_menu = lw6_scm_to_menu (menu);

  ret = lw6gui_menu_scroll_up (c_menu) ? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_menu_scroll_down (SCM menu)
{
  lw6gui_menu_t *c_menu;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.menu,
	       menu), menu, SCM_ARG1, __FUNCTION__);

  c_menu = lw6_scm_to_menu (menu);

  ret = lw6gui_menu_scroll_down (c_menu) ? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_menu_set_breadcrumbs (SCM menu, SCM breadcrumbs)
{
  lw6gui_menu_t *c_menu;
  lw6sys_list_t *c_breadcrumbs;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.menu,
	       menu), menu, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_CONSP (breadcrumbs)
	      || breadcrumbs == SCM_EOL, breadcrumbs, SCM_ARG2, __FUNCTION__);

  c_menu = lw6_scm_to_menu (menu);
  c_breadcrumbs = to_sys_str_list (breadcrumbs);
  if (c_breadcrumbs)
    {
      lw6gui_menu_set_breadcrumbs (c_menu, c_breadcrumbs);
      lw6sys_list_free (c_breadcrumbs);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_menu_close_popup (SCM menu)
{
  lw6gui_menu_t *c_menu;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.menu,
	       menu), menu, SCM_ARG1, __FUNCTION__);
  c_menu = lw6_scm_to_menu (menu);
  lw6gui_menu_close_popup (c_menu);

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_BOOL_F;
}

static SCM
_scm_lw6gui_menu_has_popup (SCM menu)
{
  SCM ret = SCM_BOOL_F;
  lw6gui_menu_t *c_menu;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.menu,
	       menu), menu, SCM_ARG1, __FUNCTION__);
  c_menu = lw6_scm_to_menu (menu);
  ret = lw6gui_menu_has_popup (c_menu) ? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_default_look ()
{
  lw6gui_look_t *c_look;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  c_look = lw6gui_look_new (NULL);
  if (c_look)
    {
      ret = lw6_make_scm_look (c_look);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_look_set (SCM look, SCM key, SCM value)
{
  lw6gui_look_t *c_look;
  char *c_key;
  char *c_value;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.look,
	       look), look, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (key), key, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_string (value), value, SCM_ARG3, __FUNCTION__);

  c_look = lw6_scm_to_look (look);
  if (c_look)
    {
      c_key = to_0str (key);
      if (c_key)
	{
	  c_value = to_0str (value);
	  if (c_value)
	    {
	      ret =
		lw6gui_look_set (c_look, c_key,
				 c_value) ? SCM_BOOL_T : SCM_BOOL_F;
	      LW6SYS_FREE (c_value);
	    }
	  LW6SYS_FREE (c_key);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_look_get (SCM look, SCM key)
{
  lw6gui_look_t *c_look = NULL;
  char *c_key;
  char *c_value;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.look,
	       look), look, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (key), key, SCM_ARG2, __FUNCTION__);

  c_look = lw6_scm_to_look (look);
  if (c_look)
    {
      c_key = to_0str (key);
      if (c_key)
	{
	  c_value = lw6gui_look_get (c_look, c_key);
	  if (c_value)
	    {
	      ret = scm_from_locale_string (c_value);
	      LW6SYS_FREE (c_value);
	    }
	  LW6SYS_FREE (c_key);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_look_zoom_in (SCM look, SCM zoom_step)
{
  lw6gui_look_t *c_look;
  float c_zoom_step;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.look,
	       look), look, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_number (zoom_step), zoom_step, SCM_ARG2, __FUNCTION__);

  c_look = lw6_scm_to_look (look);
  if (c_look)
    {
      c_zoom_step = scm_to_double (zoom_step);
      if (lw6gui_look_zoom_in (c_look, c_zoom_step))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_look_zoom_out (SCM look, SCM zoom_step)
{
  lw6gui_look_t *c_look;
  float c_zoom_step;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.look,
	       look), look, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_number (zoom_step), zoom_step, SCM_ARG2, __FUNCTION__);

  c_look = lw6_scm_to_look (look);
  if (c_look)
    {
      c_zoom_step = scm_to_double (zoom_step);
      if (lw6gui_look_zoom_out (c_look, c_zoom_step))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_input_reset (SCM dsp)
{
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      lw6gui_input_reset (c_dsp->input);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6gui_mouse_poll_move (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_mouse_poll_move (&(c_dsp->input->mouse), NULL, NULL))
	{
	  ret = mouse_get_state (c_dsp);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_mouse_get_state (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      ret = mouse_get_state (c_dsp);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_is_pressed (SCM dsp, SCM keysym)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;
  int c_keysym;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (keysym), keysym, SCM_ARG2, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      c_keysym = scm_to_int (keysym);

      ret =
	lw6gui_keyboard_is_pressed (&(c_dsp->input->keyboard),
				    c_keysym) ? SCM_BOOL_T : SCM_BOOL_F;
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_get_move_pad (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6gui_move_pad_t move_pad;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      lw6gui_keyboard_get_move_pad (&(c_dsp->input->keyboard), &move_pad);
      ret = scm_list_4 (scm_cons
			(scm_from_locale_string ("up"),
			 move_pad.up ? SCM_BOOL_T : SCM_BOOL_F),
			scm_cons
			(scm_from_locale_string ("down"),
			 move_pad.down ? SCM_BOOL_T : SCM_BOOL_F),
			scm_cons
			(scm_from_locale_string ("left"),
			 move_pad.left ? SCM_BOOL_T : SCM_BOOL_F),
			scm_cons
			(scm_from_locale_string ("right"),
			 move_pad.right ? SCM_BOOL_T : SCM_BOOL_F));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick1_get_move_pad (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6gui_move_pad_t move_pad;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      lw6gui_joystick_get_move_pad (&
				    (c_dsp->input->
				     joysticks[LW6GUI_JOYSTICK1_ID]),
				    &move_pad);
      ret =
	scm_list_4 (scm_cons
		    (scm_from_locale_string ("up"),
		     move_pad.up ? SCM_BOOL_T : SCM_BOOL_F),
		    scm_cons (scm_from_locale_string ("down"),
			      move_pad.down ? SCM_BOOL_T : SCM_BOOL_F),
		    scm_cons (scm_from_locale_string ("left"),
			      move_pad.left ? SCM_BOOL_T : SCM_BOOL_F),
		    scm_cons (scm_from_locale_string ("right"),
			      move_pad.right ? SCM_BOOL_T : SCM_BOOL_F));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick2_get_move_pad (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;
  lw6gui_move_pad_t move_pad;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      lw6gui_joystick_get_move_pad (&
				    (c_dsp->input->
				     joysticks[LW6GUI_JOYSTICK2_ID]),
				    &move_pad);
      ret =
	scm_list_4 (scm_cons
		    (scm_from_locale_string ("up"),
		     move_pad.up ? SCM_BOOL_T : SCM_BOOL_F),
		    scm_cons (scm_from_locale_string ("down"),
			      move_pad.down ? SCM_BOOL_T : SCM_BOOL_F),
		    scm_cons (scm_from_locale_string ("left"),
			      move_pad.left ? SCM_BOOL_T : SCM_BOOL_F),
		    scm_cons (scm_from_locale_string ("right"),
			      move_pad.right ? SCM_BOOL_T : SCM_BOOL_F));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_mouse_pop_button_left (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->mouse.button_left)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_mouse_pop_button_right (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->mouse.button_right)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_mouse_pop_button_middle (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->mouse.button_middle)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_mouse_pop_simple_click (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_simple_click (&(c_dsp->input->mouse.button_left))
	  ||
	  lw6gui_button_pop_simple_click (&(c_dsp->input->mouse.button_right))
	  ||
	  lw6gui_button_pop_simple_click (&
					  (c_dsp->input->
					   mouse.button_middle)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_mouse_pop_double_click (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_double_click (&(c_dsp->input->mouse.button_left))
	  ||
	  lw6gui_button_pop_double_click (&(c_dsp->input->mouse.button_right))
	  ||
	  lw6gui_button_pop_double_click (&
					  (c_dsp->input->
					   mouse.button_middle)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_mouse_pop_triple_click (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_triple_click (&(c_dsp->input->mouse.button_left))
	  ||
	  lw6gui_button_pop_triple_click (&(c_dsp->input->mouse.button_right))
	  ||
	  lw6gui_button_pop_triple_click (&
					  (c_dsp->input->
					   mouse.button_middle)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_mouse_pop_wheel_up (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->mouse.wheel_up)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_mouse_pop_wheel_down (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->mouse.wheel_down)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_pop_arrow_up (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->keyboard.arrow_up)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_pop_arrow_down (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->keyboard.arrow_down)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_pop_arrow_left (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->keyboard.arrow_left)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_pop_arrow_right (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->keyboard.arrow_right)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_pop_key_enter (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->keyboard.key_enter)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_pop_key_esc (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->keyboard.key_esc)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_pop_key_ctrl (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->keyboard.key_ctrl)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_pop_key_alt (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->keyboard.key_alt)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_pop_key_pgup (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->keyboard.key_pgup)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_keyboard_pop_key_pgdown (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press (&(c_dsp->input->keyboard.key_pgdown)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick1_pop_pad_up (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK1_ID].pad_up)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick1_pop_pad_down (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK1_ID].pad_down)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick1_pop_pad_left (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK1_ID].pad_left)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick1_pop_pad_right (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK1_ID].pad_right)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick1_pop_button_a (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK1_ID].button_a)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick1_pop_button_b (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK1_ID].button_b)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick1_pop_button_c (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK1_ID].button_c)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick1_pop_button_d (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK1_ID].button_d)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick1_pop_button_e (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK1_ID].button_e)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick1_pop_button_f (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK1_ID].button_f)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick2_pop_pad_up (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK2_ID].pad_up)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick2_pop_pad_down (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK2_ID].pad_down)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick2_pop_pad_left (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK2_ID].pad_left)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick2_pop_pad_right (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK2_ID].pad_right)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick2_pop_button_a (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK2_ID].button_a)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick2_pop_button_b (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK2_ID].button_b)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick2_pop_button_c (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK2_ID].button_c)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick2_pop_button_d (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK2_ID].button_d)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick2_pop_button_e (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK2_ID].button_e)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6gui_joystick2_pop_button_f (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (lw6gui_button_pop_press
	  (&(c_dsp->input->joysticks[LW6GUI_JOYSTICK2_ID].button_f)))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In liquidwar6gfx
 */

/*
 * In backend.c
 */
static SCM
_scm_lw6gfx_get_backends ()
{
  SCM ret = SCM_BOOL_F;
  lw6sys_assoc_t *backends;
  lw6sys_list_t *keys;
  lw6sys_list_t *key;
  char *module_id;
  char *module_name;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  backends = lw6gfx_get_backends (lw6_global.argc, lw6_global.argv);
  if (backends)
    {
      keys = lw6sys_assoc_keys (backends);
      if (keys)
	{
	  ret = SCM_EOL;
	  key = keys;
	  while (key)
	    {
	      if (key->data)
		{
		  module_id = (char *) key->data;
		  module_name =
		    (char *) lw6sys_assoc_get (backends, module_id);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string (module_id),
			       scm_from_locale_string (module_name)), ret);
		}
	      key = lw6sys_list_next (key);
	    }
	  lw6sys_list_free (keys);
	  ret = scm_reverse (ret);
	}
      lw6sys_assoc_free (backends);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6dsp_new (SCM backend_name, SCM param)
{
  SCM ret = SCM_BOOL_F;
  char *c_backend_name = NULL;
  lw6dsp_param_t c_param;
  lw6dsp_backend_t *c_ret = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (backend_name),
	      backend_name, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_hash_table_p (param), param, SCM_ARG2, __FUNCTION__);

  c_backend_name = to_0str (backend_name);
  if (backend_name)
    {
      if (prepare_update_param_bootstrap (&c_param, param))
	{
	  c_ret = lw6dsp_create_backend (lw6_global.argc, lw6_global.argv,
					 c_backend_name);
	  if (c_ret)
	    {
	      if (lw6dsp_init (c_ret, &c_param, lw6_resize_callback))
		{
		  ret = lw6_make_scm_dsp (c_ret);
		}
	      else
		{
		  lw6dsp_destroy_backend (c_ret);
		}
	    }
	}
      LW6SYS_FREE (c_backend_name);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6dsp_release (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      lw6dsp_quit (c_dsp);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6dsp_update (SCM dsp, SCM param)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;
  lw6dsp_param_t c_param;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_hash_table_p (param), param, SCM_ARG2, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      if (prepare_update_param (dsp, &c_param, param))
	{
	  ret = lw6dsp_update (c_dsp, &c_param) ? SCM_BOOL_T : SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6dsp_get_nb_frames (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      ret = scm_from_int (lw6dsp_get_nb_frames (c_dsp));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6dsp_get_last_frame_rendering_time (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      ret = scm_from_int (lw6dsp_get_last_frame_rendering_time (c_dsp));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6dsp_get_instant_fps (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      ret = scm_from_int (lw6dsp_get_instant_fps (c_dsp));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6dsp_get_average_fps (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      ret = scm_from_int (lw6dsp_get_average_fps (c_dsp));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6dsp_get_video_mode (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      lw6gui_video_mode_t video_mode;

      if (lw6dsp_get_video_mode (c_dsp, &video_mode))
	{
	  ret =
	    scm_list_3 (scm_cons
			(scm_from_locale_string ("width"),
			 scm_from_int (video_mode.width)),
			scm_cons (scm_from_locale_string ("height"),
				  scm_from_int (video_mode.height)),
			scm_cons (scm_from_locale_string ("fullscreen"),
				  video_mode.fullscreen ? SCM_BOOL_T :
				  SCM_BOOL_F));
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6dsp_get_fullscreen_modes (SCM dsp)
{
  SCM ret = SCM_BOOL_F;
  lw6dsp_backend_t *c_dsp = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.dsp, dsp), dsp, SCM_ARG1, __FUNCTION__);

  c_dsp = lw6_scm_to_dsp (dsp);
  if (c_dsp)
    {
      lw6gui_fullscreen_modes_t fullscreen_modes;

      if (lw6dsp_get_fullscreen_modes (c_dsp, &fullscreen_modes))
	{
	  ret = scm_list_3 (scm_cons (scm_from_locale_string ("low"),
				      scm_list_3 (scm_cons
						  (scm_from_locale_string
						   ("width"),
						   scm_from_int
						   (fullscreen_modes.low.
						    width)),
						  scm_cons
						  (scm_from_locale_string
						   ("height"),
						   scm_from_int
						   (fullscreen_modes.
						    low.height)),
						  scm_cons
						  (scm_from_locale_string
						   ("fullscreen"),
						   fullscreen_modes.low.
						   fullscreen ? SCM_BOOL_T :
						   SCM_BOOL_F))),
			    scm_cons (scm_from_locale_string ("standard"),
				      scm_list_3 (scm_cons
						  (scm_from_locale_string
						   ("width"),
						   scm_from_int
						   (fullscreen_modes.standard.
						    width)),
						  scm_cons
						  (scm_from_locale_string
						   ("height"),
						   scm_from_int
						   (fullscreen_modes.standard.
						    height)),
						  scm_cons
						  (scm_from_locale_string
						   ("fullscreen"),
						   fullscreen_modes.standard.
						   fullscreen ? SCM_BOOL_T :
						   SCM_BOOL_F))),
			    scm_cons (scm_from_locale_string ("high"),
				      scm_list_3 (scm_cons
						  (scm_from_locale_string
						   ("width"),
						   scm_from_int
						   (fullscreen_modes.high.
						    width)),
						  scm_cons
						  (scm_from_locale_string
						   ("height"),
						   scm_from_int
						   (fullscreen_modes.
						    high.height)),
						  scm_cons
						  (scm_from_locale_string
						   ("fullscreen"),
						   fullscreen_modes.high.
						   fullscreen ? SCM_BOOL_T :
						   SCM_BOOL_F))));

	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In liquidwar6ldr
 */
static SCM
_scm_lw6ldr_get_entries (SCM map_path, SCM relative_path)
{
  lw6sys_list_t *c_maps;
  lw6ldr_entry_t *c_entry;
  char *c_map_path;
  char *c_relative_path;
  char *user_dir;
  SCM ret = SCM_BOOL_F;
  SCM item = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (map_path), relative_path, SCM_ARG1,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_string (relative_path), relative_path, SCM_ARG2,
	      __FUNCTION__);

  ret = SCM_LIST0;
  c_map_path = to_0str (map_path);
  if (c_map_path)
    {
      c_relative_path = to_0str (relative_path);
      if (c_relative_path)
	{
	  user_dir =
	    lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
	  if (user_dir)
	    {
	      c_maps =
		lw6ldr_get_entries (c_map_path, c_relative_path, user_dir);
	      if (c_maps)
		{
		  while (!lw6sys_list_is_empty (c_maps))
		    {
		      c_entry = (lw6ldr_entry_t *) lw6sys_lifo_pop (&c_maps);
		      if (c_entry)
			{
			  item = SCM_EOL;
			  item = scm_cons (scm_cons
					   (scm_from_locale_string ("title"),
					    scm_from_locale_string
					    (c_entry->metadata.title)), item);
			  item =
			    scm_cons (scm_cons
				      (scm_from_locale_string ("author"),
				       scm_from_locale_string
				       (c_entry->metadata.author)), item);
			  item =
			    scm_cons (scm_cons
				      (scm_from_locale_string ("description"),
				       scm_from_locale_string
				       (c_entry->metadata.description)),
				      item);
			  item =
			    scm_cons (scm_cons
				      (scm_from_locale_string ("license"),
				       scm_from_locale_string
				       (c_entry->metadata.license)), item);
			  item =
			    scm_cons (scm_cons
				      (scm_from_locale_string
				       ("absolute-path"),
				       scm_from_locale_string
				       (c_entry->absolute_path)), item);
			  item =
			    scm_cons (scm_cons
				      (scm_from_locale_string
				       ("relative-path"),
				       scm_from_locale_string
				       (c_entry->relative_path)), item);
			  item =
			    scm_cons (scm_cons
				      (scm_from_locale_string ("has-subdirs"),
				       c_entry->has_subdirs ? SCM_BOOL_T :
				       SCM_BOOL_F), item);
			  item =
			    scm_cons (scm_cons
				      (scm_from_locale_string ("nb-submaps"),
				       scm_from_int (c_entry->nb_submaps)),
				      item);
			  item =
			    scm_cons (scm_cons
				      (scm_from_locale_string ("exp"),
				       scm_from_int (c_entry->
						     metadata.vanilla_exp)),
				      item);
			  item =
			    scm_cons (scm_cons
				      (scm_from_locale_string ("forbidden"),
				       c_entry->forbidden ? SCM_BOOL_T :
				       SCM_BOOL_F), item);
			  ret = scm_cons (item, ret);
			  lw6ldr_free_entry (c_entry);
			}
		    }
		  lw6sys_list_free (c_maps);
		}
	      LW6SYS_FREE (user_dir);
	    }
	  LW6SYS_FREE (c_relative_path);
	}
      LW6SYS_FREE (c_map_path);
    }

  ret = scm_reverse (ret);
  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ldr_read (SCM dirname, SCM default_param, SCM forced_param,
		  SCM display_width, SCM display_height, SCM bench_value,
		  SCM magic_number)
{
  char *c_dirname;
  lw6sys_assoc_t *c_default_param;
  lw6sys_assoc_t *c_forced_param;
  int c_display_width;
  int c_display_height;
  int c_bench_value;
  int c_magic_number;
  lw6map_level_t *c_level;
  SCM ret = SCM_BOOL_F;
  char *user_dir;
  lw6sys_progress_t progress;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (dirname), dirname, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_CONSP (default_param)
	      || default_param == SCM_EOL, default_param, SCM_ARG2,
	      __FUNCTION__);
  SCM_ASSERT (SCM_CONSP (forced_param)
	      || forced_param == SCM_EOL, forced_param, SCM_ARG3,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (display_width), display_width, SCM_ARG4,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (display_height), display_height, SCM_ARG5,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (bench_value), bench_value, SCM_ARG6,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (magic_number), magic_number, SCM_ARG7,
	      __FUNCTION__);

  lw6sys_progress_default (&progress, &(lw6_global.progress));
  lw6sys_progress_begin (&progress);

  c_dirname = to_0str (dirname);
  if (c_dirname)
    {
      c_default_param = to_sys_str_assoc (default_param);
      if (c_default_param)
	{
	  c_forced_param = to_sys_str_assoc (forced_param);
	  if (c_forced_param)
	    {
	      c_display_width = scm_to_int (display_width);
	      c_display_height = scm_to_int (display_height);
	      c_bench_value = scm_to_int (bench_value);
	      c_magic_number = scm_to_int (magic_number);

	      user_dir =
		lw6cfg_unified_get_user_dir (lw6_global.argc,
					     lw6_global.argv);
	      if (user_dir)
		{
		  c_level =
		    lw6ldr_read (c_dirname, c_default_param, c_forced_param,
				 c_display_width, c_display_height,
				 c_bench_value, c_magic_number, user_dir,
				 &progress);
		  if (c_level)
		    {
		      ret = lw6_make_scm_map (c_level);
		    }
		  LW6SYS_FREE (user_dir);
		}
	      lw6sys_assoc_free (c_forced_param);
	    }
	  lw6sys_assoc_free (c_default_param);
	}
      LW6SYS_FREE (c_dirname);
    }
  lw6sys_progress_begin (&progress);

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ldr_read_relative (SCM map_path, SCM relative_path, SCM default_param,
			   SCM forced_param, SCM display_width,
			   SCM display_height, SCM bench_value,
			   SCM magic_number)
{
  char *c_map_path;
  char *c_relative_path;
  lw6sys_assoc_t *c_default_param;
  lw6sys_assoc_t *c_forced_param;
  int c_display_width;
  int c_display_height;
  int c_bench_value;
  int c_magic_number;
  lw6map_level_t *c_level;
  SCM ret = SCM_BOOL_F;
  char *user_dir;
  lw6sys_progress_t progress;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (map_path), map_path, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (relative_path), relative_path, SCM_ARG2,
	      __FUNCTION__);
  SCM_ASSERT (SCM_CONSP (default_param)
	      || default_param == SCM_EOL, default_param, SCM_ARG3,
	      __FUNCTION__);
  SCM_ASSERT (SCM_CONSP (forced_param)
	      || forced_param == SCM_EOL, forced_param, SCM_ARG4,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (display_width), display_width, SCM_ARG5,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (display_height), display_height, SCM_ARG6,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (bench_value), bench_value, SCM_ARG7,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (magic_number), magic_number, SCM_ARGn,
	      __FUNCTION__);

  progress.min = 0.0f;
  progress.max = 1.0f;
  progress.value = &(lw6_global.progress);

  c_map_path = to_0str (map_path);
  if (c_map_path)
    {
      c_relative_path = to_0str (relative_path);
      if (c_relative_path)
	{
	  c_default_param = to_sys_str_assoc (default_param);
	  if (c_default_param)
	    {
	      c_forced_param = to_sys_str_assoc (forced_param);
	      if (c_forced_param)
		{
		  c_display_width = scm_to_int (display_width);
		  c_display_height = scm_to_int (display_height);
		  c_bench_value = scm_to_int (bench_value);
		  c_magic_number = scm_to_int (magic_number);

		  user_dir =
		    lw6cfg_unified_get_user_dir (lw6_global.argc,
						 lw6_global.argv);
		  if (user_dir)
		    {
		      c_level =
			lw6ldr_read_relative (c_map_path, c_relative_path,
					      c_default_param, c_forced_param,
					      c_display_width,
					      c_display_height, c_bench_value,
					      c_magic_number, user_dir,
					      &progress);
		      if (c_level)
			{
			  ret = lw6_make_scm_map (c_level);
			}
		      LW6SYS_FREE (user_dir);
		    }
		  lw6sys_assoc_free (c_forced_param);
		}
	      lw6sys_assoc_free (c_default_param);
	    }
	  LW6SYS_FREE (c_relative_path);
	}
      LW6SYS_FREE (c_map_path);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ldr_print_examples ()
{
  char *user_dir = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  user_dir = lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
  if (user_dir)
    {
      lw6ldr_print_examples (user_dir);
      LW6SYS_FREE (user_dir);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6ldr_hints_get_default (SCM key)
{
  SCM ret = SCM_BOOL_F;
  char *c_key = NULL;
  char *c_value = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

  c_key = to_0str (key);
  if (c_key)
    {
      c_value = lw6ldr_hints_get_default (c_key);
      if (c_value)
	{
	  ret = scm_from_locale_string (c_value);
	  LW6SYS_FREE (c_value);
	}
      LW6SYS_FREE (c_key);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ldr_exp_validate (SCM level)
{
  SCM ret = SCM_BOOL_F;
  lw6map_level_t *c_level;
  char *user_dir = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.map, level), level,
	      SCM_ARG1, __FUNCTION__);

  c_level = lw6_scm_to_map (level);
  if (c_level)
    {
      user_dir =
	lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
      if (user_dir)
	{
	  ret =
	    lw6ldr_exp_validate (c_level, user_dir) ? SCM_BOOL_T : SCM_BOOL_F;
	  LW6SYS_FREE (user_dir);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ldr_chain_entry (SCM map_path, SCM relative_path)
{
  SCM ret = SCM_BOOL_F;
  char *c_map_path = NULL;
  char *c_relative_path = NULL;
  char *user_dir = NULL;
  lw6ldr_entry_t *c_entry = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (map_path), map_path, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (relative_path), relative_path, SCM_ARG2,
	      __FUNCTION__);

  c_map_path = to_0str (map_path);
  if (c_map_path)
    {
      c_relative_path = to_0str (relative_path);
      if (c_relative_path)
	{
	  user_dir =
	    lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
	  if (user_dir)
	    {
	      c_entry =
		lw6ldr_chain_entry (c_map_path, c_relative_path, user_dir);
	      if (c_entry)
		{
		  ret = SCM_EOL;
		  ret = scm_cons (scm_cons
				  (scm_from_locale_string ("title"),
				   scm_from_locale_string (c_entry->
							   metadata.title)),
				  ret);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string ("author"),
			       scm_from_locale_string (c_entry->
						       metadata.author)),
			      ret);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string ("description"),
			       scm_from_locale_string (c_entry->
						       metadata.description)),
			      ret);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string ("license"),
			       scm_from_locale_string (c_entry->
						       metadata.license)),
			      ret);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string ("absolute-path"),
			       scm_from_locale_string
			       (c_entry->absolute_path)), ret);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string ("relative-path"),
			       scm_from_locale_string
			       (c_entry->relative_path)), ret);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string ("has-subdirs"),
			       c_entry->has_subdirs ? SCM_BOOL_T :
			       SCM_BOOL_F), ret);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string ("nb-submaps"),
			       scm_from_int (c_entry->nb_submaps)), ret);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string ("exp"),
			       scm_from_int (c_entry->metadata.vanilla_exp)),
			      ret);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string ("forbidden"),
			       c_entry->forbidden ? SCM_BOOL_T : SCM_BOOL_F),
			      ret);
		  lw6ldr_free_entry (c_entry);
		}
	      LW6SYS_FREE (user_dir);
	    }
	  LW6SYS_FREE (c_relative_path);
	}
      LW6SYS_FREE (c_map_path);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}


/*
 * In liquidwar6map
 */

static SCM
_scm_lw6map_get_look (SCM level)
{
  SCM ret = SCM_BOOL_F;
  lw6map_level_t *c_level;
  lw6gui_look_t *c_look;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.map, level), level,
	      SCM_ARG1, __FUNCTION__);

  c_level = lw6_scm_to_map (level);
  if (c_level)
    {
      c_look = lw6gui_look_new (&(c_level->param.style));
      if (c_look)
	{
	  ret = lw6_make_scm_look (c_look);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_param_get (SCM level, SCM key)
{
  lw6map_level_t *c_level = NULL;
  char *c_key;
  char *c_value;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.map,
	       level), level, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (key), key, SCM_ARG2, __FUNCTION__);

  c_level = lw6_scm_to_map (level);
  if (c_level)
    {
      c_key = to_0str (key);
      if (c_key)
	{
	  c_value = lw6map_param_get (&(c_level->param), c_key);
	  if (c_value)
	    {
	      ret = scm_from_locale_string (c_value);
	      LW6SYS_FREE (c_value);
	    }
	  LW6SYS_FREE (c_key);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_get_music_dir (SCM level)
{
  SCM ret = SCM_BOOL_F;
  lw6map_level_t *c_level;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.map, level), level,
	      SCM_ARG1, __FUNCTION__);

  c_level = lw6_scm_to_map (level);
  if (c_level)
    {
      if (c_level->local_info.music_dir)
	{
	  ret = scm_from_locale_string (c_level->local_info.music_dir);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_team_color_index_to_key (SCM index)
{
  SCM ret = SCM_BOOL_F;

  int c_index;

  SCM_ASSERT (scm_is_integer (index), index, SCM_ARG1, __FUNCTION__);

  c_index = scm_to_int (index);
  ret = scm_from_locale_string (lw6map_team_color_index_to_key (c_index));

  return ret;
}

static SCM
_scm_lw6map_team_color_key_to_index (SCM key)
{
  SCM ret = SCM_BOOL_F;

  char *c_key = NULL;

  SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

  c_key = to_0str (key);
  if (c_key)
    {
      ret = scm_from_int (lw6map_team_color_key_to_index (c_key));
      LW6SYS_FREE (c_key);
    }

  return ret;
}

static SCM
_scm_lw6map_team_color_index_to_label (SCM index)
{
  SCM ret = SCM_BOOL_F;

  int c_index;

  SCM_ASSERT (scm_is_integer (index), index, SCM_ARG1, __FUNCTION__);

  c_index = scm_to_int (index);
  ret = scm_from_locale_string (lw6map_team_color_index_to_label (c_index));

  return ret;
}

static SCM
_scm_lw6map_team_color_list ()
{
  SCM ret = SCM_BOOL_F;
  int i = 0;
  int exp = LW6MAP_RULES_MIN_EXP;
  char *color_key = NULL;
  char *user_dir = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  user_dir = lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
  if (user_dir)
    {
      lw6cfg_load_exp (user_dir, &exp);
      ret = SCM_EOL;
      for (i = lw6map_exp_get_highest_team_color_allowed (exp); i >= 0; --i)
	{
	  color_key = lw6map_team_color_index_to_key (i);
	  if (color_key)
	    {
	      ret = scm_cons (scm_from_locale_string (color_key), ret);
	    }
	}
      LW6SYS_FREE (user_dir);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_weapon_index_to_key (SCM index)
{
  SCM ret = SCM_BOOL_F;

  int c_index;

  SCM_ASSERT (scm_is_integer (index), index, SCM_ARG1, __FUNCTION__);

  c_index = scm_to_int (index);
  ret = scm_from_locale_string (lw6map_weapon_index_to_key (c_index));

  return ret;
}

static SCM
_scm_lw6map_weapon_key_to_index (SCM key)
{
  SCM ret = SCM_BOOL_F;

  char *c_key = NULL;

  SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

  c_key = to_0str (key);
  if (c_key)
    {
      ret = scm_from_int (lw6map_weapon_key_to_index (c_key));
      LW6SYS_FREE (c_key);
    }

  return ret;
}

static SCM
_scm_lw6map_weapon_index_to_label (SCM index)
{
  SCM ret = SCM_BOOL_F;

  int c_index;

  SCM_ASSERT (scm_is_integer (index), index, SCM_ARG1, __FUNCTION__);

  c_index = scm_to_int (index);
  ret = scm_from_locale_string (lw6map_weapon_index_to_label (c_index));

  return ret;
}

static SCM
_scm_lw6map_weapon_list ()
{
  SCM ret = SCM_BOOL_F;
  int i = 0;
  int exp = 0;
  char *weapon_key = NULL;
  char *user_dir = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  user_dir = lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
  if (user_dir)
    {
      if (lw6cfg_load_exp (user_dir, &exp))
	{
	  ret = SCM_EOL;
	  for (i = lw6map_exp_get_highest_weapon_allowed (exp); i >= 0; --i)
	    {
	      weapon_key = lw6map_weapon_index_to_key (i);
	      if (weapon_key)
		{
		  ret = scm_cons (scm_from_locale_string (weapon_key), ret);
		}
	    }
	}
      LW6SYS_FREE (user_dir);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In static.c
 */
static SCM
_scm_lw6map_rules_get_default (SCM key)
{
  SCM ret = SCM_BOOL_F;
  char *c_key = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

  c_key = to_0str (key);
  if (c_key)
    {
      ret = scm_from_int (lw6map_rules_get_default (c_key));

      LW6SYS_FREE (c_key);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_rules_get_min (SCM key)
{
  SCM ret = SCM_BOOL_F;
  char *c_key = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

  c_key = to_0str (key);
  if (c_key)
    {
      ret = scm_from_int (lw6map_rules_get_min (c_key));

      LW6SYS_FREE (c_key);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_rules_get_max (SCM key)
{
  SCM ret = SCM_BOOL_F;
  char *c_key = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

  c_key = to_0str (key);
  if (c_key)
    {
      ret = scm_from_int (lw6map_rules_get_max (c_key));

      LW6SYS_FREE (c_key);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_rules_get_int (SCM game_struct, SCM key)
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  lw6ker_game_struct_t *c_game_struct;
  char *c_key;

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_struct,
	       game_struct), game_struct, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (key), key, SCM_ARG2, __FUNCTION__);

  c_game_struct = lw6_scm_to_game_struct (game_struct);
  if (c_game_struct)
    {
      c_key = to_0str (key);
      if (c_key)
	{
	  ret =
	    scm_from_int (lw6map_rules_get_int
			  (&(c_game_struct->rules), c_key));
	  LW6SYS_FREE (c_key);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_style_get_default (SCM key)
{
  SCM ret = SCM_BOOL_F;
  char *c_key = NULL;
  char *c_value = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

  c_key = to_0str (key);
  if (c_key)
    {
      c_value = lw6map_style_get_default (c_key);
      if (c_value)
	{
	  ret = scm_from_locale_string (c_value);
	  LW6SYS_FREE (c_value);
	}
      LW6SYS_FREE (c_key);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_teams_get_default (SCM key)
{
  SCM ret = SCM_BOOL_F;
  char *c_key = NULL;
  char *c_value = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (key), key, SCM_ARG1, __FUNCTION__);

  c_key = to_0str (key);
  if (c_key)
    {
      c_value = lw6map_teams_get_default (c_key);
      if (c_value)
	{
	  ret = scm_from_locale_string (c_value);
	  LW6SYS_FREE (c_value);
	}
      LW6SYS_FREE (c_key);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_exp_is_team_color_allowed (SCM level, SCM team_color)
{
  lw6map_level_t *c_level;
  int c_team_color;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.map, level), level,
	      SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (team_color), team_color, SCM_ARG2,
	      __FUNCTION__);

  c_level = lw6_scm_to_map (level);
  if (c_level)
    {
      c_team_color = scm_to_int (team_color);

      ret =
	scm_from_int (lw6map_exp_is_team_color_allowed
		      (&(c_level->param.rules), c_team_color));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_exp_is_weapon_allowed (SCM level, SCM weapon)
{
  lw6map_level_t *c_level;
  int c_weapon;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.map, level), level,
	      SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (weapon), weapon, SCM_ARG2, __FUNCTION__);

  c_level = lw6_scm_to_map (level);
  if (c_level)
    {
      c_weapon = scm_to_int (weapon);

      ret =
	scm_from_int (lw6map_exp_is_weapon_allowed
		      (&(c_level->param.rules), c_weapon));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_exp_get_unlocked_team_color ()
{
  SCM ret = SCM_BOOL_F;
  int exp = LW6MAP_RULES_MIN_EXP;
  char *user_dir;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  user_dir = lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
  if (user_dir)
    {
      lw6cfg_load_exp (user_dir, &exp);

      ret = scm_from_int (lw6map_exp_get_unlocked_team_color (exp));

      LW6SYS_FREE (user_dir);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6map_exp_get_unlocked_weapon (SCM weapon)
{
  SCM ret = SCM_BOOL_F;
  int exp = LW6MAP_RULES_MIN_EXP;
  char *user_dir;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  user_dir = lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
  if (user_dir)
    {
      lw6cfg_load_exp (user_dir, &exp);

      ret = scm_from_int (lw6map_exp_get_unlocked_weapon (exp));

      LW6SYS_FREE (user_dir);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In liquidwar6ker
 */
static SCM
_scm_lw6ker_build_game_struct (SCM level)
{
  lw6map_level_t *c_level;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.map, level), level,
	      SCM_ARG1, __FUNCTION__);
  c_level = lw6_scm_to_map (level);
  if (c_level)
    {
      lw6ker_game_struct_t *c_game_struct;

      c_game_struct = lw6ker_game_struct_new (c_level, NULL);
      if (c_game_struct)
	{
	  ret = lw6_make_scm_game_struct (c_game_struct, level);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_build_game_state (SCM game_struct)
{
  lw6ker_game_struct_t *c_game_struct;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_struct,
	       game_struct), game_struct, SCM_ARG1, __FUNCTION__);
  c_game_struct = lw6_scm_to_game_struct (game_struct);
  if (c_game_struct)
    {
      lw6ker_game_state_t *c_game_state;
      c_game_state = lw6ker_game_state_new (c_game_struct, NULL);
      if (c_game_state)
	{
	  ret = lw6_make_scm_game_state (c_game_state, game_struct);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_sync_game_state (SCM dst, SCM src)
{
  lw6ker_game_state_t *c_dst;
  lw6ker_game_state_t *c_src;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       dst), dst, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       src), src, SCM_ARG2, __FUNCTION__);
  c_dst = lw6_scm_to_game_state (dst);
  c_src = lw6_scm_to_game_state (src);
  if (c_dst && c_src)
    {
      ret = lw6ker_game_state_sync (c_dst, c_src) ? SCM_BOOL_T : SCM_BOOL_F;
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_dup_game_state (SCM game_struct, SCM game_state)
{
  lw6ker_game_struct_t *c_game_struct;
  lw6ker_game_state_t *c_game_state;
  lw6ker_game_state_t *c_ret;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_struct,
	       game_struct), game_struct, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG2, __FUNCTION__);

  c_game_struct = lw6_scm_to_game_struct (game_struct);
  if (c_game_struct)
    {
      c_game_state = lw6_scm_to_game_state (game_state);
      if (c_game_state)
	{
	  c_ret = lw6ker_game_state_new (c_game_struct, NULL);
	  if (c_ret)
	    {
	      lw6ker_game_state_sync (c_ret, c_game_state);
	      ret = lw6_make_scm_game_state (c_ret, game_struct);
	    }
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_game_struct_checksum (SCM game_struct)
{
  lw6ker_game_struct_t *c_game_struct;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_struct,
	       game_struct), game_struct, SCM_ARG1, __FUNCTION__);
  c_game_struct = lw6_scm_to_game_struct (game_struct);
  if (c_game_struct)
    {
      ret = scm_from_int (lw6ker_game_struct_checksum (c_game_struct));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_game_state_checksum (SCM game_state)
{
  lw6ker_game_state_t *c_game_state;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      ret = scm_from_int (lw6ker_game_state_checksum (c_game_state));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_register_node (SCM game_state, SCM node_id)
{
  SCM ret = SCM_BOOL_F;
  char *c_node_id_str;
  u_int64_t c_node_id_int = 0LL;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (node_id), node_id, SCM_ARG2, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_node_id_str = to_0str (node_id);
      if (c_node_id_str)
	{
	  c_node_id_int = lw6sys_id_atol (c_node_id_str);
	  LW6SYS_FREE (c_node_id_str);
	}

      if (lw6sys_check_id (c_node_id_int))
	{
	  ret =
	    lw6ker_game_state_register_node (c_game_state,
					     c_node_id_int) ? SCM_BOOL_T :
	    SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_unregister_node (SCM game_state, SCM node_id)
{
  SCM ret = SCM_BOOL_F;
  char *c_node_id_str;
  u_int64_t c_node_id_int = 0LL;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (node_id), node_id, SCM_ARG2, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_node_id_str = to_0str (node_id);
      if (c_node_id_str)
	{
	  c_node_id_int = lw6sys_id_atol (c_node_id_str);
	  LW6SYS_FREE (c_node_id_str);
	}

      if (lw6sys_check_id (c_node_id_int))
	{
	  ret =
	    lw6ker_game_state_unregister_node (c_game_state,
					       c_node_id_int) ? SCM_BOOL_T
	    : SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_node_exists (SCM game_state, SCM node_id)
{
  SCM ret = SCM_BOOL_F;
  char *c_node_id_str;
  u_int64_t c_node_id_int = 0;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (node_id), node_id, SCM_ARG2, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_node_id_str = to_0str (node_id);
      if (c_node_id_str)
	{
	  c_node_id_int = lw6sys_id_atol (c_node_id_str);
	  LW6SYS_FREE (c_node_id_str);
	}

      if (lw6sys_check_id (c_node_id_int))
	{
	  ret =
	    lw6ker_game_state_node_exists (c_game_state,
					   c_node_id_int) ? SCM_BOOL_T :
	    SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_add_cursor (SCM game_state, SCM node_id, SCM cursor_id,
			SCM team_color)
{
  SCM ret = SCM_BOOL_F;
  char *c_node_id_str;
  char *c_cursor_id_str;
  char *c_team_color_str;
  u_int64_t c_node_id_int = 0LL;
  u_int16_t c_cursor_id_int = 0;
  int c_team_color_int = LW6MAP_TEAM_COLOR_INVALID;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (node_id), node_id, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_string (cursor_id), cursor_id, SCM_ARG3, __FUNCTION__);
  SCM_ASSERT (scm_is_string (team_color), team_color, SCM_ARG4, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_node_id_str = to_0str (node_id);
      if (c_node_id_str)
	{
	  c_node_id_int = lw6sys_id_atol (c_node_id_str);
	  LW6SYS_FREE (c_node_id_str);
	}
      c_cursor_id_str = to_0str (cursor_id);
      if (c_cursor_id_str)
	{
	  c_cursor_id_int = lw6sys_id_atol (c_cursor_id_str);
	  LW6SYS_FREE (c_cursor_id_str);
	}
      c_team_color_str = to_0str (team_color);
      if (c_team_color_str)
	{
	  c_team_color_int =
	    lw6map_team_color_key_to_index (c_team_color_str);
	  LW6SYS_FREE (c_team_color_str);
	}

      if (lw6sys_check_id (c_node_id_int)
	  && lw6sys_check_id (c_cursor_id_int)
	  && c_team_color_int != LW6MAP_TEAM_COLOR_INVALID)
	{
	  ret =
	    lw6ker_game_state_add_cursor (c_game_state, c_node_id_int,
					  c_cursor_id_int,
					  c_team_color_int) ? SCM_BOOL_T :
	    SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_remove_cursor (SCM game_state, SCM node_id, SCM cursor_id)
{
  SCM ret = SCM_BOOL_F;
  char *c_node_id_str;
  char *c_cursor_id_str;
  u_int64_t c_node_id_int = 0LL;
  u_int16_t c_cursor_id_int = 0;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (node_id), node_id, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_string (cursor_id), cursor_id, SCM_ARG3, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_node_id_str = to_0str (node_id);
      if (c_node_id_str)
	{
	  c_node_id_int = lw6sys_id_atol (c_node_id_str);
	  LW6SYS_FREE (c_node_id_str);
	}
      c_cursor_id_str = to_0str (cursor_id);
      if (c_cursor_id_str)
	{
	  c_cursor_id_int = lw6sys_id_atol (c_cursor_id_str);
	  LW6SYS_FREE (c_cursor_id_str);
	}

      if (lw6sys_check_id (c_node_id_int)
	  && lw6sys_check_id (c_cursor_id_int))
	{
	  ret =
	    lw6ker_game_state_remove_cursor (c_game_state, c_node_id_int,
					     c_cursor_id_int) ? SCM_BOOL_T :
	    SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_cursor_exists (SCM game_state, SCM cursor_id)
{
  SCM ret = SCM_BOOL_F;
  char *c_cursor_id_str;
  u_int16_t c_cursor_id_int = 0;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (cursor_id), cursor_id, SCM_ARG2, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_cursor_id_str = to_0str (cursor_id);
      if (c_cursor_id_str)
	{
	  c_cursor_id_int = lw6sys_id_atol (c_cursor_id_str);
	  LW6SYS_FREE (c_cursor_id_str);
	}

      if (lw6sys_check_id (c_cursor_id_int))
	{
	  ret =
	    lw6ker_game_state_cursor_exists (c_game_state,
					     c_cursor_id_int) ? SCM_BOOL_T :
	    SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_get_cursor (SCM game_state, SCM cursor_id)
{
  SCM ret = SCM_BOOL_F;
  char *c_cursor_id_str;
  u_int16_t c_cursor_id_int = 0;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (cursor_id), cursor_id, SCM_ARG2, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_cursor_id_str = to_0str (cursor_id);
      if (c_cursor_id_str)
	{
	  c_cursor_id_int = lw6sys_id_atol (c_cursor_id_str);
	  LW6SYS_FREE (c_cursor_id_str);
	}

      if (lw6sys_check_id (c_cursor_id_int))
	{
	  char *c_team_color_str = NULL;
	  char *c_node_id_str = NULL;
	  char c_letter_char = '\0';
	  char c_letter_str[2] = { 0, 0 };
	  lw6ker_cursor_t c_cursor;

	  if (lw6ker_game_state_get_cursor (c_game_state,
					    &c_cursor, c_cursor_id_int))
	    {
	      c_team_color_str =
		lw6map_team_color_index_to_key (c_cursor.team_color);
	      if (c_team_color_str)
		{
		  c_node_id_str = lw6sys_id_ltoa (c_cursor.node_id);
		  if (c_node_id_str)
		    {
		      c_letter_str[0] = c_letter_char;
		      ret = scm_list_5 (scm_cons
					(scm_from_locale_string ("node-id"),
					 scm_from_locale_string
					 (c_node_id_str)),
					scm_cons (scm_from_locale_string
						  ("letter"),
						  scm_from_locale_string
						  (c_letter_str)),
					scm_cons (scm_from_locale_string
						  ("team-color"),
						  scm_from_locale_string
						  (c_team_color_str)),
					scm_cons (scm_from_locale_string
						  ("x"),
						  scm_from_int (c_cursor.
								pos.x)),
					scm_cons (scm_from_locale_string
						  ("y"),
						  scm_from_int (c_cursor.
								pos.y)));
		      LW6SYS_FREE (c_node_id_str);
		    }
		}
	    }
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_set_cursor (SCM game_state, SCM node_id, SCM cursor_id, SCM x,
			SCM y)
{
  SCM ret = SCM_BOOL_F;
  char *c_node_id_str;
  char *c_cursor_id_str;
  lw6ker_cursor_t c_cursor;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (node_id), node_id, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_string (cursor_id), cursor_id, SCM_ARG3, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (x), x, SCM_ARG4, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (y), y, SCM_ARG5, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      lw6ker_cursor_reset (&c_cursor);

      c_node_id_str = to_0str (node_id);
      if (c_node_id_str)
	{
	  c_cursor.node_id = lw6sys_id_atol (c_node_id_str);
	  LW6SYS_FREE (c_node_id_str);
	}
      c_cursor_id_str = to_0str (cursor_id);
      if (c_cursor_id_str)
	{
	  c_cursor.cursor_id = lw6sys_id_atol (c_cursor_id_str);
	  LW6SYS_FREE (c_cursor_id_str);
	}
      c_cursor.pos.x = scm_to_int (x);
      c_cursor.pos.y = scm_to_int (y);

      if (lw6sys_check_id (c_cursor.node_id)
	  && lw6sys_check_id (c_cursor.cursor_id))
	{
	  ret =
	    lw6ker_game_state_set_cursor (c_game_state, &c_cursor) ?
	    SCM_BOOL_T : SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_do_round (SCM game_state)
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  lw6ker_game_state_t *c_game_state;
  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      lw6ker_game_state_do_round (c_game_state);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6ker_get_moves (SCM game_state)
{
  SCM ret = SCM_BOOL_F;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      ret = scm_from_int (lw6ker_game_state_get_moves (c_game_state));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_get_spreads (SCM game_state)
{
  SCM ret = SCM_BOOL_F;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      ret = scm_from_int (lw6ker_game_state_get_spreads (c_game_state));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_get_rounds (SCM game_state)
{
  SCM ret = SCM_BOOL_F;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      ret = scm_from_int (lw6ker_game_state_get_rounds (c_game_state));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_is_over (SCM game_state)
{
  SCM ret = SCM_BOOL_F;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      if (lw6ker_game_state_is_over (c_game_state))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6ker_did_cursor_win (SCM game_state, SCM cursor_id)
{
  SCM ret = SCM_BOOL_F;
  char *c_cursor_id_str;
  u_int16_t c_cursor_id_int = 0;
  lw6ker_game_state_t *c_game_state;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (cursor_id), cursor_id, SCM_ARG2, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_cursor_id_str = to_0str (cursor_id);
      if (c_cursor_id_str)
	{
	  c_cursor_id_int = lw6sys_id_atol (c_cursor_id_str);
	  LW6SYS_FREE (c_cursor_id_str);
	}

      if (lw6sys_check_id (c_cursor_id_int))
	{
	  ret =
	    lw6ker_game_state_did_cursor_win (c_game_state,
					      c_cursor_id_int) ? SCM_BOOL_T :
	    SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In liquidwar6pil
 */
static SCM
_scm_lw6pil_bench ()
{
  SCM ret = SCM_BOOL_F;
  lw6sys_progress_t progress;
  float c_ret = 0.0f;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  lw6sys_progress_default (&progress, &(lw6_global.progress));
  lw6sys_progress_begin (&progress);

  if (lw6pil_bench (&c_ret, &progress))
    {
      ret = scm_from_int (ceil (c_ret));
    }

  lw6sys_progress_begin (&progress);

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_build_pilot (SCM game_state, SCM seq_0, SCM timestamp)
{
  lw6ker_game_state_t *c_game_state;
  int64_t c_seq_0;
  int64_t c_timestamp;
  lw6pil_pilot_t *c_ret;
  SCM ret = SCM_BOOL_F;
  lw6sys_progress_t progress;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (seq_0), seq_0, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (timestamp), timestamp, SCM_ARG3, __FUNCTION__);

  lw6sys_progress_default (&progress, &(lw6_global.progress));
  lw6sys_progress_begin (&progress);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_seq_0 = scm_to_long_long (seq_0);
      c_timestamp = scm_to_long_long (timestamp);
      c_ret =
	lw6pil_pilot_new (c_game_state, c_seq_0, c_timestamp, &progress);
      if (c_ret)
	{
	  ret = lw6_make_scm_pilot (c_ret);
	}
    }

  lw6sys_progress_begin (&progress);

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_send_command (SCM pilot, SCM command_text, SCM verified)
{
  lw6pil_pilot_t *c_pilot = NULL;
  char *c_command_text = NULL;
  int c_verified;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (command_text), command_text, SCM_ARG2,
	      __FUNCTION__);
  SCM_ASSERT (SCM_BOOLP (verified), verified, SCM_ARG3, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      c_command_text = to_0str (command_text);
      if (c_command_text)
	{
	  c_verified = SCM_NFALSEP (verified);
	  ret =
	    lw6pil_pilot_send_command (c_pilot, c_command_text,
				       c_verified) ? SCM_BOOL_T : SCM_BOOL_F;
	  LW6SYS_FREE (c_command_text);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_local_command (SCM pilot, SCM command_text)
{
  lw6pil_pilot_t *c_pilot = NULL;
  char *c_command_text = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (command_text), command_text, SCM_ARG2,
	      __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      c_command_text = to_0str (command_text);
      if (c_command_text)
	{
	  ret =
	    lw6pil_pilot_local_command (c_pilot,
					c_command_text) ? SCM_BOOL_T :
	    SCM_BOOL_F;
	  LW6SYS_FREE (c_command_text);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_commit (SCM pilot)
{
  lw6pil_pilot_t *c_pilot = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      ret = lw6pil_pilot_commit (c_pilot) ? SCM_BOOL_T : SCM_BOOL_F;
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_fix_coords (SCM game_state, SCM x, SCM y, SCM z)
{
  lw6ker_game_state_t *c_game_state = NULL;
  lw6sys_whd_t shape;
  float c_x = 0.0f;
  float c_y = 0.0f;
  float c_z = 0.0f;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_number (x), x, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_number (y), y, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_number (z), z, SCM_ARG3, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_x = scm_to_double (x);
      c_y = scm_to_double (y);
      c_z = scm_to_double (z);

      lw6ker_game_state_get_shape (c_game_state, &shape);
      lw6pil_coords_fix (&(c_game_state->game_struct->rules),
			 &shape, &c_x, &c_y, &c_z);
      ret =
	scm_list_3 (scm_cons
		    (scm_from_locale_string ("x"), scm_from_double (c_x)),
		    scm_cons (scm_from_locale_string ("y"),
			      scm_from_double (c_y)),
		    scm_cons (scm_from_locale_string ("z"),
			      scm_from_double (c_z)));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_fix_coords_x10 (SCM game_state, SCM x, SCM y, SCM z)
{
  lw6ker_game_state_t *c_game_state = NULL;
  lw6sys_whd_t shape;
  float c_x = 0.0f;
  float c_y = 0.0f;
  float c_z = 0.0f;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_number (x), x, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_number (y), y, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_number (z), z, SCM_ARG3, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_x = scm_to_double (x);
      c_y = scm_to_double (y);
      c_z = scm_to_double (z);

      lw6ker_game_state_get_shape (c_game_state, &shape);
      lw6pil_coords_fix_x10 (&(c_game_state->game_struct->rules),
			     &shape, &c_x, &c_y, &c_z);
      ret =
	scm_list_3 (scm_cons
		    (scm_from_locale_string ("x"), scm_from_double (c_x)),
		    scm_cons (scm_from_locale_string ("y"),
			      scm_from_double (c_y)),
		    scm_cons (scm_from_locale_string ("z"),
			      scm_from_double (c_z)));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_execute_command (SCM game_state, SCM command_text, SCM seq_0)
{
  lw6ker_game_state_t *c_game_state = NULL;
  char *c_command_text = NULL;
  int64_t c_seq_0;

  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (command_text), command_text, SCM_ARG2,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (seq_0), seq_0, SCM_ARG3, __FUNCTION__);

  c_game_state = lw6_scm_to_game_state (game_state);
  if (c_game_state)
    {
      c_command_text = to_0str (command_text);
      c_seq_0 = scm_to_long_long (seq_0);
      if (c_command_text)
	{
	  ret =
	    lw6pil_command_execute_text (c_game_state,
					 c_command_text,
					 c_seq_0) ? SCM_BOOL_T : SCM_BOOL_F;
	  LW6SYS_FREE (c_command_text);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_local_cursors_set_main (SCM pilot, SCM cursor_id)
{
  lw6pil_pilot_t *c_pilot = NULL;
  char *c_cursor_id_str = NULL;
  u_int16_t c_cursor_id_int;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (cursor_id), cursor_id, SCM_ARG2, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      c_cursor_id_str = to_0str (cursor_id);
      if (c_cursor_id_str)
	{
	  c_cursor_id_int = lw6sys_id_atol (c_cursor_id_str);
	  ret =
	    lw6pil_local_cursors_set_main (lw6pil_pilot_get_local_cursors
					   (c_pilot),
					   c_cursor_id_int) ? SCM_BOOL_T :
	    SCM_BOOL_F;
	  LW6SYS_FREE (c_cursor_id_str);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_local_cursors_set_mouse_controlled (SCM pilot, SCM cursor_id,
						SCM mouse_controlled)
{
  lw6pil_pilot_t *c_pilot = NULL;
  char *c_cursor_id_str = NULL;
  u_int16_t c_cursor_id_int;
  int c_mouse_controlled;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (cursor_id), cursor_id, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (SCM_BOOLP (mouse_controlled), mouse_controlled, SCM_ARG3,
	      __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      c_cursor_id_str = to_0str (cursor_id);
      if (c_cursor_id_str)
	{
	  c_cursor_id_int = lw6sys_id_atol (c_cursor_id_str);
	  c_mouse_controlled = SCM_NFALSEP (mouse_controlled);
	  ret =
	    lw6pil_local_cursors_set_mouse_controlled
	    (lw6pil_pilot_get_local_cursors (c_pilot), c_cursor_id_int,
	     c_mouse_controlled) ? SCM_BOOL_T : SCM_BOOL_F;
	  LW6SYS_FREE (c_cursor_id_str);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_make_backup (SCM pilot)
{
  lw6pil_pilot_t *c_pilot = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      ret = lw6pil_pilot_make_backup (c_pilot) ? SCM_BOOL_T : SCM_BOOL_F;
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_sync_from_backup (SCM target, SCM pilot)
{
  lw6ker_game_state_t *c_target = NULL;
  lw6pil_pilot_t *c_pilot = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       target), target, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG2, __FUNCTION__);

  c_target = lw6_scm_to_game_state (target);
  if (c_target)
    {
      c_pilot = lw6_scm_to_pilot (pilot);
      if (c_pilot)
	{
	  ret =
	    lw6pil_pilot_sync_from_backup (c_target,
					   c_pilot) ? SCM_BOOL_T : SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_sync_from_reference (SCM target, SCM pilot)
{
  lw6ker_game_state_t *c_target = NULL;
  lw6pil_pilot_t *c_pilot = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       target), target, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG2, __FUNCTION__);

  c_target = lw6_scm_to_game_state (target);
  if (c_target)
    {
      c_pilot = lw6_scm_to_pilot (pilot);
      if (c_pilot)
	{
	  ret =
	    lw6pil_pilot_sync_from_reference (c_target,
					      c_pilot) ? SCM_BOOL_T :
	    SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_sync_from_draft (SCM target, SCM pilot, SCM dirty_read)
{
  lw6ker_game_state_t *c_target = NULL;
  lw6pil_pilot_t *c_pilot = NULL;
  int c_dirty_read = 0;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       target), target, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (SCM_BOOLP (dirty_read), dirty_read, SCM_ARG3, __FUNCTION__);

  c_target = lw6_scm_to_game_state (target);
  if (c_target)
    {
      c_pilot = lw6_scm_to_pilot (pilot);
      if (c_pilot)
	{
	  c_dirty_read = SCM_NFALSEP (dirty_read);
	  ret =
	    lw6pil_pilot_sync_from_draft (c_target,
					  c_pilot,
					  c_dirty_read) ? SCM_BOOL_T :
	    SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_calibrate (SCM pilot, SCM timestamp, SCM round)
{
  lw6pil_pilot_t *c_pilot = NULL;
  int64_t c_timestamp;
  int c_round;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (timestamp), timestamp, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (round), round, SCM_ARG3, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      c_timestamp = scm_to_long_long (timestamp);
      c_round = scm_to_int (round);

      lw6pil_pilot_calibrate (c_pilot, c_timestamp, c_round);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6pil_speed_up (SCM pilot, SCM round_inc)
{
  lw6pil_pilot_t *c_pilot = NULL;
  int c_round_inc;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (round_inc), round_inc, SCM_ARG2, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      c_round_inc = scm_to_int (round_inc);

      lw6pil_pilot_speed_up (c_pilot, c_round_inc);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6pil_slow_down (SCM pilot, SCM round_dec)
{
  lw6pil_pilot_t *c_pilot = NULL;
  int c_round_dec;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (round_dec), round_dec, SCM_ARG2, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      c_round_dec = scm_to_int (round_dec);

      lw6pil_pilot_slow_down (c_pilot, c_round_dec);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6pil_get_next_seq (SCM pilot, SCM timestamp)
{
  lw6pil_pilot_t *c_pilot = NULL;
  int64_t c_timestamp;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (timestamp), timestamp, SCM_ARG2, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      c_timestamp = scm_to_long_long (timestamp);

      ret =
	scm_from_long_long (lw6pil_pilot_get_next_seq (c_pilot, c_timestamp));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_get_last_commit_seq (SCM pilot)
{
  lw6pil_pilot_t *c_pilot = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      ret = scm_from_long_long (lw6pil_pilot_get_last_commit_seq (c_pilot));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_get_reference_current_seq (SCM pilot)
{
  lw6pil_pilot_t *c_pilot = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      ret =
	scm_from_long_long (lw6pil_pilot_get_reference_current_seq (c_pilot));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_get_reference_target_seq (SCM pilot)
{
  lw6pil_pilot_t *c_pilot = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      ret =
	scm_from_long_long (lw6pil_pilot_get_reference_target_seq (c_pilot));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_get_max_seq (SCM pilot)
{
  lw6pil_pilot_t *c_pilot = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      ret = scm_from_long_long (lw6pil_pilot_get_max_seq (c_pilot));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_is_over (SCM pilot)
{
  lw6pil_pilot_t *c_pilot = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      if (lw6pil_pilot_is_over (c_pilot))
	{
	  ret = SCM_BOOL_T;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_did_cursor_win (SCM pilot, SCM cursor_id)
{
  SCM ret = SCM_BOOL_F;
  char *c_cursor_id_str;
  u_int16_t c_cursor_id_int = 0;
  lw6pil_pilot_t *c_pilot;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (cursor_id), cursor_id, SCM_ARG2, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      c_cursor_id_str = to_0str (cursor_id);
      if (c_cursor_id_str)
	{
	  c_cursor_id_int = lw6sys_id_atol (c_cursor_id_str);
	  LW6SYS_FREE (c_cursor_id_str);
	}

      if (lw6sys_check_id (c_cursor_id_int))
	{
	  ret =
	    lw6pil_pilot_did_cursor_win (c_pilot,
					 c_cursor_id_int) ? SCM_BOOL_T :
	    SCM_BOOL_F;
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_get_winner (SCM pilot)
{
  SCM ret = SCM_BOOL_F;
  lw6pil_pilot_t *c_pilot;
  int c_ret;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      c_ret = lw6pil_pilot_get_winner (c_pilot);
      if (lw6map_team_color_is_valid (c_ret))
	{
	  ret = scm_from_int (c_ret);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_get_looser (SCM pilot)
{
  SCM ret = SCM_BOOL_F;
  lw6pil_pilot_t *c_pilot;
  int c_ret;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG1, __FUNCTION__);

  c_pilot = lw6_scm_to_pilot (pilot);
  if (c_pilot)
    {
      c_ret = lw6pil_pilot_get_looser (c_pilot);
      if (lw6map_team_color_is_valid (c_ret))
	{
	  ret = scm_from_int (c_ret);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6pil_seq_random_0 ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_long_long (lw6pil_seq_random_0 ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In liquidwar6snd
 */

/*
 * In backend.c
 */
static SCM
_scm_lw6snd_get_backends ()
{
  SCM ret = SCM_BOOL_F;
  lw6sys_assoc_t *backends;
  lw6sys_list_t *keys;
  lw6sys_list_t *key;
  char *module_id;
  char *module_name;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  backends = lw6snd_get_backends (lw6_global.argc, lw6_global.argv);
  if (backends)
    {
      keys = lw6sys_assoc_keys (backends);
      if (keys)
	{
	  ret = SCM_EOL;
	  key = keys;
	  while (key)
	    {
	      if (key->data)
		{
		  module_id = (char *) key->data;
		  module_name =
		    (char *) lw6sys_assoc_get (backends, module_id);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string (module_id),
			       scm_from_locale_string (module_name)), ret);
		}
	      key = lw6sys_list_next (key);
	    }
	  lw6sys_list_free (keys);
	  ret = scm_reverse (ret);
	}
      lw6sys_assoc_free (backends);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6snd_new (SCM backend_name, SCM fx_volume, SCM water_volume,
		 SCM music_volume)
{
  SCM ret = SCM_BOOL_F;
  char *c_backend_name;
  float c_fx_volume;
  float c_water_volume;
  float c_music_volume;
  lw6snd_backend_t *c_ret;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (backend_name),
	      backend_name, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_REALP (fx_volume), fx_volume, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (SCM_REALP (music_volume), music_volume, SCM_ARG3, __FUNCTION__);
  SCM_ASSERT (SCM_REALP (water_volume), water_volume, SCM_ARG4, __FUNCTION__);

  c_backend_name = to_0str (backend_name);
  if (backend_name)
    {
      c_fx_volume = scm_to_double (fx_volume);
      c_water_volume = scm_to_double (water_volume);
      c_music_volume = scm_to_double (music_volume);

      c_ret = lw6snd_create_backend (lw6_global.argc, lw6_global.argv,
				     c_backend_name);
      if (c_ret)
	{
	  if (lw6snd_init
	      (c_ret, c_fx_volume, c_water_volume, c_music_volume))
	    {
	      ret = lw6_make_scm_snd (c_ret);
	    }
	  else
	    {
	      lw6snd_destroy_backend (c_ret);
	    }
	}
      LW6SYS_FREE (c_backend_name);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6snd_poll (SCM snd)
{
  SCM ret = SCM_BOOL_F;
  lw6snd_backend_t *c_snd;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.snd, snd), snd, SCM_ARG1, __FUNCTION__);

  c_snd = lw6_scm_to_snd (snd);
  if (c_snd)
    {
      lw6snd_poll (c_snd);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6snd_release (SCM snd)
{
  SCM ret = SCM_BOOL_F;
  lw6snd_backend_t *c_snd;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.snd, snd), snd, SCM_ARG1, __FUNCTION__);

  c_snd = lw6_scm_to_snd (snd);
  if (c_snd)
    {
      lw6snd_quit (c_snd);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6snd_play_fx (SCM snd, SCM fx_id)
{
  SCM ret = SCM_BOOL_F;
  lw6snd_backend_t *c_snd;
  int c_fx_id;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.snd, snd), snd, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (fx_id), fx_id, SCM_ARG2, __FUNCTION__);

  c_snd = lw6_scm_to_snd (snd);
  if (c_snd)
    {
      c_fx_id = scm_to_int (fx_id);

      ret = scm_from_int (lw6snd_play_fx (c_snd, c_fx_id));
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6snd_set_fx_volume (SCM snd, SCM fx_volume)
{
  lw6snd_backend_t *c_snd;
  float c_fx_volume;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.snd, snd), snd, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_REALP (fx_volume), fx_volume, SCM_ARG2, __FUNCTION__);

  c_snd = lw6_scm_to_snd (snd);
  if (c_snd)
    {
      c_fx_volume = scm_to_double (fx_volume);
      lw6snd_set_fx_volume (c_snd, c_fx_volume);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6snd_set_water_volume (SCM snd, SCM water_volume)
{
  lw6snd_backend_t *c_snd;
  float c_water_volume;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.snd, snd), snd, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_REALP (water_volume), water_volume, SCM_ARG2, __FUNCTION__);

  c_snd = lw6_scm_to_snd (snd);
  if (c_snd)
    {
      c_water_volume = scm_to_double (water_volume);
      lw6snd_set_water_volume (c_snd, c_water_volume);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6snd_is_music_file (SCM snd, SCM map_dir, SCM music_path,
			   SCM music_file)
{
  SCM ret = SCM_BOOL_F;
  lw6snd_backend_t *c_snd;
  char *c_map_dir = NULL;
  char *c_music_path = NULL;
  char *c_music_file = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.snd, snd), snd, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (map_dir), map_dir, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_string (music_path), music_path, SCM_ARG3, __FUNCTION__);
  SCM_ASSERT (scm_is_string (music_file), music_file, SCM_ARG4, __FUNCTION__);

  c_snd = lw6_scm_to_snd (snd);
  if (c_snd)
    {
      c_map_dir = to_0str (map_dir);
      if (c_map_dir)
	{
	  c_music_path = to_0str (music_path);
	  if (c_music_path)
	    {
	      c_music_file = to_0str (music_file);
	      if (c_music_file)
		{
		  if (lw6snd_is_music_file
		      (c_snd, c_map_dir, c_music_path, c_music_file))
		    {
		      ret = SCM_BOOL_T;
		    }
		  LW6SYS_FREE (c_music_file);
		}
	      LW6SYS_FREE (c_music_path);
	    }
	  LW6SYS_FREE (c_map_dir);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6snd_play_music_file (SCM snd, SCM map_dir, SCM music_path,
			     SCM music_file)
{
  SCM ret = SCM_BOOL_F;
  lw6snd_backend_t *c_snd;
  char *c_map_dir = NULL;
  char *c_music_path = NULL;
  char *c_music_file = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.snd, snd), snd, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (map_dir), map_dir, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_string (music_path), music_path, SCM_ARG3, __FUNCTION__);
  SCM_ASSERT (scm_is_string (music_file), music_file, SCM_ARG4, __FUNCTION__);

  c_snd = lw6_scm_to_snd (snd);
  if (c_snd)
    {
      c_map_dir = to_0str (map_dir);
      if (c_map_dir)
	{
	  c_music_path = to_0str (music_path);
	  if (c_music_path)
	    {
	      c_music_file = to_0str (music_file);
	      if (c_music_file)
		{
		  ret =
		    scm_from_int (lw6snd_play_music_file
				  (c_snd, c_map_dir, c_music_path,
				   c_music_file));
		  LW6SYS_FREE (c_music_file);
		}
	      LW6SYS_FREE (c_music_path);
	    }
	  LW6SYS_FREE (c_map_dir);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6snd_play_music_random (SCM snd, SCM music_path, SCM music_filter,
			       SCM music_exclude)
{
  SCM ret = SCM_BOOL_F;
  lw6snd_backend_t *c_snd;
  char *c_music_path = NULL;
  char *c_music_filter = NULL;
  char *c_music_exclude = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.snd, snd), snd, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (music_path), music_path, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_string (music_filter), music_filter, SCM_ARG3,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_string (music_exclude), music_exclude, SCM_ARG3,
	      __FUNCTION__);

  c_snd = lw6_scm_to_snd (snd);
  if (c_snd)
    {
      c_music_path = to_0str (music_path);
      if (c_music_path)
	{
	  c_music_filter = to_0str (music_filter);
	  if (c_music_filter)
	    {
	      c_music_exclude = to_0str (music_exclude);
	      if (c_music_exclude)
		{
		  ret =
		    scm_from_int (lw6snd_play_music_random
				  (c_snd, c_music_path, c_music_filter,
				   c_music_exclude));
		  LW6SYS_FREE (c_music_exclude);
		}
	      LW6SYS_FREE (c_music_filter);
	    }
	  LW6SYS_FREE (c_music_path);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6snd_stop_music (SCM snd)
{
  lw6snd_backend_t *c_snd;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.snd, snd), snd, SCM_ARG1, __FUNCTION__);

  c_snd = lw6_scm_to_snd (snd);
  if (c_snd)
    {
      lw6snd_stop_music (c_snd);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6snd_set_music_volume (SCM snd, SCM music_volume)
{
  lw6snd_backend_t *c_snd;
  float c_music_volume;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.snd, snd), snd, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (SCM_REALP (music_volume), music_volume, SCM_ARG2, __FUNCTION__);

  c_snd = lw6_scm_to_snd (snd);
  if (c_snd)
    {
      c_music_volume = scm_to_double (music_volume);
      lw6snd_set_music_volume (c_snd, c_music_volume);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

/*
 * In liquidwar6cns
 */

/*
 * In support.c
 */
static SCM
_scm_lw6cns_support ()
{
  SCM ret = SCM_BOOL_F;
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = lw6cns_support ()? SCM_BOOL_T : SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In backend.c
 */
static SCM
_scm_lw6cns_init ()
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (!lw6_global.cns_initialized)
    {
      lw6cns_handler_install (lw6_cns_handler);
      lw6_global.cns_initialized = 1;
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6cns_quit ()
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (lw6_global.cns_initialized)
    {
      lw6cns_handler_remove ();
      lw6_global.cns_initialized = 0;
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6cns_poll ()
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (lw6_global.cns_initialized)
    {
      lw6cns_handler_poll ();
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

/*
 * In liquidwar6tsk
 */
static SCM
_scm_lw6tsk_loader_new (SCM sleep)
{
  float c_sleep = 0.0f;
  char *user_dir = NULL;
  lw6tsk_loader_t *c_loader = NULL;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_number (sleep), sleep, SCM_ARG1, __FUNCTION__);

  c_sleep = scm_to_double (sleep);
  user_dir = lw6cfg_unified_get_user_dir (lw6_global.argc, lw6_global.argv);
  if (user_dir)
    {
      c_loader =
	lw6tsk_loader_new (c_sleep, user_dir, &(lw6_global.progress));
      if (c_loader)
	{
	  ret = lw6_make_scm_loader (c_loader);
	}
      LW6SYS_FREE (user_dir);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6tsk_loader_push (SCM loader, SCM map_path, SCM relative_path,
			 SCM default_param, SCM forced_param,
			 SCM display_width, SCM display_height,
			 SCM bench_value, SCM magic_number)
{
  lw6tsk_loader_t *c_loader;
  char *c_map_path;
  char *c_relative_path;
  lw6sys_assoc_t *c_default_param;
  lw6sys_assoc_t *c_forced_param;
  int c_display_width;
  int c_display_height;
  int c_bench_value;
  int c_magic_number;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.loader,
	       loader), loader, SCM_ARG1, __FUNCTION__);
  SCM_ASSERT (scm_is_string (map_path), map_path, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (scm_is_string (relative_path), relative_path, SCM_ARG3,
	      __FUNCTION__);
  SCM_ASSERT (SCM_CONSP (default_param)
	      || default_param == SCM_EOL, default_param, SCM_ARG4,
	      __FUNCTION__);
  SCM_ASSERT (SCM_CONSP (forced_param)
	      || forced_param == SCM_EOL, forced_param, SCM_ARG5,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (display_width), display_width, SCM_ARG6,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (display_height), display_height, SCM_ARG7,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (bench_value), bench_value, SCM_ARGn,
	      __FUNCTION__);
  SCM_ASSERT (scm_is_integer (magic_number), magic_number, SCM_ARGn,
	      __FUNCTION__);

  c_loader = lw6_scm_to_loader (loader);
  c_map_path = to_0str (map_path);
  if (c_map_path)
    {
      c_relative_path = to_0str (relative_path);
      if (c_relative_path)
	{
	  c_default_param = to_sys_str_assoc (default_param);
	  if (c_default_param)
	    {
	      c_forced_param = to_sys_str_assoc (forced_param);
	      if (c_forced_param)
		{
		  c_display_width = scm_to_int (display_width);
		  c_display_height = scm_to_int (display_height);
		  c_bench_value = scm_to_int (bench_value);
		  c_magic_number = scm_to_int (magic_number);

		  lw6tsk_loader_push (c_loader, c_map_path, c_relative_path,
				      c_default_param, c_forced_param,
				      c_display_width, c_display_height,
				      c_bench_value, c_magic_number);
		  ret = SCM_BOOL_T;

		  lw6sys_assoc_free (c_forced_param);
		}
	      lw6sys_assoc_free (c_default_param);
	    }
	  LW6SYS_FREE (c_relative_path);
	}
      LW6SYS_FREE (c_map_path);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6tsk_loader_pop (SCM loader)
{
  lw6tsk_loader_t *c_loader;
  lw6map_level_t *c_level = NULL;
  lw6ker_game_struct_t *c_game_struct = NULL;
  lw6ker_game_state_t *c_game_state = NULL;
  SCM level = SCM_BOOL_F;
  SCM game_struct = SCM_BOOL_F;
  SCM game_state = SCM_BOOL_F;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.loader,
	       loader), loader, SCM_ARG1, __FUNCTION__);

  c_loader = lw6_scm_to_loader (loader);
  if (c_loader)
    {
      if (lw6tsk_loader_pop
	  (&c_level, &c_game_struct, &c_game_state, c_loader))
	{
	  if (c_level)
	    {
	      level = lw6_make_scm_map (c_level);
	      if (c_game_struct)
		{
		  game_struct =
		    lw6_make_scm_game_struct (c_game_struct, level);
		  if (c_game_state)
		    {
		      game_state =
			lw6_make_scm_game_state (c_game_state, game_struct);
		    }
		}
	    }
	  ret =
	    scm_list_3 (scm_cons
			(scm_from_locale_string ("level"), level),
			scm_cons
			(scm_from_locale_string ("game-struct"), game_struct),
			scm_cons (scm_from_locale_string ("game-state"),
				  game_state));
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6tsk_loader_get_stage (SCM loader)
{
  lw6tsk_loader_t *c_loader;
  int c_ret = 0;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.loader,
	       loader), loader, SCM_ARG1, __FUNCTION__);

  c_loader = lw6_scm_to_loader (loader);
  if (c_loader)
    {
      c_ret = lw6tsk_loader_get_stage (c_loader);
      ret = scm_from_int (c_ret);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In liquidwar6net
 */

/*
 * In setup.c
 */
static SCM
_scm_lw6net_init (SCM net_log)
{
  SCM ret = SCM_BOOL_F;
  int c_net_log = 0;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_BOOLP (net_log), net_log, SCM_ARG1, __FUNCTION__);

  c_net_log = SCM_NFALSEP (net_log);
  if (lw6net_init (lw6_global.argc, lw6_global.argv, c_net_log))
    {
      lw6_global.net_initialized = 1;
      ret = SCM_BOOL_F;
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6net_quit ()
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (lw6_global.net_initialized)
    {
      lw6net_quit ();
    }
  lw6_global.net_initialized = 0;

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

/*
 * In liquidwar6cli
 */

/*
 * In backend.c
 */
static SCM
_scm_lw6cli_get_backends ()
{
  SCM ret = SCM_BOOL_F;
  lw6sys_assoc_t *backends;
  lw6sys_list_t *keys;
  lw6sys_list_t *key;
  char *module_id;
  char *module_name;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  backends = lw6cli_get_backends (lw6_global.argc, lw6_global.argv);
  if (backends)
    {
      keys = lw6sys_assoc_keys (backends);
      if (keys)
	{
	  ret = SCM_EOL;
	  key = keys;
	  while (key)
	    {
	      if (key->data)
		{
		  module_id = (char *) key->data;
		  module_name =
		    (char *) lw6sys_assoc_get (backends, module_id);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string (module_id),
			       scm_from_locale_string (module_name)), ret);
		}
	      key = lw6sys_list_next (key);
	    }
	  lw6sys_list_free (keys);
	  ret = scm_reverse (ret);
	}
      lw6sys_assoc_free (backends);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In liquidwar6srv
 */

/*
 * In backend.c
 */
static SCM
_scm_lw6srv_get_backends ()
{
  SCM ret = SCM_BOOL_F;
  lw6sys_assoc_t *backends;
  lw6sys_list_t *keys;
  lw6sys_list_t *key;
  char *module_id;
  char *module_name;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  backends = lw6srv_get_backends (lw6_global.argc, lw6_global.argv);
  if (backends)
    {
      keys = lw6sys_assoc_keys (backends);
      if (keys)
	{
	  ret = SCM_EOL;
	  key = keys;
	  while (key)
	    {
	      if (key->data)
		{
		  module_id = (char *) key->data;
		  module_name =
		    (char *) lw6sys_assoc_get (backends, module_id);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string (module_id),
			       scm_from_locale_string (module_name)), ret);
		}
	      key = lw6sys_list_next (key);
	    }
	  lw6sys_list_free (keys);
	  ret = scm_reverse (ret);
	}
      lw6sys_assoc_free (backends);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In liquidwar6p2p
 */
static SCM
_scm_lw6p2p_db_new (SCM name)
{
  SCM ret = SCM_BOOL_F;
  char *c_name = NULL;
  lw6p2p_db_t *c_db;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (name), name, SCM_ARG1, __FUNCTION__);

  if (lw6_global.net_initialized)
    {
      c_name = to_0str (name);
      if (c_name)
	{
	  c_db = lw6p2p_db_open (lw6_global.argc, lw6_global.argv, c_name);
	  if (c_db)
	    {
	      ret = lw6_make_scm_db (c_db);
	    }
	  LW6SYS_FREE (c_name);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6p2p_db_reset (SCM name)
{
  SCM ret = SCM_BOOL_F;
  char *c_name = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (name), name, SCM_ARG1, __FUNCTION__);

  c_name = to_0str (name);
  if (c_name)
    {
      ret =
	lw6p2p_db_reset (lw6_global.argc, lw6_global.argv,
			 c_name) ? SCM_BOOL_T : SCM_BOOL_F;

      LW6SYS_FREE (c_name);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6p2p_db_default_name ()
{
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  ret = scm_from_locale_string (lw6p2p_db_default_name ());

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6p2p_node_new (SCM db, SCM param)
{
  lw6p2p_node_t *c_node;
  SCM ret = SCM_BOOL_F;
  SCM client_backends = SCM_BOOL_F;
  SCM server_backends = SCM_BOOL_F;
  SCM bind_ip = SCM_BOOL_F;
  SCM bind_port = SCM_BOOL_F;
  SCM broadcast = SCM_BOOL_F;
  SCM public_url = SCM_BOOL_F;
  SCM password = SCM_BOOL_F;
  SCM title = SCM_BOOL_F;
  SCM description = SCM_BOOL_F;
  SCM bench = SCM_BOOL_F;
  SCM open_relay = SCM_BOOL_F;
  SCM known_nodes = SCM_BOOL_F;
  SCM network_reliability = SCM_BOOL_F;
  SCM trojan = SCM_BOOL_F;
  lw6p2p_db_t *c_db;
  char *c_client_backends;
  char *c_server_backends;
  char *c_bind_ip;
  int c_bind_port;
  int c_broadcast;
  char *c_public_url;
  char *c_password;
  char *c_title;
  char *c_description;
  int c_open_relay;
  int c_bench;
  char *c_known_nodes;
  int c_network_reliability;
  int c_trojan;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  if (lw6_global.net_initialized)
    {
      client_backends =
	scm_assoc_ref (param, scm_from_locale_string ("client-backends"));
      server_backends =
	scm_assoc_ref (param, scm_from_locale_string ("server-backends"));
      bind_ip = scm_assoc_ref (param, scm_from_locale_string ("bind-ip"));
      bind_port = scm_assoc_ref (param, scm_from_locale_string ("bind-port"));
      broadcast = scm_assoc_ref (param, scm_from_locale_string ("broadcast"));
      public_url =
	scm_assoc_ref (param, scm_from_locale_string ("public-url"));
      password = scm_assoc_ref (param, scm_from_locale_string ("password"));
      title = scm_assoc_ref (param, scm_from_locale_string ("title"));
      description =
	scm_assoc_ref (param, scm_from_locale_string ("description"));
      bench = scm_assoc_ref (param, scm_from_locale_string ("bench"));
      open_relay =
	scm_assoc_ref (param, scm_from_locale_string ("open-relay"));
      known_nodes =
	scm_assoc_ref (param, scm_from_locale_string ("known-nodes"));
      network_reliability =
	scm_assoc_ref (param, scm_from_locale_string ("network-reliability"));
      trojan = scm_assoc_ref (param, scm_from_locale_string ("trojan"));

      SCM_ASSERT (SCM_SMOB_PREDICATE
		  (lw6_global.smob_types.db, db), db, SCM_ARG1, __FUNCTION__);
      SCM_ASSERT (SCM_CONSP (param)
		  || param == SCM_EOL, param, SCM_ARG2, __FUNCTION__);

      SCM_ASSERT (scm_is_string (client_backends), param, SCM_ARG2,
		  __FUNCTION__);
      SCM_ASSERT (scm_is_string (server_backends), param, SCM_ARG2,
		  __FUNCTION__);
      SCM_ASSERT (scm_is_string (bind_ip), param, SCM_ARG2, __FUNCTION__);
      SCM_ASSERT (scm_is_integer (bind_port), param, SCM_ARG2, __FUNCTION__);
      SCM_ASSERT (SCM_BOOLP (broadcast), param, SCM_ARG2, __FUNCTION__);
      SCM_ASSERT (scm_is_string (public_url), param, SCM_ARG2, __FUNCTION__);
      SCM_ASSERT (scm_is_string (password), param, SCM_ARG2, __FUNCTION__);
      SCM_ASSERT (scm_is_string (title), param, SCM_ARG2, __FUNCTION__);
      SCM_ASSERT (scm_is_string (description), param, SCM_ARG2, __FUNCTION__);
      SCM_ASSERT (scm_is_integer (bench), param, SCM_ARG2, __FUNCTION__);
      SCM_ASSERT (SCM_BOOLP (open_relay), param, SCM_ARG2, __FUNCTION__);
      SCM_ASSERT (scm_is_string (known_nodes), param, SCM_ARG2, __FUNCTION__);
      SCM_ASSERT (scm_is_integer (network_reliability), param, SCM_ARG2,
		  __FUNCTION__);
      SCM_ASSERT (SCM_BOOLP (trojan), param, SCM_ARG2, __FUNCTION__);

      c_db = lw6_scm_to_db (db);
      if (c_db)
	{
	  c_client_backends = to_0str (client_backends);
	  if (c_client_backends)
	    {
	      c_server_backends = to_0str (server_backends);
	      if (c_server_backends)
		{
		  c_bind_ip = to_0str (bind_ip);
		  if (c_bind_ip)
		    {
		      c_bind_port = scm_to_int (bind_port);
		      c_broadcast = SCM_NFALSEP (broadcast);
		      c_public_url = to_0str (public_url);
		      if (c_public_url)
			{
			  c_title = to_0str (title);
			  if (c_title)
			    {
			      c_description = to_0str (description);
			      if (c_description)
				{
				  c_password = to_0str (password);
				  if (c_password)
				    {
				      c_bench = scm_to_int (bench);
				      c_open_relay = SCM_NFALSEP (open_relay);
				      c_known_nodes = to_0str (known_nodes);
				      if (c_known_nodes)
					{
					  c_network_reliability =
					    scm_to_int (network_reliability);
					  c_trojan = SCM_NFALSEP (trojan);

					  c_node =
					    lw6p2p_node_new
					    (lw6_global.argc,
					     lw6_global.argv, c_db,
					     c_client_backends,
					     c_server_backends, c_bind_ip,
					     c_bind_port, c_broadcast,
					     c_public_url,
					     c_title,
					     c_description, c_password,
					     c_bench, c_open_relay,
					     c_known_nodes,
					     c_network_reliability, c_trojan);
					  if (c_node)
					    {
					      ret =
						lw6_make_scm_node (c_node,
								   db);
					    }
					  LW6SYS_FREE (c_known_nodes);
					}
				      LW6SYS_FREE (c_password);
				    }
				  LW6SYS_FREE (c_description);
				}
			      LW6SYS_FREE (c_title);
			    }
			  LW6SYS_FREE (c_public_url);
			}
		      LW6SYS_FREE (c_bind_ip);
		    }
		  LW6SYS_FREE (c_server_backends);
		}
	      LW6SYS_FREE (c_client_backends);
	    }
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6p2p_node_poll (SCM node)
{
  lw6p2p_node_t *c_node;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.node,
	       node), node, SCM_ARG1, __FUNCTION__);

  c_node = lw6_scm_to_node (node);
  if (c_node)
    {
      if (lw6p2p_node_poll (c_node))
	{
	  ret = SCM_BOOL_T;
	}
    }
  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6p2p_node_close (SCM node)
{
  lw6p2p_node_t *c_node;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.node,
	       node), node, SCM_ARG1, __FUNCTION__);

  c_node = lw6_scm_to_node (node);
  if (c_node)
    {
      lw6p2p_node_close (c_node);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6p2p_node_get_id (SCM node)
{
  lw6p2p_node_t *c_node;
  SCM ret = SCM_BOOL_F;
  u_int64_t c_ret_int = 0;
  char *c_ret_str = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.node,
	       node), node, SCM_ARG1, __FUNCTION__);

  c_node = lw6_scm_to_node (node);
  if (c_node)
    {
      c_ret_int = lw6p2p_node_get_id (c_node);
      c_ret_str = lw6sys_id_ltoa (c_ret_int);
      if (c_ret_str)
	{
	  ret = scm_from_locale_string (c_ret_str);
	  LW6SYS_FREE (c_ret_str);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static void
_lw6p2p_node_get_entries_callback (void *func_data, void *data)
{
  SCM *ret = (SCM *) func_data;
  lw6p2p_entry_t *entry = (lw6p2p_entry_t *) data;
  SCM item;

  item = SCM_EOL;

  // constant data
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("creation-timestamp"),
	       scm_from_int (entry->creation_timestamp)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("version"),
	       scm_from_locale_string (entry->version)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("codename"),
	       scm_from_locale_string (entry->codename)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("stamp"), scm_from_int (entry->stamp)),
	      item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("id"),
	       scm_from_locale_string (entry->id)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("url"),
	       scm_from_locale_string (entry->url)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("title"),
	       scm_from_locale_string (entry->title)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("description"),
	       scm_from_locale_string (entry->description)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("has-password"),
	       entry->has_password ? SCM_BOOL_T : SCM_BOOL_F), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("bench"), scm_from_int (entry->bench)),
	      item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("open-relay"),
	       entry->open_relay ? SCM_BOOL_T : SCM_BOOL_F), item);
  // variable data
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("community-id"),
	       scm_from_locale_string (entry->community_id)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("round"), scm_from_int (entry->round)),
	      item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("level"),
	       scm_from_locale_string (entry->level)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("required-bench"),
	       scm_from_int (entry->required_bench)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("nb-colors"),
	       scm_from_int (entry->nb_colors)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("max-nb-colors"),
	       scm_from_int (entry->max_nb_colors)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("nb-cursors"),
	       scm_from_int (entry->nb_cursors)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("max-nb-cursors"),
	       scm_from_int (entry->max_nb_cursors)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("nb-nodes"),
	       scm_from_int (entry->nb_nodes)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("max-nb-nodes"),
	       scm_from_int (entry->max_nb_nodes)), item);
  // additionnal data
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("ip"),
	       scm_from_locale_string (entry->ip)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("port"), scm_from_int (entry->port)),
	      item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("last-ping-timestamp"),
	       scm_from_int (entry->last_ping_timestamp)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("ping-delay-msec"),
	       scm_from_int (entry->ping_delay_msec)), item);
  item =
    scm_cons (scm_cons
	      (scm_from_locale_string ("available"),
	       entry->available ? SCM_BOOL_T : SCM_BOOL_F), item);

  item = scm_reverse (item);

  (*ret) = scm_cons (item, *ret);
}

static SCM
_scm_lw6p2p_node_get_entries (SCM node)
{
  lw6p2p_node_t *c_node;
  SCM ret = SCM_BOOL_F;
  lw6sys_list_t *c_ret = NULL;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.node,
	       node), node, SCM_ARG1, __FUNCTION__);

  c_node = lw6_scm_to_node (node);
  if (c_node)
    {
      c_ret = lw6p2p_node_get_entries (c_node);
      if (c_ret)
	{
	  ret = SCM_EOL;
	  lw6sys_list_map (c_ret, _lw6p2p_node_get_entries_callback,
			   (void *) &ret);
	  lw6sys_list_free (c_ret);
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * In liquidwar6bot
 */

/*
 * In backend.c
 */
static SCM
_scm_lw6bot_get_backends ()
{
  SCM ret = SCM_BOOL_F;
  lw6sys_assoc_t *backends;
  lw6sys_list_t *keys;
  lw6sys_list_t *key;
  char *module_id;
  char *module_name;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  backends = lw6bot_get_backends (lw6_global.argc, lw6_global.argv);
  if (backends)
    {
      keys = lw6sys_assoc_keys (backends);
      if (keys)
	{
	  ret = SCM_EOL;
	  key = keys;
	  while (key)
	    {
	      if (key->data)
		{
		  module_id = (char *) key->data;
		  module_name =
		    (char *) lw6sys_assoc_get (backends, module_id);
		  ret =
		    scm_cons (scm_cons
			      (scm_from_locale_string (module_id),
			       scm_from_locale_string (module_name)), ret);
		}
	      key = lw6sys_list_next (key);
	    }
	  lw6sys_list_free (keys);
	  ret = scm_reverse (ret);
	}
      lw6sys_assoc_free (backends);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6bot_new (SCM backend_name, SCM game_state, SCM pilot, SCM dirty_read,
		 SCM cursor_id, SCM speed, SCM iq)
{
  lw6ker_game_state_t *c_game_state;
  lw6pil_pilot_t *c_pilot;
  int c_dirty_read;
  char *c_backend_name;
  char *c_cursor_id_str = NULL;
  u_int16_t c_cursor_id_int;
  float c_speed;
  int c_iq;
  lw6bot_backend_t *c_ret;
  lw6bot_seed_t c_seed;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (scm_is_string (backend_name), backend_name, SCM_ARG1,
	      __FUNCTION__);
  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.game_state,
	       game_state), game_state, SCM_ARG2, __FUNCTION__);
  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.pilot,
	       pilot), pilot, SCM_ARG3, __FUNCTION__);
  SCM_ASSERT (scm_is_string (cursor_id), cursor_id, SCM_ARG5, __FUNCTION__);
  SCM_ASSERT (scm_is_number (speed), speed, SCM_ARG6, __FUNCTION__);
  SCM_ASSERT (scm_is_integer (iq), iq, SCM_ARG7, __FUNCTION__);

  c_backend_name = to_0str (backend_name);
  if (backend_name)
    {
      c_game_state = lw6_scm_to_game_state (game_state);
      if (c_game_state)
	{
	  c_pilot = lw6_scm_to_pilot (pilot);
	  if (c_pilot)
	    {
	      c_dirty_read = scm_to_int (dirty_read);
	      c_cursor_id_str = to_0str (cursor_id);
	      if (c_cursor_id_str)
		{
		  c_cursor_id_int = lw6sys_id_atol (c_cursor_id_str);
		  c_speed = scm_to_double (speed);
		  c_iq = scm_to_int (iq);
		  c_ret =
		    lw6bot_create_backend (lw6_global.argc, lw6_global.argv,
					   c_backend_name);
		  if (c_ret)
		    {
		      memset (&c_seed, 0, sizeof (lw6bot_seed_t));
		      c_seed.game_state = c_game_state;
		      c_seed.pilot = c_pilot;
		      c_seed.dirty_read = c_dirty_read;
		      c_seed.param.cursor_id = c_cursor_id_int;
		      c_seed.param.speed = c_speed;
		      c_seed.param.iq = c_iq;
		      if (lw6bot_init (c_ret, &c_seed))
			{
			  ret = lw6_make_scm_bot (c_ret, game_state, pilot);
			}
		      else
			{
			  lw6bot_destroy_backend (c_ret);
			}
		    }
		  LW6SYS_FREE (c_cursor_id_str);
		}
	    }
	}
      LW6SYS_FREE (c_backend_name);
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

static SCM
_scm_lw6bot_next_move (SCM bot)
{
  lw6bot_backend_t *c_bot;
  int c_x = 0;
  int c_y = 0;
  SCM ret = SCM_BOOL_F;

  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  SCM_ASSERT (SCM_SMOB_PREDICATE
	      (lw6_global.smob_types.bot, bot), bot, SCM_ARG1, __FUNCTION__);

  c_bot = lw6_scm_to_bot (bot);
  if (c_bot)
    {
      if (lw6bot_next_move (c_bot, &c_x, &c_y))
	{
	  ret = scm_list_2 (scm_cons
			    (scm_from_locale_string ("x"),
			     scm_from_int (c_x)),
			    scm_cons (scm_from_locale_string ("y"),
				      scm_from_int (c_y)));
	}
    }

  LW6SYS_SCRIPT_FUNCTION_END;

  return ret;
}

/*
 * Global control functions
 */
static SCM
_scm_lw6_release ()
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  lw6_release ();

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

static SCM
_scm_lw6_exit ()
{
  LW6SYS_SCRIPT_FUNCTION_BEGIN;
  lw6scm_coverage_call (lw6_global.coverage, __FUNCTION__);

  lw6_exit ();

  LW6SYS_SCRIPT_FUNCTION_END;

  return SCM_UNDEFINED;
}

/**
 * lw6_register_funcs
 *
 * Register all the functions, make them callable from Guile.
 * This is a very simple yet long and very usefull function,
 * without it Guile has no knowledge of what LW6 is.
 *
 * Return value: 1 on success, 0 if failed.
 */
int
lw6_register_funcs ()
{
  int ret = 1;

  lw6scm_c_define_gsubr ("C-GETTEXT", 1, 0, 0, (SCM (*)())_scm_gettext);
  /*
   * This one uses scm_define_gsubr and not the binding
   * lw6scm_c_define_gsubr for _ is not documented due
   * to internal parsing limitations.
   */
  scm_c_define_gsubr ("_", 1, 0, 0, (SCM (*)())_scm_gettext);

  /*
   * In liquidwar6sys
   */
  /*
   * In bazooka.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_SET_MEMORY_BAZOOKA_SIZE, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_set_memory_bazooka_size);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_MEMORY_BAZOOKA_SIZE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_memory_bazooka_size);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_SET_MEMORY_BAZOOKA_ERASER, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_set_memory_bazooka_eraser);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_MEMORY_BAZOOKA_ERASER, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_memory_bazooka_eraser);
  /*
   * In build.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_PACKAGE_TARNAME, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_package_tarname);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_PACKAGE_NAME, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_package_name);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_PACKAGE_STRING, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_package_string);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_PACKAGE_ID, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_package_id);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_VERSION, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_version);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_CODENAME, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_codename);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_STAMP, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_stamp);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_MD5SUM, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_md5sum);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_COPYRIGHT, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_copyright);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_LICENSE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_license);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_HOME_URL, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_home_url);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_BUGS_URL, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_bugs_url);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_CONFIGURE_ARGS, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_configure_args);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_GCC_VERSION, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_gcc_version);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_CFLAGS, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_cflags);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_LDFLAGS, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_ldflags);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_HOSTNAME, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_hostname);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_DATE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_date);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_TIME, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_time);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_HOST_CPU, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_host_cpu);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENDIANNESS, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_endianness);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_POINTER_SIZE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_pointer_size);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_IS_X86, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_is_x86);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_HOST_OS, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_host_os);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_IS_GNU, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_is_gnu);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_IS_UNIX, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_is_unix);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_IS_MS_WINDOWS, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_is_ms_windows);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_IS_MAC_OS_X, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_is_mac_os_x);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_IS_GP2X, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_is_gp2x);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_TOP_SRCDIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_top_srcdir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_PREFIX, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_prefix);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_DATADIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_datadir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_LIBDIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_libdir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_INCLUDEDIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_includedir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_LOCALEDIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_localedir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_DOCDIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_docdir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_CONSOLE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_console);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_GTK, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_gtk);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_MOD_GL, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_mod_gl);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_MOD_CSOUND, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_mod_csound);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_MOD_OGG, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_mod_ogg);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_MOD_HTTP, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_mod_http);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_OPENMP, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_openmp);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_OPTIMIZE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_optimize);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_ALLINONE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_allinone);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_FULLSTATIC, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_fullstatic);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_PARANOID, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_paranoid);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_GPROF, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_gprof);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_INSTRUMENT, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_instrument);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_PROFILER, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_profiler);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_GCOV, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_gcov);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_ENABLE_VALGRIND, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_enable_valgrind);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_BUILD_GET_BIN_ID, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_build_get_bin_id);
  /*
   * In debug.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_DEBUG_GET, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_debug_get);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_DEBUG_SET, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_debug_set);
  /*
   * In dump.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_DUMP, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_dump);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_DUMP_CLEAR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_dump_clear);
  /*
   * In env.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_USERNAME, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_username);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_HOSTNAME, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_hostname);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GETENV, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_getenv);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GETENV_PREFIXED, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_getenv_prefixed);

  /*
   * In id.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GENERATE_ID_16, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_generate_id_16);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GENERATE_ID_32, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_generate_id_32);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GENERATE_ID_64, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_generate_id_64);

  /*
   * In log.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_LOG, 2, 0, 0,
			 (SCM (*)())_scm_lw6sys_log);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_LOG_GET_LEVEL, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_log_get_level);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_LOG_SET_LEVEL, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_log_set_level);
  /*
   * in mem.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_MEGABYTES_AVAILABLE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_megabytes_available);
  /*
   * in openmp.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_OPENMP_GET_NUM_PROCS, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_openmp_get_num_procs);
  /*
   * In options.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DEFAULT_USER_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_default_user_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DEFAULT_CONFIG_FILE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_default_config_file);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DEFAULT_LOG_FILE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_default_log_file);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DEFAULT_PREFIX, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_default_prefix);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DEFAULT_MOD_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_default_mod_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DEFAULT_DATA_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_default_data_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DEFAULT_MUSIC_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_default_music_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DEFAULT_MUSIC_PATH, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_default_music_path);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DEFAULT_MAP_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_default_map_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DEFAULT_MAP_PATH, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_default_map_path);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DEFAULT_SCRIPT_FILE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_default_script_file);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_CWD, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_cwd);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_RUN_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_run_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_USER_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_user_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_CONFIG_FILE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_config_file);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_LOG_FILE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_log_file);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_PREFIX, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_prefix);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_MOD_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_mod_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_DATA_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_data_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_MUSIC_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_music_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_MUSIC_PATH, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_music_path);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_MAP_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_map_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_MAP_PATH, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_map_path);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_SCRIPT_FILE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_get_script_file);
  /*
   * In path.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_PATH_CONCAT, 2, 0, 0,
			 (SCM (*)())_scm_lw6sys_path_concat);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_PATH_FILE_ONLY, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_path_file_only);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_PATH_PARENT, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_path_parent);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_PATH_SPLIT, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_path_split);
  /*
   * In signal.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_SIGNAL_CUSTOM,
			 1, 0, 0, (SCM (*)())_scm_lw6sys_signal_custom);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_SIGNAL_DEFAULT,
			 0, 0, 0, (SCM (*)())_scm_lw6sys_signal_default);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_SIGNAL_SEND_QUIT,
			 0, 0, 0, (SCM (*)())_scm_lw6sys_signal_send_quit);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_SIGNAL_POLL_QUIT,
			 0, 0, 0, (SCM (*)())_scm_lw6sys_signal_poll_quit);
  /*
   * In time.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_TIMESTAMP,
			 0, 0, 0, (SCM (*)())_scm_lw6sys_get_timestamp);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_UPTIME,
			 0, 0, 0, (SCM (*)())_scm_lw6sys_get_uptime);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_GET_CYCLE,
			 0, 0, 0, (SCM (*)())_scm_lw6sys_get_cycle);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_SLEEP, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_sleep);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_DELAY, 1, 0, 0,
			 (SCM (*)())_scm_lw6sys_delay);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_IDLE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_idle);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_SNOOZE, 0, 0, 0,
			 (SCM (*)())_scm_lw6sys_snooze);
  /*
   * In url.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SYS_URL_CANONIZE,
			 1, 0, 0, (SCM (*)())_scm_lw6sys_url_canonize);

  /*
   * In liquidwar6hlp
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_ABOUT, 1, 0, 0,
			 (SCM (*)())_scm_lw6hlp_about);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_GET_DEFAULT_VALUE, 1, 0, 0,
			 (SCM (*)())_scm_lw6hlp_get_default_value);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_QUICK,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_quick);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_DOC,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_doc);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_SHOW,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_show);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_PATH,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_path);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_PLAYERS,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_players);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_INPUT,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_input);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_GRAPHICS,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_graphics);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_SOUND,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_sound);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_NETWORK,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_network);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_MAP,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_map);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_MAP_RULES,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_map_rules);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_MAP_HINTS,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_map_hints);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_MAP_STYLE,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_map_style);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_MAP_TEAMS,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_map_teams);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_FUNCS,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_funcs);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_HOOKS,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_hooks);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_ADVANCED,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_advanced);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_ALIASES,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_aliases);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_TEAM_COLORS,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_team_colors);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST_WEAPONS,
			 0, 0, 0, (SCM (*)())_scm_lw6hlp_list_weapons);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6HLP_LIST, 0, 0, 0,
			 (SCM (*)())_scm_lw6hlp_list);

  /*
   * In liquidwar6cfg
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_DEFAULTS, 0, 0, 0,
			 (SCM (*)())_scm_lw6cfg_defaults);
  /*
   * In load.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_LOAD, 1, 0, 0,
			 (SCM (*)())_scm_lw6cfg_load);
  /*
   * In option.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_OPTION_EXISTS,
			 1, 0, 0, (SCM (*)())_scm_lw6cfg_option_exists);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_GET_OPTION, 1, 0, 0,
			 (SCM (*)())_scm_lw6cfg_get_option);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_SET_OPTION, 2, 0, 0,
			 (SCM (*)())_scm_lw6cfg_set_option);
  /*
   * In save.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_SAVE, 1, 0, 0,
			 (SCM (*)())_scm_lw6cfg_save);
  /*
   * In setup.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_INIT, 0, 0, 0,
			 (SCM (*)())_scm_lw6cfg_init);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_QUIT, 0, 0, 0,
			 (SCM (*)())_scm_lw6cfg_quit);
  /*
   * In unified.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_UNIFIED_GET_USER_DIR, 0, 0, 0,
			 (SCM (*)())_scm_lw6cfg_unified_get_user_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_UNIFIED_GET_LOG_FILE, 0, 0, 0,
			 (SCM (*)())_scm_lw6cfg_unified_get_log_file);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_UNIFIED_GET_MUSIC_PATH, 0, 0, 0,
			 (SCM (*)())_scm_lw6cfg_unified_get_music_path);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CFG_UNIFIED_GET_MAP_PATH, 0, 0, 0,
			 (SCM (*)())_scm_lw6cfg_unified_get_map_path);

  /*
   * In liquidwar6gui
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MENU_NEW, 5, 0, 0,
			 (SCM (*)())_scm_lw6gui_menu_new);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MENU_APPEND, 2, 0, 0,
			 (SCM (*)())_scm_lw6gui_menu_append);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MENU_SYNC, 2, 0, 0,
			 (SCM (*)())_scm_lw6gui_menu_sync);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MENU_SELECT, 3, 0, 0,
			 (SCM (*)())_scm_lw6gui_menu_select);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MENU_SELECT_ESC, 2, 0, 0,
			 (SCM (*)())_scm_lw6gui_menu_select_esc);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MENU_ENABLE_ESC, 2, 0, 0,
			 (SCM (*)())_scm_lw6gui_menu_enable_esc);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MENU_SCROLL_UP, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_menu_scroll_up);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MENU_SCROLL_DOWN, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_menu_scroll_down);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MENU_SET_BREADCRUMBS, 2, 0, 0,
			 (SCM (*)())_scm_lw6gui_menu_set_breadcrumbs);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MENU_CLOSE_POPUP, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_menu_close_popup);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MENU_HAS_POPUP, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_menu_has_popup);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_DEFAULT_LOOK,
			 0, 0, 0, (SCM (*)())_scm_lw6gui_default_look);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_LOOK_SET,
			 3, 0, 0, (SCM (*)())_scm_lw6gui_look_set);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_LOOK_GET,
			 2, 0, 0, (SCM (*)())_scm_lw6gui_look_get);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_LOOK_ZOOM_IN,
			 2, 0, 0, (SCM (*)())_scm_lw6gui_look_zoom_in);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_LOOK_ZOOM_OUT,
			 2, 0, 0, (SCM (*)())_scm_lw6gui_look_zoom_out);

  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_INPUT_RESET, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_input_reset);

  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MOUSE_POLL_MOVE, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_mouse_poll_move);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MOUSE_GET_STATE, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_mouse_get_state);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_IS_PRESSED, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_is_pressed);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_GET_MOVE_PAD, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_get_move_pad);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK1_GET_MOVE_PAD, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick1_get_move_pad);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK2_GET_MOVE_PAD, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick2_get_move_pad);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MOUSE_POP_BUTTON_LEFT, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_mouse_pop_button_left);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MOUSE_POP_BUTTON_RIGHT, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_mouse_pop_button_right);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MOUSE_POP_BUTTON_MIDDLE, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_mouse_pop_button_middle);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MOUSE_POP_SIMPLE_CLICK, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_mouse_pop_simple_click);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MOUSE_POP_DOUBLE_CLICK, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_mouse_pop_double_click);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MOUSE_POP_TRIPLE_CLICK, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_mouse_pop_triple_click);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MOUSE_POP_WHEEL_UP, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_mouse_pop_wheel_up);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_MOUSE_POP_WHEEL_DOWN, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_mouse_pop_wheel_down);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_POP_ARROW_UP, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_pop_arrow_up);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_POP_ARROW_DOWN, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_pop_arrow_down);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_POP_ARROW_LEFT, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_pop_arrow_left);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_POP_ARROW_RIGHT, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_pop_arrow_right);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_POP_KEY_ENTER, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_pop_key_enter);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_POP_KEY_ESC, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_pop_key_esc);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_POP_KEY_CTRL, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_pop_key_ctrl);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_POP_KEY_ALT, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_pop_key_alt);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_POP_KEY_PGUP, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_pop_key_pgup);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_KEYBOARD_POP_KEY_PGDOWN, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_keyboard_pop_key_pgdown);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK1_POP_PAD_UP, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick1_pop_pad_up);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK1_POP_PAD_DOWN, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick1_pop_pad_down);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK1_POP_PAD_LEFT, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick1_pop_pad_left);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK1_POP_PAD_RIGHT, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick1_pop_pad_right);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK1_POP_BUTTON_A, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick1_pop_button_a);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK1_POP_BUTTON_B, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick1_pop_button_b);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK1_POP_BUTTON_C, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick1_pop_button_c);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK1_POP_BUTTON_D, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick1_pop_button_d);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK1_POP_BUTTON_E, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick1_pop_button_e);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK1_POP_BUTTON_F, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick1_pop_button_f);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK2_POP_PAD_UP, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick2_pop_pad_up);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK2_POP_PAD_DOWN, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick2_pop_pad_down);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK2_POP_PAD_LEFT, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick2_pop_pad_left);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK2_POP_PAD_RIGHT, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick2_pop_pad_right);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK2_POP_BUTTON_A, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick2_pop_button_a);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK2_POP_BUTTON_B, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick2_pop_button_b);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK2_POP_BUTTON_C, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick2_pop_button_c);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK2_POP_BUTTON_D, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick2_pop_button_d);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK2_POP_BUTTON_E, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick2_pop_button_e);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GUI_JOYSTICK2_POP_BUTTON_F, 1, 0, 0,
			 (SCM (*)())_scm_lw6gui_joystick2_pop_button_f);

  /*
   * In liquidwar6gfx
   */
  /*
   * In backend.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6GFX_GET_BACKENDS, 0, 0, 0,
			 (SCM (*)())_scm_lw6gfx_get_backends);

  /*
   * In liquidwar6dsp
   */
  /*
   * In display.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6DSP_NEW, 2, 0, 0,
			 (SCM (*)())_scm_lw6dsp_new);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6DSP_RELEASE, 1, 0, 0,
			 (SCM (*)())_scm_lw6dsp_release);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6DSP_UPDATE, 2, 0, 0,
			 (SCM (*)())_scm_lw6dsp_update);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6DSP_GET_NB_FRAMES, 1, 0, 0,
			 (SCM (*)())_scm_lw6dsp_get_nb_frames);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6DSP_GET_LAST_FRAME_RENDERING_TIME, 1, 0,
			 0,
			 (SCM (*)
			  ())_scm_lw6dsp_get_last_frame_rendering_time);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6DSP_GET_INSTANT_FPS, 1, 0, 0,
			 (SCM (*)())_scm_lw6dsp_get_instant_fps);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6DSP_GET_AVERAGE_FPS, 1, 0, 0,
			 (SCM (*)())_scm_lw6dsp_get_average_fps);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6DSP_GET_VIDEO_MODE, 1, 0, 0,
			 (SCM (*)())_scm_lw6dsp_get_video_mode);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6DSP_GET_FULLSCREEN_MODES, 1, 0, 0,
			 (SCM (*)())_scm_lw6dsp_get_fullscreen_modes);

  /*
   * In liquidwar6ldr
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6LDR_GET_ENTRIES, 2, 0, 0,
			 (SCM (*)())_scm_lw6ldr_get_entries);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6LDR_READ, 7, 0, 0,
			 (SCM (*)())_scm_lw6ldr_read);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6LDR_READ_RELATIVE, 8, 0, 0,
			 (SCM (*)())_scm_lw6ldr_read_relative);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6LDR_PRINT_EXAMPLES, 0, 0, 0,
			 (SCM (*)())_scm_lw6ldr_print_examples);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6LDR_HINTS_GET_DEFAULT, 1, 0, 0,
			 (SCM (*)())_scm_lw6ldr_hints_get_default);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6LDR_EXP_VALIDATE, 1, 0, 0,
			 (SCM (*)())_scm_lw6ldr_exp_validate);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6LDR_CHAIN_ENTRY, 2, 0, 0,
			 (SCM (*)())_scm_lw6ldr_chain_entry);

  /*
   * In liquidwar6map
   */
  lw6scm_c_define_gsubr
    (LW6DEF_C_LW6MAP_GET_LOOK, 1, 0, 0, (SCM (*)())_scm_lw6map_get_look);
  lw6scm_c_define_gsubr
    (LW6DEF_C_LW6MAP_PARAM_GET, 2, 0, 0, (SCM (*)())_scm_lw6map_param_get);
  lw6scm_c_define_gsubr
    (LW6DEF_C_LW6MAP_GET_MUSIC_DIR, 1, 0, 0,
     (SCM (*)())_scm_lw6map_get_music_dir);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_TEAM_COLOR_INDEX_TO_KEY, 1, 0, 0,
			 (SCM (*)())_scm_lw6map_team_color_index_to_key);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_TEAM_COLOR_KEY_TO_INDEX, 1, 0, 0,
			 (SCM (*)())_scm_lw6map_team_color_key_to_index);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_TEAM_COLOR_INDEX_TO_LABEL, 1, 0, 0,
			 (SCM (*)())_scm_lw6map_team_color_index_to_label);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_TEAM_COLOR_LIST, 0, 0, 0,
			 (SCM (*)())_scm_lw6map_team_color_list);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_WEAPON_INDEX_TO_KEY, 1, 0, 0,
			 (SCM (*)())_scm_lw6map_weapon_index_to_key);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_WEAPON_KEY_TO_INDEX, 1, 0, 0,
			 (SCM (*)())_scm_lw6map_weapon_key_to_index);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_WEAPON_INDEX_TO_LABEL, 1, 0, 0,
			 (SCM (*)())_scm_lw6map_weapon_index_to_label);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_WEAPON_LIST, 0, 0, 0,
			 (SCM (*)())_scm_lw6map_weapon_list);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_RULES_GET_DEFAULT, 1, 0, 0,
			 (SCM (*)())_scm_lw6map_rules_get_default);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_RULES_GET_MIN, 1, 0, 0,
			 (SCM (*)())_scm_lw6map_rules_get_min);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_RULES_GET_MAX, 1, 0, 0,
			 (SCM (*)())_scm_lw6map_rules_get_max);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_RULES_GET_INT, 2, 0, 0,
			 (SCM (*)())_scm_lw6map_rules_get_int);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_STYLE_GET_DEFAULT, 1, 0, 0,
			 (SCM (*)())_scm_lw6map_style_get_default);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_TEAMS_GET_DEFAULT, 1, 0, 0,
			 (SCM (*)())_scm_lw6map_teams_get_default);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_EXP_IS_TEAM_COLOR_ALLOWED, 2, 0, 0,
			 (SCM (*)())_scm_lw6map_exp_is_team_color_allowed);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_EXP_IS_WEAPON_ALLOWED, 2, 0, 0,
			 (SCM (*)())_scm_lw6map_exp_is_weapon_allowed);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_EXP_GET_UNLOCKED_TEAM_COLOR, 0, 0, 0,
			 (SCM (*)())_scm_lw6map_exp_get_unlocked_team_color);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6MAP_EXP_GET_UNLOCKED_WEAPON, 0, 0, 0,
			 (SCM (*)())_scm_lw6map_exp_get_unlocked_weapon);

  /*
   * In liquidwar6ker
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_BUILD_GAME_STRUCT,
			 1, 0, 0, (SCM (*)())_scm_lw6ker_build_game_struct);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_BUILD_GAME_STATE, 1,
			 0, 0, (SCM (*)())_scm_lw6ker_build_game_state);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_SYNC_GAME_STATE, 2,
			 0, 0, (SCM (*)())_scm_lw6ker_sync_game_state);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_DUP_GAME_STATE, 2,
			 0, 0, (SCM (*)())_scm_lw6ker_dup_game_state);
  lw6scm_c_define_gsubr
    (LW6DEF_C_LW6KER_GAME_STRUCT_CHECKSUM, 1, 0, 0,
     (SCM (*)())_scm_lw6ker_game_struct_checksum);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_GAME_STATE_CHECKSUM,
			 1, 0, 0, (SCM (*)())_scm_lw6ker_game_state_checksum);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_REGISTER_NODE,
			 2, 0, 0, (SCM (*)())_scm_lw6ker_register_node);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_UNREGISTER_NODE,
			 2, 0, 0, (SCM (*)())_scm_lw6ker_unregister_node);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_NODE_EXISTS,
			 2, 0, 0, (SCM (*)())_scm_lw6ker_node_exists);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_ADD_CURSOR,
			 4, 0, 0, (SCM (*)())_scm_lw6ker_add_cursor);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_REMOVE_CURSOR,
			 3, 0, 0, (SCM (*)())_scm_lw6ker_remove_cursor);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_CURSOR_EXISTS,
			 2, 0, 0, (SCM (*)())_scm_lw6ker_cursor_exists);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_GET_CURSOR,
			 2, 0, 0, (SCM (*)())_scm_lw6ker_get_cursor);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_SET_CURSOR,
			 5, 0, 0, (SCM (*)())_scm_lw6ker_set_cursor);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_DO_ROUND,
			 1, 0, 0, (SCM (*)())_scm_lw6ker_do_round);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_GET_MOVES, 1, 0, 0,
			 (SCM (*)())_scm_lw6ker_get_moves);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_GET_SPREADS, 1, 0, 0,
			 (SCM (*)())_scm_lw6ker_get_spreads);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_GET_ROUNDS, 1, 0, 0,
			 (SCM (*)())_scm_lw6ker_get_rounds);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_IS_OVER, 1, 0, 0,
			 (SCM (*)())_scm_lw6ker_is_over);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6KER_DID_CURSOR_WIN,
			 5, 0, 0, (SCM (*)())_scm_lw6ker_did_cursor_win);

  /*
   * In liquidwar6pil
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_BENCH,
			 0, 0, 0, (SCM (*)())_scm_lw6pil_bench);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_BUILD_PILOT,
			 3, 0, 0, (SCM (*)())_scm_lw6pil_build_pilot);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_SEND_COMMAND,
			 3, 0, 0, (SCM (*)())_scm_lw6pil_send_command);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_LOCAL_COMMAND,
			 2, 0, 0, (SCM (*)())_scm_lw6pil_local_command);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_COMMIT,
			 1, 0, 0, (SCM (*)())_scm_lw6pil_commit);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_FIX_COORDS,
			 4, 0, 0, (SCM (*)())_scm_lw6pil_fix_coords);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_FIX_COORDS_X10,
			 4, 0, 0, (SCM (*)())_scm_lw6pil_fix_coords_x10);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_EXECUTE_COMMAND,
			 3, 0, 0, (SCM (*)())_scm_lw6pil_execute_command);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_LOCAL_CURSORS_SET_MAIN,
			 2, 0, 0,
			 (SCM (*)())_scm_lw6pil_local_cursors_set_main);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_LOCAL_CURSORS_SET_MOUSE_CONTROLLED,
			 3, 0, 0,
			 (SCM (*)
			  ())_scm_lw6pil_local_cursors_set_mouse_controlled);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_MAKE_BACKUP, 1, 0, 0,
			 (SCM (*)())_scm_lw6pil_make_backup);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_SYNC_FROM_BACKUP, 2, 0, 0,
			 (SCM (*)())_scm_lw6pil_sync_from_backup);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_SYNC_FROM_REFERENCE, 2, 0, 0,
			 (SCM (*)())_scm_lw6pil_sync_from_reference);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_SYNC_FROM_DRAFT, 3, 0, 0,
			 (SCM (*)())_scm_lw6pil_sync_from_draft);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_CALIBRATE, 3, 0, 0,
			 (SCM (*)())_scm_lw6pil_calibrate);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_SPEED_UP, 2, 0, 0,
			 (SCM (*)())_scm_lw6pil_speed_up);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_SLOW_DOWN, 2, 0, 0,
			 (SCM (*)())_scm_lw6pil_slow_down);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_GET_NEXT_SEQ, 2, 0, 0,
			 (SCM (*)())_scm_lw6pil_get_next_seq);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_GET_LAST_COMMIT_SEQ, 1, 0, 0,
			 (SCM (*)())_scm_lw6pil_get_last_commit_seq);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_GET_REFERENCE_CURRENT_SEQ, 1, 0, 0,
			 (SCM (*)())_scm_lw6pil_get_reference_current_seq);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_GET_REFERENCE_TARGET_SEQ, 1, 0, 0,
			 (SCM (*)())_scm_lw6pil_get_reference_target_seq);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_GET_MAX_SEQ, 1, 0, 0,
			 (SCM (*)())_scm_lw6pil_get_max_seq);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_IS_OVER, 1, 0, 0,
			 (SCM (*)())_scm_lw6pil_is_over);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_DID_CURSOR_WIN, 2, 0, 0,
			 (SCM (*)())_scm_lw6pil_did_cursor_win);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_GET_WINNER, 1, 0, 0,
			 (SCM (*)())_scm_lw6pil_get_winner);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_GET_LOOSER, 1, 0, 0,
			 (SCM (*)())_scm_lw6pil_get_looser);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6PIL_SEQ_RANDOM_0, 0, 0, 0,
			 (SCM (*)())_scm_lw6pil_seq_random_0);

  /*
   * In liquidwar6snd
   */
  /*
   * In backend.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_GET_BACKENDS, 0, 0, 0,
			 (SCM (*)())_scm_lw6snd_get_backends);
  /*
   * In setup.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_NEW, 4, 0, 0,
			 (SCM (*)())_scm_lw6snd_new);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_POLL, 1, 0, 0,
			 (SCM (*)())_scm_lw6snd_poll);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_RELEASE, 1, 0, 0,
			 (SCM (*)())_scm_lw6snd_release);

  /*
   * In fx.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_PLAY_FX, 2,
			 0, 0, (SCM (*)())_scm_lw6snd_play_fx);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_SET_FX_VOLUME, 2,
			 0, 0, (SCM (*)())_scm_lw6snd_set_fx_volume);

  /*
   * In water.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_SET_WATER_VOLUME, 2,
			 0, 0, (SCM (*)())_scm_lw6snd_set_water_volume);

  /*
   * In music.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_IS_MUSIC_FILE, 4,
			 0, 0, (SCM (*)())_scm_lw6snd_is_music_file);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_PLAY_MUSIC_FILE, 4,
			 0, 0, (SCM (*)())_scm_lw6snd_play_music_file);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_PLAY_MUSIC_RANDOM, 4,
			 0, 0, (SCM (*)())_scm_lw6snd_play_music_random);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_STOP_MUSIC, 1,
			 0, 0, (SCM (*)())_scm_lw6snd_stop_music);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SND_SET_MUSIC_VOLUME, 2,
			 0, 0, (SCM (*)())_scm_lw6snd_set_music_volume);

  /*
   * In liquidwar6con
   */
  /*
   * In support.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CNS_SUPPORT, 0, 0, 0,
			 (SCM (*)())_scm_lw6cns_support);
  /*
   * In handler.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CNS_INIT, 0, 0, 0,
			 (SCM (*)())_scm_lw6cns_init);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CNS_QUIT, 0, 0, 0,
			 (SCM (*)())_scm_lw6cns_quit);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CNS_POLL, 0, 0, 0,
			 (SCM (*)())_scm_lw6cns_poll);

  /*
   * In liquidwar6tsk
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6TSK_LOADER_NEW, 1, 0, 0,
			 (SCM (*)())_scm_lw6tsk_loader_new);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6TSK_LOADER_PUSH, 9, 0, 0,
			 (SCM (*)())_scm_lw6tsk_loader_push);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6TSK_LOADER_POP, 1, 0, 0,
			 (SCM (*)())_scm_lw6tsk_loader_pop);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6TSK_LOADER_GET_STAGE, 1, 0, 0,
			 (SCM (*)())_scm_lw6tsk_loader_get_stage);

  /*
   * In liquidwar6net
   */
  /*
   * In setup.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6NET_INIT, 1, 0, 0,
			 (SCM (*)())_scm_lw6net_init);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6NET_QUIT, 0, 0, 0,
			 (SCM (*)())_scm_lw6net_quit);

  /*
   * In liquidwar6cli
   */
  /*
   * In backend.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6CLI_GET_BACKENDS, 0, 0, 0,
			 (SCM (*)())_scm_lw6cli_get_backends);

  /*
   * In liquidwar6srv
   */
  /*
   * In backend.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6SRV_GET_BACKENDS, 0, 0, 0,
			 (SCM (*)())_scm_lw6srv_get_backends);

  /*
   * In liquidwar6p2p
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6P2P_DB_NEW, 1, 0, 0,
			 (SCM (*)())_scm_lw6p2p_db_new);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6P2P_DB_RESET, 1, 0, 0,
			 (SCM (*)())_scm_lw6p2p_db_reset);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6P2P_DB_DEFAULT_NAME, 0, 0, 0,
			 (SCM (*)())_scm_lw6p2p_db_default_name);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6P2P_NODE_NEW, 2, 0, 0,
			 (SCM (*)())_scm_lw6p2p_node_new);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6P2P_NODE_POLL, 1, 0, 0,
			 (SCM (*)())_scm_lw6p2p_node_poll);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6P2P_NODE_CLOSE, 1, 0, 0,
			 (SCM (*)())_scm_lw6p2p_node_close);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6P2P_NODE_GET_ID, 1, 0, 0,
			 (SCM (*)())_scm_lw6p2p_node_get_id);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6P2P_NODE_GET_ENTRIES, 1, 0, 0,
			 (SCM (*)())_scm_lw6p2p_node_get_entries);

  /*
   * In liquidwar6bot
   */
  /*
   * In backend.c
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6BOT_GET_BACKENDS, 0, 0, 0,
			 (SCM (*)())_scm_lw6bot_get_backends);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6BOT_NEW, 7, 0, 0,
			 (SCM (*)())_scm_lw6bot_new);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6BOT_NEXT_MOVE, 1, 0, 0,
			 (SCM (*)())_scm_lw6bot_next_move);

  /*
   * Global control funcs
   */
  lw6scm_c_define_gsubr (LW6DEF_C_LW6_RELEASE, 0, 0, 0,
			 (SCM (*)())_scm_lw6_release);
  lw6scm_c_define_gsubr (LW6DEF_C_LW6_EXIT, 0, 0, 0,
			 (SCM (*)())_scm_lw6_exit);

  return ret;
}

/**
 * lw6_cns_handler
 *
 * c_line: the line typed by the user
 *
 * This function will be called every time a message
 * is typed on the console. It runs the given line in
 * the current Guile environment.
 *
 * Return value: none
 */
extern void
lw6_cns_handler (char *c_line)
{
  SCM line;
  SCM func_symbol;
  SCM func;

  if (c_line)
    {
      lw6sys_log (LW6SYS_LOG_INFO, _x_ ("interpreting console input \"%s\""),
		  c_line);

      lw6cns_history_add_if_needed (c_line);
      line = scm_from_locale_string (c_line);

      func_symbol = scm_c_lookup ("lw6-console-try-catch");
      func = scm_variable_ref (func_symbol);
      scm_call_1 (func, line);
      free (c_line);		// exceptionnally, don't use LW6SYS_FREE
    }
  else
    {
      printf (_x_ ("(quit) or CTRL_C to quit"));
      printf ("\n");
    }
}
