include $(top_srcdir)/Makefile-shared-lib.inc

if HAVE_SLP
noinst_LIBRARIES = libowslpprovider.a

thelibdir = $(pkglibdir)/c++providers

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -s $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	



OW_SHARED_LIBS = libowslpprovider.so

all-local: $(OW_SHARED_LIBS)
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


libowslpprovider_a_SOURCES = \
OW_SLPProvider.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/thread \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/client

else

clean-local:

endif


