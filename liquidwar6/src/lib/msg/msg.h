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

#ifndef LIQUIDWAR6MSG_H
#define LIQUIDWAR6MSG_H

#include "../sys/sys.h"
#include "../glb/glb.h"
#include "../nod/nod.h"

#define LW6MSG_OOB_INFO "INFO"
#define LW6MSG_OOB_LIST "LIST"
#define LW6MSG_OOB_PING "PING"
#define LW6MSG_OOB_PONG "PONG"

#define LW6MSG_OOB_PROGRAM "PROGRAM"
#define LW6MSG_OOB_VERSION "VERSION"
#define LW6MSG_OOB_CODENAME "CODENAME"
#define LW6MSG_OOB_STAMP "STAMP"
#define LW6MSG_OOB_ID "ID"
#define LW6MSG_OOB_URL "URL"
#define LW6MSG_OOB_TITLE "TITLE"
#define LW6MSG_OOB_DESCRIPTION "DESCRIPTION"
#define LW6MSG_OOB_PASSWORD "PASSWORD"
#define LW6MSG_OOB_UPTIME "UPTIME"
#define LW6MSG_OOB_LEVEL "LEVEL"
#define LW6MSG_OOB_BENCH "BENCH"
#define LW6MSG_OOB_REQUIRED_BENCH "REQUIRED_BENCH"
#define LW6MSG_OOB_NB_COLORS "NB_COLORS"
#define LW6MSG_OOB_MAX_NB_COLORS "MAX_NB_COLORS"
#define LW6MSG_OOB_NB_CURSORS "NB_CURSORS"
#define LW6MSG_OOB_MAX_NB_CURSORS "MAX_NB_CURSORS"
#define LW6MSG_OOB_NB_NODES "NB_NODES"
#define LW6MSG_OOB_MAX_NB_NODES "MAX_NB_NODES"

#define LW6MSG_ERROR "ERROR"
#define LW6MSG_FORBIDDEN "FORBIDDEN"

#define LW6MSG_YES "YES"
#define LW6MSG_NO "NO"

/* msg-oob.c */
extern char *lw6msg_oob_generate_info (lw6nod_info_t * info);
extern char *lw6msg_oob_generate_list (lw6nod_info_t * info);
extern char *lw6msg_oob_generate_pong (lw6nod_info_t * info);
extern char *lw6msg_oob_generate_request (char *command, char *remote_url,
					  char *password, char *local_url);
extern int lw6msg_oob_analyse_request (int *syntax_ok, char **command,
				       int *password_ok, char **remote_url,
				       char *request, char *local_url,
				       char *password);
extern char *lw6msg_oob_analyse_pong (char *text);

/* nod-test.c */
extern int lw6msg_test (int mode);

/* msg-utils.c */
extern int lw6msg_utils_parse_key_value_to_ptr (char **key, char **value,
						char *line);
extern int lw6msg_utils_parse_key_value_to_assoc (lw6sys_assoc_t ** assoc,
						  char *line);
extern char *lw6msg_utils_get_assoc_str_with_default (lw6sys_assoc_t * assoc,
						      char *key,
						      char *default_value);
extern int lw6msg_utils_get_assoc_int_with_default (lw6sys_assoc_t * assoc,
						    char *key,
						    int default_value);

#endif