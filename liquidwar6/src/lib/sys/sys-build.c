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

#include <string.h>

#include "sys.h"
#include "sys-build.h"

#define COPYRIGHT "Copyright (C)  2010  Christian Mauduit <ufoot@ufoot.org>"
#define LICENSE "GPLv3+ (GNU GPL version 3 or later)"
#define URL "http://www.gnu.org/software/liquidwar6/"
#define TOP_SRCDIR_TEST_FILE "liquidwar6.spec.in"
#define TOP_SRCDIR_MAX_UNPARENT 4
#define ENABLE_YES "yes"
#define ENABLE_NO "no"

/**
 * lw6sys_build_get_package_tarname:
 *
 * Returns the name of the package. This is the @PACKAGE_TARNAME constant
 * defined by the GNU Autoconf ./configure script. While it's always possible
 * to use the defined constant directly, using this function will return
 * the value defined when compiling the binary, not the one you're using
 * when compiling another program relying on Liquid War as a library.
 *
 * Return value: a non-NULL string "liquidwar6", must not be freed.
 */
char *
lw6sys_build_get_package_tarname ()
{
  return PACKAGE_TARNAME;
}

/**
 * lw6sys_build_get_package_name:
 *
 * Returns the name of the package, in a user friendly form, which can
 * include spaces, for instance. This is the @PACKAGE_NAME constant
 * defined by the GNU Autoconf ./configure script. While it's always possible
 * to use the defined constant directly, using this function will return
 * the value defined when compiling the binary, not the one you're using
 * when compiling another program relying on Liquid War as a library.
 *
 * Return value: a non-NULL string "Liquid War 6", must not be freed.
 */
char *
lw6sys_build_get_package_name ()
{
  return PACKAGE_NAME;
}

/**
 * lw6sys_build_get_package_string:
 *
 * Returns the description of the package. This is the @PACKAGE_STRING constant
 * defined by the GNU Autoconf ./configure script. It's the concatenation
 * of @PACKAGE_NAME and @VERSION. While it's always possible
 * to use the defined constant directly, using this function will return
 * the value defined when compiling the binary, not the one you're using
 * when compiling another program relying on Liquid War as a library.
 *
 * Return value: a non-NULL string "Liquid War 6 <version>", must not be freed.
 */
char *
lw6sys_build_get_package_string ()
{
  return PACKAGE_STRING;
}

/**
 * lw6sys_build_get_version:
 *
 * Returns the version of the program. This is the @VERSION constant
 * defined by the GNU Autoconf ./configure script. Same as @PACKAGE_VERSION.
 * Note that while using a function to get @PACKAGE_TARNAME might seem
 * useless, having both ways to get the version, that is, a function and
 * a constant, is very usefull. Think, for instance, that a dynamically
 * loaded shared library might need to check its own version against the
 * version of the core program.
 *
 * Return value: a non-NULL string, which must not be freed.
 */
char *
lw6sys_build_get_version ()
{
  return VERSION;
}

/**
 * lw6sys_build_get_codename:
 *
 * Returns the the program codename. This is the little name of the
 * version. It's been decided that all LW6 releases would take the
 * name of a famous general, warrior, whatever. For instance, it could
 * be "Napoleon".
 *
 * Return value: a non-NULL string, traditionnally the name of a famous
 *   general, someone which has been involved in war. Must not be freed
 *   (I mean, the string, not the general).
 */
char *
lw6sys_build_get_codename ()
{
  return LW6_CODENAME;
}

/**
 * lw6sys_build_get_stamp:
 *
 * Returns the program stamp. This is like a serial number. It's is not
 * the same as the version. The version is meant to be set to something
 * readable. This is just a cryptic thing, incremented at each ./configure
 * or each developper's "I feel like it needs to be incremented". The
 * idea is just to keep (one more...) track of which source code is build.
 * Ideally, this would be plugged to the source revision control system
 * but this has some drawbacks, including that it would require it to
 * modify files before commiting them, which is not safe, and almost
 * impossible if you sign archives. One more point: this is a string. It's
 * true the return value is actually a string containing the representation
 * of an integer, but because all other build parameters are strings, and
 * because we don't know what the future reserves, it's a string.
 *
 * Return value: a non-NULL string like "42", which must not be freed.
 */
