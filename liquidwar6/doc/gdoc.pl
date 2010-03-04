#!/usr/bin/perl

## Copyright (c) 2002, 2003 Simon Josefsson                      ##
##                    added -texinfo, -listfunc                  ##
##                    man page revamp                            ##
##                    various improvements                       ##
## Copyright (c) 1998 Michael Zucchi, All Rights Reserved        ##
##                    hacked to allow -tex option --nmav         ##
##                                                               ##
## This software falls under the GNU Public License. Please read ##
##              the COPYING file for more information            ##

#
# This will read a 'c' file and scan for embedded comments in the
# style of gnome comments (+minor extensions - see below).
#
# This program is modified by Nikos Mavroyanopoulos, for the gnutls
# project.

# Note: This only supports 'c'.

# usage:
# gdoc [ -docbook | -html | -text | -man | -tex | -texinfo | -listfunc ]
#      [ -sourceversion verno ] [ -includefuncprefix ] [ -bugsto address ]
#      [ -seeinfo infonode ] [ -copyright notice ] [ -verbatimcopying ]
#      [ -function funcname [ -function funcname ...] ] c file(s)s > outputfile
#
#  Set output format using one of -docbook, -html, -text, -man, -tex,
#  -texinfo, or -listfunc.  Default is man.
#
#  -sourceversion
#       Version number for source code, e.g. '1.0.4'.  Used in 'man' headers.
#       Defaults to using current date.
#
#  -includefuncprefix
#       For man pages, generate a #include <FILE.h> based on the function
#       prefix.  For example, a function gss_init_sec_context will generate
#       an include statement of #include <gss.h>.
#
#  -bugsto address
#       For man pages, include a section about reporting bugs and mention
#       the given e-mail address, e.g 'bug-libidn@gnu.org'.
#
#  -seeinfo infonode
#       For man pages, include a section that point to an info manual
#       for more information.
#
#  -copyright notice
#       For man pages, include a copyright section with the given
#       notice after a preamble.  Use, e.g., '2002, 2003 Simon Josefsson'.
#
#  -verbatimcopying
#       For man pages, and when the -copyright parameter is used,
#       add a licensing statement that say verbatim copying is permitted.
#
#  -function funcname
#	If set, then only generate documentation for the given function(s).  All
#	other functions are ignored.
#
#  c files - list of 'c' files to process
#
#  All output goes to stdout, with errors to stderr.

#
# format of comments.
# In the following table, (...)? signifies optional structure.
#                         (...)* signifies 0 or more structure elements
# /**
#  * function_name(:)? (- short description)?
# (* @parameterx: (description of parameter x)?)*
# (* a blank line)?
#  * (Description:)? (Description of function)?
#  * (Section header: (section description)? )*
#  (*)?*/
#
# So .. the trivial example would be:
#
# /**
#  * my_function
#  **/
#
# If the Description: header tag is ommitted, then there must be a blank line
# after the last parameter specification.
# e.g.
# /**
#  * my_function - does my stuff
#  * @my_arg: its mine damnit
#  *
#  * Does my stuff explained. 
#  */
#
#  or, could also use:
# /**
#  * my_function - does my stuff
#  * @my_arg: its mine damnit
#  * Description: Does my stuff explained. 
#  */
# etc.
#
# All descriptions can be multiline, apart from the short function description.
#
# All descriptive text is further processed, scanning for the following special
# patterns, which are highlighted appropriately.
#
# 'funcname()' - function
# '$ENVVAR' - environmental variable
# '&struct_name' - name of a structure
# '@parameter' - name of a parameter
# '%CONST' - name of a constant.

#
# Extensions for LaTeX:
#
# 1. the symbol '->' will be replaced with a rightarrow
# 2. x^y with ${x}^{y}$.
# 3. xxx\: with xxx:

use POSIX qw(strftime);

# match expressions used to find embedded type information
$type_constant = "\\\%(\\w+)";
$type_func = "(\\w+\\(\\))";
$type_param = "\\\@(\\w+)";
$type_struct = "\\\&(\\w+)";
$type_env = "(\\\$\\w+)";


# Output conversion substitutions.
#  One for each output format

