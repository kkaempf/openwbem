#!/bin/bash
ulimit -c unlimited
./testSPNEGOGSSAPI http://e71.suse.de ../../../src/services/http/
