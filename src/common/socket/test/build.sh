#!/bin/sh
#g++ -g -I../ -I../../../inc/ -I../../../ -o OW_InetAddressTest OW_InetAddressTest.cpp -L../../../../src/base/.libs -lOWBase 
g++ -g -I../ -I../../../inc/ -I../../../ -o gethttpdata gethttpdata.cpp -L../../../../src/base/.libs -lOWBase 
g++ -g -I../ -I../../../inc/ -I../../../ -o gethttpsdata gethttpsdata.cpp -L../../../../src/base/.libs -lOWBase 
#g++ -g -I../ -I../../../inc/ -I../../../ -o server server.cpp -L../../../../src/base/.libs -lOWBase 
