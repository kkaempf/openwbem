DEBUG = 1
WARNLEVEL = /W0
LIBNAME = libopenwbem
OPENSSLBASE = c:\openssl

BASEDIR = ..
TARGET = $(LIBNAME).dll

CPP = cl
LINK = link

SRCDIR = ..\..\..\..\src
TOOLSDIR = ..\tools

!IFDEF DEBUG
BUILDTYPE = Debug
CPPDEFS = /D"OW_COMMON_BUILD" /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CPPFLAGS = /GF /GX /RTC1 /Zi /Zc:forScope /GR $(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
LINKDBG = /DEBUG
!ELSE
BUILDTYPE = Release
CPPDEFS = /D"OW_COMMON_BUILD" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /GX /Zc:forScope /GR $(WARNLEVL) /nologo /Wp64 /MD /c
LINKDBG = 
!ENDIF

OBJDIR = $(BUILDTYPE)
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)

INCS = \
/I".." \
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
	$(OBJDIR)\OW_Array.obj \
	$(OBJDIR)\OW_Assertion.obj \
	$(OBJDIR)\OW_AtomicOps.obj \
	$(OBJDIR)\OW_BaseStreamBuffer.obj \
	$(OBJDIR)\OW_BinarySerialization.obj \
	$(OBJDIR)\OW_Bool.obj \
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
	$(OBJDIR)\OW_Format.obj \
	$(OBJDIR)\OW_IntrusiveCountableBase.obj \
	$(OBJDIR)\OW_IOException.obj \
	$(OBJDIR)\OW_IOIFCStreamBuffer.obj \
	$(OBJDIR)\OW_Logger.obj \
	$(OBJDIR)\OW_LoggerImpls.obj \
	$(OBJDIR)\OW_MD5.obj \
	$(OBJDIR)\OW_MemTracer.obj \
	$(OBJDIR)\OW_Mutex.obj \
	$(OBJDIR)\OW_MutexImpl.obj \
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
	$(OBJDIR)\OW_ServiceIFC.obj 

$(TARGETDIR)\$(TARGET): $(OBJDIR) $(TARGETDIR) $(COMMONOBJS) $(CIMOBJS) $(SOCKETOBJS) $(IFCOBJS)
	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATH) /DLL /DEBUG \
		/MAP:"$(TARGETDIR)\$(LIBNAME).map" /MAPINFO:EXPORTS /SUBSYSTEM:WINDOWS \
		/IMPLIB:"$(TARGETDIR)\$(LIBNAME).lib" $(COMMONOBJS) $(CIMOBJS) \
		$(SOCKETOBJS) $(IFCOBJS) $(LIBS)

clean:
	-@del $(OBJDIR)\*.obj
	-@del $(TARGETDIR)\$(LIBNAME).map
	-@del $(TARGETDIR)\$(LIBNAME).def
	-@del $(TARGETDIR)\$(LIBNAME).dll
	-@del $(TARGETDIR)\$(LIBNAME).lib
	-@del $(TARGETDIR)\$(LIBNAME).pdb

$(TARGETDIR):
	IF NOT EXIST $(TARGETDIR) (md $(TARGETDIR))

$(OBJDIR):
	IF NOT EXIST $(OBJDIR) (md $(OBJDIR))


###############################################################################

$(OBJDIR)\OW_Array.obj: $(COMMONSRCDIR)\OW_Array.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Array.cpp

$(OBJDIR)\OW_Assertion.obj: $(COMMONSRCDIR)\OW_Assertion.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Assertion.cpp

$(OBJDIR)\OW_AtomicOps.obj: $(COMMONSRCDIR)\OW_AtomicOps.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_AtomicOps.cpp

$(OBJDIR)\OW_BaseStreamBuffer.obj: $(COMMONSRCDIR)\OW_BaseStreamBuffer.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_BaseStreamBuffer.cpp

$(OBJDIR)\OW_BinarySerialization.obj: $(COMMONSRCDIR)\OW_BinarySerialization.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_BinarySerialization.cpp

