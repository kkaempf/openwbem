!include ..\common.mk

LIBNAME = libopenwbem$(BLDCHAR)
CPPDEFS = $(CPPDEFS) /D"OW_COMMON_BUILD"
TARGET = $(LIBNAME).dll

INCS = \
/I$(BASEDIR) \
/I$(SRCDIR)\common \
/I$(SRCDIR)\cim \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\common\socket \
/I$(OPENSSLBASE)\include

LIBPATH = \
/LIBPATH:$(OPENSSLBASE)\lib\vc

LIBS = \
ws2_32.lib \
advapi32.lib \
libeay32.lib \
ssleay32.lib \
kernel32.lib \
user32.lib \
uuid.lib


COMMONSRCDIR = $(SRCDIR)\common
COMMONOBJS = \
	$(OBJDIR)\OW_AppenderLogger.obj \
	$(OBJDIR)\OW_Array.obj \
	$(OBJDIR)\OW_Assertion.obj \
	$(OBJDIR)\OW_AtomicOps.obj \
	$(OBJDIR)\OW_BaseStreamBuffer.obj \
	$(OBJDIR)\OW_BinarySerialization.obj \
	$(OBJDIR)\OW_Bool.obj \
	$(OBJDIR)\OW_CerrAppender.obj \
	$(OBJDIR)\OW_CIMErrorException.obj \
	$(OBJDIR)\OW_COWIntrusiveCountableBase.obj \
	$(OBJDIR)\OW_COWIntrusiveReference.obj \
	$(OBJDIR)\OW_COWReferenceBase.obj \
	$(OBJDIR)\OW_Char16.obj \
	$(OBJDIR)\OW_CmdLineParser.obj \
	$(OBJDIR)\OW_Condition.obj \
	$(OBJDIR)\OW_ConfigException.obj \
	$(OBJDIR)\OW_ConfigFile.obj \
	$(OBJDIR)\OW_DataStreams.obj \
	$(OBJDIR)\OW_DLLSharedLibrary.obj \
	$(OBJDIR)\OW_DLLSharedLibraryLoader.obj \
	$(OBJDIR)\OW_DateTime.obj \
	$(OBJDIR)\OW_EnumerationException.obj \
	$(OBJDIR)\OW_Exception.obj \
	$(OBJDIR)\OW_FileAppender.obj \
	$(OBJDIR)\OW_Format.obj \
	$(OBJDIR)\OW_IntrusiveCountableBase.obj \
	$(OBJDIR)\OW_IOException.obj \
	$(OBJDIR)\OW_IOIFCStreamBuffer.obj \
	$(OBJDIR)\OW_LogAppender.obj \
	$(OBJDIR)\OW_Logger.obj \
	$(OBJDIR)\OW_LogMessagePatternFormatter.obj \
	$(OBJDIR)\OW_LoggerImpls.obj \
	$(OBJDIR)\OW_MD5.obj \
	$(OBJDIR)\OW_MemTracer.obj \
	$(OBJDIR)\OW_Mutex.obj \
	$(OBJDIR)\OW_MutexImpl.obj \
	$(OBJDIR)\OW_NullAppender.obj \
	$(OBJDIR)\OW_NullLogger.obj \
	$(OBJDIR)\OW_NULLValueException.obj \
	$(OBJDIR)\OW_NoSuchPropertyException.obj \
	$(OBJDIR)\OW_NoSuchProviderException.obj \
	$(OBJDIR)\OW_NoSuchQualifierException.obj \
	$(OBJDIR)\OW_NonRecursiveMutex.obj \
	$(OBJDIR)\OW_NonRecursiveMutexImpl.obj \
	$(OBJDIR)\OW_OperationContext.obj \
	$(OBJDIR)\OW_OutOfBoundsException.obj \
	$(OBJDIR)\OW_PosixFile.obj \
	$(OBJDIR)\OW_PosixFileSystem.obj \
	$(OBJDIR)\OW_PosixUnnamedPipe.obj \
	$(OBJDIR)\OW_RWLocker.obj \
	$(OBJDIR)\OW_RandomNumber.obj \
	$(OBJDIR)\OW_ReferenceBase.obj \
	$(OBJDIR)\OW_RepositoryCIMOMHandle.obj \
	$(OBJDIR)\OW_ResultHandlers.obj \
	$(OBJDIR)\OW_Runnable.obj \
	$(OBJDIR)\OW_Select.obj \
	$(OBJDIR)\OW_SelectEngine.obj \
	$(OBJDIR)\OW_SessionLanguage.obj \
	$(OBJDIR)\OW_SharedLibrary.obj \
	$(OBJDIR)\OW_SharedLibraryException.obj \
	$(OBJDIR)\OW_SharedLibraryLoader.obj \
	$(OBJDIR)\OW_StackTrace.obj \
	$(OBJDIR)\OW_String.obj \
	$(OBJDIR)\OW_StringBuffer.obj \
	$(OBJDIR)\OW_StringStream.obj \
	$(OBJDIR)\OW_System.obj \
	$(OBJDIR)\OW_TempFileStream.obj \
	$(OBJDIR)\OW_TempFileEnumerationImplBase.obj \
	$(OBJDIR)\OW_Thread.obj \
	$(OBJDIR)\OW_ThreadBarrier.obj \
	$(OBJDIR)\OW_ThreadCancelledException.obj \
	$(OBJDIR)\OW_ThreadCounter.obj \
	$(OBJDIR)\OW_ThreadDoneCallback.obj \
	$(OBJDIR)\OW_ThreadImpl.obj \
	$(OBJDIR)\OW_ThreadPool.obj \
	$(OBJDIR)\OW_TimeoutException.obj \
	$(OBJDIR)\OW_TmpFile.obj \
	$(OBJDIR)\OW_URL.obj \
	$(OBJDIR)\OW_UTF8Utils.obj \
	$(OBJDIR)\OW_UTF8UtilscompareToIgnoreCase.obj \
	$(OBJDIR)\OW_UUID.obj \
	$(OBJDIR)\OW_UnnamedPipe.obj \
	$(OBJDIR)\OW_UserInfo.obj \
	$(OBJDIR)\OW_UserUtils.obj \
	$(OBJDIR)\OW_dlSharedLibrary.obj \
	$(OBJDIR)\OW_dlSharedLibraryLoader.obj \
	$(OBJDIR)\OW_dyldSharedLibrary.obj \
	$(OBJDIR)\OW_dyldSharedLibraryLoader.obj \
	$(OBJDIR)\OW_shlSharedLibrary.obj \
	$(OBJDIR)\OW_shlSharedLibraryLoader.obj

