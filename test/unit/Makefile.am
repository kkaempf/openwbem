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
PROGS= MemTracerTest exitWithSignal fillOutputBuffer runMultiProcessLogger ExecTestProgram
endif
endif
endif
endif

noinst_PROGRAMS = $(PROGS) $(BUILT_TESTS)

COMMON_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS)
# CppUnit is listed here because as a static object, it must occur *AFTER* the
# objects that use it.
COMMON_EXTERNAL_LIBS = \
-L$(top_builddir)/test/CppUnit -lCppUnit \
$(SSL_LIBS)


MemTracerTest_PROJECT_LIBS = $(COMMON_PROJECT_LIBS)
MemTracerTest_EXTERNAL_LIBS = $(SSL_LIBS)
MemTracerTest_SOURCES = MemTracerTest.cpp

ExecTestProgram_PROJECT_LIBS = $(BLOCXX_LIBS)
ExecTestProgram_EXTERNAL_LIBS = $(SSL_LIBS) $(REALTIME_LIBS)
ExecTestProgram_SOURCES = ExecTestProgram.cpp

runMultiProcessLogger_PROJECT_LIBS = $(COMMON_PROJECT_LIBS)
runMultiProcessLogger_EXTERNAL_LIBS = $(SSL_LIBS)
runMultiProcessLogger_SOURCES = runMultiProcessLogger.cpp

INCLUDES = \
-I$(top_srcdir)/test/CppUnit \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/cimom/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/monitor \
-I$(top_srcdir)/src/providers/cpp/indicationexport \
-I$(top_srcdir)/src/xml

check_SCRIPTS = set_test_libpath.sh
EXTRA_DIST = set_test_libpath.sh.in run_make_check.sh
BUILT_SOURCES = $(check_SCRIPTS)
CLEANFILES = $(BUILT_SOURCES)

# For each program we run as a test, execute it with this script.
TESTS_ENVIRONMENT = $(top_builddir)/test/unit/set_test_libpath.sh

# The built test files.  Do not delete or change this next line.  The
# newtest.sh script depends on it.
BUILT_TESTS = \
PrivilegeManagerMockObjectTestCases \
OW_LazyGlobalTestCases \
OW_FormatTestCases \
OW_StringTestCases \
OW_LoggerTestCases \
OW_HTTPChunkerTestCases \
OW_ProviderManagerTestCases \
OW_SharedLibraryTestCases \
OW_SharedLibraryLoaderTestCases \
OW_ProviderMuxLoaderTestCases \
OW_ProviderMuxTestCases \
OW_LinuxSharedLibraryLoaderTestCases \
OW_ExecTestCases \
OW_StackTraceTestCases \
OW_EnumerationTestCases \
OW_XMLUnescapeTestCases \
OW_XMLEscapeTestCases \
OW_MD5TestCases \
OW_InetAddressTestCases \
OW_ListenerAuthenticatorTestCases \
OW_CIMUrlTestCases \
OW_MutexTestCases \
OW_MemTracerTestCases \
OW_StringStreamTestCases \
OW_ExceptionTestCases \
OW_CIMClassTestCases \
OW_CIMValueTestCases \
OW_StringArrayPropertyTestCases \
OW_CIMInstanceTestCases \
OW_CIMPropertyTestCases \
OW_FileSystemTestCases \
OW_RWLockerTestCases \
OW_ReferenceTestCases \
OW_ThreadBarrierTestCases \
OW_ThreadTestCases \
OW_ThreadPoolTestCases \
OW_UUIDTestCases \
OW_RandomNumberTestCases \
OW_UTF8UtilsTestCases \
OW_DateTimeTestCases \
OW_CIMDateTimeTestCases \
OW_HTTPUtilsTestCases \
CIMRepository2TestCases \
MOFCompilerTestCases \
OperationContextTestCases \
OW_BinarySerializationTestCases \
URLTestCases \
CmdLineParserTestCases \
COWIntrusiveReferenceTestCases \
CryptographicRandomNumberTestCases \
ThreadOnceTestCases \
TimeoutTimerTestCases \
ConditionTestCases \
ProcessTestCases \
IStringStreamTestCases \
GlobalPtrTestCases \
OW_ScopeLoggerTestCases \
OW_DelayedFormatTestCases \
LogAppenderTestCases \
PrivilegeMonitorParserTestCases \
PrivilegeConfigTestCases \
XMLParserCoreTestCases \
EnvironmentVariablePatternsTestCases \
ConfigureCppIndicationExportXMLHTTPProviderTestCases \
CppIndicationExportXMLHTTPProviderTestCases \
SendIndicationBurstTaskTestCases \
IndicationBufferingThreadTestCases \
CIMObjectPathTestCases \
TestTests

