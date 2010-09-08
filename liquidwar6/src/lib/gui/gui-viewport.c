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

#include "gui.h"

/**
 * lw6gui_viewport_init
 *
 * @viewport: the viewport to initalize
 * @screen_w: screen width
 * @screen_h: screen height
 * @drawable_x1: viewport min x
 * @drawable_y1: viewport min y
 * @drawable_x2: viewport max x
 * @drawable_y2: viewport max y
 * @center_x: center of display (in map coordinates)
 * @center_y: center of display (in map coordinates)
 * @map_w: map width (shape)
 * @map_h: map height (shape)
 * @x_polarity: x polarity
 * @y_polarity: y polarity
 * @x_wrap: wether to wrap horizontally
 * @y_wrap: wether to wrap vertically
 * @keep_ratio: wether to adapt to viewport shape or keep original
 * @global_zoom: global zoom is style_zoom * dynamic_zoom
 *
 * Initializes all the (jumbo?) viewport structure which will
 * contain valuable informations for a simple "flat" display.
 * Special renderers might not find usefull some fields and
 * handle wrapping and zooming their own way, but this offers
 * a basic skeleton.
 *
 * Return value: 1 if ok, 0 on failure
 */
int
lw6gui_viewport_init (lw6gui_viewport_t * viewport, int screen_w,
		      int screen_h, float drawable_x1, float drawable_y1,
		      float drawable_x2, float drawable_y2, float center_x,
		      float center_y, int map_w, int map_h, int x_polarity,
		      int y_polarity, int x_wrap, int y_wrap, int keep_ratio,
		      float global_zoom)
{
  int ret = 0;
  float map_ratio = 1.0f;
  float drawable_ratio = 1.0f;
  float map_main_x = 0.0f;
  float map_main_y = 0.0f;
  float map_main_w = 0.0f;
  float map_main_h = 0.0f;
  float map_visible_x1 = 0.0f;
  float map_visible_y1 = 0.0f;
  float map_visible_x2 = 0.0f;
  float map_visible_y2 = 0.0f;

  if (viewport)
    {
      memset (viewport, 0, sizeof (lw6gui_viewport_t));

      viewport->map_shape.w = map_w;
      viewport->map_shape.h = map_h;
      viewport->center_x = center_x;
      viewport->center_y = center_y;
      viewport->x_polarity = x_polarity;
      viewport->y_polarity = y_polarity;
      viewport->x_wrap = x_wrap;
      viewport->y_wrap = y_wrap;
      viewport->screen_shape.w = screen_w;
      viewport->screen_shape.h = screen_h;

      lw6gui_zone_init_x1y1x2y2 (&(viewport->drawable), drawable_x1,
				 drawable_y1, drawable_x2, drawable_y2);

      if (viewport->map_shape.w > 0 && viewport->map_shape.h > 0
	  && viewport->screen_shape.w > 0 && viewport->screen_shape.h
	  && viewport->drawable.w > 0.0f && viewport->drawable.h > 0.0f)
	{
	  /*
	   * First, we choose the right w & h value (on the screen,
	   * in pixels) for the main map zone, neglecting clip and/or
	   * wrapping.
	   */
	  map_ratio =
	    ((float) viewport->map_shape.w) / ((float) viewport->map_shape.h);
	  drawable_ratio = viewport->drawable.w / viewport->drawable.h;

	  if (keep_ratio)
	    {
	      if (map_ratio > drawable_ratio)
		{
		  map_main_w = viewport->drawable.w;
		  map_main_h =
		    viewport->drawable.h * drawable_ratio / map_ratio;
		}
	      else
		{
		  map_main_w =
		    viewport->drawable.w * map_ratio / drawable_ratio;
		  map_main_h = viewport->drawable.h;
		}
	    }
	  else
	    {
	      map_main_w = viewport->drawable.w;
	      map_main_h = viewport->drawable.h;
	    }

	  map_main_w *= global_zoom;
	  map_main_h *= global_zoom;

	  /*
	   * Then, we center this zone on the screen.
	   */
	  map_main_x = viewport->drawable.x1;
	  map_main_y = viewport->drawable.y1;

	  if (map_main_w < viewport->drawable.w)
	    {
	      map_main_x =
		viewport->drawable.x1 + (viewport->drawable.w -
					 map_main_w) / 2.0f;
	    }
	  else if (map_main_w > viewport->drawable.w)
	    {
	      map_main_x =
		viewport->drawable.x1 + (viewport->drawable.w -
					 map_main_w) / 2.0f -
		(viewport->center_x -
		 viewport->map_shape.w / 2) * (map_main_w /
					       viewport->map_shape.w);
	      if (viewport->x_polarity == 0 || viewport->x_wrap == 0)
		{
		  if (map_main_x + map_main_w <
		      viewport->drawable.x1 + viewport->drawable.w)
		    {
		      lw6sys_log (LW6SYS_LOG_DEBUG, _x_ ("x correct 1 %0.0f"),
				  map_main_x);
		      map_main_x =
			viewport->drawable.x1 + viewport->drawable.w -
			map_main_w;
		    }
		  if (map_main_x > viewport->drawable.x1)
		    {
		      lw6sys_log (LW6SYS_LOG_DEBUG, _x_ ("x correct 2 %0.0f"),
				  map_main_x);
		      map_main_x = viewport->drawable.x1;
		    }
		}
	    }

	  if (map_main_h < viewport->drawable.h)
	    {
	      map_main_y =
		viewport->drawable.y1 + (viewport->drawable.h -
					 map_main_h) / 2.0f;
	    }
	  else if (map_main_h > viewport->drawable.h)
	    {
	      map_main_y =
		viewport->drawable.y1 + (viewport->drawable.h -
					 map_main_h) / 2.0f -
		(viewport->center_y -
		 viewport->map_shape.h / 2) * (map_main_h /
					       viewport->map_shape.h);
	      if (viewport->y_polarity == 0 || viewport->y_wrap == 0)
		{
		  if (map_main_y + map_main_h <
		      viewport->drawable.y1 + viewport->drawable.h)
		    {
		      lw6sys_log (LW6SYS_LOG_DEBUG, _x_ ("y correct 1 %0.0f"),
				  map_main_y);
		      map_main_y =
			viewport->drawable.y1 + viewport->drawable.h -
			map_main_h;
		    }
		  if (map_main_y > viewport->drawable.y1)
		    {
		      lw6sys_log (LW6SYS_LOG_DEBUG, _x_ ("y correct 2 %0.0f"),
				  map_main_y);
		      map_main_y = viewport->drawable.y1;
		    }
		}
	    }

	  lw6gui_zone_init_xywh (&(viewport->map_main), map_main_x,
				 map_main_y, map_main_w, map_main_h);
	  lw6gui_zone_clip (&(viewport->map_main_clipped),
			    &(viewport->map_main), &(viewport->drawable));

	  /*
	   * Last, we update the "visible" part, that is, what should
	   * be drawn including repetitions induced by polarity
	   */
	  map_visible_x1 = viewport->map_main_clipped.x1;
	  map_visible_y1 = viewport->map_main_clipped.y1;
	  map_visible_x2 = viewport->map_main_clipped.x2;
	  map_visible_y2 = viewport->map_main_clipped.y2;
	  if (viewport->x_polarity != 0 && viewport->x_wrap != 0)
	    {
	      map_visible_x1 = viewport->drawable.x1;
	      map_visible_x2 = viewport->drawable.x2;
	    }
	  if (viewport->y_polarity != 0 && viewport->y_wrap != 0)
	    {
	      map_visible_y1 = viewport->drawable.y1;
	      map_visible_y2 = viewport->drawable.y2;
	    }
	  lw6gui_zone_init_x1y1x2y2 (&(viewport->map_visible), map_visible_x1,
				     map_visible_y1, map_visible_x2,
				     map_visible_y2);

	  ret = 1;
	}
      else
	{
	  lw6sys_log (LW6SYS_LOG_INFO,
		      _x_
		      ("nothing is likely to be drawn, map_w=%d, map_h=%d, screen_w=%d, screen_h=%d, drawable_w=%f, drawable_h=%f"),
		      viewport->map_shape.w, viewport->map_shape.h,
		      viewport->screen_shape.w, viewport->screen_shape.h,
		      viewport->drawable.w, viewport->drawable.h);
	}
    }

  return ret;
}