CIMSRCDIR = $(SRCDIR)\cim
CIMOBJS = \
	$(OBJDIR)\OW_CIMBase.obj \
	$(OBJDIR)\OW_CIMClass.obj \
	$(OBJDIR)\OW_CIMDataType.obj \
	$(OBJDIR)\OW_CIMDateTime.obj \
	$(OBJDIR)\OW_CIMElement.obj \
	$(OBJDIR)\OW_CIMException.obj \
	$(OBJDIR)\OW_CIMFlavor.obj \
	$(OBJDIR)\OW_CIMInstance.obj \
	$(OBJDIR)\OW_CIMMethod.obj \
	$(OBJDIR)\OW_CIMName.obj \
	$(OBJDIR)\OW_CIMNameSpace.obj \
	$(OBJDIR)\OW_CIMNameSpaceUtils.obj \
	$(OBJDIR)\OW_CIMObjectPath.obj \
	$(OBJDIR)\OW_CIMParamValue.obj \
	$(OBJDIR)\OW_CIMParameter.obj \
	$(OBJDIR)\OW_CIMProperty.obj \
	$(OBJDIR)\OW_CIMQualifier.obj \
	$(OBJDIR)\OW_CIMQualifierType.obj \
	$(OBJDIR)\OW_CIMScope.obj \
	$(OBJDIR)\OW_CIMUrl.obj \
	$(OBJDIR)\OW_CIMValue.obj \
	$(OBJDIR)\OW_CIMValueCast.obj 

SOCKETSRCDIR = $(SRCDIR)\common\socket
SOCKETOBJS = \
	$(OBJDIR)\OW_GetPass.obj \
	$(OBJDIR)\OW_NwIface.obj \
	$(OBJDIR)\OW_SSLCtxMgr.obj \
	$(OBJDIR)\OW_SSLSocketImpl.obj \
	$(OBJDIR)\OW_ServerSocketImpl.obj \
	$(OBJDIR)\OW_ServerSocket.obj \
	$(OBJDIR)\OW_Socket.obj \
	$(OBJDIR)\OW_SocketAddress.obj \
	$(OBJDIR)\OW_SocketBaseImpl.obj \
	$(OBJDIR)\OW_SocketImpl.obj \
	$(OBJDIR)\OW_SocketStreamBuffer.obj \
	$(OBJDIR)\OW_SocketUtils.obj \
	$(OBJDIR)\OW_Win32SocketBaseImpl.obj

IFCSRCDIR = $(SRCDIR)\ifcs
IFCOBJS = \
	$(OBJDIR)\OW_AuthenticationException.obj \
	$(OBJDIR)\OW_AuthenticatorIFC.obj \
	$(OBJDIR)\OW_AuthorizerIFC.obj \
	$(OBJDIR)\OW_CIMOMHandleIFC.obj \
	$(OBJDIR)\OW_CIMProtocolException.obj \
	$(OBJDIR)\OW_CIMProtocolIFC.obj \
	$(OBJDIR)\OW_CIMProtocolIStreamIFC.obj \
	$(OBJDIR)\OW_ClientAuthCBIFC.obj \
	$(OBJDIR)\OW_IOIFC.obj \
	$(OBJDIR)\OW_RepositoryIFC.obj \
	$(OBJDIR)\OW_RequestHandlerIFC.obj \
	$(OBJDIR)\OW_SelectableCallbackIFC.obj \
	$(OBJDIR)\OW_SelectableIFC.obj \
	$(OBJDIR)\OW_SerializableIFC.obj \
	$(OBJDIR)\OW_ServiceEnvironmentIFC.obj \
	$(OBJDIR)\OW_ServiceIFC.obj \
	$(OBJDIR)\OW_WQLIFC.obj

