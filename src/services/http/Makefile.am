include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = libowservicehttp.so
SHAREDLIBADD = -L$(top_builddir)/src/http/common -lowhttpcommon

noinst_LIBRARIES = \
libowservicehttp.a

thelibdir = $(pkglibdir)/services
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

libowservicehttp_a_SOURCES = \
OW_HTTPServer.cpp \
OW_DigestAuthentication.cpp \
OW_HTTPSvrConnection.cpp \
OW_HTTPServer.hpp \
OW_DigestAuthentication.hpp \
OW_HTTPSvrConnection.hpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/http/common

