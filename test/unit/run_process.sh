#!/bin/sh

# This script is a helper for the unit tests.  It could be avoided
# altogether by opening some form of pipe in the tests... As long as
# the pipe is readable and writeable after the Exec() functions close
# the pipe.  For now, this is easier.

# What it does is runs our test program with an output descriptor
# option (so stdin/out/err are preserved) with the output redirecting
# to a file.  It does this because the Exec functions close the
# descriptors in an attempt to force a quit.  Since we actually want
# to test the signals that come after the close has no effect, we need
# the output after the SIGPIPE would have been received.

OW_OUTPUT_FILE=${OW_OUTPUT_FILE:-/tmp/ow_test_exec.$$}

echo "${OW_OUTPUT_FILE}"

exec "$@" --output-descriptor 3 3> "${OW_OUTPUT_FILE}"