/*
  Liquid War 6 is a unique multiplayer wargame.
  Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010, 2011  Christian Mauduit <ufoot@ufoot.org>

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

#include "cfg.h"
#include "cfg-internal.h"
#include "../hlp/hlp.h"

#define _EXP_FILE "exp.xml"
#define _EXP_KEY "exp"
#define _CHECKSUM_KEY "checksum"

typedef struct _exp_s
{
  int exp;
  int checksum;
} _exp_t;

static char *
_get_filename (char *user_dir)
{
  return lw6sys_path_concat (user_dir, _EXP_FILE);
}

static u_int32_t
_calc_checksum (int exp)
{
  char *username = NULL;
  char *hostname = NULL;
  u_int32_t checksum = 0;

  username = lw6sys_get_username ();
  if (username)
    {
      hostname = lw6sys_get_hostname ();
      if (hostname)
	{
	  lw6sys_checksum_update_str (&checksum, username);
	  lw6sys_checksum_update_str (&checksum, hostname);
	  lw6sys_checksum_update_int32 (&checksum, exp);
	  LW6SYS_FREE (hostname);
	}
      LW6SYS_FREE (username);
    }

  return checksum;
}

static void
load_callback (void *callback_data, char *element, char *key, char *value)
{
  _exp_t *exp = (_exp_t *) callback_data;

  exp = (_exp_t *) callback_data;

  if (!strcmp (key, _EXP_KEY))
    {
      exp->exp = lw6sys_atoi (value);
    }
  else if (!strcmp (key, _CHECKSUM_KEY))
    {
      exp->checksum = lw6sys_atoi (value);
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_INFO, _x_ ("item \"%s\" ignored"), key);
    }
}

/**
 * lw6cfg_load_exp
 *
 * @user_dir: the user directory
 * @exp: the exp (out param)
 * 
 * Gets exp from file.
 *
 * Return value: 1 on success, 0 on failure
 */
int
lw6cfg_load_exp (char *user_dir, int *exp)
{
  int ret = 0;
  char *filename = NULL;
  int checksum = 0;
  _exp_t exp_t;

  exp_t.exp = LW6CFG_DEFAULT_EXP;
  exp_t.checksum = 0;

  filename = _get_filename (user_dir);
  if (filename)
    {
      lw6sys_log (LW6SYS_LOG_INFO, _x_ ("loading exp from \"%s\""), filename);

      if (lw6sys_file_exists (filename))
	{
	  ret =
	    lw6cfg_read_key_value_xml_file (filename, load_callback,
					    (void *) &exp_t);
	  checksum = _calc_checksum (exp_t.exp);
	  if (checksum != exp_t.checksum)
	    {
	      lw6sys_log (LW6SYS_LOG_INFO, _x_ ("bad exp checksum"));
	      exp_t.exp = LW6CFG_DEFAULT_EXP;
	    }
	}
      else
	{
	  lw6sys_log (LW6SYS_LOG_INFO,
		      _x_ ("exp file \"%s\" doesn't exist, using defaults"),
		      filename);
	}

      LW6SYS_FREE (filename);
    }

  exp_t.exp =
    lw6sys_min (LW6CFG_MAX_EXP, lw6sys_max (LW6CFG_MIN_EXP, exp_t.exp));
  (*exp) = exp_t.exp;

  return ret;
}

/**
 * lw6cfg_save_exp
 *
 * @user_dir: the user directory
 * @exp: the exp
 * 
 * Saves exp to file.
 *
 * Return value: 1 on success, 0 on failure
 */
int
lw6cfg_save_exp (char *user_dir, int exp)
{
  int ret = 0;
  FILE *f;
  char *filename = NULL;
  int checksum = 0;

  filename = _get_filename (user_dir);
  if (filename)
    {
      lw6sys_log (LW6SYS_LOG_INFO, _x_ ("saving exp to \"%s\""), filename);

      f = fopen (filename, "wb");
      if (f)
	{
	  lw6sys_print_xml_header (f,
				   _x_
				   ("This is where your exp is kept. Please do not edit, this would be assimilated to cheating, while it's not that hard to fool the game and make it believe you're super strong when you are not, such practice is not encouraged. It's believed it's more fun to wait until this number increases naturally."));

	  exp = lw6sys_min (LW6CFG_MAX_EXP, lw6sys_max (LW6CFG_MIN_EXP, exp));
	  checksum = _calc_checksum (exp);
	  fprintf (f, "<int key=\"%s\" value=\"%d\" />%s", _EXP_KEY, exp,
		   lw6sys_eol ());
	  fprintf (f, "<int key=\"%s\" value=\"%d\" />%s", _CHECKSUM_KEY,
		   checksum, lw6sys_eol ());
	  lw6sys_print_xml_footer (f);
	  fclose (f);
	  ret = 1;
	}
      else
	{
	  lw6sys_log (LW6SYS_LOG_WARNING,
		      _x_ ("could not open file \"%s\" in write mode"),
		      filename);
	}
      LW6SYS_FREE (filename);
    }

  return ret;
}