show-tests:
	@echo ${BUILT_TESTS}

TESTS = $(BUILT_TESTS)

XML_LIBS = -L$(top_builddir)/src/xml -lowxml
PRIVMAN_LIBS = -L$(top_builddir)/src/monitor -lowprivman
INDICATION_LIBS = -L$(top_builddir)/src/providers/cpp/indicationexport -lcppindicationexportxmlhttpprovider
PROVIDER_LIBS = \
	-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
	-L$(top_builddir)/src/provider -lowprovider
PROVIDER_INCLUDES = \
	-I$(top_srcdir)/src/provider \
	-I$(top_srcdir)/src/providerifcs/cpp
HTTP_CLIENT_LIBS = \
	-L$(top_builddir)/src/http/common -lowhttpcommon \
	-L$(top_builddir)/src/http/client -lowhttpclient \
	$(Z_LIBS)
CIMOM_LIBS = \
	-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
	-L$(top_builddir)/src/cimom/server -lowserver \
	-L$(top_builddir)/db/db -lowdb \
	-L$(top_builddir)/src/cimom/common -lowcimomcommon



# Test case definitions -- DO NOT EDIT THIS COMMENT
PrivilegeManagerMockObjectTestCases_SOURCES = \
	PrivilegeManagerMockObjectTestCases.cpp \
	MockObject.cpp \
	MockObject.hpp
PrivilegeManagerMockObjectTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(PRIVMAN_LIBS)

OW_LazyGlobalTestCases_SOURCES = OW_LazyGlobalTestCases.cpp

OW_FormatTestCases_SOURCES = OW_FormatTestCases.cpp

OW_StringTestCases_SOURCES = OW_StringTestCases.cpp

OW_LoggerTestCases_SOURCES = OW_LoggerTestCases.cpp


OW_HTTPChunkerTestCases_SOURCES = \
	OW_HTTPChunkerTestCases.cpp
OW_HTTPChunkerTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	-L$(top_builddir)/src/http/common -lowhttpcommon \
	$(Z_LIBS)


OW_ProviderManagerTestCases_SOURCES = \
	OW_ProviderManagerTestCases.cpp \
	UnitTestEnvironment.cpp \
	UnitTestEnvironment.hpp \
	testSharedLibraryLoader.hpp
OW_ProviderManagerTestCases_INCLUDES = \
	$(PROVIDER_INCLUDES)
OW_ProviderManagerTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(PROVIDER_LIBS) \
	$(CIMOM_LIBS)


OW_SharedLibraryTestCases_SOURCES = \
	OW_SharedLibraryTestCases.cpp \
	UnitTestEnvironment.cpp \
	UnitTestEnvironment.hpp
OW_SharedLibraryTestCases_INCLUDES = $(PROVIDER_INCLUDES)
OW_SharedLibraryTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(PROVIDER_LIBS) \
	$(CIMOM_LIBS)

OW_SharedLibraryLoaderTestCases_SOURCES = \
	OW_SharedLibraryLoaderTestCases.cpp \
	UnitTestEnvironment.cpp \
	UnitTestEnvironment.hpp