$(OBJDIR)\OW_Bool.obj: $(COMMONSRCDIR)\OW_Bool.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Bool.cpp

$(OBJDIR)\OW_CIMErrorException.obj: $(COMMONSRCDIR)\OW_CIMErrorException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_CIMErrorException.cpp

$(OBJDIR)\OW_COWIntrusiveCountableBase.obj: $(COMMONSRCDIR)\OW_COWIntrusiveCountableBase.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_COWIntrusiveCountableBase.cpp

$(OBJDIR)\OW_COWIntrusiveReference.obj: $(COMMONSRCDIR)\OW_COWIntrusiveReference.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_COWIntrusiveReference.cpp

$(OBJDIR)\OW_COWReferenceBase.obj: $(COMMONSRCDIR)\OW_COWReferenceBase.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_COWReferenceBase.cpp

$(OBJDIR)\OW_Char16.obj: $(COMMONSRCDIR)\OW_Char16.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Char16.cpp

$(OBJDIR)\OW_CmdLineParser.obj: $(COMMONSRCDIR)\OW_CmdLineParser.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_CmdLineParser.cpp

$(OBJDIR)\OW_Condition.obj: $(COMMONSRCDIR)\OW_Condition.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Condition.cpp

$(OBJDIR)\OW_ConfigException.obj: $(COMMONSRCDIR)\OW_ConfigException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_ConfigException.cpp

$(OBJDIR)\OW_ConfigFile.obj: $(COMMONSRCDIR)\OW_ConfigFile.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_ConfigFile.cpp

$(OBJDIR)\OW_DLLSharedLibrary.obj: $(COMMONSRCDIR)\OW_DLLSharedLibrary.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_DLLSharedLibrary.cpp

$(OBJDIR)\OW_DLLSharedLibraryLoader.obj: $(COMMONSRCDIR)\OW_DLLSharedLibraryLoader.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_DLLSharedLibraryLoader.cpp

$(OBJDIR)\OW_DataStreams.obj: $(COMMONSRCDIR)\OW_DataStreams.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_DataStreams.cpp

$(OBJDIR)\OW_DateTime.obj: $(COMMONSRCDIR)\OW_DateTime.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_DateTime.cpp

$(OBJDIR)\OW_EnumerationException.obj: $(COMMONSRCDIR)\OW_EnumerationException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_EnumerationException.cpp

$(OBJDIR)\OW_Exception.obj: $(COMMONSRCDIR)\OW_Exception.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Exception.cpp

$(OBJDIR)\OW_Format.obj: $(COMMONSRCDIR)\OW_Format.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Format.cpp

$(OBJDIR)\OW_IntrusiveCountableBase.obj: $(COMMONSRCDIR)\OW_IntrusiveCountableBase.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_IntrusiveCountableBase.cpp

$(OBJDIR)\OW_IOException.obj: $(COMMONSRCDIR)\OW_IOException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_IOException.cpp

$(OBJDIR)\OW_IOIFCStreamBuffer.obj: $(COMMONSRCDIR)\OW_IOIFCStreamBuffer.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_IOIFCStreamBuffer.cpp

$(OBJDIR)\OW_Logger.obj: $(COMMONSRCDIR)\OW_Logger.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Logger.cpp

$(OBJDIR)\OW_LoggerImpls.obj: $(COMMONSRCDIR)\OW_LoggerImpls.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_LoggerImpls.cpp

$(OBJDIR)\OW_MD5.obj: $(COMMONSRCDIR)\OW_MD5.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_MD5.cpp

$(OBJDIR)\OW_MemTracer.obj: $(COMMONSRCDIR)\OW_MemTracer.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_MemTracer.cpp

$(OBJDIR)\OW_Mutex.obj: $(COMMONSRCDIR)\OW_Mutex.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Mutex.cpp

$(OBJDIR)\OW_MutexImpl.obj: $(COMMONSRCDIR)\OW_MutexImpl.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_MutexImpl.cpp

$(OBJDIR)\OW_NULLValueException.obj: $(COMMONSRCDIR)\OW_NULLValueException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_NULLValueException.cpp

