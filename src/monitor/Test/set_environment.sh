#!/bin/sh

LD_LIBRARY_PATH="`pwd`/..:`pwd`/../../common":${LD_LIBRARY_PATH:-}

# AIX
LIBPATH=$LD_LIBRARY_PATH:${LIBPATH:-}

# HP-UX
SHLIB_PATH=$LD_LIBRARY_PATH:${SHLIB_PATH:-}

# DARWIN (OSX)
DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH:${DYLD_LIBRARY_PATH:-}

export LD_LIBRARY_PATH LIBPATH SHLIB_PATH DYLD_LIBRARY_PATH