OW_SharedLibraryLoaderTestCases_INCLUDES = \
	$(PROVIDER_INCLUDES)
OW_SharedLibraryLoaderTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(PROVIDER_LIBS) \
	$(CIMOM_LIBS)


OW_ProviderMuxLoaderTestCases_SOURCES = \
	OW_ProviderMuxLoaderTestCases.cpp \
	UnitTestEnvironment.cpp \
	UnitTestEnvironment.hpp \
	testSharedLibraryLoader.hpp
OW_ProviderMuxLoaderTestCases_INCLUDES = \
	$(PROVIDER_INCLUDES)
OW_ProviderMuxLoaderTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(PROVIDER_LIBS) \
	$(CIMOM_LIBS)


OW_ProviderMuxTestCases_SOURCES = \
	OW_ProviderMuxTestCases.cpp \
	UnitTestEnvironment.cpp \
	UnitTestEnvironment.hpp
OW_ProviderMuxTestCases_INCLUDES = \
	$(PROVIDER_INCLUDES)
OW_ProviderMuxTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(PROVIDER_LIBS) \
	$(CIMOM_LIBS)


OW_LinuxSharedLibraryLoaderTestCases_SOURCES = \
	OW_LinuxSharedLibraryLoaderTestCases.cpp \
	testSharedLibraryLoader.hpp


OW_ExecTestCases_SOURCES = OW_ExecTestCases.cpp


OW_StackTraceTestCases_SOURCES = OW_StackTraceTestCases.cpp

OW_EnumerationTestCases_SOURCES = OW_EnumerationTestCases.cpp

OW_XMLUnescapeTestCases_SOURCES = OW_XMLUnescapeTestCases.cpp
OW_XMLUnescapeTestCases_PROJECT_LIBS = $(COMMON_PROJECT_LIBS) $(XML_LIBS)

OW_XMLEscapeTestCases_SOURCES = OW_XMLEscapeTestCases.cpp
OW_XMLEscapeTestCases_PROJECT_LIBS = $(COMMON_PROJECT_LIBS) $(XML_LIBS)

OW_MD5TestCases_SOURCES = OW_MD5TestCases.cpp

OW_InetAddressTestCases_SOURCES = OW_InetAddressTestCases.cpp

OW_ListenerAuthenticatorTestCases_SOURCES = OW_ListenerAuthenticatorTestCases.cpp
OW_ListenerAuthenticatorTestCases_INCLUDES = -I$(top_srcdir)/src/cimxmllistener
OW_ListenerAuthenticatorTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(HTTP_CLIENT_LIBS) \
	$(XML_LIBS) \
	$(PRIVMAN_LIBS) \
	-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
	-L$(top_builddir)/src/client -lowclient \
	-L$(top_builddir)/src/services/http -lowservicehttp

OW_CIMUrlTestCases_SOURCES = OW_CIMUrlTestCases.cpp

OW_MutexTestCases_SOURCES = OW_MutexTestCases.cpp

OW_MemTracerTestCases_SOURCES = OW_MemTracerTestCases.cpp

OW_StringStreamTestCases_SOURCES = OW_StringStreamTestCases.cpp

OW_ExceptionTestCases_SOURCES = OW_ExceptionTestCases.cpp

OW_CIMClassTestCases_SOURCES = OW_CIMClassTestCases.cpp

OW_CIMValueTestCases_SOURCES = OW_CIMValueTestCases.cpp

OW_StringArrayPropertyTestCases_SOURCES = OW_StringArrayPropertyTestCases.cpp

OW_CIMInstanceTestCases_SOURCES = OW_CIMInstanceTestCases.cpp

OW_CIMPropertyTestCases_SOURCES = OW_CIMPropertyTestCases.cpp
OW_CIMPropertyTestCases_PROJECT_LIBS = $(COMMON_PROJECT_LIBS) $(XML_LIBS)

OW_FileSystemTestCases_SOURCES = OW_FileSystemTestCases.cpp

