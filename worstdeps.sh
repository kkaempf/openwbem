#!/bin/sh
find -name \*.P | xargs wc -l | sort -r
