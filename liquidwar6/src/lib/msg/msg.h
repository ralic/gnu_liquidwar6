/*
  Liquid War 6 is a unique multiplayer wargame.
  Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013  Christian Mauduit <ufoot@ufoot.org>

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
#include "../cnx/cnx.h"

#define LW6MSG_LW6 "LW6"
#define LW6MSG_LW6_LC "lw6"

#define LW6MSG_ERROR "ERROR"
#define LW6MSG_FORBIDDEN "FORBIDDEN"

#define LW6MSG_YES "YES"
#define LW6MSG_NO "NO"

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
#define LW6MSG_OOB_HAS_PASSWORD "HAS_PASSWORD"
#define LW6MSG_OOB_BENCH "BENCH"
#define LW6MSG_OOB_OPEN_RELAY "OPEN_RELAY"
#define LW6MSG_OOB_UPTIME "UPTIME"
#define LW6MSG_OOB_COMMUNITY "COMMUNITY"
#define LW6MSG_OOB_ROUND "ROUND"
#define LW6MSG_OOB_LEVEL "LEVEL"
#define LW6MSG_OOB_REQUIRED_BENCH "REQUIRED_BENCH"
#define LW6MSG_OOB_NB_COLORS "NB_COLORS"
#define LW6MSG_OOB_MAX_NB_COLORS "MAX_NB_COLORS"
#define LW6MSG_OOB_NB_CURSORS "NB_CURSORS"
#define LW6MSG_OOB_MAX_NB_CURSORS "MAX_NB_CURSORS"
#define LW6MSG_OOB_NB_NODES "NB_NODES"
#define LW6MSG_OOB_MAX_NB_NODES "MAX_NB_NODES"

#define LW6MSG_CMD_HELLO "HELLO"
#define LW6MSG_CMD_TICKET "TICKET"
#define LW6MSG_CMD_FOO "FOO"
#define LW6MSG_CMD_BAR "BAR"
#define LW6MSG_CMD_JOIN "JOIN"
#define LW6MSG_CMD_GOODBYE "GOODBYE"
#define LW6MSG_CMD_DATA "DATA"
#define LW6MSG_CMD_MISS "MISS"

//#define LW6MSG_DATA_KER "KER"
//#define LW6MSG_DATA_JOIN "JOIN"
//#define LW6MSG_DATA_WELCOME "WELCOME"
//#define LW6MSG_DATA_QUIT "QUIT"
//#define LW6MSG_DATA_TIMEOUT "TIMEOUT"
//#define LW6MSG_DATA_KICK "KICK"
//#define LW6MSG_DATA_CHECKSUM "CHECKSUM"
//#define LW6MSG_DATA_SYNC "SYNC"
//#define LW6MSG_DATA_META "META"
//#define LW6MSG_DATA_DUMP "DUMP"
//#define LW6MSG_DATA_SNAPSHOT "SNAPSHOT"
//#define LW6MSG_DATA_CHAT "CHAT"

#define LW6MSG_MAX_WORD_SIZE 999
#define LW6MSG_Z_PREFIX "Z"

#define LW6MSG_TELNET_SEP ' '
#define LW6MSG_URL_SEP '/'

#define LW6MSG_UNDEF "-"

typedef enum lw6msg_envelope_mode_e
{
  LW6MSG_ENVELOPE_MODE_TELNET = 0,
  LW6MSG_ENVELOPE_MODE_URL = 1
} lw6msg_envelope_mode_t;

/**
 * This structure is used to retrieve words from messages.
 * We use a structure here with a fixed sized buffer and
 * a len member, this is to avoid mallocating too often
 * when parsing.
 */
typedef struct lw6msg_word_s
{
  /// Length of word, in bytes.
  int len;
  /**
   * Word data, containing a 0 char at the end, so
   * it is safe to call standard C string functions on it.
   */
  char buf[LW6MSG_MAX_WORD_SIZE + 1];

} lw6msg_word_t;

/* msg-cmd.c */
extern char *lw6msg_cmd_generate_hello (lw6nod_info_t * info);
extern char *lw6msg_cmd_generate_ticket (lw6nod_info_t * info,
					 u_int64_t ticket);
extern char *lw6msg_cmd_generate_foo (lw6nod_info_t * info, u_int32_t key,
				      int serial);
extern char *lw6msg_cmd_generate_bar (lw6nod_info_t * info, u_int32_t key,
				      int serial);
extern char *lw6msg_cmd_generate_join (lw6nod_info_t * info, int64_t seq,
				       int serial);
extern char *lw6msg_cmd_generate_goodbye (lw6nod_info_t * info);
extern char *lw6msg_cmd_generate_data (int serial, int i, int n, int64_t seq,
				       const char *ker_msg);
extern char *lw6msg_cmd_generate_miss (u_int64_t id_from, u_int64_t id_to,
				       int serial_min, int serial_max);
extern int lw6msg_cmd_analyse_hello (lw6nod_info_t ** info, const char *msg);
extern int lw6msg_cmd_analyse_ticket (lw6nod_info_t ** info,
				      u_int64_t * ticket, const char *msg);
extern int lw6msg_cmd_analyse_foo (lw6nod_info_t ** info, u_int32_t * key,
				   int *serial, const char *msg);