# these work fairly well
%highlights_html = ( $type_constant, "<i>\$1</i>",
		     $type_func, "<b>\$1</b>",
		     $type_struct, "<i>\$1</i>",
		     $type_param, "<tt><b>\$1</b></tt>" );
$blankline_html = "<p>";

%highlights_texinfo = ( $type_constant, "\\\@var{\$1}",
			$type_func, "\\\@code{\$1}",
			$type_struct, "\\\@code{\$1}",
			$type_param, "\\\@code{\$1}" );
$blankline_texinfo = "";

%highlights_tex = ( $type_constant, "{\\\\it \$1}",
		     $type_func, "{\\\\bf \$1}",
		     $type_struct, "{\\\\it \$1}",
		     $type_param, "{\\\\bf \$1}" );
$blankline_tex = "\\\\";

# sgml, docbook format
%highlights_sgml = ( $type_constant, "<replaceable class=\"option\">\$1</replaceable>",
		     $type_func, "<function>\$1</function>",
		     $type_struct, "<structname>\$1</structname>",
		     $type_env, "<envar>\$1</envar>",
		     $type_param, "<parameter>\$1</parameter>" );
$blankline_sgml = "</para><para>\n";

# these are pretty rough
%highlights_man = ( $type_constant, "\\n.I \\\"\$1\\\"\\n",
		    $type_func, "\\n.B \\\"\$1\\\"\\n",
		    $type_struct, "\\n.I \\\"\$1\\\"\\n",
		    $type_param."([\.\, ]*)\n?", "\\n.I \\\"\$1\$2\\\"\\n" );
$blankline_man = "";

# text-mode
%highlights_text = ( $type_constant, "\$1",
		     $type_func, "\$1",
		     $type_struct, "\$1",
		     $type_param, "\$1" );
$blankline_text = "";


sub usage {
    print "Usage: $0 [ -v ] [ -docbook | -html | -text | -man | -tex | -texinfo  -listfunc ]\n";
    print "         [ -sourceversion verno ] [ -includefuncprefix ]\n";
    print "         [ -bugsto address ] [ -seeinfo infonode ] [ -copyright notice]\n";
    print "         [ -verbatimcopying ]\n";
    print "         [ -function funcname [ -function funcname ...] ]\n";
    print "         c source file(s) > outputfile\n";
    exit 1;
}

# read arguments
if ($#ARGV==-1) {
    usage();
}

$verbose = 0;
$output_mode = "man";
%highlights = %highlights_man;
$blankline = $blankline_man;
$modulename = "API Documentation";
$sourceversion = strftime "%Y-%m-%d", localtime;
$function_only = 0;
while ($ARGV[0] =~ m/^-(.*)/) {
    $cmd = shift @ARGV;
    if ($cmd eq "-html") {
	$output_mode = "html";
	%highlights = %highlights_html;
	$blankline = $blankline_html;
    } elsif ($cmd eq "-man") {
	$output_mode = "man";
	%highlights = %highlights_man;
	$blankline = $blankline_man;
    } elsif ($cmd eq "-tex") {
	$output_mode = "tex";
	%highlights = %highlights_tex;
	$blankline = $blankline_tex;
    } elsif ($cmd eq "-texinfo") {
	$output_mode = "texinfo";
	%highlights = %highlights_texinfo;
	$blankline = $blankline_texinfo;
    } elsif ($cmd eq "-text") {
	$output_mode = "text";
	%highlights = %highlights_text;
	$blankline = $blankline_text;
    } elsif ($cmd eq "-docbook") {
	$output_mode = "sgml";
	%highlights = %highlights_sgml;
	$blankline = $blankline_sgml;
    } elsif ($cmd eq "-listfunc") {
	$output_mode = "listfunc";
    } elsif ($cmd eq "-module") { # not needed for sgml, inherits from calling document
	$modulename = shift @ARGV;
    } elsif ($cmd eq "-sourceversion") {
	$sourceversion = shift @ARGV;
    } elsif ($cmd eq "-includefuncprefix") {
	$includefuncprefix = 1;
    } elsif ($cmd eq "-bugsto") {
	$bugsto = shift @ARGV;
    } elsif ($cmd eq "-copyright") {
	$copyright = shift @ARGV;
    } elsif ($cmd eq "-verbatimcopying") {
	$verbatimcopying = 1;
    } elsif ($cmd eq "-seeinfo") {
	$seeinfo = shift @ARGV;
    } elsif ($cmd eq "-function") { # to only output specific functions
	$function_only = 1;
	$function = shift @ARGV;
	$function_table{$function} = 1;
    } elsif ($cmd eq "-v") {
	$verbose = 1;
    } elsif (($cmd eq "-h") || ($cmd eq "--help")) {
	usage();
    }
}

