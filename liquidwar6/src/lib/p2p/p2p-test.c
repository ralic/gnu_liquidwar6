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

#include "p2p.h"

#define _TEST_ARGC 1
#define _TEST_ARGV0 "prog"
#define _TEST_DB_NAME12 "p2p.test.12"
#define _TEST_DB_NAME34 "p2p.test.34"
#define _TEST_NODE_BIND_IP "0.0.0.0"
#define _TEST_NODE_BIND_PORT1 (LW6NET_DEFAULT_PORT + 11)
#define _TEST_NODE_BIND_PORT2 (LW6NET_DEFAULT_PORT + 12)
#define _TEST_NODE_BIND_PORT3 (LW6NET_DEFAULT_PORT + 13)
#define _TEST_NODE_BIND_PORT4 (LW6NET_DEFAULT_PORT + 14)
#define _TEST_NODE_BROADCAST 1
#define _TEST_NODE_NODE_ID1 0x1234123412341234LL
#define _TEST_NODE_NODE_ID2 0x2345234523452345LL
#define _TEST_NODE_NODE_ID3 0x3456345634563456LL
#define _TEST_NODE_NODE_ID4 0x4567456745674567LL
/*
 * following depends on LW6NET_DEFAULT_PORT
 */
#define _TEST_NODE_PUBLIC_URL1 "http://localhost:8067/"
#define _TEST_NODE_PUBLIC_URL2 "http://localhost:8068/"
#define _TEST_NODE_PUBLIC_URL3 "http://localhost:8069/"
#define _TEST_NODE_PUBLIC_URL4 "http://localhost:8070/"
#define _TEST_NODE_PASSWORD "toto"
#define _TEST_NODE_TITLE1 "Node 1"
#define _TEST_NODE_TITLE2 "Node 2"
#define _TEST_NODE_TITLE3 "Node 3"
#define _TEST_NODE_TITLE4 "Node 4"
#define _TEST_NODE_DESCRIPTION "This is the description of a node."
#define _TEST_NODE_BENCH 10
#define _TEST_NODE_KNOWN_NODES1 "http://localhost:8068/"
#define _TEST_NODE_KNOWN_NODES2 "http://localhost:8069/"
#define _TEST_NODE_KNOWN_NODES3 "http://localhost:8070/"
#define _TEST_NODE_KNOWN_NODES4 "http://localhost:8067/"

#define TEST_NODE_OOB_DURATION 90000

/* 
 * Testing db
 */
static int
_test_db ()
{
  int ret = 1;
  LW6SYS_TEST_FUNCTION_BEGIN;

  {
    int argc = _TEST_ARGC;
    char *argv[] = { _TEST_ARGV0 };
    lw6p2p_db_t *db = NULL;
    char *repr = NULL;

    lw6sys_log (LW6SYS_LOG_NOTICE,
		_
		("default database name is \"%s\" but we use \"%s\" for some tests"),
		lw6p2p_db_default_name (), _TEST_DB_NAME12);

    db = lw6p2p_db_open (argc, argv, _TEST_DB_NAME12);
    if (db)
      {
	repr = lw6p2p_db_repr (db);
	if (repr)
	  {
	    lw6sys_log (LW6SYS_LOG_NOTICE, _("created db \"%s\""), repr);
	    LW6SYS_FREE (repr);
	  }
	lw6p2p_db_close (db);
      }
    else
      {
	lw6sys_log (LW6SYS_LOG_WARNING, _("can't create db"));
	ret = 0;
      }

    if (lw6p2p_db_reset (argc, argv, _TEST_DB_NAME12))
      {
	lw6sys_log (LW6SYS_LOG_NOTICE, _("deleted db"));
      }
    else
      {
	lw6sys_log (LW6SYS_LOG_WARNING, _("can't delete db"));
	ret = 0;
      }
  }

  LW6SYS_TEST_FUNCTION_END;
  return ret;
}

/* 
 * Testing node initialisation
 */
static int
_test_node_init ()
{
  int ret = 1;
  LW6SYS_TEST_FUNCTION_BEGIN;

  {
    int argc = _TEST_ARGC;
    char *argv[] = { _TEST_ARGV0 };
    lw6p2p_db_t *db = NULL;
    lw6p2p_node_t *node = NULL;
    char *repr = NULL;

    db = lw6p2p_db_open (argc, argv, _TEST_DB_NAME12);
    if (db)
      {
	node =
	  lw6p2p_node_new (argc, argv, db, lw6cli_default_backends (),
			   lw6srv_default_backends (), _TEST_NODE_BIND_IP,
			   _TEST_NODE_BIND_PORT1, _TEST_NODE_BROADCAST,
			   _TEST_NODE_NODE_ID1, _TEST_NODE_PUBLIC_URL1, NULL,
			   _TEST_NODE_TITLE1, _TEST_NODE_DESCRIPTION,
			   _TEST_NODE_BENCH, _TEST_NODE_KNOWN_NODES1);
	if (node)
	  {
	    repr = lw6p2p_node_repr (node);
	    if (repr)
	      {
		lw6sys_log (LW6SYS_LOG_NOTICE, _("created node \"%s\""),
			    repr);
		LW6SYS_FREE (repr);
	      }
	    lw6p2p_node_close (node);
	    lw6p2p_node_close (node);	// yes, do it twice just to check
	    lw6p2p_node_free (node);
	  }
	else
	  {
	    lw6sys_log (LW6SYS_LOG_WARNING, _("can't create node"));
	    ret = 0;
	  }
	lw6p2p_db_close (db);
      }
    else
      {
	lw6sys_log (LW6SYS_LOG_WARNING, _("can't create db"));
	ret = 0;
      }
  }

  LW6SYS_TEST_FUNCTION_END;
  return ret;
}

