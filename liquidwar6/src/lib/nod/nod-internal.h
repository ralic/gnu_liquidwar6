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

#ifndef LIQUIDWAR6NOD_INTERNAL_H
#define LIQUIDWAR6NOD_INTERNAL_H

#include "nod.h"

/* nod-constinfo.c */
extern int _lw6nod_const_info_init (lw6nod_const_info_t * const_info,
				    u_int64_t id, char *url, char *title,
				    char *description, int bench,
				    int idle_screenshot_size,
				    void *idle_screenshot_data);
extern void _lw6nod_const_info_reset (lw6nod_const_info_t * const_info);

/* nod-dyninfo.c */
extern void _lw6nod_dyn_info_reset (lw6nod_dyn_info_t * dyn_info);
extern int _lw6nod_dyn_info_update (lw6nod_dyn_info_t * dyn_info, char *level,
				    int required,
				    int limit,
				    int colors,
				    int nodes,
				    int cursors,
				    int game_screenshot_size,
				    void *game_screenshot_data);

#endif