##
# dumps section contents to arrays/hashes intended for that purpose.
#
sub dump_section {
    my $name = shift @_;
    my $contents = join "\n", @_;

    if ($name =~ m/$type_constant/) {
	$name = $1;
#	print STDERR "constant section '$1' = '$contents'\n";
	$constants{$name} = $contents;
    } elsif ($name =~ m/$type_param/) {
#	print STDERR "parameter def '$1' = '$contents'\n";
	$name = $1;
	$parameters{$name} = $contents;
    } else {
#	print STDERR "other section '$name' = '$contents'\n";
	$sections{$name} = $contents;
	push @sectionlist, $name;
    }
}

##
# output function
#
# parameters, a hash.
#  function => "function name"
#  parameterlist => @list of parameters
#  parameters => %parameter descriptions
#  sectionlist => @list of sections
#  sections => %descriont descriptions
#  

sub repstr {
    $pattern = shift;
    $repl = shift;
    $match1 = shift;
    $match2 = shift;
    $match3 = shift;
    $match4 = shift;

    $output = $repl;
    $output =~ s,\$1,$match1,g;
    $output =~ s,\$2,$match2,g;
    $output =~ s,\$3,$match3,g;
    $output =~ s,\$4,$match4,g;

    eval "\$return = qq/$output/";

#    print "pattern $pattern matched 1=$match1 2=$match2 3=$match3 4=$match4 replace $repl yielded $output interpolated $return\n";

    $return;
}

sub output_highlight {
    my $contents = join "\n", @_;
    my $line;

    foreach $pattern (keys %highlights) {
#	print "scanning pattern $pattern ($highlights{$pattern})\n";
	$contents =~ s:$pattern:repstr($pattern, $highlights{$pattern}, $1, $2, $3, $4):gse;
    }
    foreach $line (split "\n", $contents) {
	if ($line eq ""){
	    print $lineprefix, $blankline;
	} else {
	    print $lineprefix, $line;
	}
	print "\n";
    }
}

sub just_highlight {
    my $contents = join "\n", @_;
    my $line;
    my $ret = "";

    foreach $pattern (keys %highlights) {
#	print "scanning pattern $pattern ($highlights{$pattern})\n";
	$contents =~ s:$pattern:repstr($pattern, $highlights{$pattern}, $1, $2, $3, $4):gse;
    }
    foreach $line (split "\n", $contents) {
	if ($line eq ""){
	    $ret = $ret . $lineprefix . $blankline;
	} else {
	    $ret = $ret . $lineprefix . $line;
	}
	$ret = $ret . "\n";
    }

    return $ret;
}

# output in texinfo
sub output_texinfo {
    my %args = %{$_[0]};
    my ($parameter, $section);
    my $count;

    print "\@deftypefun {" . $args{'functiontype'} . "} ";
    print "{".$args{'function'}."} ";
    print "(";
    $count = 0;
    foreach $parameter (@{$args{'parameterlist'}}) {
	print $args{'parametertypes'}{$parameter}." \@var{".$parameter."}";
	if ($count != $#{$args{'parameterlist'}}) {
	    $count++;
	    print ", ";
	}
    }
    print ")\n";
    foreach $parameter (@{$args{'parameterlist'}}) {
	if ($args{'parameters'}{$parameter}) {
	    print "\@var{".$parameter."}: ";
	    output_highlight($args{'parameters'}{$parameter});
	    print "\n";
	}
    }
    # double description output, hack follows
    # http://www.perl.com/doc/FAQs/FAQ/oldfaq-html/Q5.4.html
    undef %saw;
    @sections_unique = grep(!$saw{$_}++, @{$args{'sectionlist'}});
    foreach $section (@sections_unique) {
	print "\n\@strong{$section:} " if $section ne $section_default;
	$args{'sections'}{$section} =~ s:([{}]):\@\1:gs;
	output_highlight($args{'sections'}{$section});
    }
    print "\@end deftypefun\n\n";
}

