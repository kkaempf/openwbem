include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libowdb.a 
OW_SHARED_LIBS = libowdb.so


thelibdir = $(libdir)
all-local: $(OW_SHARED_LIBS)

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


libowdb_a_SOURCES =  db.c

libowdb_a_LIBADD = \
../btree/libbtree.a \
../mpool/libmpool.a



INCLUDES = -I$(top_srcdir)/db/include -I$(top_srcdir)/src/common