/**
 * lw6gui_viewport_map_to_screen
 *
 * @viewport: the viewport to use
 * @screen_x: the x coord on the screen
 * @screen_y: the y coord on the screen
 * @map_x: the x coord in map coordinates
 * @map_y: the y coord in map coordinates
 * @clip: wether to clip returned values
 *
 * Translates from map coords to screen coords. Returned values might
 * be outside screen boundaries if clip is 0. If screen coords are outside
 * drawable area anc clip is 1, then they will be clipped.
 *
 * Return value: NULL
 */
void
lw6gui_viewport_map_to_screen (lw6gui_viewport_t * viewport, float *screen_x,
			       float *screen_y, float map_x, float map_y,
			       int clip)
{
  float x = 0.0f;
  float y = 0.0f;

  lw6gui_coord_calc_xy (&x, &y,
			viewport->map_main.x1, viewport->map_main.y1,
			viewport->map_main.w, viewport->map_main.h,
			map_x, map_y,
			0, 0, viewport->map_shape.w, viewport->map_shape.h);

  if (clip)
    {
      if (x < viewport->drawable.x1)
	{
	  x = viewport->drawable.x1;
	}
      if (x > viewport->drawable.x2)
	{
	  x = viewport->drawable.x2;
	}
      if (y < viewport->drawable.y1)
	{
	  y = viewport->drawable.y1;
	}
      if (y > viewport->drawable.y2)
	{
	  y = viewport->drawable.y2;
	}
    }

  *screen_x = x;
  *screen_y = y;
}

