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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "liquidwar6.h"

#define _TEST_FILE_0_MAIN "test-0-main.scm"
#define _TEST_FILE_0_NODE_A "test-0-node-a.scm"
#define _TEST_FILE_0_NODE_B "test-0-node-b.scm"
#define _TEST_FILE_0_NODE_C "test-0-node-c.scm"
#define _TEST_FILE_1_MAIN "test-1-main.scm"
#define _TEST_FILE_1_NODE_A "test-1-node-a.scm"
#define _TEST_FILE_1_NODE_B "test-1-node-b.scm"
#define _TEST_FILE_1_NODE_C "test-1-node-c.scm"
#define _TEST_ARGC 1
#define _TEST_ARGV0 "prog"

#define _TEST_COVERAGE_PERCENT_MIN 33

#define _TEST_SUITE_MAIN 0
#define _TEST_SUITE_NODE_A 1
#define _TEST_SUITE_NODE_B 2
#define _TEST_SUITE_NODE_C 3

#define _TEST_RUN_RANDOM_RANGE 2

typedef struct _lw6_test_param_s
{
  int ret;
  int argc;
  const char **argv;
  int mode;
  int suite;
  int coverage_check;
  int log_level_id;
}
_lw6_test_param_t;

static char *
get_test_file (int argc, const char **argv, int mode, int suite)
{
  char *script_file = NULL;
  char *script_dir = NULL;
  char *ret = NULL;

  script_file = lw6sys_get_script_file (argc, argv);
  if (script_file)
    {
      script_dir = lw6sys_path_parent (script_file);
      if (script_dir)
	{
	  if (!mode)
	    {
	      switch (suite)
		{
		case _TEST_SUITE_NODE_A:
		  ret = lw6sys_path_concat (script_dir, _TEST_FILE_0_NODE_A);
		  break;
		case _TEST_SUITE_NODE_B:
		  ret = lw6sys_path_concat (script_dir, _TEST_FILE_0_NODE_B);
		  break;
		case _TEST_SUITE_NODE_C:
		  ret = lw6sys_path_concat (script_dir, _TEST_FILE_0_NODE_C);
		  break;
		default:
		  ret = lw6sys_path_concat (script_dir, _TEST_FILE_0_MAIN);
		  break;
		}
	    }
	  else
	    {
	      switch (suite)
		{
		case _TEST_SUITE_NODE_A:
		  ret = lw6sys_path_concat (script_dir, _TEST_FILE_1_NODE_A);
		  break;
		case _TEST_SUITE_NODE_B:
		  ret = lw6sys_path_concat (script_dir, _TEST_FILE_1_NODE_B);
		  break;
		case _TEST_SUITE_NODE_C:
		  ret = lw6sys_path_concat (script_dir, _TEST_FILE_1_NODE_C);
		  break;
		default:
		  ret = lw6sys_path_concat (script_dir, _TEST_FILE_1_MAIN);
		  break;
		}
	    }
	  LW6SYS_FREE (script_dir);
	}
      LW6SYS_FREE (script_file);
    }

  if (ret && !lw6sys_file_exists (ret))
    {
      lw6sys_log (LW6SYS_LOG_WARNING, _x_ ("can't open \"%s\""), ret);
      LW6SYS_FREE (ret);
      ret = NULL;
    }

  return ret;
}

static void
_test_callback (_lw6_test_param_t * param)
{
  int ret = 1;

  LW6SYS_TEST_FUNCTION_BEGIN;

  {
    char *test_file = NULL;
    lw6sys_list_t *funcs = NULL;
    int coverage_percent = 0;

    /*
     * Change log level, this way on spawned processes there
     * are no logs and this avoids dirty interferences.
     * Will will run each test in the main thread anyway.
     */
    lw6sys_log_set_level (param->log_level_id);

    lw6sys_log (LW6SYS_LOG_NOTICE, _x_ ("registering Guile smobs"));
    if (lw6_register_smobs ())
      {

	lw6sys_log (LW6SYS_LOG_NOTICE, _x_ ("registering Guile functions"));
	if (lw6_register_funcs ())
	  {
	    test_file =
	      get_test_file (param->argc, param->argv, param->mode,
			     param->suite);
	    if (test_file)
	      {
		lw6sys_log (LW6SYS_LOG_NOTICE, _x_ ("loading \"%s\""),
			    test_file);
		lw6scm_c_primitive_load (test_file);
		LW6SYS_FREE (test_file);
	      }
	  }
	else
	  {
	    lw6sys_log (LW6SYS_LOG_WARNING, _x_ ("unable to register funcs"));
	    ret = 0;
	  }
      }
    else
      {
	lw6sys_log (LW6SYS_LOG_WARNING, _x_ ("unable to register smobs"));
	ret = 0;
      }

    scm_gc ();

    if (param->coverage_check)
      {
	funcs = lw6hlp_list_funcs ();
	if (funcs)
	  {
	    if (lw6scm_coverage_check
		(&coverage_percent, lw6_global.coverage, funcs)
		|| coverage_percent >= _TEST_COVERAGE_PERCENT_MIN)
	      {
		lw6sys_log (LW6SYS_LOG_NOTICE,
			    _x_ ("script coverage OK %d%% (%d%% required)"),
			    coverage_percent, _TEST_COVERAGE_PERCENT_MIN);
	      }
	    else
	      {
		lw6sys_log (LW6SYS_LOG_WARNING,
			    _x_
			    ("script coverage is only %d%% (%d%% required)"),
			    coverage_percent, _TEST_COVERAGE_PERCENT_MIN);
		ret = 0;
	      }
	    lw6sys_list_free (funcs);
	  }
	else
	  {
	    lw6sys_log (LW6SYS_LOG_WARNING, _x_ ("unable to get funcs list"));
	    ret = 0;
	  }
      }
    else
      {
	lw6sys_log (LW6SYS_LOG_NOTICE, _x_ ("skipping coverage check"));
      }

    if (ret)
      {
	if (lw6_global.ret)
	  {
	    lw6sys_log (LW6SYS_LOG_NOTICE,
			_x_
			("script returned true, looks like tests were OK"));
	  }
	else
	  {
	    lw6sys_log (LW6SYS_LOG_WARNING,
			_x_
			("script returned false, at least one test failed"));
	    ret = 0;
	  }
      }
  }

  LW6SYS_TEST_FUNCTION_END;

  /*
   * Local ret is initialized to 1, but the caller's param->ret
   * was set to 0 (else non-running script would return 1).
   */
  param->ret = ret;
}

