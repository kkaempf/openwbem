MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


owstatlib_LIBRARIES = libowdb.a 
OW_SHARED_LIBS = libowdb.$(LIB_EXT)
LIBRARY_BINARY_VERSION = 4


thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

libowdb_a_SOURCES =  db.c

libowdb_a_LIBADD = \
../btree/libbtree.a \
../mpool/libmpool.a



INCLUDES = -I$(top_srcdir)/db/include -I$(top_srcdir)/src/common
