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

#ifndef LIQUIDWAR6BOT_H
#define LIQUIDWAR6BOT_H

#include "../sys/sys.h"
#include "../map/map.h"
#include "../ker/ker.h"
#include "../pil/pil.h"
#include "../dyn/dyn.h"

/**
 * Parameters usable by a bot engine. Those are the
 * stable, fixed parameters passed at bot creation, they
 * don't change during the bot life.
 */
typedef struct lw6bot_param_s
{
  /**
   * Speed of the bot, this is a value between 0.0f and 1.0f,
   * 1 means normal speed, 0 is as slow as possible. Values
   * over 1 will make the bot act/move faster than usual.
   */
  float speed;
  /**
   * IQ is supposed to reflect the cleverness of the bot.
   * The default is 100 (this value is basically a percentage),
   * 0 means just so stupid, and a high value, for instance
   * 200, means very clever. 
   */
  int iq;
  /**
   * The cursor ID, which is a 16-bit non-null integer.
   */
  u_int16_t cursor_id;
} lw6bot_param_t;

typedef struct lw6bot_data_s
{
  lw6ker_game_state_t *game_state;
  lw6bot_param_t param;
} lw6bot_data_t;

typedef struct lw6bot_seed_s
{
  lw6ker_game_state_t *game_state;
  lw6pil_pilot_t *pilot;
  int dirty_read;
  lw6bot_param_t param;
} lw6bot_seed_t;

typedef struct lw6bot_backend_s
{
  lw6dyn_dl_handle_t *dl_handle;
  void *bot_context;
  int argc;
  const char **argv;
  u_int32_t id;
  lw6bot_seed_t seed;

  void *(*init) (int argc, const char *argv[], lw6bot_data_t * data);
  void (*quit) (void *bot_context);
  int (*next_move) (void *bot_context, int *x, int *y, lw6bot_data_t * data);
  char *(*repr) (void *bot_context, u_int32_t id);
}
lw6bot_backend_t;

/* bot-api.c */
extern int lw6bot_init (lw6bot_backend_t * backend, lw6bot_seed_t * seed);
extern void lw6bot_quit (lw6bot_backend_t * backend);
extern int lw6bot_next_move (lw6bot_backend_t * backend, int *x, int *y);
extern char *lw6bot_repr (lw6bot_backend_t * backend);

/* bot-register.c */
extern lw6sys_assoc_t *lw6bot_get_backends (int argc, const char *argv[]);
extern lw6bot_backend_t *lw6bot_create_backend (int argc, const char *argv[],
						const char *name);
extern void lw6bot_destroy_backend (lw6bot_backend_t * backend);

/* bot-test.c */
extern int lw6bot_test (int mode);

#endif