# output in html
sub output_html {
    my %args = %{$_[0]};
    my ($parameter, $section);
    my $count;
    print "\n\n<a name=\"". $args{'function'} . "\">&nbsp</a><h2>Function</h2>\n";

    print "<i>".$args{'functiontype'}."</i>\n";
    print "<b>".$args{'function'}."</b>\n";
    print "(";
    $count = 0;
    foreach $parameter (@{$args{'parameterlist'}}) {
	print "<i>".$args{'parametertypes'}{$parameter}."</i> <b>".$parameter."</b>\n";
	if ($count != $#{$args{'parameterlist'}}) {
	    $count++;
	    print ", ";
	}
    }
    print ")\n";

    print "<h3>Arguments</h3>\n";
    print "<dl>\n";
    foreach $parameter (@{$args{'parameterlist'}}) {
	print "<dt><i>".$args{'parametertypes'}{$parameter}."</i> <b>".$parameter."</b>\n";
	print "<dd>";
	output_highlight($args{'parameters'}{$parameter});
    }
    print "</dl>\n";
    foreach $section (@{$args{'sectionlist'}}) {
	print "<h3>$section</h3>\n";
	print "<ul>\n";
	output_highlight($args{'sections'}{$section});
	print "</ul>\n";
    }
    print "<hr>\n";
}

# output in tex
sub output_tex {
    my %args = %{$_[0]};
    my ($parameter, $section);
    my $count;
    my $func = $args{'function'};
    my $param;
    my $param2;
    my $sec;
    my $check;
    my $type;

    $func =~ s/_/\\_/g;

    print "\n\n\\subsection{". $func . "}\n\\label{" . $args{'function'} . "}\n";

    $type = $args{'functiontype'};
    $type =~ s/_/\\_/g;

    print "{\\it ".$type."}\n";
    print "{\\bf ".$func."}\n";
    print "(";
    $count = 0;
    foreach $parameter (@{$args{'parameterlist'}}) {
        $param = $args{'parametertypes'}{$parameter};
        $param2 = $parameter;
	$param =~ s/_/\\_/g;
        $param2 =~ s/_/\\_/g;

	print "{\\it ".$param."} {\\bf ".$param2."}";
	if ($count != $#{$args{'parameterlist'}}) {
	    $count++;
	    print ", ";
	}
    }
    print ")\n";

    print "\n{\\large{Arguments}}\n";

    print "\\begin{itemize}\n";
    $check=0;
    foreach $parameter (@{$args{'parameterlist'}}) {
        $param1 = $args{'parametertypes'}{$parameter};
        $param1 =~ s/_/\\_/g;
        $param2 = $parameter;
	$param2 =~ s/_/\\_/g;

	$check = 1;
	print "\\item {\\it ".$param1."} {\\bf ".$param2."}: \n";
#	print "\n";

	$param3 = $args{'parameters'}{$parameter};
	$param3 =~ s/&([a-zA-Z\_]+)/{\\it \1}/g;

	$out = just_highlight($param3);
	$out =~ s/_/\\_/g;
	print $out;
    }
    if ($check==0) {
	print "\\item void\n";
    }
    print "\\end{itemize}\n";

    foreach $section (@{$args{'sectionlist'}}) {
	$sec = $section;
	$sec =~ s/_/\\_/g;
	$sec =~ s/&([a-zA-Z\_]+)/{\\it \1}/g;

	print "\n{\\large{$sec}}\\\\\n";
	print "\\begin{rmfamily}\n";

	$sec = $args{'sections'}{$section};
	$sec =~ s/\\:/:/g;
	$sec =~ s/&([a-zA-Z\_]+)/{\\it \1}/g;
	$sec =~ s/->/\$\\rightarrow\$/g;
	$sec =~ s/([0-9]+)\^([0-9]+)/\$\{\1\}\^\{\2\}\$/g;

	$out = just_highlight($sec);
	$out =~ s/_/\\_/g;

	print $out;
	print "\\end{rmfamily}\n";
    }
    print "\n";
}