char *
lw6sys_build_get_stamp ()
{
  return LW6_STAMP;
}

/**
 * lw6sys_build_get_md5sum:
 *
 * Returns an md5 checkum which is caculated from C (.c and .h) source
 * files. This is complementary with the build stamp. By default the stamp
 * will be enough to check what has been compiled, but one can always
 * imagine a case where Bob compiles something a little different than Alice,
 * with the same stamp, incremented by 1 from a common source tree. They
 * apply their own patches, for instance. This md5sum double-checks that
 * two binaries have been built from the same sources. Note that this
 * is not the md5 checksum of the generated binary. Nor does it include
 * any information about scheme scripts and data.
 *
 * Return value: a non-NULL string, which must not be freed.
 */
char *
lw6sys_build_get_md5sum ()
{
  return LW6_MD5SUM;
}

/**
 * lw6sys_build_get_copyright:
 *
 * Returns a (very) short copyright information about the program. 
 *
 * Return value: a non-NULL string, single line whithout '\n' at the end. 
 *   Must not be freed.
 */
char *
lw6sys_build_get_copyright ()
{
  return COPYRIGHT;
}

/**
 * lw6sys_build_get_license:
 *
 * Returns the license for the program (GNU GPL v3 or later).
 *
 * Return value: a non-NULL string, single line whithout '\n' at the end. 
 *   Must not be freed.
 */
char *
lw6sys_build_get_license ()
{
  return LICENSE;
}

/**
 * lw6sys_build_get_url:
 *
 * Returns the URL of the game, its homepage.
 *
 * Return value: a non-NULL string, single line whithout '\n' at the end. 
 *   Must not be freed.
 */
char *
lw6sys_build_get_url ()
{
  return URL;
}

/**
 * lw6sys_build_get_configure_args:
 *
 * Returns the arguments passed to the GNU Autoconf ./configure script when
 * buildling the game. Very usefull to know how the binary was generated,
 * that is, what kind of optimizations are peculiar settings it uses.
 *
 * Return value: a non-NULL string, which, passed to ./configure again, would
 *   hopefully generate the same binary. Must not be freed.
 */
char *
lw6sys_build_get_configure_args ()
{
  return LW6_CONFIGURE_ARGS;
}

/**
 * lw6sys_build_get_gcc_version:
 *
 * Returns __VERSION__ GCC preprocessor value, that is, the human readable
 * version of the compiler.
 *
 * Return value: a non-NULL string, must not be freed.
 */
char *
lw6sys_build_get_gcc_version ()
{
#ifdef __GNUC__
  return __VERSION__;
#else
  return "?";
#endif
}

/**
 * lw6sys_build_get_cflags:
 *
 * Returns the arguments which would allow another program to
 * use liquidwar6 as a library. Typically, pass this to gcc when
 * compiling your sources. Basically contains "-I" switches
 * which tell where the headers are.
 *
 * Return value: a non-NULL string, which must not be freed.
 */
char *
lw6sys_build_get_cflags ()
{
  return "-I" LW6_INCLUDEDIR " -I" LW6_INCLUDEDIR "/" PACKAGE_TARNAME " "
    LW6_CFLAGS;
}

/**
 * lw6sys_build_get_ldflags:
 *
 * Returns the arguments which would allow another program to
 * link against liquidwar6. Pass this to gcc or libtool when
 * compiling your program. Basically contains a "-L" option
 * which says where the library is. Note that this will only
 * allow you to link against the main libliquidwar6 library,
 * but not the dynamically loaded modules.
 *
 * Return value: a non-NULL string, which must not be freed.
 */
char *
lw6sys_build_get_ldflags ()
{
  return "-L" LW6_LIBDIR " -l" PACKAGE_TARNAME " " LW6_LDFLAGS;
}

/**
 * lw6sys_build_get_hostname:
 *
 * Returns the value return by the standard shell @hostname command
 * on the machine where the game has been built. Usefull to track binaries
 * and know where do they come from.
 *
 * Return value: a non-NULL string, must not be freed.
 */
char *
lw6sys_build_get_hostname ()
{
  return LW6_HOSTNAME;
}

