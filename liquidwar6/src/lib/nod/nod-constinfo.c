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

#include "nod.h"
#include "nod-internal.h"

int
_lw6nod_const_info_init (lw6nod_const_info_t * const_info, u_int64_t id,
			 char *url, char *title, char *description, int bench,
			 int idle_screenshot_size, void *idle_screenshot_data)
{
  int ret = 0;

  const_info->creation_timestamp = lw6sys_get_timestamp ();
  const_info->id = lw6sys_id_ltoa (id);
  const_info->url = lw6sys_str_copy (url);
  if (title && strlen (title) > 0)
    {
      const_info->title = lw6sys_str_copy (title);
    }
  else
    {
      const_info->title = lw6sys_get_hostname ();
    }
  if (description)
    {
      const_info->description = lw6sys_str_copy (description);
    }
  else
    {
      const_info->description = lw6sys_str_copy ("");
    }
  const_info->bench = bench;
  const_info->idle_screenshot_size = idle_screenshot_size;
  const_info->idle_screenshot_data = LW6SYS_MALLOC (idle_screenshot_size);
  if (const_info->idle_screenshot_data)
    {
      memcpy (const_info->idle_screenshot_data, idle_screenshot_data,
	      idle_screenshot_size);
    }

  ret = (const_info->id && const_info->url
	 && const_info->title && const_info->description
	 && const_info->idle_screenshot_data);

  return ret;
}

void
_lw6nod_const_info_reset (lw6nod_const_info_t * const_info)
{
  if (const_info->id)
    {
      LW6SYS_FREE (const_info->id);
    }
  if (const_info->url)
    {
      LW6SYS_FREE (const_info->url);
    }
  if (const_info->title)
    {
      LW6SYS_FREE (const_info->title);
    }
  if (const_info->description)
    {
      LW6SYS_FREE (const_info->description);
    }
  if (const_info->idle_screenshot_data)
    {
      LW6SYS_FREE (const_info->idle_screenshot_data);
    }
}
