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

#ifndef LIQUIDWAR6GFX_MOD_GL1_MENU_CYLINDER_INTERNAL_H
#define LIQUIDWAR6GFX_MOD_GL1_MENU_CYLINDER_INTERNAL_H

#include "gl1-cylinder.h"

typedef struct _mod_gl1_menu_cylinder_const_data_s
{
  int max_displayed_items;
  float radius1;
  float between1;
  float cyl_height;
  int slices1;
  int slices_min;
  int stacks;
  float oscil_range1;
  int oscil_period;
  float texture_offset;
  int nb_spheres;
  float sphere_radius1;
  float sphere_between1;
  int sphere_slices1;
  int sphere_slices_min;
  int sphere_blink_period;
  float progress_offset;
  float progress_radius;
  float progress_cyl_height;
  float esc_offset;
  float esc_radius;
  float esc_cyl_height;
  float esc_cyl_height_offset;
  float esc_rotate;
  float tooltip_alpha_bg;
  int tooltip_max_width;
  int tooltip_max_height;
  int tooltip_border_size;
  int tooltip_margin_size;
  int tooltip_reformat_width;
  float tooltip_relative_size;
  float help_alpha_bg;
  int help_max_width;
  int help_max_height;
  int help_border_size;
  int help_margin_size;
  int help_reformat_width;
  float help_relative_size;
  float breadcrumbs_alpha_bg;
  int breadcrumbs_max_width;
  int breadcrumbs_max_height;
  int breadcrumbs_border_size;
  int breadcrumbs_margin_size;
  int breadcrumbs_reformat_width;
  float breadcrumbs_relative_size;
  float popup_alpha_bg;
  int popup_max_width;
  int popup_max_height;
  int popup_border_size;
  int popup_margin_size;
  int popup_reformat_width;
  float popup_relative_size;
}
_mod_gl1_menu_cylinder_const_data_t;

typedef struct _mod_gl1_menu_cylinder_context_s
{
  int i_right_point;
  int j_tooltip;
  _mod_gl1_menu_cylinder_const_data_t const_data;
}
_mod_gl1_menu_cylinder_context_t;

/*
 * In center.c
 */
extern void
_mod_gl1_menu_cylinder_center_on_menuitem (mod_gl1_utils_context_t *
					   utils_context,
					   _mod_gl1_menu_cylinder_context_t
					   * cylinder_context,
					   lw6gui_look_t * look,
					   lw6gui_menu_t * menu,
					   int position);
/*
 * In display.c
 */
extern void _mod_gl1_menu_cylinder_display_menu (mod_gl1_utils_context_t *
						 utils_context,
						 _mod_gl1_menu_cylinder_context_t
						 * cylinder_context,
						 lw6gui_look_t * look,
						 lw6gui_menu_t * menu);
extern void _mod_gl1_menu_cylinder_display_progress (mod_gl1_utils_context_t *
						     utils_context,
						     _mod_gl1_menu_cylinder_context_t
						     * cylinder_context,
						     lw6gui_look_t * look,
						     float progress);
extern void _mod_gl1_menu_cylinder_display_meta (mod_gl1_utils_context_t *
						 utils_context,
						 _mod_gl1_menu_cylinder_context_t
						 * cylinder_context,
						 lw6gui_look_t * look,
						 lw6gui_menu_t * menu);

/*
 * In data.c
 */
extern int _mod_gl1_menu_cylinder_load_data (mod_gl1_utils_context_t *
					     utils_context,
					     _mod_gl1_menu_cylinder_context_t
					     * cylinder_context);
extern void _mod_gl1_menu_cylinder_unload_data (mod_gl1_utils_context_t *
						utils_context,
						_mod_gl1_menu_cylinder_context_t
						* cylinder_context);

/*
 * In draw.c
 */
extern float _mod_gl1_menu_cylinder_get_cylinder_y (mod_gl1_utils_context_t *
						    utils_context,
						    _mod_gl1_menu_cylinder_context_t
						    * cylinder_context, int i,
						    int n);
extern void _mod_gl1_menu_cylinder_draw_cylinder (mod_gl1_utils_context_t *
						  utils_context,
						  _mod_gl1_menu_cylinder_context_t
						  * cylinder_context,
						  GLenum mode, int i, int n,
						  float relative_text_width);
extern void
_mod_gl1_menu_cylinder_draw_cylinder_corners (mod_gl1_utils_context_t *
					      utils_context,
					      _mod_gl1_menu_cylinder_context_t
					      * cylinder_context, int i,
					      int n,
					      float relative_text_width,
					      int pass_through);
extern void _mod_gl1_menu_cylinder_draw_sphere (mod_gl1_utils_context_t *
						utils_context,
						_mod_gl1_menu_cylinder_context_t
						* cylinder_context,
						GLenum mode, int i, int n,
						int sphere_i, int nb_spheres);
extern void
_mod_gl1_menu_cylinder_draw_sphere_corners (mod_gl1_utils_context_t *
					    utils_context,
					    _mod_gl1_menu_cylinder_context_t *
					    cylinder_context, int i, int n,
					    int sphere_i, int nb_spheres,
					    int pass_through);
extern void
_mod_gl1_menu_cylinder_draw_fixed_cylinder (mod_gl1_utils_context_t *
					    utils_context,
					    _mod_gl1_menu_cylinder_context_t *
					    cylinder_context, GLenum mode,
					    float offset, float radius,
					    float cyl_height,
					    float cyl_height_offset,
					    float rotate);
extern void
_mod_gl1_menu_cylinder_draw_fixed_cylinder_corners (mod_gl1_utils_context_t *
						    utils_context,
						    _mod_gl1_menu_cylinder_context_t
						    * cylinder_context,
						    float offset,
						    float radius,
						    float cyl_height,
						    float cyl_height_offset,
						    float rotate,
						    float realtive_text_width,
						    int pass_through);

/*
 * In pick.c
 */
extern void
_mod_gl1_menu_cylinder_pick_item (mod_gl1_utils_context_t * utils_context,
				  _mod_gl1_menu_cylinder_context_t *
				  cylinder_context, lw6gui_look_t * look,
				  int *position, int *scroll, int *esc,
				  lw6gui_menu_t * menu, int screen_x,
				  int screen_y);
extern void
_mod_gl1_menu_cylinder_get_cylinder_right_point (mod_gl1_utils_context_t *
						 utils_context,
						 _mod_gl1_menu_cylinder_context_t
						 * cylinder_context, int i,
						 int n,
						 float relative_text_width,
						 float *right_point_x,
						 float *right_point_y);

/*
 * In progress.c
 */
extern void _mod_gl1_menu_cylinder_draw_progress (mod_gl1_utils_context_t *
						  utils_context,
						  _mod_gl1_menu_cylinder_context_t
						  * cylinder_context,
						  lw6gui_look_t * look,
						  float progress);

/*
 * In setup.c
 */
extern _mod_gl1_menu_cylinder_context_t
  * _mod_gl1_menu_cylinder_init (mod_gl1_utils_context_t * utils_context);
extern void _mod_gl1_menu_cylinder_quit (mod_gl1_utils_context_t *
					 utils_context,
					 _mod_gl1_menu_cylinder_context_t *
					 cylinder_context);

/*
 * In warp.c
 */
extern int
_mod_gl1_menu_cylinder_warp_mouse_on_menuitem (mod_gl1_utils_context_t *
					       utils_context,
					       _mod_gl1_menu_cylinder_context_t
					       * cylinder_context,
					       lw6gui_look_t * look,
					       lw6gui_menu_t * menu,
					       int position);

#endif