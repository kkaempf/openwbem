#!/bin/sh

for i in `find . -name Makefile`; do
 perl -p -i -e 's/^srcdir = /srcdir := \$(shell pwd)\//g' $i
 perl -p -i -e 's/^top_srcdir = /top_srcdir := \$(shell pwd)\//g' $i
 perl -p -i -e 's/^VPATH = /VPATH := \$(shell pwd)\//g' $i
done