$(TARGETDIR)\$(TARGET): $(OBJDIR) $(TARGETDIR) $(COMMONOBJS) $(CIMOBJS) $(SOCKETOBJS) $(IFCOBJS)
	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATH) /DLL /DEBUG \
		/MAP:"$(TARGETDIR)\$(LIBNAME).map" /MAPINFO:EXPORTS \
		/SUBSYSTEM:WINDOWS /IMPLIB:"$(TARGETDIR)\$(LIBNAME).lib" \
		$(COMMONOBJS) $(CIMOBJS) $(SOCKETOBJS) $(IFCOBJS) $(LIBS)

clean:
	-@del /Q $(OBJDIR)\*.*
	-@del /Q $(TARGETDIR)\$(LIBNAME).*

$(TARGETDIR):
	IF NOT EXIST $(TARGETDIR) (md $(TARGETDIR))

$(OBJDIR):
	IF NOT EXIST $(OBJDIR) (md $(OBJDIR))


###############################################################################

$(OBJDIR)\OW_AppenderLogger.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Array.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Assertion.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_AtomicOps.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_BaseStreamBuffer.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_BinarySerialization.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Bool.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CerrAppender.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMErrorException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_COWIntrusiveCountableBase.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_COWIntrusiveReference.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_COWReferenceBase.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Char16.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CmdLineParser.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Condition.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ConfigException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ConfigFile.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_DLLSharedLibrary.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_DLLSharedLibraryLoader.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_DataStreams.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_DateTime.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_EnumerationException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Exception.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_FileAppender.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Format.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_IntrusiveCountableBase.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_IOException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_IOIFCStreamBuffer.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_LogAppender.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Logger.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_LogMessagePatternFormatter.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_LoggerImpls.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_MD5.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_MemTracer.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Mutex.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_MutexImpl.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_NullAppender.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_NullLogger.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_NULLValueException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_NoSuchPropertyException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_NoSuchProviderException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_NoSuchQualifierException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_NonRecursiveMutex.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_NonRecursiveMutexImpl.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_OperationContext.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_OutOfBoundsException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_PosixFile.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_PosixFileSystem.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_PosixUnnamedPipe.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_RWLocker.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_RandomNumber.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ReferenceBase.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_RepositoryCIMOMHandle.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ResultHandlers.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Runnable.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Select.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_SelectEngine.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_SessionLanguage.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_SharedLibrary.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_SharedLibraryLoader.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_SharedLibraryException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_StackTrace.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_String.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_StringBuffer.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_StringStream.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_System.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_TempFileStream.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_TempFileEnumerationImplBase.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Thread.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ThreadBarrier.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ThreadCancelledException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ThreadCounter.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ThreadDoneCallback.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ThreadImpl.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ThreadPool.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_TimeoutException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_TmpFile.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_URL.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_UTF8Utils.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_UTF8UtilscompareToIgnoreCase.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_UUID.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_UnnamedPipe.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_UserInfo.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_UserUtils.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_dlSharedLibrary.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_dlSharedLibraryLoader.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_dyldSharedLibrary.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_dyldSharedLibraryLoader.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_shlSharedLibrary.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_shlSharedLibraryLoader.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

###############################################################################
$(OBJDIR)\OW_CIMBase.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMClass.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMDataType.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMDateTime.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMElement.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMException.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMFlavor.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMInstance.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMMethod.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMName.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMNameSpace.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMNameSpaceUtils.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMObjectPath.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMParamValue.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMParameter.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMProperty.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMQualifier.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMQualifierType.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMScope.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMUrl.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMValue.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMValueCast.obj: $(CIMSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\$(@B).cpp

###############################################################################
	
$(OBJDIR)\OW_GetPass.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_NwIface.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_SSLCtxMgr.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_SSLSocketImpl.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_ServerSocketImpl.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_ServerSocket.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_Socket.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_SocketAddress.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_SocketBaseImpl.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_SocketImpl.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_SocketStreamBuffer.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_SocketUtils.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

$(OBJDIR)\OW_Win32SocketBaseImpl.obj: $(SOCKETSRCDIR)/$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/$(@B).cpp

###############################################################################

$(OBJDIR)\OW_AuthenticationException.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_AuthenticatorIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_AuthorizerIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMOMHandleIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMProtocolException.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMProtocolIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMProtocolIStreamIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ClientAuthCBIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_IOIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_RepositoryIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_RequestHandlerIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_SelectableCallbackIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_SelectableIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_SerializableIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ServiceEnvironmentIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ServiceIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_WQLIFC.obj: $(IFCSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\$(@B).cpp

