MAKEDYLIB=1
SUBDIRS = socket

include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

OW_SHARED_LIBS = libopenwbem.$(LIB_EXT) 
SHAREDLIBADD = $(DL_LIB) $(SSL_LIBS)



owstatlib_LIBRARIES = libopenwbem.a

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)


# Shouldn't depend on anything not in common. DON'T ADD ANYTHING TO THIS LIST!
INCLUDES = \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/common/socket


libopenwbem_a_SOURCES =	\
OW_Array.cpp \
OW_Assertion.cpp \
OW_AtomicOps.cpp \
OW_BaseStreamBuffer.cpp \
OW_BinarySerialization.cpp \
OW_Bool.cpp \
OW_Char16.cpp \
OW_CIMErrorException.cpp \
OW_CmdLineParser.cpp \
OW_Condition.cpp \
OW_ConfigException.cpp \
OW_ConfigFile.cpp \
OW_COWIntrusiveCountableBase.cpp \
OW_COWIntrusiveReference.cpp \
OW_COWReferenceBase.cpp \
OW_DataStreams.cpp \
OW_DateTime.cpp \
OW_dlSharedLibrary.cpp \
OW_dlSharedLibraryLoader.cpp \
OW_dyldSharedLibrary.cpp \
OW_dyldSharedLibraryLoader.cpp \
OW_EnumerationException.cpp \
OW_Exception.cpp \
OW_Exec.cpp \
OW_Format.cpp \
OW_IntrusiveCountableBase.cpp \
OW_IOException.cpp \
OW_IOIFCStreamBuffer.cpp \
OW_Logger.cpp \
OW_LoggerImpls.cpp \
OW_MD5.cpp \
OW_MemTracer.cpp \
OW_Mutex.cpp \
OW_MutexImpl.cpp \
OW_NonRecursiveMutex.cpp \
OW_NonRecursiveMutexImpl.cpp \
OW_NoSuchPropertyException.cpp \
OW_NoSuchProviderException.cpp \
OW_NoSuchQualifierException.cpp \
OW_NULLValueException.cpp \
OW_OperationContext.cpp \
OW_OutOfBoundsException.cpp \
OW_PosixFile.cpp \
OW_PosixFileSystem.cpp \
OW_PosixUnnamedPipe.cpp \
OW_RandomNumber.cpp \
OW_ReferenceBase.cpp \
OW_RepositoryCIMOMHandle.cpp \
OW_ResultHandlers.cpp \
OW_Runnable.cpp \
OW_RWLocker.cpp \
OW_Select.cpp \
OW_SelectEngine.cpp \
OW_SessionLanguage.cpp \
OW_SharedLibrary.cpp \
OW_SharedLibraryException.cpp \
OW_SharedLibraryLoader.cpp \
OW_shlSharedLibrary.cpp \
OW_shlSharedLibraryLoader.cpp \
OW_StackTrace.cpp \
OW_StringBuffer.cpp \
OW_String.cpp \
OW_StringStream.cpp \
OW_TempFileEnumerationImplBase.cpp \
OW_TempFileStream.cpp \
OW_ThreadBarrier.cpp \
OW_ThreadCancelledException.cpp \
OW_ThreadCounter.cpp \
OW_Thread.cpp \
OW_ThreadDoneCallback.cpp \
OW_ThreadImpl.cpp \
OW_ThreadPool.cpp \
OW_TimeoutException.cpp \
OW_TmpFile.cpp \
OW_UnnamedPipe.cpp \
OW_URL.cpp \
OW_UserInfo.cpp \
OW_UserUtils.cpp \
OW_UTF8UtilscompareToIgnoreCase.cpp \
OW_UTF8Utils.cpp \
OW_System.cpp \
OW_UUID.cpp