/**
 * lw6gui_viewport_screen_to_map
 *
 * @viewport: the viewport to use
 * @map_x: the x coord in map coordinates
 * @map_y: the y coord in map coordinates
 * @screen_x: the x coord on the screen
 * @screen_y: the y coord on the screen
 * @wrap: wether to use polarity informations to wrap coords.
 *
 * Translates from screen coords to map coords. If wrap is set, it will
 * interpret coords the way @lw6map_coords_fix_xy would, only it can still
 * be formally outside map boundaries for it can return a value exactly
 * equal to w,h while in interger mode it would be w-1,h-1.
 *
 * Return value: NULL
 */
void
lw6gui_viewport_screen_to_map (lw6gui_viewport_t * viewport, float *map_x,
			       float *map_y, float screen_x, float screen_y,
			       int wrap)
{
  float x = 0.0f;
  float y = 0.0f;
  float w = viewport->map_shape.w;
  float h = viewport->map_shape.h;

  lw6gui_coord_calc_xy (&x, &y,
			0, 0, viewport->map_shape.w, viewport->map_shape.h,
			screen_x, screen_y,
			viewport->map_main.x1, viewport->map_main.y1,
			viewport->map_main.w, viewport->map_main.h);

  if (wrap)
    {
      /*
       * The algorithm is copied/pasted from map code, only here it 
       * operates with floats, not integers.
       */
      if (x < 0.0f || y < 0.0f || x > w || y > h)
	{
	  if (viewport->x_polarity > 0 && viewport->x_wrap)
	    {
	      while (x > w)
		{
		  x -= w;
		}
	      while (x < 0)
		{
		  x += w;
		}
	    }
	  if (viewport->y_polarity > 0 && viewport->y_wrap)
	    {
	      while (y > h)
		{
		  y -= h;
		}
	      while (y < 0)
		{
		  y += h;
		}
	    }
	  if (viewport->x_polarity < 0 && viewport->x_wrap)
	    {
	      while (x > w)
		{
		  x -= w;
		  y = h - 1 - y;
		}
	      while (x < 0)
		{
		  x += w;
		  y = h - 1 - y;
		}
	    }
	  if (viewport->y_polarity < 0 && viewport->y_wrap)
	    {
	      while (y > h)
		{
		  x = w - 1 - x;
		  y -= h;
		}
	      while (y < 0)
		{
		  x = w - 1 - x;
		  y += h;
		}
	    }

	  /*
	   * Last, we check for absolute limits, this will also
	   * handle the 0 polarity case.
	   */
	  if (x < 0.0f)
	    {
	      x = 0.0f;
	    }
	  if (x > w)
	    {
	      x = w;
	    }
	  if (y < 0.0f)
	    {
	      y = 0.0f;
	    }
	  if (y > h)
	    {
	      y = h;
	    }
	}
    }

  *map_x = x;
  *map_y = y;
}