# output in sgml DocBook
sub output_sgml {
    my %args = %{$_[0]};
    my ($parameter, $section);
    my $count;
    my $id;

    $id = $args{'module'}."-".$args{'function'};
    $id =~ s/[^A-Za-z0-9]/-/g;

    print "<refentry>\n";
    print "<refmeta>\n";
    print "<refentrytitle><phrase id=\"$id\">".$args{'function'}."</phrase></refentrytitle>\n";
    print "</refmeta>\n";
    print "<refnamediv>\n";
    print " <refname>".$args{'function'}."</refname>\n";
    print " <refpurpose>\n";
    print "  ".$args{'purpose'}."\n";
    print " </refpurpose>\n";
    print "</refnamediv>\n";

    print "<refsynopsisdiv>\n";
    print " <title>Synopsis</title>\n";
    print "  <funcsynopsis>\n";
    print "   <funcdef>".$args{'functiontype'}." ";
    print "<function>".$args{'function'}." ";
    print "</function></funcdef>\n";

#    print "<refsect1>\n";
#    print " <title>Synopsis</title>\n";
#    print "  <funcsynopsis>\n";
#    print "   <funcdef>".$args{'functiontype'}." ";
#    print "<function>".$args{'function'}." ";
#    print "</function></funcdef>\n";

    $count = 0;
    if ($#{$args{'parameterlist'}} >= 0) {
	foreach $parameter (@{$args{'parameterlist'}}) {
	    print "   <paramdef>".$args{'parametertypes'}{$parameter};
	    print " <parameter>$parameter</parameter></paramdef>\n";
	}
    } else {
	print "  <void>\n";
    }
    print "  </funcsynopsis>\n";
    print "</refsynopsisdiv>\n";
#    print "</refsect1>\n";

    # print parameters
    print "<refsect1>\n <title>Arguments</title>\n";
#    print "<para>\nArguments\n";
    if ($#{$args{'parameterlist'}} >= 0) {
	print " <variablelist>\n";
	foreach $parameter (@{$args{'parameterlist'}}) {
	    print "  <varlistentry>\n   <term><parameter>$parameter</parameter></term>\n";
	    print "   <listitem>\n    <para>\n";
	    $lineprefix="     ";
	    output_highlight($args{'parameters'}{$parameter});
	    print "    </para>\n   </listitem>\n  </varlistentry>\n";
	}
	print " </variablelist>\n";
    } else {
	print " <para>\n  None\n </para>\n";
    }
    print "</refsect1>\n";

    # print out each section
    $lineprefix="   ";
    foreach $section (@{$args{'sectionlist'}}) {
	print "<refsect1>\n <title>$section</title>\n <para>\n";
#	print "<para>\n$section\n";
	if ($section =~ m/EXAMPLE/i) {
	    print "<example><para>\n";
	}
	output_highlight($args{'sections'}{$section});
#	print "</para>";
	if ($section =~ m/EXAMPLE/i) {
	    print "</para></example>\n";
	}
	print " </para>\n</refsect1>\n";
    }

    print "\n\n";
}

##
# output in man
sub output_man {
    my %args = %{$_[0]};
    my ($parameter, $section);
    my $count;

    print ".TH \"$args{'function'}\" 3 \"$args{'sourceversion'}\" \"". $args{'module'} . "\" \"". $args{'module'} . "\"\n";

    print ".SH NAME\n";

    print $args{'function'}."\n";

    print ".SH SYNOPSIS\n";
    print ".B #include <". lc((split /_/, $args{'function'})[0]) . ".h>\n"
	if $args{'includefuncprefix'};
    print ".sp\n";
    print ".BI \"".$args{'functiontype'}." ".$args{'function'}."(";
    $count = 0;
    foreach $parameter (@{$args{'parameterlist'}}) {
	print $args{'parametertypes'}{$parameter}." \" ".$parameter." \"";
	if ($count != $#{$args{'parameterlist'}}) {
	    $count++;
	    print ", ";
	}
    }
    print ");\"\n";

    print ".SH ARGUMENTS\n";
    foreach $parameter (@{$args{'parameterlist'}}) {
	print ".IP \"".$args{'parametertypes'}{$parameter}." ".$parameter."\" 12\n";
	output_highlight($args{'parameters'}{$parameter});
    }
    foreach $section (@{$args{'sectionlist'}}) {
	print ".SH \"" . uc($section) . "\"\n";
	output_highlight($args{'sections'}{$section});
    }

    if ($args{'bugsto'}) {
	print ".SH \"REPORTING BUGS\"\n";
	print "Report bugs to <". $args{'bugsto'} . ">.\n";
    }

    if ($args{'copyright'}) {
	print ".SH COPYRIGHT\n";
	print "Copyright \\(co ". $args{'copyright'} . ".\n";
	if ($args{'verbatimcopying'}) {
	    print ".br\n";
	    print "Permission is granted to make and distribute verbatim copies of this\n";
	    print "manual provided the copyright notice and this permission notice are\n";
	    print "preserved on all copies.\n";
	}
    }

    if ($args{'seeinfo'}) {
	print ".SH \"SEE ALSO\"\n";
	print "The full documentation for\n";
	print ".B " . $args{'module'} . "\n";
	print "is maintained as a Texinfo manual.  If the\n";
	print ".B info\n";
	print "and\n";
	print ".B " . $args{'module'} . "\n";
	print "programs are properly installed at your site, the command\n";
	print ".IP\n";
	print ".B info " . $args{'seeinfo'} . "\n";
	print ".PP\n";
	print "should give you access to the complete manual.\n";
    }
}

