#!/bin/sh



LD_LIBRARY_PATH="`pwd`/../../db/db:`pwd`/../../src/common:`pwd`/../../src/cimom/server:`pwd`/../../src/cimom/common:`pwd`/../../src/listener:`pwd`/../../src/services/http:`pwd`/../../src/http/common:`pwd`/../../src/http/client:`pwd`/../../src/client:`pwd`/../../src/xml:`pwd`/../../src/cimxmllistener:`pwd`/../../src/providerifcs/cpp:`pwd`/../../src/repositories/hdb:`pwd`/../../src/mof:`pwd`/../../src/provider:$LD_LIBRARY_PATH" 

# AIX
LIBPATH="$LD_LIBRARY_PATH:$LIBPATH" 

# HP-UX
SHLIB_PATH="$LD_LIBRARY_PATH:$SHLIB_PATH"

export LD_LIBRARY_PATH LIBPATH SHLIB_PATH

# DARWIN (OSX)
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$LD_LIBRARY_PATH

./unitMain $@


