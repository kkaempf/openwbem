include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libowprovinstOW_NameSpace.a

thelibdir = $(pkglibdir)/c++providers

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done

OW_SHARED_LIBS = libowprovinstOW_NameSpace.so

all-local: $(OW_SHARED_LIBS)
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


libowprovinstOW_NameSpace_a_SOURCES = \
OW_NameSpaceProvider.hpp \
OW_NameSpaceProvider.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp


