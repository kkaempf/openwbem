noinst_PROGRAMS = MemTracerTest


MemTracerTest_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/ -lopenwbem  \
$(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

MemTracerTest_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o MemTracerTest

MemTracerTest_SOURCES = \
MemTracerTest.cpp

check_PROGRAMS = unitMain
# Note: Do not break up the following line, the newtest.sh script
# expects all the files to be on one line.
unitMain_SOURCES = UnitMain.cpp UnitTestEnvironment.cpp UnitTestEnvironment.hpp OW_FormatTestCases.cpp OW_StringTestCases.cpp OW_LoggerTestCases.cpp OW_HTTPChunkerTestCases.cpp OW_ProviderManagerTestCases.cpp OW_SharedLibraryTestCases.cpp OW_SharedLibraryLoaderTestCases.cpp OW_ProviderMuxLoaderTestCases.cpp OW_ProviderMuxTestCases.cpp OW_LinuxSharedLibraryLoaderTestCases.cpp OW_FormatTestCases.hpp OW_HTTPChunkerTestCases.hpp OW_LinuxSharedLibraryLoaderTestCases.hpp OW_LoggerTestCases.hpp OW_ProviderManagerTestCases.hpp OW_ProviderMuxLoaderTestCases.hpp OW_ProviderMuxTestCases.hpp OW_SharedLibraryLoaderTestCases.hpp OW_SharedLibraryTestCases.hpp OW_StringTestCases.hpp testSharedLibraryLoader.hpp OW_ExecTestCases.cpp OW_ExecTestCases.hpp OW_StackTraceTestCases.cpp OW_StackTraceTestCases.hpp OW_EnumerationTestCases.cpp OW_EnumerationTestCases.hpp OW_XMLUnescapeTestCases.cpp OW_XMLUnescapeTestCases.hpp OW_XMLEscapeTestCases.cpp OW_XMLEscapeTestCases.hpp OW_MD5TestCases.cpp OW_MD5TestCases.hpp OW_Base64TestCases.cpp OW_Base64TestCases.hpp OW_InetAddressTestCases.cpp OW_InetAddressTestCases.hpp OW_ListenerAuthenticatorTestCases.cpp OW_ListenerAuthenticatorTestCases.hpp OW_CIMUrlTestCases.cpp OW_CIMUrlTestCases.hpp OW_MutexTestCases.cpp OW_MutexTestCases.hpp OW_MemTracerTestCases.cpp OW_MemTracerTestCases.hpp OW_StringStreamTestCases.cpp OW_StringStreamTestCases.hpp OW_ExceptionTestCases.cpp OW_ExceptionTestCases.hpp OW_CIMClassTestCases.cpp OW_CIMClassTestCases.hpp OW_CIMValueTestCases.cpp OW_CIMValueTestCases.hpp OW_StringArrayPropertyTestCases.cpp OW_StringArrayPropertyTestCases.hpp OW_CIMInstanceTestCases.cpp OW_CIMInstanceTestCases.hpp OW_CIMPropertyTestCases.cpp OW_CIMPropertyTestCases.hpp OW_FileSystemTestCases.cpp OW_FileSystemTestCases.hpp OW_RWLockerTestCases.cpp OW_RWLockerTestCases.hpp OW_SocketUtilsTestCases.cpp OW_SocketUtilsTestCases.hpp OW_ReferenceTestCases.cpp OW_ReferenceTestCases.hpp OW_ThreadBarrierTestCases.cpp OW_ThreadBarrierTestCases.hpp OW_ThreadTestCases.cpp OW_ThreadTestCases.hpp OW_ThreadPoolTestCases.cpp OW_ThreadPoolTestCases.hpp OW_UUIDTestCases.cpp OW_UUIDTestCases.hpp OW_RandomNumberTestCases.cpp OW_RandomNumberTestCases.hpp OW_UTF8UtilsTestCases.cpp OW_UTF8UtilsTestCases.hpp OW_DateTimeTestCases.cpp OW_DateTimeTestCases.hpp OW_CIMDateTimeTestCases.cpp OW_CIMDateTimeTestCases.hpp OW_HTTPUtilsTestCases.cpp OW_HTTPUtilsTestCases.hpp MOFCompilerTestCases.cpp MOFCompilerTestCases.hpp OperationContextTestCases.cpp OperationContextTestCases.hpp OW_BinarySerializationTestCases.cpp OW_BinarySerializationTestCases.hpp URLTestCases.cpp URLTestCases.hpp CIMObjectPathTestCases.cpp CIMObjectPathTestCases.hpp

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
-I$(top_srcdir)/src/mof

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
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/common -lopenwbem \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)


unitMain_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o unitMain

check_SCRIPTS = run_make_check.sh
EXTRA_DIST = $(check_SCRIPTS)

TESTS = run_make_check.sh

