include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = libowhttpxmllistener.so
SHAREDLIBADD = 

noinst_LIBRARIES = \
libowhttpxmllistener.a

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

libowhttpxmllistener_a_SOURCES = \
OW_XMLListener.cpp \
OW_ListenerAuthenticator.cpp \
OW_HTTPXMLCIMListener.cpp

pkginclude_HEADERS = \
OW_CIMListenerCallback.hpp \
OW_ListenerAuthenticator.hpp \
OW_XMLListener.hpp \
OW_HTTPXMLCIMListener.hpp


INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/services/http \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/requesthandlers/cimxml