/**
 * lw6sys_build_get_date:
 *
 * Returns the compilation date. While this information can easily be
 * obtained with the C @__DATE__ macro, having this function is convenient
 * for it returns a value which is the same for the whole program, and does
 * not possibly change in every file.
 *
 * Return value: a non-NULL string, must not be freed.
 */
char *
lw6sys_build_get_date ()
{
  return __DATE__;
}

/**
 * lw6sys_build_get_date:
 *
 * Returns the compilation date. While this information can easily be
 * obtained with the C @__TIME__ macro, having this function is convenient
 * for it returns a value which is the same for the whole program, and does
 * not possibly change in every file.
 *
 * Return value: a non-NULL string, must not be freed.
 */
char *
lw6sys_build_get_time ()
{
  return __TIME__;
}

/**
 * lw6sys_build_get_host_cpu:
 *
 * Returns the CPU this program is designed for. Convenient on i386 compatible
 * CPUs to know which flavor (i386, i586...) the binary is made for.
 *
 * Return value: a non-NULL string, must not be freed.
 */
char *
lw6sys_build_get_host_cpu ()
{
  return LW6_HOST_CPU;
}

/**
 * lw6sys_build_get_endianness:
 *
 * Returns the endianness of the computer.
 *
 * Return value: 'little' (x86-like) or 'big' (ppc-like), as a string.
 *   Must not be freed.
 */
char *
lw6sys_build_get_endianness ()
{
  char *ret = "little";		// do not translate this, could be used by another program

  if (lw6sys_is_big_endian ())
    {
      ret = "big";		// do not translate this, could be used by another program
    }

  return ret;
}

/**
 * lw6sys_build_get_pointer_size:
 *
 * Returns the system pointer size, in bytes.
 *
 * Return value: 4 for 32-bit, 8 for 64-bit.
 */
int
lw6sys_build_get_pointer_size ()
{
  return sizeof (void *);
}

/**
 * lw6sys_build_is_x86:
 *
 * Tells wether CPU belongs to x86 family or not.
 *
 * Return value: 1 if x86, 0 if not
 */
int
lw6sys_build_is_x86 ()
{
#ifdef LW6_X86
  return 1;
#else
  return 0;
#endif
}

/**
 * lw6sys_build_get_host_os:
 *
 * Returns the OS this program is designed for. Usefull for bug reports.
 *
 * Return value: a non-NULL string, must not be freed.
 */
char *
lw6sys_build_get_host_os ()
{
  return LW6_HOST_OS;
}

/**
 * lw6sys_build_is_ms_windows:
 *
 * Tells wether the program was compiled for Microsoft Windows, or not.
 *
 * Return value: 1 if compiled on windows, 0 if not
 */
int
lw6sys_build_is_ms_windows ()
{
#ifdef LW6_MS_WINDOWS
  return 1;
#else
  return 0;
#endif
}

/**
 * lw6sys_build_is_mac_os_x:
 *
 * Tells wether the program was compiled for Mac OS X, or not.
 *
 * Return value: 1 if compiled on OS X, 0 if not
 */
int
lw6sys_build_is_mac_os_x ()
{
#ifdef LW6_MAC_OS_X
  return 1;
#else
  return 0;
#endif
}

/**
 * lw6sys_build_is_gp2x:
 *
 * Tells wether the program was compiled for GP2X, or not.
 *
 * Return value: 1 if compiled on OS X, 0 if not
 */
int
lw6sys_build_is_gp2x ()
{
#ifdef LW6_GP2X
  return 1;
#else
  return 0;
#endif
}

/**
 * lw6sys_build_get_top_srcdir:
 *
 * Returns the top source directory, when the game was built. This can
 * seem useless and non relevant on the end-user's machine, but... it's
 * a must-have for developpers and packagers. Without this, binaries
 * would never find their associated data, especially when building
 * outside the source tree. Or, testing the game would be impossible
 * without installing it, given the fact that most of the code is in
 * scripts that are stored in /usr/local by default, this would be
 * painfull. So this function is here to help finding data within the
 * source tree when the game is not installed yet. Note that the function
 * is rather clever, since it will automatically try to remove useless
 * '../' sequences at the beginning of a possibly relative path.
 *
 * Return value: a non-NULL string, must not be freed.
 */
