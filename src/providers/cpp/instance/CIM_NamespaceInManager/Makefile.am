include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libowprovinstCIM_NamespaceInManager.a

thelibdir = $(pkglibdir)/c++providers

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -s $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done

OW_SHARED_LIBS = libowprovinstCIM_NamespaceInManager.so

all-local: $(OW_SHARED_LIBS)
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


libowprovinstCIM_NamespaceInManager_a_SOURCES = \
OW_provinstCIM_NamespaceInManager.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp


