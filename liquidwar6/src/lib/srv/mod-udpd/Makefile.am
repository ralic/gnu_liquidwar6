# Liquid War 6 is a unique multiplayer wargame.
# Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015  Christian Mauduit <ufoot@ufoot.org>
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

AM_CFLAGS = @LW6_CFLAGS@
AM_LDFLAGS = @LW6_LDFLAGS@

libdir = @libdir@/@PACKAGE_TARNAME@-@LW6_VERSION_BASE@/srv

noinst_LTLIBRARIES = libmod_udpd_s.la
MOD_UDPD_FILES = mod-udpd-backend.c mod-udpd-data.c mod-udpd-handshake.c mod-udpd-info.c mod-udpd-message.c mod-udpd-oob.c mod-udpd-setup.c mod-udpd-state.c
libmod_udpd_s_la_SOURCES = $(MOD_UDPD_FILES) 

if !ALLINONE
lib_LTLIBRARIES = libmod_udpd.la
libmod_udpd_la_SOURCES = $(MOD_UDPD_FILES) 
libmod_udpd_la_LDFLAGS = -release @PACKAGE_VERSION@ @LW6_LDFLAGS@
libmod_udpddir = $(includedir)/@PACKAGE_TARNAME@/srv/mod-udpd
libmod_udpd_HEADERS = mod-udpd.h
endif # !ALLINONE

EXTRA_DIST = mod-udpd-internal.h