char *
lw6sys_build_get_top_srcdir ()
{
  char *top_srcdir = NULL;
  char *test_dir = NULL;
  char *test_file = NULL;
  int i = 0;

  test_dir = LW6_TOP_SRCDIR;
  while (i < TOP_SRCDIR_MAX_UNPARENT && !top_srcdir)
    {
      test_file = lw6sys_path_concat (test_dir, TOP_SRCDIR_TEST_FILE);
      if (test_file)
	{
	  if (lw6sys_file_exists (test_file))
	    {
	      top_srcdir = test_dir;
	    }
	  LW6SYS_FREE (test_file);
	}
      test_dir = lw6sys_path_unparent_no_malloc (test_dir);
      ++i;
    }

  if (!top_srcdir)
    {
      top_srcdir = LW6_TOP_SRCDIR;
    }

  return top_srcdir;
}

/**
 * lw6sys_build_get_prefix:
 *
 * Returns the @prefix value as given to the GNU Autoconf ./configure script.
 * Used to deduce the path to other directories and files.
 *
 * Return value: a non-NULL string, "/usr/local" by default. 
 *   Must not be freed.
 */
char *
lw6sys_build_get_prefix ()
{
  return LW6_PREFIX;
}

/**
 * lw6sys_build_get_datadir:
 *
 * Returns the @datadir value defined by the GNU Autoconf ./configure script.
 * This is not the value which can be overriden by the Liquid War 6 specific.
 * "--data-dir" option. @datadir is usually something like "/usr/local/share"
 * while the actual Liquid War 6 defined data dir is a more profound path
 * which includes the name of the package, its version, and so on.
 *
 * Return value: a non-NULL string, "/usr/local/share" by default.
 *   Must not be freed.
 */
char *
lw6sys_build_get_datadir ()
{
  return LW6_DATADIR;
}

/**
 * lw6sys_build_get_libdir:
 *
 * Returns the @libdir value defined by the GNU Autoconf ./configure script.
 * This is not the value which can be overriden by the Liquid War 6 specific.
 * "--mod-dir" option. @datadir is usually something like "/usr/local/lib"
 * while the actual Liquid War 6 defined module dir is a more profound path
 * which includes the name of the package, its version, and so on.
 *
 * Return value: a non-NULL string, "/usr/local/lib" by default.
 *   Must not be freed.
 */
char *
lw6sys_build_get_libdir ()
{
  return LW6_LIBDIR;
}

/**
 * lw6sys_build_get_includedir:
 *
 * Returns the @includedir value defined by the GNU Autoconf ./configure script.
 * As for other options, it's interesting to have this value, this enables
 * the program to inform people who want to hack the game of the place
 * headers are supposed to be installed.
 *
 * Return value: a non-NULL string, "/usr/local/include" by default.
 *   Must not be freed.
 */
char *
lw6sys_build_get_includedir ()
{
  return LW6_INCLUDEDIR;
}

/**
 * lw6sys_build_get_localedir
 *
 * Returns the @localedir value defined by the GNU Autoconf
 * ./configure script.
 * Used as an argument for gettext / libintl functions.
 *
 * Return value: a non-NULL string, "/usr/local/share/locale" by default.
 *   Must not be freed.
 */
char *
lw6sys_build_get_localedir ()
{
  return LW6_LOCALEDIR;
}

/**
 * lw6sys_build_get_docdir
 *
 * Returns the @docdir value defined by the GNU Autoconf
 * ./configure script.
 * Used to write consistent XML file headers.
 *
 * Return value: a non-NULL string, "/usr/local/share/doc/liquidwar6" by default.
 *   Must not be freed.
 */
char *
lw6sys_build_get_docdir ()
{
  return LW6_DOCDIR;
}