$(OBJDIR)\OW_NoSuchPropertyException.obj: $(COMMONSRCDIR)\OW_NoSuchPropertyException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_NoSuchPropertyException.cpp

$(OBJDIR)\OW_NoSuchProviderException.obj: $(COMMONSRCDIR)\OW_NoSuchProviderException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_NoSuchProviderException.cpp

$(OBJDIR)\OW_NoSuchQualifierException.obj: $(COMMONSRCDIR)\OW_NoSuchQualifierException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_NoSuchQualifierException.cpp

$(OBJDIR)\OW_NonRecursiveMutex.obj: $(COMMONSRCDIR)\OW_NonRecursiveMutex.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_NonRecursiveMutex.cpp

$(OBJDIR)\OW_NonRecursiveMutexImpl.obj: $(COMMONSRCDIR)\OW_NonRecursiveMutexImpl.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_NonRecursiveMutexImpl.cpp

$(OBJDIR)\OW_OperationContext.obj: $(COMMONSRCDIR)\OW_OperationContext.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_OperationContext.cpp

$(OBJDIR)\OW_OutOfBoundsException.obj: $(COMMONSRCDIR)\OW_OutOfBoundsException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_OutOfBoundsException.cpp

$(OBJDIR)\OW_PosixFile.obj: $(COMMONSRCDIR)\OW_PosixFile.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_PosixFile.cpp

$(OBJDIR)\OW_PosixFileSystem.obj: $(COMMONSRCDIR)\OW_PosixFileSystem.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_PosixFileSystem.cpp

$(OBJDIR)\OW_PosixUnnamedPipe.obj: $(COMMONSRCDIR)\OW_PosixUnnamedPipe.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_PosixUnnamedPipe.cpp

$(OBJDIR)\OW_RWLocker.obj: $(COMMONSRCDIR)\OW_RWLocker.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_RWLocker.cpp

$(OBJDIR)\OW_RandomNumber.obj: $(COMMONSRCDIR)\OW_RandomNumber.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_RandomNumber.cpp

$(OBJDIR)\OW_ReferenceBase.obj: $(COMMONSRCDIR)\OW_ReferenceBase.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_ReferenceBase.cpp

$(OBJDIR)\OW_RepositoryCIMOMHandle.obj: $(COMMONSRCDIR)\OW_RepositoryCIMOMHandle.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_RepositoryCIMOMHandle.cpp

$(OBJDIR)\OW_ResultHandlers.obj: $(COMMONSRCDIR)\OW_ResultHandlers.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_ResultHandlers.cpp

$(OBJDIR)\OW_Runnable.obj: $(COMMONSRCDIR)\OW_Runnable.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Runnable.cpp

$(OBJDIR)\OW_Select.obj: $(COMMONSRCDIR)\OW_Select.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Select.cpp

$(OBJDIR)\OW_SelectEngine.obj: $(COMMONSRCDIR)\OW_SelectEngine.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_SelectEngine.cpp

$(OBJDIR)\OW_SessionLanguage.obj: $(COMMONSRCDIR)\OW_SessionLanguage.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_SessionLanguage.cpp

$(OBJDIR)\OW_SharedLibrary.obj: $(COMMONSRCDIR)\OW_SharedLibrary.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_SharedLibrary.cpp

$(OBJDIR)\OW_SharedLibraryLoader.obj: $(COMMONSRCDIR)\OW_SharedLibraryLoader.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_SharedLibraryLoader.cpp

$(OBJDIR)\OW_SharedLibraryException.obj: $(COMMONSRCDIR)\OW_SharedLibraryException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_SharedLibraryException.cpp

$(OBJDIR)\OW_StackTrace.obj: $(COMMONSRCDIR)\OW_StackTrace.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_StackTrace.cpp

$(OBJDIR)\OW_String.obj: $(COMMONSRCDIR)\OW_String.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_String.cpp

$(OBJDIR)\OW_StringBuffer.obj: $(COMMONSRCDIR)\OW_StringBuffer.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_StringBuffer.cpp

