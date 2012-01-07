#!/bin/sh

# Liquid War 6 is a unique multiplayer wargame.
# Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012  Christian Mauduit <ufoot@ufoot.org>
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

echo "# Automatically generated by $0, do not edit." > Makefile.gdoc
echo >> Makefile.gdoc

echo "\$(srcdir)/liquidwar6-gdoc.texi: "`ls ../src/lib/*.c* | sed "s/\.\./\$\(top_srcdir\)/g" | sort` >> Makefile.gdoc
echo "\techo gdoc $i && cd \$(top_srcdir)/doc && \$(PERL) \$(top_srcdir)/doc/gdoc.pl -texinfo \$^ > \$(top_srcdir)/doc/liquidwar6-gdoc.texi" >> Makefile.gdoc
echo >> Makefile.gdoc
perl gdoc.pl -texinfo ../src/lib/*.c* > liquidwar6-gdoc.texi

for i in bot cfg cli cns cnx dat dsp dyn gfx glb gui hlp img ker ldr map msg net nod p2p pil scm sim snd srv sys tsk vox ; do
    echo "\$(srcdir)/$i-gdoc.texi: "`ls ../src/lib/$i/*.c* | sed "s/\.\./\$\(top_srcdir\)/g" | sort` >> Makefile.gdoc
    echo "\techo gdoc $i && cd \$(top_srcdir)/doc && \$(PERL) \$(top_srcdir)/doc/gdoc.pl -texinfo \$^ > \$(top_srcdir)/doc/$i-gdoc.texi" >> Makefile.gdoc
    echo >> Makefile.gdoc
    perl gdoc.pl -texinfo ../src/lib/$i/*.c* > $i-gdoc.texi
done

for i in bot/mod-brute bot/mod-follow bot/mod-idiot bot/mod-random gfx/mod-gl snd/mod-csound snd/mod-ogg cli/mod-tcp cli/mod-udp cli/mod-http srv/mod-tcpd srv/mod-udpd srv/mod-httpd ; do
    j=`echo $i | cut -d "/" -f 2`
    echo "\$(srcdir)/$j-gdoc.texi: "`find ../src/lib/$i -name "*.c*" | sed "s/\.\./\$\(top_srcdir\)/g" | sort` >> Makefile.gdoc
    echo "\techo gdoc $j && cd \$(top_srcdir)/doc && \$(PERL) \$(top_srcdir)/doc/gdoc.pl -texinfo \$^ > \$(top_srcdir)/doc/$j-gdoc.texi" >> Makefile.gdoc
    echo >> Makefile.gdoc
    perl gdoc.pl -texinfo `find ../src/lib/$i -name "*.c*" | sort` > $j-gdoc.texi
done

