include $(top_srcdir)/Makefile-shared-lib.inc
include $(top_srcdir)/Makefile-shared-lib-inst.inc

noinst_LIBRARIES = libowdb.a 
OW_SHARED_LIBS = libowdb.so


thelibdir = $(libdir)

libowdb_a_SOURCES =  db.c

libowdb_a_LIBADD = \
../btree/libbtree.a \
../mpool/libmpool.a



INCLUDES = -I$(top_srcdir)/db/include -I$(top_srcdir)/src/common