$(OBJDIR)\OW_StringStream.obj: $(COMMONSRCDIR)\OW_StringStream.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_StringStream.cpp

$(OBJDIR)\OW_System.obj: $(COMMONSRCDIR)\OW_System.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_System.cpp

$(OBJDIR)\OW_TempFileStream.obj: $(COMMONSRCDIR)\OW_TempFileStream.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_TempFileStream.cpp

$(OBJDIR)\OW_TempFileEnumerationImplBase.obj: $(COMMONSRCDIR)\OW_TempFileEnumerationImplBase.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_TempFileEnumerationImplBase.cpp

$(OBJDIR)\OW_Thread.obj: $(COMMONSRCDIR)\OW_Thread.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_Thread.cpp

$(OBJDIR)\OW_ThreadBarrier.obj: $(COMMONSRCDIR)\OW_ThreadBarrier.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_ThreadBarrier.cpp

$(OBJDIR)\OW_ThreadCancelledException.obj: $(COMMONSRCDIR)\OW_ThreadCancelledException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_ThreadCancelledException.cpp

$(OBJDIR)\OW_ThreadCounter.obj: $(COMMONSRCDIR)\OW_ThreadCounter.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_ThreadCounter.cpp

$(OBJDIR)\OW_ThreadDoneCallback.obj: $(COMMONSRCDIR)\OW_ThreadDoneCallback.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_ThreadDoneCallback.cpp

$(OBJDIR)\OW_ThreadImpl.obj: $(COMMONSRCDIR)\OW_ThreadImpl.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_ThreadImpl.cpp

$(OBJDIR)\OW_ThreadPool.obj: $(COMMONSRCDIR)\OW_ThreadPool.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_ThreadPool.cpp

$(OBJDIR)\OW_TimeoutException.obj: $(COMMONSRCDIR)\OW_TimeoutException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_TimeoutException.cpp

$(OBJDIR)\OW_TmpFile.obj: $(COMMONSRCDIR)\OW_TmpFile.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_TmpFile.cpp

$(OBJDIR)\OW_URL.obj: $(COMMONSRCDIR)\OW_URL.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_URL.cpp

$(OBJDIR)\OW_UTF8Utils.obj: $(COMMONSRCDIR)\OW_UTF8Utils.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_UTF8Utils.cpp

$(OBJDIR)\OW_UTF8UtilscompareToIgnoreCase.obj: $(COMMONSRCDIR)\OW_UTF8UtilscompareToIgnoreCase.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_UTF8UtilscompareToIgnoreCase.cpp

$(OBJDIR)\OW_UUID.obj: $(COMMONSRCDIR)\OW_UUID.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_UUID.cpp

$(OBJDIR)\OW_UnnamedPipe.obj: $(COMMONSRCDIR)\OW_UnnamedPipe.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_UnnamedPipe.cpp

$(OBJDIR)\OW_UserInfo.obj: $(COMMONSRCDIR)\OW_UserInfo.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_UserInfo.cpp

$(OBJDIR)\OW_UserUtils.obj: $(COMMONSRCDIR)\OW_UserUtils.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_UserUtils.cpp

$(OBJDIR)\OW_dlSharedLibrary.obj: $(COMMONSRCDIR)\OW_dlSharedLibrary.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_dlSharedLibrary.cpp

$(OBJDIR)\OW_dlSharedLibraryLoader.obj: $(COMMONSRCDIR)\OW_dlSharedLibraryLoader.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_dlSharedLibraryLoader.cpp

$(OBJDIR)\OW_dyldSharedLibrary.obj: $(COMMONSRCDIR)\OW_dyldSharedLibrary.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_dyldSharedLibrary.cpp

$(OBJDIR)\OW_dyldSharedLibraryLoader.obj: $(COMMONSRCDIR)\OW_dyldSharedLibraryLoader.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_dyldSharedLibraryLoader.cpp

$(OBJDIR)\OW_shlSharedLibrary.obj: $(COMMONSRCDIR)\OW_shlSharedLibrary.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_shlSharedLibrary.cpp

