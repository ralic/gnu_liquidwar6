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

#include <string.h>

#include "ldr.h"
#include "ldr-internal.h"

typedef struct style_update_data_s
{
  lw6map_style_t *style;
  lw6sys_assoc_t *values;
}
style_update_data_t;

static void
read_callback (void *callback_data, char *element, char *key, char *value)
{
  lw6map_style_t *style_data;

  style_data = (lw6map_style_t *) callback_data;

  lw6map_style_set (style_data, key, value);
}

/**
 * lw6ldr_style_read
 *
 * @param: the style struct to fill with values (read/write parameter)
 * @dirname: the directory of the map
 *
 * Read the style (style.xml) of a map. Pointer to style must be valid,
 * and values already initialized, either zeroed or filled in with defaults
 * or custom values.
 *
 * Return value: 1 if success, 0 if failed.
 */
int
lw6ldr_style_read (lw6map_style_t * style, char *dirname)
{
  int ret = 0;
  char *buf = NULL;

  buf = lw6sys_path_concat (dirname, _LW6LDR_FILE_STYLE_XML);

  if (buf)
    {
      if (lw6sys_file_exists (buf))
	{
	  lw6sys_log (LW6SYS_LOG_INFO, _x_ ("reading style \"%s\""), buf);
	  ret =
	    lw6cfg_read_key_value_xml_file (buf, read_callback,
					    (void *) style);
	}
      else
	{
	  // using defaults
	  ret = 1;
	}

      LW6SYS_FREE (buf);
    }

  if (!ret)
    {
      lw6sys_log (LW6SYS_LOG_WARNING, _x_ ("unable to read map style"));
    }

  return ret;
}

/**
 * lw6ldr_style_set
 *
 * @style: the style to modify
 * @key: the key to modify
 * @value: the value to affect to the key, as a string
 *
 * Sets one single parameter in a style structure. Value must
 * always be passed as a string, will be converted to the right
 * type automatically when storing it in the structure.
 *
 * Return value: 1 if success, 0 if failed. Note that while 0 really
 * means there's a problem, some affectations can fail and return 1,
 * needs to be worked on.
 */
int
lw6ldr_style_set (lw6map_style_t * style, char *key, char *value)
{
  int ret = 0;
  char *about = NULL;
  char *element = NULL;
  lw6hlp_type_t type = LW6HLP_TYPE_VOID;

  about = lw6hlp_about (&type, NULL, NULL, NULL, key);
  element = lw6cfg_xml_element (type);
  if (about && element)
    {
      ret = 1;
      read_callback (style, element, key, value);
    }

  return ret;
}

static void
style_update_callback (void *func_data, void *data)
{
  style_update_data_t *update_data;
  char *key;
  char *value;

  key = (char *) data;
  update_data = (style_update_data_t *) func_data;

  if (lw6sys_assoc_has_key (update_data->values, key))
    {
      value = lw6sys_assoc_get (update_data->values, key);
      lw6ldr_style_set (update_data->style, key, value);
    }
}

/**
 * lw6ldr_style_update
 *
 * @style: the style struct to fill with values (read/write parameter)
 * @values: an assoc containing strings with the new values
 *
 * Overrides style with values. Pointer to style must be valid,
 * and values already initialized, either zeroed or filled in with defaults
 * or custom values. Not all parameters need be defined in values. It can
 * even be NULL. The idea is just that if something is defined in values,
 * it will override the existing style.
 *
 * Return value: 1 if success, 0 if failed.
 */
int
lw6ldr_style_update (lw6map_style_t * style, lw6sys_assoc_t * values)
{
  int ret = 0;
  lw6sys_list_t *list;
  style_update_data_t data;

  if (values)
    {
      list = lw6hlp_list_map_style ();
      if (list)
	{
	  data.style = style;
	  data.values = values;
	  lw6sys_list_map (list, style_update_callback, &data);
	  lw6sys_list_free (list);
	  ret = 1;
	}
    }
  else
    {
      // NULL values is allowed
      ret = 1;
    }

  return ret;
}
