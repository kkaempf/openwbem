#!/bin/sh


LD_LIBRARY_PATH="`pwd`/../../db/db:`pwd`/../../src/common:`pwd`/../../src/cimom/server:`pwd`/../../src/cimom/common:`pwd`/../../src/listener:`pwd`/../../src/services/http:`pwd`/../../src/http/common:`pwd`/../../src/http/client:`pwd`/../../src/client:`pwd`/../../src/xml:`pwd`/../../src/cimxmllistener:`pwd`/../../src/providerifcs/cpp:`pwd`/../../src/repositories/hdb:`pwd`/../../src/mof:$LD_LIBRARY_PATH" 

LIBPATH="`pwd`/../../db/db:`pwd`/../../src/common:`pwd`/../../src/cimom/server:`pwd`/../../src/cimom/common:`pwd`/../../src/listener:`pwd`/../../src/services/http:`pwd`/../../src/http/common:`pwd`/../../src/http/client:`pwd`/../../src/client:`pwd`/../../src/xml:`pwd`/../../src/cimxmllistener:`pwd`/../../src/providerifcs/cpp:`pwd`/../../src/repositories/hdb:`pwd`/../../src/mof:$LIBPATH" 

export LD_LIBRARY_PATH LIBPATH

./unitMain $@


