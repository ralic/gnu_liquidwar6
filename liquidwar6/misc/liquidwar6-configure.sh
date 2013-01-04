#!/bin/sh

# Liquid War 6 is a unique multiplayer wargame.
# Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013  Christian Mauduit <ufoot@ufoot.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
# Liquid War 6 homepage : http://www.gnu.org/software/liquidwar6/
# Contact author        : ufoot@ufoot.org

BUILDDIR=/ramdisk/liquidwar6
SRCDIR=$HOME/Home/_/liquidwar6
INSTALLDIR=$HOME/Home/local/liquidwar6

if test -e $SRCDIR/configure.ac ; then
	cd $SRCDIR
	libtoolize -c --install
	aclocal -I m4 --install
	automake
	autoconf
	install -d $BUILDDIR
	cd $BUILDDIR
	$SRCDIR/configure --enable-profiler --prefix=$INSTALLDIR
	make clean > /dev/null
	make -j2
fi

