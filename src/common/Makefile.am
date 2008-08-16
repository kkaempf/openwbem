
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

OW_SHARED_LIBS = libopenwbem.$(LIB_EXT)
SHAREDLIBADD = $(DL_LIB) $(SSL_LIBS) $(BLOCXX_LIBS)
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)
MAKEDYLIB=1



owstatlib_LIBRARIES = libopenwbem.a

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)


# Shouldn't depend on anything not in common. DON'T ADD ANYTHING TO THIS LIST!
INCLUDES = \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs 


libopenwbem_a_SOURCES =	\
OW_BinarySerialization.cpp \
OW_CIMErrorException.cpp \
OW_ConfigException.cpp \
OW_ConfigFile.cpp \
OW_ConfigOpts.cpp \
OW_dlSharedLibrary.cpp \
OW_IOException.cpp \
OW_LocalOperationContext.cpp \
OW_NoSuchPropertyException.cpp \
OW_NoSuchProviderException.cpp \
OW_NoSuchQualifierException.cpp \
OW_NULLValueException.cpp \
OW_OperationContext.cpp \
OW_OperationRWLock.cpp \
OW_RepositoryCIMOMHandle.cpp \
OW_ResultHandlers.cpp \
OW_SafeLibCreate.cpp \
OW_SessionLanguage.cpp \
OW_ThreadSafeProcess.cpp \
OW_URL.cpp \
OW_UserInfo.cpp \
OW_WQLSelectStatement.cpp \
OW_WQLCompile.cpp \
OW_WQLOperand.cpp \
OW_WQLPropertySource.cpp \
OW_WQLInstancePropertySource.cpp

