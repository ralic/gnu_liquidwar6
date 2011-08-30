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

#include "gui.h"

/**
 * lw6gui_mouse_register_move
 *
 * @mouse: the mouse object to work on
 * @x: the x position
 * @y: the y position
 * @timestamp: current timestamp
 *
 * Registers a mouse move event.
 *
 * Return value: note. 
 */
void
lw6gui_mouse_register_move (lw6gui_mouse_t * mouse, int x, int y,
			    int64_t timestamp)
{
  if (mouse->x != x || mouse->y != y)
    {
      mouse->x = x;
      mouse->y = y;
      mouse->moved = 1;
      mouse->last_moved = timestamp;
    }
}

/**
 * lw6gui_mouse_poll_move
 *
 * @mouse: the mouse object to poll
 * @x: pointer to the x position (can be NULL), will be updated even if no move
 * @y: pointer to the y position (can be NULL), will be updated even if no move
 *
 * Asks wether the mouse has moved or not.
 *
 * Return value: 1 if mouse was moved since last call, 0 if not. 
 */
int
lw6gui_mouse_poll_move (lw6gui_mouse_t * mouse, int *x, int *y)
{
  int ret = 0;

  if (mouse->moved)
    {
      mouse->moved = 0;
      ret = 1;
    }

  if (x)
    {
      (*x) = mouse->x;
    }
  if (y)
    {
      (*y) = mouse->y;
    }

  return ret;
}

/**
 * lw6gui_mouse_update_repeat
 *
 * @mouse: the mouse to update
 * @repeat_settings: the repeat settings (delay + interval)
 * @timestamp: the current ticks (milliseconds)
 *
 * Updates the repeat informations for a mouse, must be called
 * regularly, as often as possible.
 *
 * Return value: none.
 */
void
lw6gui_mouse_update_repeat (lw6gui_mouse_t * mouse,
			    lw6gui_repeat_settings_t * repeat_settings,
			    int64_t timestamp)
{
  lw6gui_button_update_repeat (&(mouse->button_left), repeat_settings,
			       timestamp);
  lw6gui_button_update_repeat (&(mouse->button_right), repeat_settings,
			       timestamp);
  lw6gui_button_update_repeat (&(mouse->button_middle), repeat_settings,
			       timestamp);
  lw6gui_button_update_repeat (&(mouse->wheel_up), repeat_settings,
			       timestamp);
  lw6gui_button_update_repeat (&(mouse->wheel_down), repeat_settings,
			       timestamp);
}

/**
 * lw6gui_mouse_sync
 *
 * @dst: the target mouse object
 * @src: the source mouse object
 *
 * Synchronizes two mouse objects. This is typically used to pass data from
 * one thread to another. Will handle "mouse move" attribute and clear it
 * in src if needed while setting it in dst.
 *
 * Return value: 1 if success, O if failure.
 */
int
lw6gui_mouse_sync (lw6gui_mouse_t * dst, lw6gui_mouse_t * src)
{
  int ret = 1;

  dst->x = src->x;
  dst->y = src->y;
  dst->moved = dst->moved || src->moved;
  src->moved = 0;		// yes, src is cleared
  dst->last_moved = src->last_moved;
  dst->map_x = src->map_x;
  dst->map_y = src->map_y;
  dst->menu_position = src->menu_position;
  dst->menu_scroll = src->menu_scroll;
  dst->menu_esc = src->menu_esc;

  ret = lw6gui_button_sync (&(dst->button_left), &(src->button_left)) &&
    lw6gui_button_sync (&(dst->button_right), &(src->button_right)) &&
    lw6gui_button_sync (&(dst->button_middle), &(src->button_middle)) &&
    lw6gui_button_sync (&(dst->wheel_up), &(src->wheel_up)) &&
    lw6gui_button_sync (&(dst->wheel_down), &(src->wheel_down));

  return ret;
}
