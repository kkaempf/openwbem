#!/usr/bin/awk -f

# mgrep.awk
# Written by Kevin Harris.

# This horrible script implements a (partial) multi-line grep.
# The reason I say it is a partial multi-line grep, is that it will only check
# a fixed number of lines (the number of lines in your expression).
#
# Command line usage:
# mgrep.awk <REGEX> <input file>
#
# Where the <REGEX> is one or more regular expression with "\n" included to
# show multiple lines.
# If an input file is '-', it will be read from stdin.

# Example:
# $ echo -e "fred\nalice\nbob\njoe\nbart\nbarf" | ./mgrep.awk  'a.*\nb.*' -
# alice\nbob

# NOTE: This script would be much smaller if it didn't have the debug output,
# but there may be some platform where we run into trouble... The debug will be
# very useful there.

function debug(text)
{
	# If you want debug output, uncomment this next line.
#	print text
}

BEGIN {
	split("", buffered_lines); # empty the array.
	buffer_count = 0;
	if ( ARGC == 1 ) exit 1;

	# Convert any "\\n" into "\n"
	expression=ARGV[1];
	# If this gsub fails on anything (I remember having gsub problems before),
	# uncomment the functional replacement below it.
	gsub(/\\n/,"\n",expression);
# 	expression2=expression;
# 	sub(/\\n/,"\n",expression2);
# 	while( expression2 != expression ) {
# 		expression = expression2;
# 		sub(/\\n/,"\n",expression2);
# 	}
	split(expression, realargs,"\n");

	# The split function uses 1-based arrays.  This is inconvenient for people
	# who can no longer think in base 1 (i.e.. me), so we'll shift everything
	# down to be zero-based.
	realargc=0;
	for( arg in realargs ) {
		++realargc;
	}
	for( i = 0; i < realargc; ++i ) {
		realargs[i] = realargs[i + 1];
	}
	delete realargs[realargc]

	# Debug output...
	for( arg in realargs ) {
		debug("realargs[" arg "]=" realargs[arg])
	}
	debug("realargc=" realargc)

	# We ate the argument, so we'll delete it.  All of the other arguments will
	# be processed by awk as input files. Kind of neat, yet sick at the same time.
	delete ARGV[1]

	have_match=0
	debug("end of begin");
}

{
	debug("Checking line: " $0)
	if( buffer_count < realargc ) {
		debug("Buffer not full...")
		buffered_lines[buffer_count++] = $0
	}
	else {
		debug("Buffer full and shifting...")
		# Shift down to eat the next arg.
		for( i = 0; i < buffer_count - 1; ++i ) {
			buffered_lines[i] = buffered_lines[i + 1];
		}
		debug("Appending to the end of the buffer: " $0)
		buffered_lines[buffer_count - 1] = $0

		debug("Dumping the buffer:")
		for( foo in buffered_lines ) {
			debug("buffer[" foo "]=" buffered_lines[foo])
		}
	}

	if( buffer_count == realargc ) {
		debug("Trying to match (buffer_count=" buffer_count ")")
		# Try to match all arguments to the buffered lines.
		for( i = 0; i < buffer_count; ++i) {
			debug("Checking buffer \"" buffered_lines[i] "\" against arg \"" realargs[i] "\"")
			if( buffered_lines[i] ~  "^" realargs[i] "$" ) {
				debug("arg" i " matched...")
			}
			else {
				debug("No match...")
				next
			}
		}
		# We've got a match.... Dump it.
		output=""
		for( i = 0; i < buffer_count; ++i) {
			if( output != "" ) {
				output = output "\\n"
			}
			output = output buffered_lines[i]
		}
		print output
		have_match=1
	}
}

END {
	debug("End reached...")
	if ( !have_match ) {
		debug("Found no matches.")
		exit 1
	}
}
