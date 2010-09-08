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

#include "../snd.h"
#include "mod-ogg-internal.h"

#define SOUND_DIR "sound"

int
_mod_ogg_play_sound (_mod_ogg_context_t * snd_context, int sound_id)
{
  int ret = 0;

  if (sound_id >= 0 && sound_id < LW6SND_NB_SOUNDS)
    {
      if (snd_context->sounds.sounds[sound_id])
	{
	  Mix_PlayChannel (-1, snd_context->sounds.sounds[sound_id], 0);
	}
      else
	{
	  lw6sys_log (LW6SYS_LOG_WARNING,
		      _x_
		      ("unable to play sound %d, was not correctly loaded"),
		      sound_id);
	}
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _x_ ("unable to play sound %d, index out of range"),
		  sound_id);
    }

  ret = 1;

  return ret;
}

static Mix_Chunk *
load_sound (_mod_ogg_context_t * ogg_context, char *file)
{
  char *path1 = NULL;
  char *path2 = NULL;
  Mix_Chunk *ret = NULL;

  if (file)
    {
      path1 = lw6sys_path_concat (SOUND_DIR, file);
      if (path1)
	{
	  path2 = lw6sys_path_concat (ogg_context->path.data_dir, path1);
	  if (path2)
	    {
	      lw6sys_log (LW6SYS_LOG_INFO, _x_ ("loading sound \"%s\""),
			  path2);
	      ret = Mix_LoadWAV (path2);
	      if (!ret)
		{
		  lw6sys_log (LW6SYS_LOG_WARNING,
			      _x_
			      ("unable to load sound \"%s\" Mix_GetError returned \"%s\""),
			      path2, Mix_GetError ());
		}
	      LW6SYS_FREE (path2);
	    }
	  LW6SYS_FREE (path1);
	}
    }

  return ret;
}

int
_mod_ogg_load_sounds (_mod_ogg_context_t * ogg_context)
{
  int ret = 0;
  int i;

  ogg_context->sounds.sounds[LW6SND_SOUND_SPLASH] =
    load_sound (ogg_context, ogg_context->const_data.file_splash);
  ogg_context->sounds.sounds[LW6SND_SOUND_VALID] =
    load_sound (ogg_context, ogg_context->const_data.file_valid);
  ogg_context->sounds.sounds[LW6SND_SOUND_SELECT] =
    load_sound (ogg_context, ogg_context->const_data.file_select);
  ogg_context->sounds.sounds[LW6SND_SOUND_START] =
    load_sound (ogg_context, ogg_context->const_data.file_start);
  ogg_context->sounds.sounds[LW6SND_SOUND_DEATH] =
    load_sound (ogg_context, ogg_context->const_data.file_death);
  ogg_context->sounds.sounds[LW6SND_SOUND_BELL] =
    load_sound (ogg_context, ogg_context->const_data.file_bell);
  ogg_context->sounds.sounds[LW6SND_SOUND_SCORE] =
    load_sound (ogg_context, ogg_context->const_data.file_score);
  ogg_context->sounds.sounds[LW6SND_SOUND_HELLO] =
    load_sound (ogg_context, ogg_context->const_data.file_hello);
  ogg_context->sounds.sounds[LW6SND_SOUND_GOODBYE] =
    load_sound (ogg_context, ogg_context->const_data.file_goodbye);

  ret = 1;
  for (i = 0; i < LW6SND_NB_SOUNDS; ++i)
    {
      if (!ogg_context->sounds.sounds[i])
	{
	  ret = 0;
	}
    }

  return ret;
}

void
_mod_ogg_unload_sounds (_mod_ogg_context_t * ogg_context)
{
  int i;

  for (i = 0; i < LW6SND_NB_SOUNDS; ++i)
    {
      if (ogg_context->sounds.sounds[i])
	{
	  Mix_FreeChunk (ogg_context->sounds.sounds[i]);
	}
      ogg_context->sounds.sounds[i] = NULL;
    }
}
