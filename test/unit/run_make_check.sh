#!/bin/sh


LD_LIBRARY_PATH="`pwd`/../../db/db:`pwd`/../../src/common:`pwd`/../../src/server:`pwd`/../../src/listener:`pwd`/../../src/services/http:`pwd`/../../src/http/common:`pwd`/../../src/http/client:`pwd`/../../src/client:`pwd`/../../src/xml:`pwd`/../../src/cimxmllistener:`pwd`/../../src/providerifcs/cpp:`pwd`/../../src/repositories/hdb:$LD_LIBRARY_PATH" ./unitMain $@