sub output_listfunc {
    my %args = %{$_[0]};
    print $args{'function'} . "\n";
}

##
# output in text
sub output_text {
    my %args = %{$_[0]};
    my ($parameter, $section);

    print "Function = ".$args{'function'}."\n";
    print "  return type: ".$args{'functiontype'}."\n\n";
    foreach $parameter (@{$args{'parameterlist'}}) {
	print " ".$args{'parametertypes'}{$parameter}." ".$parameter."\n";
	print "    -> ".$args{'parameters'}{$parameter}."\n";
    }
    foreach $section (@{$args{'sectionlist'}}) {
	print " $section:\n";
	print "    -> ";
	output_highlight($args{'sections'}{$section});
    }
}

##
# generic output function - calls the right one based
# on current output mode.
sub output_function {
#    output_html(@_);
    eval "output_".$output_mode."(\@_);";
}


##
# takes a function prototype and spits out all the details
# stored in the global arrays/hsahes.
sub dump_function {
    my $prototype = shift @_;

    if ($prototype =~ m/^()([a-zA-Z0-9_~:]+)\s*\(([^\)]*)\)/ ||
	$prototype =~ m/^(\w+)\s+([a-zA-Z0-9_~:]+)\s*\(([^\)]*)\)/ ||
	$prototype =~ m/^(\w+\s*\*)\s*([a-zA-Z0-9_~:]+)\s*\(([^\)]*)\)/ ||
	$prototype =~ m/^(\w+\s+\w+)\s+([a-zA-Z0-9_~:]+)\s*\(([^\)]*)\)/ ||
	$prototype =~ m/^(\w+\s+\w+\s*\*)\s*([a-zA-Z0-9_~:]+)\s*\(([^\)]*)\)/)  {
	$return_type = $1;
	$function_name = $2;
	$args = $3;

#	print STDERR "ARGS = '$args'\n";

	foreach $arg (split ',', $args) {
	    # strip leading/trailing spaces
	    $arg =~ s/^\s*//;
	    $arg =~ s/\s*$//;
#	    print STDERR "SCAN ARG: '$arg'\n";
	    @args = split('\s', $arg);

#	    print STDERR " -> @args\n";
	    $param = pop @args;
#	    print STDERR " -> @args\n";
	    if ($param =~ m/^(\*+)(.*)/) {
		$param = $2;
		push @args, $1;
	    }
	    if ($param =~ m/^(.*)(\[\])$/) {
		$param = $1;
		push @args, $2;
	    }
#	    print STDERR " :> @args\n";
	    $type = join " ", @args;

	    if ($parameters{$param} eq "" && $param != "void") {
		$parameters{$param} = "-- undescribed --";
		print STDERR "Warning($lineno): Function parameter '$param' not described in '$function_name'\n";
	    }

	    push @parameterlist, $param;
	    $parametertypes{$param} = $type;

#	    print STDERR "param = '$param', type = '$type'\n";
	}
    } else {
	print STDERR "Error($lineno): cannot understand prototype: '$prototype'\n";
	return;
    }

    if ($function_only==0 || defined($function_table{$function_name})) {
	output_function({'function' => $function_name,
			 'module' => $modulename,
			 'sourceversion' => $sourceversion,
			 'includefuncprefix' => $includefuncprefix,
			 'bugsto' => $bugsto,
			 'copyright' => $copyright,
			 'verbatimcopying' => $verbatimcopying,
			 'seeinfo' => $seeinfo,
			 'functiontype' => $return_type,
			 'parameterlist' => \@parameterlist,
			 'parameters' => \%parameters,
			 'parametertypes' => \%parametertypes,
			 'sectionlist' => \@sectionlist,
			 'sections' => \%sections,
			 'purpose' => $function_purpose
			 });
    }
}

