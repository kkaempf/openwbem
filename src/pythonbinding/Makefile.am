include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = owclient.so
SHAREDLIBADD = \
$(LIBS) \
-L../common -lopenwbem \
-L../client -lowclient \
-L../xml -lowxml \
-L../http/common -lowhttpcommon \
-L../http/client -lowhttpclient \
-lz -lslp -lssl -lcrypto

noinst_LIBRARIES = owclient.a

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

INCLUDES = \
-I$(top_srcdir)/../common \
-I$(top_srcdir)/../cim \
-I$(top_srcdir)/../client \
-I$(top_srcdir)/../ifcs

owclient_a_SOURCES = \
PyOW_CIMClient.cpp \
PyOW_Client.cpp \
PyOW_String.cpp