/* 
 * Initializes up to 3 nodes
 */
static int
_init_nodes (lw6p2p_db_t ** db12, lw6p2p_db_t ** db34, lw6p2p_node_t ** node1,
	     lw6p2p_node_t ** node2, lw6p2p_node_t ** node3,
	     lw6p2p_node_t ** node4)
{
  int argc = _TEST_ARGC;
  char *argv[] = { _TEST_ARGV0 };
  char *repr = NULL;
  int ret = 0;

  if (db12)
    {
      lw6p2p_db_reset (argc, argv, _TEST_DB_NAME12);
      (*db12) = lw6p2p_db_open (argc, argv, _TEST_DB_NAME12);
      if (*db12)
	{
	  repr = lw6p2p_db_repr (*db12);
	  if (repr)
	    {
	      lw6sys_log (LW6SYS_LOG_NOTICE, _("created db \"%s\""), repr);
	      LW6SYS_FREE (repr);
	    }
	}
    }
  if (db34)
    {
      lw6p2p_db_reset (argc, argv, _TEST_DB_NAME34);
      (*db34) = lw6p2p_db_open (argc, argv, _TEST_DB_NAME34);
      if (*db34)
	{
	  repr = lw6p2p_db_repr (*db34);
	  if (repr)
	    {
	      lw6sys_log (LW6SYS_LOG_NOTICE, _("created db \"%s\""), repr);
	      LW6SYS_FREE (repr);
	    }
	}
    }
  if (db12 && (*db12))
    {
      if (node1)
	{
	  (*node1) =
	    lw6p2p_node_new (argc, argv, *db12, lw6cli_default_backends (),
			     lw6srv_default_backends (), _TEST_NODE_BIND_IP,
			     _TEST_NODE_BIND_PORT1, _TEST_NODE_BROADCAST,
			     _TEST_NODE_NODE_ID1, _TEST_NODE_PUBLIC_URL1,
			     _TEST_NODE_PASSWORD, _TEST_NODE_TITLE1,
			     _TEST_NODE_DESCRIPTION, _TEST_NODE_BENCH,
			     _TEST_NODE_KNOWN_NODES1);
	  if (*node1)
	    {
	      repr = lw6p2p_node_repr (*node1);
	      if (repr)
		{
		  lw6sys_log (LW6SYS_LOG_NOTICE, _("created node1 \"%s\""),
			      repr);
		  LW6SYS_FREE (repr);
		}
	    }
	}

      if (node2)
	{
	  (*node2) =
	    lw6p2p_node_new (argc, argv, *db12, lw6cli_default_backends (),
			     lw6srv_default_backends (), _TEST_NODE_BIND_IP,
			     _TEST_NODE_BIND_PORT2, _TEST_NODE_BROADCAST,
			     _TEST_NODE_NODE_ID2, _TEST_NODE_PUBLIC_URL2,
			     NULL, _TEST_NODE_TITLE2, _TEST_NODE_DESCRIPTION,
			     _TEST_NODE_BENCH, _TEST_NODE_KNOWN_NODES2);
	  if (*node2)
	    {
	      repr = lw6p2p_node_repr (*node2);
	      if (repr)
		{
		  lw6sys_log (LW6SYS_LOG_NOTICE, _("created node2 \"%s\""),
			      repr);
		  LW6SYS_FREE (repr);
		}
	    }
	}
    }
  if (db34 && (*db34))
    {
      if (node3)
	{
	  (*node3) =
	    lw6p2p_node_new (argc, argv, *db34, lw6cli_default_backends (),
			     lw6srv_default_backends (), _TEST_NODE_BIND_IP,
			     _TEST_NODE_BIND_PORT3, _TEST_NODE_BROADCAST,
			     _TEST_NODE_NODE_ID3, _TEST_NODE_PUBLIC_URL3,
			     _TEST_NODE_PASSWORD, _TEST_NODE_TITLE3,
			     _TEST_NODE_DESCRIPTION, _TEST_NODE_BENCH,
			     _TEST_NODE_KNOWN_NODES3);
	  if (*node3)
	    {
	      repr = lw6p2p_node_repr (*node3);
	      if (repr)
		{
		  lw6sys_log (LW6SYS_LOG_NOTICE, _("created node3 \"%s\""),
			      repr);
		  LW6SYS_FREE (repr);
		}
	    }
	}

      if (node4)
	{
	  (*node4) =
	    lw6p2p_node_new (argc, argv, *db34, lw6cli_default_backends (),
			     lw6srv_default_backends (), _TEST_NODE_BIND_IP,
			     _TEST_NODE_BIND_PORT4, _TEST_NODE_BROADCAST,
			     _TEST_NODE_NODE_ID4, _TEST_NODE_PUBLIC_URL4,
			     NULL, _TEST_NODE_TITLE4, _TEST_NODE_DESCRIPTION,
			     _TEST_NODE_BENCH, _TEST_NODE_KNOWN_NODES4);
	  if (*node4)
	    {
	      repr = lw6p2p_node_repr (*node4);
	      if (repr)
		{
		  lw6sys_log (LW6SYS_LOG_NOTICE, _("created node4 \"%s\""),
			      repr);
		  LW6SYS_FREE (repr);
		}
	    }
	}
    }

  if (((!node1) || (node1 && *node1))
      && ((!node2) || (node2 && *node2)) && ((!node3) || (node3 && *node3))
      && ((!node4) || (node4 && *node4)))
    {
      ret = 1;
    }
  else
    {
      if (node1 && *node1)
	{
	  lw6p2p_node_free (*node1);
	  (*node1) = NULL;
	}
      if (node2 && *node2)
	{
	  lw6p2p_node_free (*node2);
	  (*node2) = NULL;
	}
      if (node3 && *node3)
	{
	  lw6p2p_node_free (*node3);
	  (*node3) = NULL;
	}
      if (node4 && *node4)
	{
	  lw6p2p_node_free (*node4);
	  (*node4) = NULL;
	}
      if (db12 && *db12)
	{
	  lw6p2p_db_close (*db12);
	  (*db12) = NULL;
	}
      if (db34 && *db34)
	{
	  lw6p2p_db_close (*db34);
	  (*db34) = NULL;
	}
    }

  return ret;
}

