#!/bin/sh
INCLUDES="			\
-I..				\
-I../..				\
-I../../cim			\
-I../../client		\
-I../../common		\
-I../../cimom		\
-I../../http		\
-I../../http/client	\
-I../../http/server	\
-I../../server		\
-I../../thread		\
-I../../xml			\
-I../../net/socket	\
-I../../platform/Linux/i386/GCC/inc	\
-I../../platform/Linux/i386/GCC/server/authorizer

CC="g++ -DOW_DEBUG -g3 -Wall -W -Woverloaded-virtual -fPIC -DPIC"
CC="g++ -g"

#$CC $INCLUDES -o pamtest pamtest.cpp -L../../../build/src/common/.libs -lopenwbem
#$CC $INCLUDES -o pamtest-short pamtest-short.cpp -L../../../build/src/common/.libs -lopenwbem
#$CC $INCLUDES -o server httpserver.cpp -L../../../build/src/common/.libs -lopenwbem
$CC $INCLUDES -o client httpclient.cpp -L../../../build/src/common/.libs -lopenwbem
$CC $INCLUDES -o chunker_client chunker_client.cpp -L../../../build/src/common/.libs -lopenwbem
#$CC $INCLUDES -o chunker_server chunker_server.cpp -L../../../build/src/common/.libs -lopenwbem
$CC $INCLUDES -o lenlimit lenlimit.cpp -L../../../build/src/common/.libs -lopenwbem


# vim: ts=4