OW_RWLockerTestCases_SOURCES = OW_RWLockerTestCases.cpp

OW_ReferenceTestCases_SOURCES = OW_ReferenceTestCases.cpp

OW_ThreadBarrierTestCases_SOURCES = OW_ThreadBarrierTestCases.cpp

OW_ThreadTestCases_SOURCES = OW_ThreadTestCases.cpp

OW_ThreadPoolTestCases_SOURCES = OW_ThreadPoolTestCases.cpp

OW_UUIDTestCases_SOURCES = OW_UUIDTestCases.cpp

OW_RandomNumberTestCases_SOURCES = OW_RandomNumberTestCases.cpp

OW_UTF8UtilsTestCases_SOURCES = OW_UTF8UtilsTestCases.cpp

OW_DateTimeTestCases_SOURCES = OW_DateTimeTestCases.cpp

OW_CIMDateTimeTestCases_SOURCES = OW_CIMDateTimeTestCases.cpp

OW_HTTPUtilsTestCases_SOURCES = OW_HTTPUtilsTestCases.cpp
OW_HTTPUtilsTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	-L$(top_builddir)/src/http/common -lowhttpcommon \
	$(Z_LIBS)

CIMRepository2TestCases_SOURCES = CIMRepository2TestCases.cpp

MOFCompilerTestCases_SOURCES = MOFCompilerTestCases.cpp
MOFCompilerTestCases_INCLUDES = -I$(top_srcdir)/src/mof
MOFCompilerTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	-L$(top_builddir)/src/mof -lowmofc

OperationContextTestCases_SOURCES = OperationContextTestCases.cpp

OW_BinarySerializationTestCases_SOURCES = OW_BinarySerializationTestCases.cpp

URLTestCases_SOURCES = URLTestCases.cpp

CmdLineParserTestCases_SOURCES = CmdLineParserTestCases.cpp

COWIntrusiveReferenceTestCases_SOURCES = COWIntrusiveReferenceTestCases.cpp

CryptographicRandomNumberTestCases_SOURCES = CryptographicRandomNumberTestCases.cpp

ThreadOnceTestCases_SOURCES = ThreadOnceTestCases.cpp

TimeoutTimerTestCases_SOURCES = TimeoutTimerTestCases.cpp

ConditionTestCases_SOURCES = ConditionTestCases.cpp

ProcessTestCases_SOURCES = ProcessTestCases.cpp

IStringStreamTestCases_SOURCES = IStringStreamTestCases.cpp

GlobalPtrTestCases_SOURCES = GlobalPtrTestCases.cpp

OW_ScopeLoggerTestCases_SOURCES = OW_ScopeLoggerTestCases.cpp

OW_DelayedFormatTestCases_SOURCES = OW_DelayedFormatTestCases.cpp

LogAppenderTestCases_SOURCES = LogAppenderTestCases.cpp

PrivilegeMonitorParserTestCases_SOURCES = PrivilegeMonitorParserTestCases.cpp
PrivilegeMonitorParserTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) $(PRIVMAN_LIBS)

PrivilegeConfigTestCases_SOURCES = PrivilegeConfigTestCases.cpp
PrivilegeConfigTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) $(PRIVMAN_LIBS)

XMLParserCoreTestCases_SOURCES = XMLParserCoreTestCases.cpp
XMLParserCoreTestCases_PROJECT_LIBS = $(COMMON_PROJECT_LIBS) $(XML_LIBS)

EnvironmentVariablePatternsTestCases_SOURCES = EnvironmentVariablePatternsTestCases.cpp
EnvironmentVariablePatternsTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) $(PRIVMAN_LIBS)

DummyProvEnv_INCLUDES = $(PROVIDER_INCLUDES)

ConfigureCppIndicationExportXMLHTTPProviderTestCases_SOURCES = \
	ConfigureCppIndicationExportXMLHTTPProviderTestCases.cpp \
	DummyProvEnv.cpp \
	DummyProvEnv.hpp