$(OBJDIR)\OW_shlSharedLibraryLoader.obj: $(COMMONSRCDIR)\OW_shlSharedLibraryLoader.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_shlSharedLibraryLoader.cpp

###############################################################################
$(OBJDIR)\OW_CIMBase.obj: $(CIMSRCDIR)\OW_CIMBase.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMBase.cpp

$(OBJDIR)\OW_CIMClass.obj: $(CIMSRCDIR)\OW_CIMClass.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMClass.cpp

$(OBJDIR)\OW_CIMDataType.obj: $(CIMSRCDIR)\OW_CIMDataType.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMDataType.cpp

$(OBJDIR)\OW_CIMDateTime.obj: $(CIMSRCDIR)\OW_CIMDateTime.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMDateTime.cpp

$(OBJDIR)\OW_CIMElement.obj: $(CIMSRCDIR)\OW_CIMElement.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMElement.cpp

$(OBJDIR)\OW_CIMException.obj: $(CIMSRCDIR)\OW_CIMException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMException.cpp

$(OBJDIR)\OW_CIMFlavor.obj: $(CIMSRCDIR)\OW_CIMFlavor.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMFlavor.cpp

$(OBJDIR)\OW_CIMInstance.obj: $(CIMSRCDIR)\OW_CIMInstance.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMInstance.cpp

$(OBJDIR)\OW_CIMMethod.obj: $(CIMSRCDIR)\OW_CIMMethod.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMMethod.cpp

$(OBJDIR)\OW_CIMNameSpace.obj: $(CIMSRCDIR)\OW_CIMNameSpace.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMNameSpace.cpp

$(OBJDIR)\OW_CIMNameSpaceUtils.obj: $(CIMSRCDIR)\OW_CIMNameSpaceUtils.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMNameSpaceUtils.cpp

$(OBJDIR)\OW_CIMObjectPath.obj: $(CIMSRCDIR)\OW_CIMObjectPath.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMObjectPath.cpp

$(OBJDIR)\OW_CIMParamValue.obj: $(CIMSRCDIR)\OW_CIMParamValue.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMParamValue.cpp

$(OBJDIR)\OW_CIMParameter.obj: $(CIMSRCDIR)\OW_CIMParameter.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMParameter.cpp

$(OBJDIR)\OW_CIMProperty.obj: $(CIMSRCDIR)\OW_CIMProperty.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMProperty.cpp

$(OBJDIR)\OW_CIMQualifier.obj: $(CIMSRCDIR)\OW_CIMQualifier.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMQualifier.cpp

$(OBJDIR)\OW_CIMQualifierType.obj: $(CIMSRCDIR)\OW_CIMQualifierType.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMQualifierType.cpp

$(OBJDIR)\OW_CIMScope.obj: $(CIMSRCDIR)\OW_CIMScope.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMScope.cpp

$(OBJDIR)\OW_CIMUrl.obj: $(CIMSRCDIR)\OW_CIMUrl.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMUrl.cpp

$(OBJDIR)\OW_CIMValue.obj: $(CIMSRCDIR)\OW_CIMValue.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMValue.cpp

$(OBJDIR)\OW_CIMValueCast.obj: $(CIMSRCDIR)\OW_CIMValueCast.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CIMSRCDIR)\OW_CIMValueCast.cpp

###############################################################################
	
$(OBJDIR)\OW_GetPass.obj: $(SOCKETSRCDIR)/OW_GetPass.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_GetPass.cpp

$(OBJDIR)\OW_NwIface.obj: $(SOCKETSRCDIR)/OW_NwIface.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_NwIface.cpp

$(OBJDIR)\OW_SSLCtxMgr.obj: $(SOCKETSRCDIR)/OW_SSLCtxMgr.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_SSLCtxMgr.cpp

$(OBJDIR)\OW_SSLSocketImpl.obj: $(SOCKETSRCDIR)/OW_SSLSocketImpl.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_SSLSocketImpl.cpp