pkginclude_HEADERS =	\
OW_BinarySerialization.hpp \
OW_AppenderLogger.hpp \
OW_ArrayFwd.hpp \
OW_Array.hpp \
OW_Assertion.hpp \
OW_AtomicOps.hpp \
OW_AutoDescriptor.hpp \
OW_AutoPtr.hpp \
OW_AutoResource.hpp \
OW_BaseStreamBuffer.hpp \
OW_BinarySerialization.hpp \
OW_Bool.hpp \
OW_ByteSwap.hpp \
OW_Cache.hpp \
OW_CerrAppender.hpp \
OW_CerrLogger.hpp \
OW_Char16.hpp \
OW_CIMErrorException.hpp \
OW_CIMFeatures.hpp \
OW_Clock.hpp \
OW_ClockImpl.hpp \
OW_CmdLineParser.hpp \
OW_CommonFwd.hpp \
OW_Condition.hpp \
OW_ConfigException.hpp \
OW_ConfigFile.hpp \
OW_ConfigOpts.hpp \
OW_COWIntrusiveCountableBase.hpp \
OW_COWIntrusiveReference.hpp \
OW_COWReferenceBase.hpp \
OW_COWReference.hpp \
OW_Cstr.hpp \
OW_DataStreams.hpp \
OW_DateTime.hpp \
OW_DelayedFormat.hpp \
OW_DescriptorUtils.hpp \
OW_DescriptorUtils_noexcept.hpp \
OW_dlSharedLibrary.hpp \
OW_dlSharedLibraryLoader.hpp \
OW_dyldSharedLibrary.hpp \
OW_dyldSharedLibraryLoader.hpp \
OW_Enumeration.hpp \
OW_EnumerationException.hpp \
OW_EnvVars.hpp \
OW_Environ.hpp \
OW_Exception.hpp \
OW_ExceptionIds.hpp \
OW_Exec.hpp \
OW_ExecMockObject.hpp \
OW_File.hpp \
OW_FileAppender.hpp \
OW_FileBuf.hpp \
OW_FileSystem.hpp \
OW_FileSystemMockObject.hpp \
OW_Format.hpp \
OW_FuncNamePrinter.hpp \
OW_GlobalPtr.hpp \
OW_HashMap.hpp \
OW_HashMultiMap.hpp \
OW_IFileStream.hpp \
OW_Infinity.hpp \
OW_IntrusiveCountableBase.hpp \
OW_IntrusiveReference.hpp \
OW_IOException.hpp \
OW_IOIFCStreamBuffer.hpp \
OW_IstreamBufIterator.hpp \
OW_LazyGlobal.hpp \
OW_List.hpp \
OW_LocalOperationContext.hpp \
OW_LogAppender.hpp \
OW_LogAppenderScope.hpp \
OW_Logger.hpp \
OW_LogLevel.hpp \
OW_LogMessage.hpp \
OW_LogMessagePatternFormatter.hpp \
OW_Map.hpp \
OW_MD5.hpp \
OW_MemoryBarrier.hpp \
OW_MemTracer.hpp \
OW_MTQueue.hpp \
OW_MultiAppender.hpp \
OW_MultiProcessFileAppender.hpp \
OW_Mutex.hpp \
OW_MutexLock.hpp \
OW_NetworkTypes.hpp \
OW_NonRecursiveMutex.hpp \
OW_NonRecursiveMutexLock.hpp \
OW_NoSuchPropertyException.hpp \
OW_NoSuchProviderException.hpp \
OW_NoSuchQualifierException.hpp \
OW_NullAppender.hpp \
OW_NullLogger.hpp \
OW_NULLValueException.hpp \
OW_OperationContext.hpp \
OW_OperationRWLock.hpp \
OW_Paths.hpp \
OW_PosixUnnamedPipe.hpp \
OW_Process.hpp \
OW_RandomNumber.hpp \
OW_RefCount.hpp \
OW_RefCountedPimpl.hpp \
OW_ReferenceBase.hpp \
OW_Reference.hpp \
OW_ReferenceHelpers.hpp \
OW_RefToValue.hpp \
OW_RepositoryCIMOMHandle.hpp \
OW_ResultHandlers.hpp \
OW_Runnable.hpp \
OW_RWLocker.hpp \
OW_SafeBool.hpp \
OW_SafeCString.hpp \
OW_SafeCStringIO.hpp \
OW_SafeLibCreate.hpp \
OW_ScopeGuard.hpp \
OW_ScopeLogger.hpp \
OW_Secure.hpp \
OW_SecureRand.hpp \
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
OW_SignalUtils.hpp \
OW_SocketBaseImpl.hpp \
OW_SortedVectorMap.hpp \
OW_SortedVectorSet.hpp \
OW_Stack.hpp \
OW_StackTrace.hpp \
OW_StaticAssert.hpp \
OW_StrictWeakOrdering.hpp \
OW_StringBuffer.hpp \
OW_StringEnumeration.hpp \
OW_String.hpp \
OW_StringStream.hpp \
OW_SyslogAppender.hpp \
OW_System.hpp \
OW_TempFileStream.hpp \
OW_ThreadBarrier.hpp \
OW_ThreadCancelledException.hpp \
OW_ThreadCounter.hpp \
OW_ThreadDoneCallback.hpp \
OW_Thread.hpp \
OW_ThreadImpl.hpp \
OW_ThreadOnce.hpp \
OW_ThreadPool.hpp \
OW_ThreadTypes.hpp \
OW_Timeout.hpp \
OW_TimeoutException.hpp \
OW_TimeoutTimer.hpp \
OW_Types.hpp \
OW_UnnamedPipe.hpp \
OW_URL.hpp \
OW_UserInfo.hpp \
OW_UserUtils.hpp \
OW_UTF8Utils.hpp \
OW_UUID.hpp \
OW_vector.hpp \
OW_WBEMFlags.hpp \
OW_WQLSelectStatement.hpp \
OW_WQLCompile.hpp \
OW_WQLOperand.hpp \
OW_WQLOperation.hpp \
OW_WQLPropertySource.hpp \
OW_WQLInstancePropertySource.hpp \
OW_GetPass.hpp \
OW_SocketAddress.hpp \
OW_ServerSocket.hpp \
OW_Socket.hpp \
OW_NwIface.hpp \
OW_SSLCtxMgr.hpp \
OW_SSLException.hpp \
OW_SocketException.hpp \
OW_SocketStreamBuffer.hpp \
OW_SocketUtils.hpp \
OW_SocketFlags.hpp \
OW_ThreadSafeProcess.hpp

libopenwbem_a_LIBADD = \
../cim/libOW_cim.a \
../ifcs/libOW_ifcs.a

