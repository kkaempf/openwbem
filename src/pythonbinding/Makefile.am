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
PyOW_String.cpp \
PyOW_CIMOMInfo.cpp \
PyOW_Array.cpp \
PyOW_Array.cpp \
PyOW_StringArray.cpp \
PyOW_CIMClassArray.cpp \
PyOW_CIMDataTypeArray.cpp \
PyOW_CIMDateTimeArray.cpp \
PyOW_CIMFlavorArray.cpp \
PyOW_CIMInstanceArray.cpp \
PyOW_CIMMethodArray.cpp \
PyOW_CIMObjectPathArray.cpp \
PyOW_CIMParamValueArray.cpp \
PyOW_CIMParameterArray.cpp \
PyOW_CIMPropertyArray.cpp \
PyOW_CIMQualifierArray.cpp \
PyOW_CIMQualifierTypeArray.cpp \
PyOW_CIMScopeArray.cpp \
PyOW_CIMValueArray.cpp \
PyOW_BoolArray.cpp \
PyOW_Char16Array.cpp \
PyOW_UInt8Array.cpp \
PyOW_Int8Array.cpp \
PyOW_UInt16Array.cpp \
PyOW_Int16Array.cpp \
PyOW_UInt32Array.cpp \
PyOW_Int32Array.cpp \
PyOW_UInt64Array.cpp \
PyOW_Int64Array.cpp \
PyOW_Real32Array.cpp \
PyOW_Real64Array.cpp \
PyOW_CIMOMInfoArray.cpp \
PyOW_CIMOMLocator.cpp \
PyOW_ClientAuthCBIFC.cpp