$(OBJDIR)\OW_ServerSocketImpl.obj: $(SOCKETSRCDIR)/OW_ServerSocketImpl.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_ServerSocketImpl.cpp

$(OBJDIR)\OW_Socket.obj: $(SOCKETSRCDIR)/OW_Socket.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_Socket.cpp

$(OBJDIR)\OW_SocketAddress.obj: $(SOCKETSRCDIR)/OW_SocketAddress.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_SocketAddress.cpp

$(OBJDIR)\OW_SocketBaseImpl.obj: $(SOCKETSRCDIR)/OW_SocketBaseImpl.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_SocketBaseImpl.cpp

$(OBJDIR)\OW_SocketImpl.obj: $(SOCKETSRCDIR)/OW_SocketImpl.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_SocketImpl.cpp

$(OBJDIR)\OW_SocketStreamBuffer.obj: $(SOCKETSRCDIR)/OW_SocketStreamBuffer.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_SocketStreamBuffer.cpp

$(OBJDIR)\OW_SocketUtils.obj: $(SOCKETSRCDIR)/OW_SocketUtils.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_SocketUtils.cpp

$(OBJDIR)\OW_Win32SocketBaseImpl.obj: $(SOCKETSRCDIR)/OW_Win32SocketBaseImpl.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SOCKETSRCDIR)/OW_Win32SocketBaseImpl.cpp

###############################################################################

$(OBJDIR)\OW_AuthenticationException.obj: $(IFCSRCDIR)\OW_AuthenticationException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_AuthenticationException.cpp

$(OBJDIR)\OW_AuthenticatorIFC.obj: $(IFCSRCDIR)\OW_AuthenticatorIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_AuthenticatorIFC.cpp

$(OBJDIR)\OW_AuthorizerIFC.obj: $(IFCSRCDIR)\OW_AuthorizerIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_AuthorizerIFC.cpp

$(OBJDIR)\OW_CIMOMHandleIFC.obj: $(IFCSRCDIR)\OW_CIMOMHandleIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_CIMOMHandleIFC.cpp

$(OBJDIR)\OW_CIMProtocolException.obj: $(IFCSRCDIR)\OW_CIMProtocolException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_CIMProtocolException.cpp

$(OBJDIR)\OW_CIMProtocolIFC.obj: $(IFCSRCDIR)\OW_CIMProtocolIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_CIMProtocolIFC.cpp

$(OBJDIR)\OW_CIMProtocolIStreamIFC.obj: $(IFCSRCDIR)\OW_CIMProtocolIStreamIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_CIMProtocolIStreamIFC.cpp

$(OBJDIR)\OW_ClientAuthCBIFC.obj: $(IFCSRCDIR)\OW_ClientAuthCBIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_ClientAuthCBIFC.cpp

$(OBJDIR)\OW_IOIFC.obj: $(IFCSRCDIR)\OW_IOIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_IOIFC.cpp

$(OBJDIR)\OW_RepositoryIFC.obj: $(IFCSRCDIR)\OW_RepositoryIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_RepositoryIFC.cpp

$(OBJDIR)\OW_RequestHandlerIFC.obj: $(IFCSRCDIR)\OW_RequestHandlerIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_RequestHandlerIFC.cpp

$(OBJDIR)\OW_SelectableCallbackIFC.obj: $(IFCSRCDIR)\OW_SelectableCallbackIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_SelectableCallbackIFC.cpp

$(OBJDIR)\OW_SelectableIFC.obj: $(IFCSRCDIR)\OW_SelectableIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_SelectableIFC.cpp

$(OBJDIR)\OW_SerializableIFC.obj: $(IFCSRCDIR)\OW_SerializableIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_SerializableIFC.cpp

$(OBJDIR)\OW_ServiceEnvironmentIFC.obj: $(IFCSRCDIR)\OW_ServiceEnvironmentIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_ServiceEnvironmentIFC.cpp

$(OBJDIR)\OW_ServiceIFC.obj: $(IFCSRCDIR)\OW_ServiceIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(IFCSRCDIR)\OW_ServiceIFC.cpp

