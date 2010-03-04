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

#include "../../cfg/cfg.h"
#include "mod-ogg-internal.h"

#define CONST_FILE "ogg-const.xml"

static void
read_callback (void *callback_data, char *element, char *key, char *value)
{
  _mod_ogg_const_data_t *const_data;

  const_data = (_mod_ogg_const_data_t *) callback_data;

  if (!strcmp (element, "int"))
    {
      lw6cfg_read_xml_int (key, value, "frequency", &const_data->frequency);
      lw6cfg_read_xml_int (key, value, "bits", &const_data->bits);
      lw6cfg_read_xml_int (key, value, "channels", &const_data->channels);
      lw6cfg_read_xml_int (key, value, "chunksize", &const_data->chunksize);
    }

  if (!strcmp (element, "float"))
    {
    }
}

/*
 * Loads constants.
 */
int
_mod_ogg_load_consts (_mod_ogg_context_t * context)
{
  int ret = 0;
  char *const_file = NULL;

  const_file = lw6sys_path_concat (context->path.data_dir, CONST_FILE);

  if (const_file)
    {
      lw6sys_log (LW6SYS_LOG_INFO, _("reading \"%s\""), const_file);

      ret =
	lw6cfg_read_key_value_xml_file (const_file, read_callback,
					(void *) &(context->const_data));

      LW6SYS_FREE (const_file);
    }

  return ret;
}

/*
 * Unload constants, free memory
 */
void
_mod_ogg_unload_consts (_mod_ogg_context_t * context)
{
  memset (&context->const_data, 0, sizeof (_mod_ogg_const_data_t));
}