extern int lw6msg_cmd_analyse_bar (lw6nod_info_t ** info, u_int32_t * key,
				   int *serial, const char *msg);
extern int lw6msg_cmd_analyse_join (lw6nod_info_t ** info, int64_t * seq,
				    int *serial, const char *msg);
extern int lw6msg_cmd_analyse_goodbye (lw6nod_info_t ** info,
				       const char *msg);
extern int lw6msg_cmd_analyse_data (int *serial, int *i, int *n,
				    int64_t * seq, char **ker_msg,
				    const char *msg);
extern int lw6msg_cmd_analyse_miss (u_int64_t * id_from, u_int64_t * id_to,
				    int *serial_min, int *serial_max,
				    const char *msg);
extern char *lw6msg_cmd_guess_from_url (const char *msg);

/* msg-enveloppe.c */
extern char *lw6msg_envelope_generate (lw6msg_envelope_mode_t mode,
				       const char *version,
				       const char *password_checksum,
				       u_int32_t physical_ticket_sig,
				       u_int32_t logical_ticket_sig,
				       u_int64_t physical_from_id,
				       u_int64_t physical_to_id,
				       u_int64_t logical_from_id,
				       u_int64_t logical_to_id,
				       const char *msg);
extern int lw6msg_envelope_analyse (const char *envelope,
				    lw6msg_envelope_mode_t mode,
				    const char *local_url,
				    const char *password,
				    u_int64_t expected_physical_from_id,
				    u_int64_t expected_physical_to_id,
				    char **msg,
				    u_int32_t * physical_ticket_sig,
				    u_int32_t * logical_ticket_sig,
				    u_int64_t * physical_from_id,
				    u_int64_t * physical_to_id,
				    u_int64_t * logical_from_id,
				    u_int64_t * logical_to_id,
				    char **physical_from_url);

/* msg-oob.c */
extern char *lw6msg_oob_generate_info (lw6nod_info_t * info);
extern char *lw6msg_oob_generate_list (lw6nod_info_t * info);
extern char *lw6msg_oob_generate_pong (lw6nod_info_t * info);
extern char *lw6msg_oob_generate_request (const char *command,
					  const char *remote_url,
					  const char *password,
					  const char *local_url);
extern int lw6msg_oob_analyse_request (int *syntax_ok, char **command,
				       int *password_ok, char **remote_url,
				       const char *request,
				       const char *local_url,
				       const char *password);
extern char *lw6msg_oob_analyse_pong (const char *text);

/* msg-sort.c */
int lw6msg_sort_str_by_seq_callback (lw6sys_list_t ** list_a,
				     lw6sys_list_t ** list_b);

/* msg-test.c */
extern int lw6msg_test (int mode);

/* msg-ticket.c */
extern u_int32_t lw6msg_ticket_calc_sig (u_int64_t ticket, u_int64_t from_id,
					 u_int64_t to_id, const char *msg);
extern int lw6msg_ticket_check_sig (u_int64_t ticket, u_int64_t from_id,
				    u_int64_t to_id, const char *msg,
				    u_int32_t ticket_sig);

/* msg-utils.c */
extern int lw6msg_utils_parse_key_value_to_ptr (char **key, char **value,
						const char *line);
extern int lw6msg_utils_parse_key_value_to_assoc (lw6sys_assoc_t ** assoc,
						  const char *line);
extern char *lw6msg_utils_get_assoc_str_with_default (lw6sys_assoc_t * assoc,
						      const char *key,
						      const char
						      *default_value);
extern int lw6msg_utils_get_assoc_int_with_default (lw6sys_assoc_t * assoc,
						    const char *key,
						    int default_value);

/* msg-word.c */
extern int lw6msg_word_first (lw6msg_word_t * word, char **next,
			      const char *msg);
extern int lw6msg_word_first_x (lw6msg_word_t * word, char **next,
				const char *msg);
extern int lw6msg_word_first_base64 (lw6msg_word_t * word, char **next,
				     const char *msg);
extern int lw6msg_word_first_int_32 (int32_t * parsed_value, char **next,
				     const char *msg);
extern int lw6msg_word_first_int_32_ge0 (int32_t * parsed_value, char **next,
					 const char *msg);
extern int lw6msg_word_first_int_32_gt0 (int32_t * parsed_value, char **next,
					 const char *msg);
extern int lw6msg_word_first_int_64 (int64_t * parsed_value, char **next,
				     const char *msg);
extern int lw6msg_word_first_int_64_ge0 (int64_t * parsed_value, char **next,
					 const char *msg);
extern int lw6msg_word_first_int_64_gt0 (int64_t * parsed_value, char **next,
					 const char *msg);
extern int lw6msg_word_first_id_16 (u_int16_t * parsed_value, char **next,
				    const char *msg);
extern int lw6msg_word_first_id_32 (u_int32_t * parsed_value, char **next,
				    const char *msg);
extern int lw6msg_word_first_id_64 (u_int64_t * parsed_value, char **next,
				    const char *msg);

/* msg-z.c */
extern char *lw6msg_z_encode (const char *msg, int limit);
extern char *lw6msg_z_decode (const char *msg);

#endif