static void *
_guile_test (void *data)
{
  _lw6_test_param_t *param = (_lw6_test_param_t *) data;

  _test_callback (param);

  return NULL;
}

static void
_guile_test_run (void *data)
{
  /*
   * We wait (or not) for some time, to simulate, (randomly!)
   * some time shift between various network flavors of tests.
   */
  if (!lw6sys_random (_TEST_RUN_RANDOM_RANGE))
    {
      lw6sys_snooze ();
    }

  lw6scm_with_guile (_guile_test, data);
}

/**
 * lw6_test
 *
 * @mode: 0 for check only, 1 for full test
 *
 * Runs the liquidwar6 core module test suite, this will mostly
 * test how Guile script integration works, loading a sample
 * script and running it. It does not launch all the other sub
 * modules tests.
 *
 * Return value: 1 if test is successfull, 0 on error.
 */
int
lw6_test (int mode)
{
  int ret = 0;
  _lw6_test_param_t param;
  _lw6_test_param_t param_a;
  _lw6_test_param_t param_b;
  _lw6_test_param_t param_c;
  const int argc = _TEST_ARGC;
  const char *argv[] = { _TEST_ARGV0 };
  u_int64_t pid_a = 0LL;
  u_int64_t pid_b = 0LL;
  u_int64_t pid_c = 0LL;
  int default_log_level_id = LW6SYS_LOG_INFO_ID;
  /*
   * Use the run{|_.} integers to trigger the corresponding
   * tests, since those can be quite long, it's convenient to
   * enable/disable them when developping/testing.
   */
  int run = 1;
  int run_a = 0;
  int run_b = 0;
  int run_c = 0;

  memset (&param, 0, sizeof (_lw6_test_param_t));
  memset (&param_a, 0, sizeof (_lw6_test_param_t));
  memset (&param_b, 0, sizeof (_lw6_test_param_t));
  memset (&param_c, 0, sizeof (_lw6_test_param_t));

  if (lw6sys_false ())
    {
      /*
       * Just to make sure most functions are stuffed in the binary
       */
      lw6sys_test (mode);
      lw6glb_test (mode);
      lw6map_test (mode);
      lw6ker_test (mode);
      lw6gen_test (mode);
      lw6pil_test (mode);
      lw6bot_test (mode);
      lw6sim_test (mode);
      lw6cns_test (mode);
      lw6hlp_test (mode);
      lw6cfg_test (mode);
      lw6ldr_test (mode);
      lw6tsk_test (mode);
      lw6gui_test (mode);
      lw6vox_test (mode);
      lw6gfx_test (mode);
      lw6dsp_test (mode);
      lw6snd_test (mode);
      lw6img_test (mode);
      lw6net_test (mode);
      lw6nod_test (mode);
      lw6cnx_test (mode);
      lw6msg_test (mode);
      lw6cli_test (mode);
      lw6srv_test (mode);
      lw6dat_test (mode);
      lw6p2p_test (mode);
      lw6scm_test (mode);
    }

  if (lw6_init_global (argc, argv))
    {
      default_log_level_id = lw6sys_log_get_level ();

      param.argc = argc;
      param.argv = argv;
      param.mode = mode;
      param.suite = _TEST_SUITE_MAIN;
      param.coverage_check = 1;
      param.log_level_id = default_log_level_id;

      memcpy (&param_a, &param, sizeof (_lw6_test_param_t));
      memcpy (&param_b, &param, sizeof (_lw6_test_param_t));
      memcpy (&param_c, &param, sizeof (_lw6_test_param_t));
      param_a.suite = _TEST_SUITE_NODE_A;
      param_b.suite = _TEST_SUITE_NODE_B;
      param_c.suite = _TEST_SUITE_NODE_C;

      if (run)
	{
	  _guile_test_run (&param);
	}

      if (param.ret || !run)
	{
	  /*
	   * We do the network tests only if the other tests worked
	   * but still we perform client and server even if the first
	   * one failed, to (that's the hop...) get interesting informations
	   * in the logs on why the other test failed.
	   */
	  if (lw6sys_process_is_fully_supported ())
	    {
	      if (run_a)
		{
		  /*
		   * First network test, we launch node_a in the main thread,
		   * and fire node_b and node_c in separate threads.
		   * We won't get the result from the node_b and node_c but
		   * this is not a problem, we run the same test in other
		   * configurations to gather that output.
		   */
		  param_b.log_level_id = LW6SYS_LOG_ERROR_ID;
		  param_c.log_level_id = LW6SYS_LOG_ERROR_ID;
		  pid_b = lw6sys_process_fork_and_call (_guile_test_run,
							&param_b);
		  pid_c = lw6sys_process_fork_and_call (_guile_test_run,
							&param_c);
		  if (pid_b != 0 && pid_c != 0)
		    {
		      _guile_test_run (&param_a);
		    }
		  if (pid_b != 0)
		    {
		      lw6sys_process_kill_1_9 (pid_b);
		    }
		  if (pid_c != 0)
		    {
		      lw6sys_process_kill_1_9 (pid_c);
		    }
		  param_b.log_level_id = default_log_level_id;
		  param_c.log_level_id = default_log_level_id;
		}

	      if (run_b)
		{
		  /*
		   * Second network test, we launch node_b in the main thread,
		   * and fire node_c and node_a in separate threads.
		   * We won't get the result from the node_c and node_a but
		   * this is not a problem, we run the same test in other
		   * configurations to gather that output.
		   */
		  param_c.log_level_id = LW6SYS_LOG_ERROR_ID;
		  param_a.log_level_id = LW6SYS_LOG_ERROR_ID;
		  pid_c = lw6sys_process_fork_and_call (_guile_test_run,
							&param_c);
		  pid_a = lw6sys_process_fork_and_call (_guile_test_run,
							&param_a);
		  if (pid_c != 0 && pid_a != 0)
		    {
		      _guile_test_run (&param_b);
		    }
		  if (pid_c != 0)
		    {
		      lw6sys_process_kill_1_9 (pid_c);
		    }
		  if (pid_a != 0)
		    {
		      lw6sys_process_kill_1_9 (pid_a);
		    }
		  param_c.log_level_id = default_log_level_id;
		  param_a.log_level_id = default_log_level_id;
		}

	      if (run_c)
		{
		  /*
		   * Third network test, we launch node_c in the main thread,
		   * and fire node_a and node_b in separate threads.
		   * We won't get the result from the node_a and node_b but
		   * this is not a problem, we run the same test in other
		   * configurations to gather that output.
		   */
		  param_a.log_level_id = LW6SYS_LOG_ERROR_ID;
		  param_b.log_level_id = LW6SYS_LOG_ERROR_ID;
		  pid_a = lw6sys_process_fork_and_call (_guile_test_run,
							&param_a);
		  pid_b = lw6sys_process_fork_and_call (_guile_test_run,
							&param_b);
		  if (pid_a != 0 && pid_b != 0)
		    {
		      _guile_test_run (&param_c);
		    }
		  if (pid_a != 0)
		    {
		      lw6sys_process_kill_1_9 (pid_a);
		    }
		  if (pid_b != 0)
		    {
		      lw6sys_process_kill_1_9 (pid_b);
		    }
		  param_a.log_level_id = default_log_level_id;
		  param_b.log_level_id = default_log_level_id;
		}
	    }
	  else
	    {
	      lw6sys_log (LW6SYS_LOG_WARNING,
			  _x_
			  ("skipping client/server test, platform does not have adequate process support and/or it's likely to fail anyway"));
	      param_a.ret = 1;
	      param_b.ret = 1;
	      param_c.ret = 1;
	    }
	}

      lw6_quit_global ();
    }

  //  ret = param.ret && param_a.ret && param_b.ret && param_c.ret;
  ret = param.ret;		// OK, temporary disabled to build, serious WIP

  if (ret)
    {
      lw6sys_log (LW6SYS_LOG_NOTICE, _x_ ("scripts tests OK"));
    }
  else
    {
      /*
       * Displaying a message detailing the status, this is important
       * else standard output is pretty much undreadable since one must
       * scroll up to the test that failed, this being akward.
       */
      lw6sys_log (LW6SYS_LOG_WARNING,
		  _x_
		  ("script tests failed main=%d node-a=%d node-b=%d node-c=%d"),
		  param.ret, param_a.ret, param_b.ret, param_c.ret);
    }

  return ret;
}
