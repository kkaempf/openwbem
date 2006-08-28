if DISABLE_INSTANCE_MANIPULATION
PROGS=
else
if DISABLE_SCHEMA_MANIPULATION
PROGS=
else
if DISABLE_QUALIFIER_DECLARATION
PROGS=
else
if DISABLE_NAMESPACE_MANIPULATION
PROGS=
else
PROGS= MemTracerTest unitMain exitWithSignal fillOutputBuffer runMultiProcessLogger
endif
endif
endif
endif

noinst_PROGRAMS = $(PROGS)
#check_PROGRAMS = unitMain


MemTracerTest_LDADD = \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
-L$(top_builddir)/src/ -lopenwbem -lblocxx  \
$(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

MemTracerTest_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o MemTracerTest$(EXEEXT)

MemTracerTest_SOURCES = \
MemTracerTest.cpp

runMultiProcessLogger_LDADD = \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
-L$(top_builddir)/src/ -lopenwbem -lblocxx  \
$(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

runMultiProcessLogger_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o runMultiProcessLogger$(EXEEXT)

runMultiProcessLogger_SOURCES = \
runMultiProcessLogger.cpp

check_PROGRAMS = unitMain
# Note: Do not change the following line, unless you also update the newtest.sh script
unitMain_SOURCES = \
OW_LazyGlobalTestCases.cpp \
OW_LazyGlobalTestCases.hpp \
LogAppenderTestCases.cpp \
LogAppenderTestCases.hpp \
OW_DelayedFormatTestCases.cpp \
OW_DelayedFormatTestCases.hpp \
OW_ScopeLoggerTestCases.cpp \
OW_ScopeLoggerTestCases.hpp \
GlobalPtrTestCases.cpp \
GlobalPtrTestCases.hpp \
IStringStreamTestCases.cpp \
IStringStreamTestCases.hpp \
UnnamedPipeTestCases.cpp \
UnnamedPipeTestCases.hpp \
ProcessTestCases.cpp \
ProcessTestCases.hpp \
ConditionTestCases.cpp \
ConditionTestCases.hpp \
TimeoutTimerTestCases.cpp \
TimeoutTimerTestCases.hpp \
ThreadOnceTestCases.cpp \
ThreadOnceTestCases.hpp \
CryptographicRandomNumberTestCases.cpp \
CryptographicRandomNumberTestCases.hpp \
CIMObjectPathTestCases.cpp \
CIMObjectPathTestCases.hpp \
CIMRepository2TestCases.cpp \
CIMRepository2TestCases.hpp \
CmdLineParserTestCases.cpp \
CmdLineParserTestCases.hpp \
COWIntrusiveReferenceTestCases.cpp \
COWIntrusiveReferenceTestCases.hpp \
MOFCompilerTestCases.cpp \
MOFCompilerTestCases.hpp \
OperationContextTestCases.cpp \
OperationContextTestCases.hpp \
OW_Base64TestCases.cpp \
OW_Base64TestCases.hpp \
OW_BinarySerializationTestCases.cpp \
OW_BinarySerializationTestCases.hpp \
OW_CIMClassTestCases.cpp \
OW_CIMClassTestCases.hpp \
OW_CIMDateTimeTestCases.cpp \
OW_CIMDateTimeTestCases.hpp \
OW_CIMInstanceTestCases.cpp \
OW_CIMInstanceTestCases.hpp \
OW_CIMPropertyTestCases.cpp \
OW_CIMPropertyTestCases.hpp \
OW_CIMUrlTestCases.cpp \
OW_CIMUrlTestCases.hpp \
OW_CIMValueTestCases.cpp \
OW_CIMValueTestCases.hpp \
OW_DateTimeTestCases.cpp \
OW_DateTimeTestCases.hpp \
OW_EnumerationTestCases.cpp \
OW_EnumerationTestCases.hpp \
OW_ExceptionTestCases.cpp \
OW_ExceptionTestCases.hpp \
OW_ExecTestCases.cpp \
OW_ExecTestCases.hpp \
OW_FileSystemTestCases.cpp \
OW_FileSystemTestCases.hpp \
OW_FormatTestCases.cpp \
OW_FormatTestCases.hpp \
OW_HTTPChunkerTestCases.cpp \
OW_HTTPChunkerTestCases.hpp \
OW_HTTPUtilsTestCases.cpp \
OW_HTTPUtilsTestCases.hpp \
OW_InetAddressTestCases.cpp \
OW_InetAddressTestCases.hpp \
OW_LinuxSharedLibraryLoaderTestCases.cpp \
OW_LinuxSharedLibraryLoaderTestCases.hpp \
OW_ListenerAuthenticatorTestCases.cpp \
OW_ListenerAuthenticatorTestCases.hpp \
OW_LoggerTestCases.cpp \
OW_LoggerTestCases.hpp \
OW_MD5TestCases.cpp \
OW_MD5TestCases.hpp \
OW_MemTracerTestCases.cpp \
OW_MemTracerTestCases.hpp \
OW_MutexTestCases.cpp \
OW_MutexTestCases.hpp \
OW_ProviderManagerTestCases.cpp \
OW_ProviderManagerTestCases.hpp \
OW_ProviderMuxLoaderTestCases.cpp \
OW_ProviderMuxLoaderTestCases.hpp \
OW_ProviderMuxTestCases.cpp \
OW_ProviderMuxTestCases.hpp \
OW_RandomNumberTestCases.cpp \
OW_RandomNumberTestCases.hpp \
OW_ReferenceTestCases.cpp \
OW_ReferenceTestCases.hpp \
OW_RWLockerTestCases.cpp \
OW_RWLockerTestCases.hpp \
OW_SharedLibraryLoaderTestCases.cpp \
OW_SharedLibraryLoaderTestCases.hpp \
OW_SharedLibraryTestCases.cpp \
OW_SharedLibraryTestCases.hpp \
OW_SocketUtilsTestCases.cpp \
OW_SocketUtilsTestCases.hpp \
OW_StackTraceTestCases.cpp \
OW_StackTraceTestCases.hpp \
OW_StringArrayPropertyTestCases.cpp \
OW_StringArrayPropertyTestCases.hpp \
OW_StringStreamTestCases.cpp \
OW_StringStreamTestCases.hpp \
OW_StringTestCases.cpp \
OW_StringTestCases.hpp \
OW_ThreadBarrierTestCases.cpp \
OW_ThreadBarrierTestCases.hpp \
OW_ThreadPoolTestCases.cpp \
OW_ThreadPoolTestCases.hpp \
OW_ThreadTestCases.cpp \
OW_ThreadTestCases.hpp \
OW_UTF8UtilsTestCases.cpp \
OW_UTF8UtilsTestCases.hpp \
OW_UUIDTestCases.cpp \
OW_UUIDTestCases.hpp \
OW_XMLEscapeTestCases.cpp \
OW_XMLEscapeTestCases.hpp \
OW_XMLUnescapeTestCases.cpp \
OW_XMLUnescapeTestCases.hpp \
URLTestCases.cpp \
URLTestCases.hpp \
testSharedLibraryLoader.hpp \
UnitMain.cpp \
UnitTestEnvironment.cpp \
UnitTestEnvironment.hpp

INCLUDES = \
-I$(top_srcdir)/test/CppUnit \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/cimom/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/cimxmllistener \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/mof \
-I$(top_srcdir)/src/repositories/tdbcr \
-I$(top_srcdir)/src/monitor

if ENABLE_DB4_REPOSITORY
REPLIBFLAGS=-L$(top_builddir)/src/repositories/tdbcr -lowrepositorytdbcr
else
REPLIBFLAGS=
endif

unitMain_LDADD = \
-L$(top_builddir)/test/CppUnit -lCppUnit \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/mof -lowmofc \
-L$(top_builddir)/src/cimom/common -lowcimomcommon \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
$(REPLIBFLAGS) \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(REALTIME_LIBS) \
$(BINLINKAPPENDFLAGS)


unitMain_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o unitMain$(EXEEXT)

check_SCRIPTS = run_make_check.sh
EXTRA_DIST = $(check_SCRIPTS)

TESTS = run_make_check.sh

exitWithSignal_LDADD = \
$(BINLINKAPPENDFLAGS)

exitWithSignal_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o exitWithSignal$(EXEEXT)

exitWithSignal_SOURCES = \
exitWithSignal.cpp

fillOutputBuffer_LDADD = \
$(BINLINKAPPENDFLAGS)

fillOutputBuffer_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o fillOutputBuffer$(EXEEXT)

fillOutputBuffer_SOURCES = \
fillOutputBuffer.cpp

include $(top_srcdir)/Makefile-shared-lib.inc
# We don't need a dylib, as we only want to load and not link this library.
MAKEDYLIB=0
MAKEBUNDLE=1
OW_SHARED_LIBS = libLazyGlobalTestLibrary.$(LIB_EXT)
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)
SHAREDLIBADD = -L$(top_builddir)/src/common -lopenwbem -lblocxx

noinst_LIBRARIES = libLazyGlobalTestLibrary.a
libLazyGlobalTestLibrary_a_SOURCES = OW_LazyGlobalTestLibrary.cpp

all-local: all-ow-shared-libs
clean-local: clean-ow-shared-libs

