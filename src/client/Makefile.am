SUBDIRS = test

include $(top_srcdir)/Makefile-shared-lib.inc
SHAREDLIBADD =

noinst_LIBRARIES = libowclient.a

OW_SHARED_LIBS = libowclient.so
thelibdir = $(libdir)

all-local: $(OW_SHARED_LIBS)

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -s $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done



libowclient_a_SOURCES =	\
OW_CIMOMLocator.cpp \
OW_CIMOMInfo.cpp	\
OW_CIMOMLocatorSLP.cpp \
OW_ClientCIMOMHandle.cpp \
OW_CIMXMLCIMOMHandle.cpp \
OW_BinaryCIMOMHandle.cpp

pkginclude_HEADERS =		\
OW_ClientAuthCBIFC.hpp \
OW_CIMOMLocator.hpp	\
OW_CIMOMLocatorSLP.hpp	\
OW_CIMOMInfo.hpp	\
OW_ClientCIMOMHandle.hpp \
OW_CIMXMLCIMOMHandle.hpp \
OW_BinaryCIMOMHandle.hpp 

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client