/**
 * lw6sys_build_get_enable_console
 *
 * Tells wether console is enabled or not.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_console ()
{
#ifdef LW6_CONSOLE
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_mod_gl
 *
 * Tells wether the graphical mod-gl backend was compiled.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_mod_gl ()
{
#ifdef MOD_GL
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_mod_csound
 *
 * Tells wether the audio mod-csound backend was compiled.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_mod_csound ()
{
#ifdef MOD_CSOUND
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_mod_ogg
 *
 * Tells wether the audio mod-ogg backend was compiled.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_mod_ogg ()
{
#ifdef MOD_OGG
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_mod_http
 *
 * Tells wether the network mod-http backend was compiled.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_mod_http ()
{
#ifdef MOD_HTTP
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_optimize
 *
 * Tells wether the game was compiled in optimize mode.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_optimize ()
{
#ifdef LW6_OPTIMIZE
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_allinone
 *
 * Tells wether the game was compiled in allinone mode.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_allinone ()
{
#ifdef LW6_ALLINONE
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_fullstatic
 *
 * Tells wether the game was compiled in fullstatic mode.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_fullstatic ()
{
#ifdef LW6_FULLSTATIC
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_paranoid
 *
 * Tells wether the game was compiled with paranoid memory management.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_paranoid ()
{
#ifdef LW6_PARANOID
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_gprof
 *
 * Tells wether the game was compiled with suitable informations for gprof.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_gprof ()
{
#ifdef LW6_GPROF
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_instrument
 *
 * Tells wether the game was compiled with the '-finstrument-fonctions' GCC flag.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_instrument ()
{
#ifdef LW6_INSTRUMENT
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_profiler
 *
 * Tells wether the game was compiled for later use with Google Profiler support.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_profiler ()
{
#ifdef LW6_PROFILER
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_gcov
 *
 * Tells wether the game was compiled with suitable informations for gcov.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_gcov ()
{
#ifdef LW6_GCOV
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_enable_valgrind
 *
 * Tells wether the game was compiled for later use with valgrind.
 *
 * Return value: "yes" or "no", must no be freed.
 */
char *
lw6sys_build_get_enable_valgrind ()
{
#ifdef LW6_VALGRIND
  return ENABLE_YES;		// do not translate this, could be interpreted by a script
#else
  return ENABLE_NO;		// do not translate this, could be interpreted by a script
#endif
}

/**
 * lw6sys_build_get_bin_id
 *
 * Returns the internal bin-id value, which does not mean anything
 * but changes at each build.
 *
 * Return value: an integer
 */
int
lw6sys_build_get_bin_id ()
{
  int ret = 0;
  u_int32_t checksum_global = 0;
  u_int32_t checksum_version = 0;
  u_int32_t ret_global = 0;
  u_int32_t ret_version = 0;

  /*
   * OK, yes, this is a little overkill for what there's to do.
   * But at least we're sure we didn't forget anything. If one
   * recompiles the game, for sure, bin-id changes.
   */
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_package_tarname ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_package_name ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_package_string ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_version ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_codename ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_stamp ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_md5sum ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_copyright ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_license ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_url ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_configure_args ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_gcc_version ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_cflags ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_ldflags ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_hostname ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_date ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_time ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_host_cpu ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_endianness ());
  lw6sys_checksum_update_int32 (&checksum_global,
				lw6sys_build_get_pointer_size ());
  lw6sys_checksum_update_int32 (&checksum_global, lw6sys_build_is_x86 ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_host_os ());
  lw6sys_checksum_update_int32 (&checksum_global,
				lw6sys_build_is_ms_windows ());
  lw6sys_checksum_update_int32 (&checksum_global,
				lw6sys_build_is_mac_os_x ());
  lw6sys_checksum_update_int32 (&checksum_global, lw6sys_build_is_gp2x ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_top_srcdir ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_prefix ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_datadir ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_libdir ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_includedir ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_localedir ());
  lw6sys_checksum_update_str (&checksum_global, lw6sys_build_get_docdir ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_console ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_mod_gl ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_mod_csound ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_mod_ogg ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_mod_http ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_optimize ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_allinone ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_fullstatic ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_paranoid ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_gprof ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_instrument ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_profiler ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_gcov ());
  lw6sys_checksum_update_str (&checksum_global,
			      lw6sys_build_get_enable_valgrind ());

  checksum_version = lw6sys_checksum_str (lw6sys_build_get_version ());

  /*
   * It can seem strange to include version *again* in another checksum
   * and "prefix" it. Why not use checksum_global only after all?
   * The reason is that most of the time, we want to track a version change.
   * We certainly don't want a collision between two binaries with different
   * versions. It's also easy to check the bin-id starts with the same
   * numbers for a given version. Yes, it's overkill to do all the above
   * for a few digits, but it's not that bad, in case of doubt there's always
   * '--pedigree'.
   */
  ret_version = 10000 * (10000 + checksum_version % 90000);
  ret_global = checksum_global % 10000;
  ret = ret_version + ret_global;

  return ret;
}

/**
 * lw6sys_build_log_all
 *
 * Dumps in the log file the whole program pedigree, host, modules,
 * that is, what are the values of all the build options. 
 * Usefull for bug reports.
 *
 * Return value: none.
 */
void
lw6sys_build_log_all ()
{
  lw6sys_log (LW6SYS_LOG_INFO, _("build package tarname is \"%s\""),
	      lw6sys_build_get_package_tarname ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build package name is \"%s\""),
	      lw6sys_build_get_package_name ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build package string is \"%s\""),
	      lw6sys_build_get_package_string ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build version is \"%s\""),
	      lw6sys_build_get_version ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build codename is \"%s\""),
	      lw6sys_build_get_codename ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build stamp is \"%s\""),
	      lw6sys_build_get_stamp ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build md5sum is \"%s\""),
	      lw6sys_build_get_md5sum ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build copyright is \"%s\""),
	      lw6sys_build_get_copyright ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build license is \"%s\""),
	      lw6sys_build_get_license ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build url is \"%s\""),
	      lw6sys_build_get_url ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build configure args is \"%s\""),
	      lw6sys_build_get_configure_args ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build gcc_version is \"%s\""),
	      lw6sys_build_get_gcc_version ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build cflags is \"%s\""),
	      lw6sys_build_get_cflags ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build ldflags is \"%s\""),
	      lw6sys_build_get_ldflags ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build date is \"%s\""),
	      lw6sys_build_get_date ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build time is \"%s\""),
	      lw6sys_build_get_time ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build host cpu is \"%s\""),
	      lw6sys_build_get_host_cpu ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build endianness is \"%s\""),
	      lw6sys_build_get_endianness ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build pointer size is %d"),
	      lw6sys_build_get_pointer_size ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build host os is \"%s\""),
	      lw6sys_build_get_host_os ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build ms windows is %d"),
	      lw6sys_build_is_ms_windows ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build mac os x is %d"),
	      lw6sys_build_is_mac_os_x ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build gp2x is %d"),
	      lw6sys_build_is_gp2x ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build top_srcdir is \"%s\""),
	      lw6sys_build_get_top_srcdir ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build prefix is \"%s\""),
	      lw6sys_build_get_prefix ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build datadir is \"%s\""),
	      lw6sys_build_get_datadir ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build libdir is \"%s\""),
	      lw6sys_build_get_libdir ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build includedir is \"%s\""),
	      lw6sys_build_get_includedir ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build localedir is \"%s\""),
	      lw6sys_build_get_localedir ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build docdir is \"%s\""),
	      lw6sys_build_get_docdir ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable console is \"%s\""),
	      lw6sys_build_get_enable_console ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable mod-gl is \"%s\""),
	      lw6sys_build_get_enable_mod_gl ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable mod-csound is \"%s\""),
	      lw6sys_build_get_enable_mod_csound ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable mod-ogg is \"%s\""),
	      lw6sys_build_get_enable_mod_ogg ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable mod-http is \"%s\""),
	      lw6sys_build_get_enable_mod_http ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable optimize is \"%s\""),
	      lw6sys_build_get_enable_optimize ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable allinone is \"%s\""),
	      lw6sys_build_get_enable_allinone ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable fullstatic is \"%s\""),
	      lw6sys_build_get_enable_fullstatic ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable paranoid is \"%s\""),
	      lw6sys_build_get_enable_paranoid ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable gprof is \"%s\""),
	      lw6sys_build_get_enable_gprof ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable instrument is \"%s\""),
	      lw6sys_build_get_enable_instrument ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable profiler is \"%s\""),
	      lw6sys_build_get_enable_profiler ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable gcov is \"%s\""),
	      lw6sys_build_get_enable_gcov ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build enable valgrind is \"%s\""),
	      lw6sys_build_get_enable_valgrind ());
  lw6sys_log (LW6SYS_LOG_INFO, _("build bin-id is %d"),
	      lw6sys_build_get_bin_id ());
}
