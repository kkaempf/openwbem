include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = libopenwbem.so


noinst_LIBRARIES = libopenwbem.a

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



INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/thread	\
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/net/socket


libopenwbem_a_SOURCES =	\
OW_Char16.cpp \
OW_CIMErrorException.cpp \
OW_DateTime.cpp \
OW_Exception.cpp \
OW_Assertion.cpp \
OW_Format.cpp \
OW_String.cpp \
OW_UnnamedPipe.cpp \
OW_TempFileStream.cpp \
OW_Bool.cpp \
OW_MemTracer.cpp \
OW_BaseStreamBuffer.cpp	\
OW_SelectEngine.cpp \
OW_StringBuffer.cpp \
OW_RequestHandlerIFC.cpp \
OW_CIMOMHandle.cpp			\
OW_BinIfcIO.cpp \
OW_URL.cpp \
OW_IOIFCStreamBuffer.cpp 

pkginclude_HEADERS =	\
OW_OS.hpp \
OW_Bool.hpp \
OW_Array.hpp \
OW_Stack.hpp \
OW_ArrayFwd.hpp \
OW_Assertion.hpp \
OW_AutoPtr.hpp \
OW_Base64FormatException.hpp \
OW_Char16.hpp \
OW_DateTime.hpp \
OW_Enumeration.hpp \
OW_Exception.hpp \
OW_Exec.hpp \
OW_Format.hpp \
OW_IOException.hpp \
OW_CIMErrorException.hpp \
OW_List.hpp \
OW_FileSystem.hpp \
OW_Map.hpp \
OW_Memory.hpp \
OW_StackTrace.hpp \
OW_String.hpp \
OW_BaseStreamBuffer.hpp  \
OW_StringBuffer.hpp \
OW_StringEnumeration.hpp \
OW_StringStream.hpp \
OW_TimeOutException.hpp \
OW_Types.h \
OW_UnnamedPipe.hpp \
OW_TmpFile.hpp \
OW_TempFileStream.hpp \
OW_LogLevel.hpp \
OW_Logger.hpp \
OW_IstreamBufIterator.hpp \
OW_MemTracer.hpp \
OW_FuncNamePrinter.hpp \
OW_RandomNumber.hpp \
OW_ThreadTypes.hpp \
OW_NetworkTypes.hpp \
OW_ByteSwap.hpp \
OW_Select.hpp \
OW_SelectableIFC.hpp \
OW_SelectableCallbackIFC.hpp \
OW_ServiceEnvironmentIFC.hpp \
OW_WQLIFC.hpp \
OW_ConfigOpts.hpp \
OW_SelectEngine.hpp \
OW_RequestHandlerIFC.hpp \
OW_IOIFCStreamBuffer.hpp \
OW_IOIFC.hpp \
OW_AuthenticatorIFC.hpp \
OW_CIMOMHandle.hpp \
OW_CIMFeatures.hpp \
OW_ServiceIFC.hpp \
OW_BinIfcIO.hpp \
OW_URL.hpp \
OW_config.h

libopenwbem_a_LIBADD =					\
../cim/libOW_cim.a					\
../net/socket/libOW_Socket.a				\
../platform/@PLATFORMDIR@/libplatformcommon.a	\
../http/libOW_Http.a					\
../http/client/libOW_HttpClient.a			\
../thread/libOW_Thread.a				\
../xml/libOW_Xml.a \
../client/libOW_Client.a

#libopenwbem_a_LDFLAGS = -release $(OPENWBEM_RELEASE)	\
#-version-info $(OPENWBEM_CURRENT):$(OPENWBEM_REVISION):$(OPENWBEM_AGE)