ConfigureCppIndicationExportXMLHTTPProviderTestCases_INCLUDES = \
	$(PROVIDER_INCLUDES)
ConfigureCppIndicationExportXMLHTTPProviderTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(PROVIDER_LIBS) \
	$(INDICATION_LIBS) \
	$(HTTP_CLIENT_LIBS) \
	$(XML_LIBS)

CppIndicationExportXMLHTTPProviderTestCases_SOURCES = CppIndicationExportXMLHTTPProviderTestCases.cpp
CppIndicationExportXMLHTTPProviderTestCases_INCLUDES = $(PROVIDER_INCLUDES)

SendIndicationBurstTaskTestCases_SOURCES = \
	SendIndicationBurstTaskTestCases.cpp \
	CIMInstanceUtils.cpp \
	CIMInstanceUtils.hpp \
	MockIndicationExporter.cpp \
	MockIndicationExporter.hpp \
	MockObject.cpp \
	MockObject.hpp
SendIndicationBurstTaskTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(INDICATION_LIBS) \
	$(PROVIDER_LIBS) \
	$(HTTP_CLIENT_LIBS) \
	$(XML_LIBS)

IndicationBufferingThreadTestCases_SOURCES = \
	IndicationBufferingThreadTestCases.cpp \
	CIMInstanceUtils.cpp \
	CIMInstanceUtils.hpp \
	MockIndicationExporter.cpp \
	MockIndicationExporter.hpp \
	MockObject.cpp \
	MockObject.hpp \
	MockClock.cpp \
	MockClock.hpp
IndicationBufferingThreadTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(INDICATION_LIBS) \
	$(PROVIDER_LIBS) \
	$(HTTP_CLIENT_LIBS) \
	$(XML_LIBS)

CIMObjectPathTestCases_SOURCES = CIMObjectPathTestCases.cpp

TestTests_SOURCES = TestTests.cpp

exitWithSignal_SOURCES = \
exitWithSignal.cpp

fillOutputBuffer_SOURCES = \
fillOutputBuffer.cpp

include $(top_srcdir)/Makefile-shared-lib.inc
# We don't need a dylib, as we only want to load and not link this library.
MAKEDYLIB=0
MAKEBUNDLE=1
OW_SHARED_LIBS = libLazyGlobalTestLibrary.$(LIB_EXT)

LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libLazyGlobalTestLibrary_PROJECT_LIBS = $(COMMON_PROJECT_LIBS)
libLazyGlobalTestLibrary_EXTERNAL_LIBS = $(COMMON_EXTERNAL_LIBS)

noinst_LIBRARIES = libLazyGlobalTestLibrary.a
libLazyGlobalTestLibrary_a_SOURCES = OW_LazyGlobalTestLibrary.cpp

local-scripts: $(check_SCRIPTS)
	chmod +x $^

all-local: all-ow-shared-libs local-scripts
clean-local: clean-ow-shared-libs


# This will set the PROJECT_LIBS and EXTERNAL_LIBS variables for each of the
# built tests if they did not define it above.  This should help keep this
# makefile simple.  This may be a good thing to put in the normal executable
# hackery of Makefile-executable.inc
define TEST_TEMPLATE
$(if $(value $(1)_PROJECT_LIBS),,$(1)_PROJECT_LIBS=$(COMMON_PROJECT_LIBS))
$(if $(value $(1)_EXTERNAL_LIBS),,$(1)_EXTERNAL_LIBS=$(COMMON_EXTERNAL_LIBS))
endef
$(foreach prog,$(BUILT_TESTS),$(eval $(call TEST_TEMPLATE,$(prog))))

# A hack to include the specific include flags for a particular target in the
# includes.  The need for hacks like this is part of why automake sucks so bad.
AM_CPPFLAGS = $($(@:%.o=%)_INCLUDES)

include $(top_srcdir)/Makefile-replacement.inc
include $(top_srcdir)/Makefile-executable.inc
