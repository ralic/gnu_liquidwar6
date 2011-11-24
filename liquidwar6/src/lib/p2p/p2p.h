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

#ifndef LIQUIDWAR6P2P_H
#define LIQUIDWAR6P2P_H

#include "../sys/sys.h"
#include "../glb/glb.h"
#include "../cfg/cfg.h"
#include "../net/net.h"
#include "../nod/nod.h"
#include "../cnx/cnx.h"
#include "../msg/msg.h"
#include "../cli/cli.h"
#include "../srv/srv.h"
#include "../dat/dat.h"

/*
 * Must be greater than LW6DAT_MAX_NB_STACKS
 */
#define LW6P2P_MAX_NB_TENTACLES 100

/*
 * Hardcoded size for database stored data, carefull
 * that these match the sizes in create-database.sql
 */
#define LW6P2P_VERSION_SIZE 64
#define LW6P2P_CODENAME_SIZE 64
#define LW6P2P_ID_SIZE 16
#define LW6P2P_URL_SIZE 1024
#define LW6P2P_TITLE_SIZE 256
#define LW6P2P_DESCRIPTION_SIZE 2048
#define LW6P2P_COMMUNITY_ID_SIZE 16
#define LW6P2P_LEVEL_SIZE 1024
#define LW6P2P_IP_SIZE 64

typedef struct lw6p2p_db_s
{
  /*
   * The first member, id, is the same as the internal
   * _lw6p2p_db_t structure. The rest of it is hidden.
   * The program will cast from lw6p2p_db_t to _lw6p2p_db_t
   * internally.
   */
  u_int32_t id;
} lw6p2p_db_t;

typedef struct lw6p2p_node_s
{
  /*
   * The first member, id, is the same as the internal
   * _lw6p2p_node_t structure. The rest of it is hidden.
   * The program will cast from lw6p2p_node_t to _lw6p2p_node_t
   * internally.
   */
  u_int32_t id;
} lw6p2p_node_t;

/*
 * This entry should match as close as possible the
 * corresponding (node) entry in the database.
 */
typedef struct lw6p2p_entry_s
{
  // constant data 
  int creation_timestamp;
  char version[LW6P2P_VERSION_SIZE + 1];
  char codename[LW6P2P_CODENAME_SIZE + 1];
  int stamp;
  char id[LW6P2P_ID_SIZE + 1];
  char url[LW6P2P_URL_SIZE + 1];
  char title[LW6P2P_TITLE_SIZE + 1];
  char description[LW6P2P_DESCRIPTION_SIZE + 1];
  int has_password;
  int bench;
  int open_relay;
  // variable data
  char community_id[LW6P2P_COMMUNITY_ID_SIZE + 1];
  int round;
  char level[LW6P2P_LEVEL_SIZE + 1];
  int required_bench;
  int nb_colors;
  int max_nb_colors;
  int nb_cursors;
  int max_nb_cursors;
  int nb_nodes;
  int max_nb_nodes;
  // additionnal data
  char ip[LW6P2P_IP_SIZE + 1];
  int port;
  int last_ping_timestamp;
  int ping_delay_msec;
  int available;
} lw6p2p_entry_t;

/* p2p-db.c */
extern lw6p2p_db_t *lw6p2p_db_open (int argc, char *argv[], char *name);
extern void lw6p2p_db_close (lw6p2p_db_t * db);
extern char *lw6p2p_db_repr (lw6p2p_db_t * db);
extern int lw6p2p_db_reset (int argc, char *argv[], char *name);
extern char *lw6p2p_db_default_name ();

/* p2p-entry.c */
lw6p2p_entry_t *lw6p2p_entry_new (int creation_timestamp, char *version,
				  char *codename, int stamp, char *node_id,
				  char *node_url, char *node_title,
				  char *node_description, int has_password,
				  int bench, int open_relay,
				  char *community_id, int round, char *level,
				  int required_bench, int nb_colors,
				  int max_nb_colors, int nb_cursors,
				  int max_nb_cursors, int nb_nodes,
				  int max_nb_nodes, char *ip, int port,
				  int last_ping_timestamp,
				  int ping_delay_msec, int available);
void lw6p2p_entry_free (lw6p2p_entry_t * entry);
char *lw6p2p_entry_repr (lw6p2p_entry_t * entry);

/* p2p-node.c */
extern lw6p2p_node_t *lw6p2p_node_new (int argc, char *argv[],
				       lw6p2p_db_t * db,
				       char *client_backends,
				       char *server_backends,
				       char *bind_ip,
				       int bind_port, int broadcast,
				       char *public_url,
				       char *title,
				       char *description, char *password,
				       int bench, int open_relay,
				       char *known_nodes,
				       int network_reliability, int trojan);
extern void lw6p2p_node_free (lw6p2p_node_t * node);
extern char *lw6p2p_node_repr (lw6p2p_node_t * node);
extern int lw6p2p_node_poll (lw6p2p_node_t * node);
extern void lw6p2p_node_close (lw6p2p_node_t * node);
extern u_int64_t lw6p2p_node_get_id (lw6p2p_node_t * node);
extern lw6sys_list_t *lw6p2p_node_get_entries (lw6p2p_node_t * node);

/* p2p-test.c */
extern int lw6p2p_test (int mode);

#endif
