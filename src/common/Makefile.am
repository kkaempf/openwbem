SUBDIRS = socket

include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = libopenwbem.so
SHAREDLIBADD = -lpthread -ldl


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



# Shouldn't depend on anything not in common. 
INCLUDES = \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs


libopenwbem_a_SOURCES =	\
OW_Assertion.cpp \
OW_BaseStreamBuffer.cpp \
OW_BinIfcIO.cpp \
OW_Bool.cpp \
OW_Char16.cpp \
OW_CIMErrorException.cpp \
OW_DateTime.cpp \
OW_Exception.cpp \
OW_Exec.cpp \
OW_Format.cpp \
OW_IOIFCStreamBuffer.cpp \
OW_MemTracer.cpp \
OW_Mutex.cpp \
OW_PosixFileSystem.cpp \
OW_PosixOS.cpp \
OW_PosixUnnamedPipe.cpp \
OW_RWLocker.cpp \
OW_RandomNumber.cpp \
OW_Select.cpp \
OW_SelectEngine.cpp \
OW_StackTrace.cpp \
OW_String.cpp \
OW_StringBuffer.cpp \
OW_TempFileStream.cpp \
OW_Thread.cpp \
OW_ThreadEvent.cpp \
OW_ThreadImpl.cpp \
OW_TmpFile.cpp \
OW_URL.cpp \
OW_dlSharedLibrary.cpp \
OW_dlSharedLibraryLoader.cpp \
OW_UnnamedPipe.cpp

pkginclude_HEADERS =	\
OW_Array.hpp \
OW_ArrayFwd.hpp \
OW_Assertion.hpp \
OW_AutoPtr.hpp \
OW_BaseStreamBuffer.hpp \
OW_BinIfcIO.hpp \
OW_Bool.hpp \
OW_ByteSwap.hpp \
OW_Char16.hpp \
OW_CIMErrorException.hpp \
OW_CIMFeatures.hpp \
OW_ConfigOpts.hpp \
OW_DateTime.hpp \
OW_Enumeration.hpp \
OW_Exception.hpp \
OW_Exec.hpp \
OW_FileSystem.hpp \
OW_Format.hpp \
OW_FuncNamePrinter.hpp \
OW_IOException.hpp \
OW_IOIFCStreamBuffer.hpp \
OW_IstreamBufIterator.hpp \
OW_List.hpp \
OW_LogLevel.hpp \
OW_Logger.hpp \
OW_Map.hpp \
OW_MemTracer.hpp \
OW_Memory.hpp \
OW_Mutex.hpp \
OW_MutexLock.hpp \
OW_NetworkTypes.hpp \
OW_OS.hpp \
OW_PosixUnnamedPipe.hpp \
OW_PthDefines.hpp \
OW_RWLocker.hpp \
OW_RandomNumber.hpp \
OW_Reference.hpp \
OW_Select.hpp \
OW_SelectEngine.hpp \
OW_Semaphore.hpp \
OW_SortedVector.hpp \
OW_Stack.hpp \
OW_StackTrace.hpp \
OW_String.hpp \
OW_StringBuffer.hpp \
OW_StringEnumeration.hpp \
OW_StringStream.hpp \
OW_TempFileStream.hpp \
OW_Thread.hpp \
OW_ThreadEvent.hpp \
OW_ThreadImpl.hpp \
OW_ThreadTypes.hpp \
OW_TmpFile.hpp \
OW_Types.h \
OW_URL.hpp \
OW_UnnamedPipe.hpp \
OW_SafeLibCreate.hpp \
OW_SharedLibrary.hpp \
OW_SharedLibraryException.hpp \
OW_SharedLibraryLoader.hpp \
OW_SignalScope.hpp \
OW_dlSharedLibrary.hpp \
OW_dlSharedLibraryLoader.hpp \
OW_SharedLibraryReference.hpp \
OW_config.h

libopenwbem_a_LIBADD = \
../cim/libOW_cim.a \
../common/socket/libOW_Socket.a \
../ifcs/libOW_ifcs.a

#libopenwbem_a_LDFLAGS = -release $(OPENWBEM_RELEASE)	\
#-version-info $(OPENWBEM_CURRENT):$(OPENWBEM_REVISION):$(OPENWBEM_AGE)

