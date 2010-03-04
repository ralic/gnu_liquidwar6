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

#ifndef LIQUIDWAR6SYS_INTERNAL_H
#define LIQUIDWAR6SYS_INTERNAL_H

#include <pthread.h>
#include <stdarg.h>
#include <sys/time.h>

#include "sys.h"

#define _LW6SYS_EXIT_CRITICAL 1

/*
 * By default memory bazooka size is just hudge, will eat up to 7 or 8 megs.
 * The game will set it up to a much lower value through its config, still
 * we keep this very high value for the purpose of debugging. All test
 * programs will use this.
 */
#ifdef LW6_PARANOID
#define _LW6SYS_BAZOOKA_FILE_SIZE 80
#define _LW6SYS_BAZOOKA_SAMPLE_SIZE 1000
#define _LW6SYS_BAZOOKA_ALLOC_MIN 16
#else
#define _LW6SYS_BAZOOKA_FILE_SIZE 50
#define _LW6SYS_BAZOOKA_SAMPLE_SIZE 100
// ALLOC_MIN not defined in non-paranoid mode
#endif

#define _LW6SYS_DEBUG_DEFAULT 0

// ticks limit set to 15 minutes
#define _LW6SYS_TICKS_LIMIT 900000

#define _LW6SYS_VTHREAD_ID -1

typedef struct _lw6sys_bazooka_s
{
  void *ptr;
  int size;
  char file[_LW6SYS_BAZOOKA_FILE_SIZE];
  int line;
  int64_t timestamp;
} _lw6sys_bazooka_t;

typedef struct _lw6sys_mutex_s
{
  pthread_mutex_t mutex;
} _lw6sys_mutex_t;

#if ((_POSIX_SPIN_LOCKS - 200112L) >= 0L)
typedef struct _lw6sys_spinlock_s
{
  pthread_spinlock_t spinlock;
} _lw6sys_spinlock_t;
#else
#if LW6_X86
typedef struct _lw6sys_spinlock_s
{
  int spinlock;
} _lw6sys_spinlock_t;
#else
#define _lw6sys_spinlock_s _lw6sys_mutex_s
#define _lw6sys_spinlock_t _lw6sys_mutex_t
#endif
#endif

typedef struct _lw6sys_thread_handler_s
{
  pthread_t thread;
  u_int32_t id;
  int callback_done;
  int can_join;
  void (*callback_func) (void *callback_data);
  void (*callback_join) (void *callback_data);
  void *callback_data;
#ifndef LW6_MS_WINDOWS
  struct itimerval itimer;
#endif
} _lw6sys_thread_handler_t;

typedef struct _lw6sys_vthread_handler_s
{
  int join_done;
  _lw6sys_thread_handler_t thread;
} _lw6sys_vthread_handler_t;

typedef struct _lw6sys_global_s
{
  int debug;
  int log_level;
} _lw6sys_global_t;

extern _lw6sys_global_t _lw6sys_global;

extern int _lw6sys_bazooka_register_malloc (char *ptr, int size, char *file,
					    int line);
extern int _lw6sys_bazooka_register_calloc (char *ptr, int size, char *file,
					    int line);
extern int _lw6sys_bazooka_register_realloc_1 (char *ptr, int size,
					       char *file, int line);
extern int _lw6sys_bazooka_register_realloc_2 (char *ptr, char *ptr2,
					       int size, char *file,
					       int line);
extern int _lw6sys_bazooka_register_free (char *ptr);
extern char *_lw6sys_new_vsnprintf (int n, char *fmt, va_list ap);

#endif