pkginclude_HEADERS =	\
OW_ArrayFwd.hpp \
OW_Array.hpp \
OW_ArrayImpl.hpp \
OW_Assertion.hpp \
OW_AtomicOps.hpp \
OW_AutoPtr.hpp \
OW_BaseStreamBuffer.hpp \
OW_BinarySerialization.hpp \
OW_Bool.hpp \
OW_ByteSwap.hpp \
OW_Cache.hpp \
OW_Char16.hpp \
OW_CIMErrorException.hpp \
OW_CIMFeatures.hpp \
OW_CmdLineParser.hpp \
OW_Condition.hpp \
OW_ConfigException.hpp \
OW_ConfigFile.hpp \
OW_config.h \
OW_ConfigOpts.hpp \
OW_COWIntrusiveCountableBase.hpp \
OW_COWIntrusiveReference.hpp \
OW_COWReferenceBase.hpp \
OW_COWReference.hpp \
OW_DataStreams.hpp \
OW_DateTime.hpp \
OW_dlSharedLibrary.hpp \
OW_dlSharedLibraryLoader.hpp \
OW_dyldSharedLibrary.hpp \
OW_dyldSharedLibraryLoader.hpp \
OW_Enumeration.hpp \
OW_EnumerationException.hpp \
OW_Exception.hpp \
OW_ExceptionIds.hpp \
OW_Exec.hpp \
OW_File.hpp \
OW_FileSystem.hpp \
OW_Format.hpp \
OW_FuncNamePrinter.hpp \
OW_HashMap.hpp \
OW_HashMultiMap.hpp \
OW_IntrusiveCountableBase.hpp \
OW_IntrusiveReference.hpp \
OW_IOException.hpp \
OW_IOIFCStreamBuffer.hpp \
OW_IstreamBufIterator.hpp \
OW_List.hpp \
OW_Logger.hpp \
OW_LogLevel.hpp \
OW_Map.hpp \
OW_MD5.hpp \
OW_MemTracer.hpp \
OW_Mutex.hpp \
OW_MutexImpl.hpp \
OW_MutexLock.hpp \
OW_NetworkTypes.hpp \
OW_NonRecursiveMutex.hpp \
OW_NonRecursiveMutexImpl.hpp \
OW_NonRecursiveMutexLock.hpp \
OW_NoSuchPropertyException.hpp \
OW_NoSuchProviderException.hpp \
OW_NoSuchQualifierException.hpp \
OW_NULLValueException.hpp \
OW_OperationContext.hpp \
OW_PosixUnnamedPipe.hpp \
OW_RandomNumber.hpp \
OW_RefCount.hpp \
OW_ReferenceBase.hpp \
OW_Reference.hpp \
OW_RepositoryCIMOMHandle.hpp \
OW_ResultHandlers.hpp \
OW_Runnable.hpp \
OW_RWLocker.hpp \
OW_SafeLibCreate.hpp \
OW_SelectEngine.hpp \
OW_SessionLanguage.hpp \
OW_Select.hpp \
OW_Semaphore.hpp \
OW_SharedLibraryException.hpp \
OW_SharedLibrary.hpp \
OW_SharedLibraryLoader.hpp \
OW_SharedLibraryReference.hpp \
OW_shlSharedLibrary.hpp \
OW_shlSharedLibraryLoader.hpp \
OW_SignalScope.hpp \
OW_SortedVectorMap.hpp \
OW_SortedVectorSet.hpp \
OW_Stack.hpp \
OW_StackTrace.hpp \
OW_StrictWeakOrdering.hpp \
OW_StringBuffer.hpp \
OW_StringEnumeration.hpp \
OW_String.hpp \
OW_StringStream.hpp \
OW_TempFileEnumerationImplBase.hpp \
OW_TempFileStream.hpp \
OW_ThreadBarrier.hpp \
OW_ThreadCancelledException.hpp \
OW_ThreadCounter.hpp \
OW_ThreadDoneCallback.hpp \
OW_Thread.hpp \
OW_ThreadImpl.hpp \
OW_ThreadPool.hpp \
OW_ThreadTypes.hpp \
OW_TimeoutException.hpp \
OW_TmpFile.hpp \
OW_Types.h \
OW_Types.hpp \
OW_UnnamedPipe.hpp \
OW_URL.hpp \
OW_UserInfo.hpp \
OW_UserUtils.hpp \
OW_UTF8Utils.hpp \
OW_UUID.hpp \
OW_vector.hpp \
OW_System.hpp \
OW_WBEMFlags.hpp

libopenwbem_a_LIBADD = \
../cim/libOW_cim.a \
../common/socket/libOW_Socket.a \
../ifcs/libOW_ifcs.a