######################################################################
# main
# states
# 0 - normal code
# 1 - looking for function name
# 2 - scanning field start.
# 3 - scanning prototype.
$state = 0;
$section = "";

$doc_special = "\@\%\$\&";

$doc_start = "^/\\*\\*\$";
$doc_end = "\\*/";
$doc_com = "\\s*\\*\\s*";
$doc_func = $doc_com."(\\w+):?";
$doc_sect = $doc_com."([".$doc_special."[:upper:]][\\w ]+):(.*)";
$doc_content = $doc_com."(.*)";

%constants = ();
%parameters = ();
@parameterlist = ();
%sections = ();
@sectionlist = ();

$contents = "";
$section_default = "Description";	# default section
$section = $section_default;

$lineno = 0;
foreach $file (@ARGV) {
    if (!open(IN,"<$file")) {
	print STDERR "Error: Cannot open file $file\n";
	next;
    }
    while (<IN>) {
	$lineno++;

	if ($state == 0) {
	    if (/$doc_start/o) {
		$state = 1;		# next line is always the function name
	    }
	} elsif ($state == 1) {	# this line is the function name (always)
	    if (/$doc_func/o) {
		$function = $1;
		$state = 2;
		if (/-(.*)/) {
		    $function_purpose = $1;
		} else {
		    $function_purpose = "";
		}
		if ($verbose) {
		    print STDERR "Info($lineno): Scanning doc for $function\n";
		}
	    } else {
		print STDERR "WARN($lineno): Cannot understand $_ on line $lineno",
		" - I thought it was a doc line\n";
		$state = 0;
	    }
	} elsif ($state == 2) {	# look for head: lines, and include content
	    if (/$doc_sect/o) {
		$newsection = $1;
		$newcontents = $2;

		if ($contents ne "") {
		    dump_section($section, $contents);
		    $section = $section_default;
		}

		$contents = $newcontents;
		if ($contents ne "") {
		    $contents .= "\n";
		}
		$section = $newsection;
	    } elsif (/$doc_end/) {

		if ($contents ne "") {
		    dump_section($section, $contents);
		    $section = $section_default;
		    $contents = "";
		}

#	    print STDERR "end of doc comment, looking for prototype\n";
		$prototype = "";
		$state = 3;
	    } elsif (/$doc_content/) {
		# miguel-style comment kludge, look for blank lines after
		# @parameter line to signify start of description
		if ($1 eq "" && $section =~ m/^@/) {
		    dump_section($section, $contents);
		    $section = $section_default;
		    $contents = "";
		} else {
		    $contents .= $1."\n";
		}
	    } else {
		# i dont know - bad line?  ignore.
		print STDERR "WARNING($lineno): bad line: $_"; 
	    }
	} elsif ($state == 3) {	# scanning for function { (end of prototype)
	    if (m#\s*/\*\s+MACDOC\s*#io) {
	      # do nothing
	    }
	    elsif (/([^\{]*)/) {
		$prototype .= $1;
	    }
	    if (/\{/) {
		$prototype =~ s@/\*.*?\*/@@gos;	# strip comments.
		$prototype =~ s@[\r\n]+@ @gos; # strip newlines/cr's.
		$prototype =~ s@^ +@@gos; # strip leading spaces
		dump_function($prototype);

		$function = "";
		%constants = ();
		%parameters = ();
		%parametertypes = ();
		@parameterlist = ();
		%sections = ();
		@sectionlist = ();
		$prototype = "";

		$state = 0;
	    }
	}
    }
}

