#!/bin/sh

# Liquid War 6 is a unique multiplayer wargame.
# Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010, 2011  Christian Mauduit <ufoot@ufoot.org>
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

echo "# Automatically generated by $0, do not edit." > Makefile.selfdoc
echo >> Makefile.selfdoc

for i in quick doc show aliases ; do
    echo "\$(srcdir)/$i-selfdoc.texi: \$(top_srcdir)/src/lib/hlp/hlp-reference.c" >> Makefile.selfdoc
    echo "\techo selfdoc $i && for k in \`\$(top_builddir)/src/liquidwar6 --list-$i\` ; do \$(PERL) \$(top_srcdir)/doc/selfdoc.pl \$(top_builddir)/src/liquidwar6 cmd \$\$k ; done > \$(srcdir)/$i-selfdoc.texi" >> Makefile.selfdoc
    echo >> Makefile.selfdoc
done

for i in path players input graphics sound network map map-rules map-hints map-style hooks advanced ; do
    echo "\$(srcdir)/$i-selfdoc.texi: \$(top_srcdir)/src/lib/hlp/hlp-reference.c" >> Makefile.selfdoc
    echo "\techo selfdoc $i && for k in \`\$(top_builddir)/src/liquidwar6 --list-$i\` ; do \$(PERL) \$(top_srcdir)/doc/selfdoc.pl \$(top_builddir)/src/liquidwar6 default \$\$k ; done > \$(srcdir)/$i-selfdoc.texi" >> Makefile.selfdoc
    echo >> Makefile.selfdoc
done

for i in funcs ; do
    echo "\$(srcdir)/$i-selfdoc.texi: \$(top_srcdir)/src/lib/hlp/hlp-reference.c" >> Makefile.selfdoc
    echo "\techo selfdoc $i && for k in \`\$(top_builddir)/src/liquidwar6 --list-$i\` ; do \$(PERL) \$(top_srcdir)/doc/selfdoc.pl \$(top_builddir)/src/liquidwar6 func \$\$k ; done > \$(srcdir)/$i-selfdoc.texi" >> Makefile.selfdoc
    echo >> Makefile.selfdoc
done