static void
_quit_nodes (lw6p2p_db_t * db12, lw6p2p_db_t * db34, lw6p2p_node_t * node1,
	     lw6p2p_node_t * node2, lw6p2p_node_t * node3,
	     lw6p2p_node_t * node4)
{
  if (node1)
    {
      lw6p2p_node_close (node1);
      lw6p2p_node_free (node1);
    }
  if (node2)
    {
      lw6p2p_node_close (node2);
      lw6p2p_node_free (node2);
    }
  if (node3)
    {
      lw6p2p_node_close (node3);
      lw6p2p_node_free (node3);
    }
  if (node4)
    {
      lw6p2p_node_close (node4);
      lw6p2p_node_free (node4);
    }
  if (db12)
    {
      lw6p2p_db_close (db12);
    }
  if (db34)
    {
      lw6p2p_db_close (db34);
    }
}

/* 
 * Testing node connection
 */
static int
_test_node_oob ()
{
  int ret = 1;
  LW6SYS_TEST_FUNCTION_BEGIN;

  {
    //int argc = _TEST_ARGC;
    //char *argv[] = { _TEST_ARGV0 };
    lw6p2p_db_t *db12 = NULL;
    lw6p2p_db_t *db34 = NULL;
    lw6p2p_node_t *node1 = NULL;
    lw6p2p_node_t *node2 = NULL;
    lw6p2p_node_t *node3 = NULL;
    lw6p2p_node_t *node4 = NULL;
    int64_t end_timestamp = 0;

    ret = 0;
    end_timestamp = lw6sys_get_timestamp () + TEST_NODE_OOB_DURATION;
    if (_init_nodes (&db12, &db34, &node1, &node2, &node3, &node4))
      {
	while (lw6sys_get_timestamp () < end_timestamp)
	  {
	    lw6p2p_node_poll (node1);
	    lw6p2p_node_poll (node2);
	    lw6p2p_node_poll (node3);
	    lw6p2p_node_poll (node4);
	    lw6sys_idle ();
	  }

	ret = 1;

	_quit_nodes (db12, db34, node1, node2, node3, node4);
      }
  }

  LW6SYS_TEST_FUNCTION_END;
  return ret;
}

/**
 * lw6p2p_test
 *
 * @mode: 0 for check only, 1 for full test
 *
 * Runs the @p2p module test suite. This test can fail if one
 * cannot bind on some network port, in a general manner it is
 * dependent on the network environment, so it's better if there's
 * some sort of human control on it.
 *
 * Return value: 1 if test is successfull, 0 on error.
 */
int
lw6p2p_test (int mode)
{
  int ret = 0;
  int argc = _TEST_ARGC;
  char *argv[] = { _TEST_ARGV0 };

  if (lw6sys_false ())
    {
      /*
       * Just to make sure most functions are stuffed in the binary
       */
      lw6sys_test (mode);
      lw6glb_test (mode);
      lw6cfg_test (mode);
      lw6net_test (mode);
      lw6nod_test (mode);
      lw6msg_test (mode);
      lw6cli_test (mode);
      lw6srv_test (mode);
    }

  if (lw6net_init (argc, argv))
    {
      ret = _test_db () && _test_node_init () && _test_node_oob ();
      lw6net_quit (argc, argv);
    }

  return ret;
}
