#!/bin/sh


LD_LIBRARY_PATH="$LD_LIBRARY_PATH:`pwd`/../../db/db:`pwd`/../../src/common:`pwd`/../../src/server:`pwd`/../../src/listener:`pwd`/../../src/services/http" ./unitMain


