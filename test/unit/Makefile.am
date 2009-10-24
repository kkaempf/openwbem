PROGS=

noinst_PROGRAMS = $(PROGS) $(BUILT_TESTS)

COMMON_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS)
# CppUnit is listed here because as a static object, it must occur *AFTER* the
# objects that use it.
COMMON_EXTERNAL_LIBS = \
-L$(top_builddir)/test/CppUnit -lCppUnit \
$(SSL_LIBS)


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
ConfigFileTestCases \
PrivilegeManagerMockObjectTestCases \
OW_HTTPChunkerTestCases \
OW_ProviderManagerTestCases \
OW_SharedLibraryTestCases \
OW_SharedLibraryLoaderTestCases \
OW_ProviderMuxLoaderTestCases \
OW_ProviderMuxTestCases \
OW_LinuxSharedLibraryLoaderTestCases \
OW_XMLUnescapeTestCases \
OW_XMLEscapeTestCases \
OW_ListenerAuthenticatorTestCases \
OW_CIMUrlTestCases \
OW_CIMClassTestCases \
OW_CIMValueTestCases \
OW_StringArrayPropertyTestCases \
OW_CIMInstanceTestCases \
OW_CIMPropertyTestCases \
OW_CIMDateTimeTestCases \
OW_HTTPUtilsTestCases \
CIMRepository2TestCases \
MOFCompilerTestCases \
OperationContextTestCases \
OW_BinarySerializationTestCases \
URLTestCases \
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
ConfigFileTestCases_SOURCES = \
	FileSystemMockObjectScope.hpp \
	FileSystemMockObjectScope.cpp \
	ConfigFileTestCases.cpp

PrivilegeManagerMockObjectTestCases_SOURCES = \
	PrivilegeManagerMockObjectTestCases.cpp \
	PrivilegeManagerMockObjectTestCases.hpp \
	MockObject.cpp \
	MockObject.hpp
PrivilegeManagerMockObjectTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(PRIVMAN_LIBS)


OW_HTTPChunkerTestCases_SOURCES = \
	OW_HTTPChunkerTestCases.cpp \
	OW_HTTPChunkerTestCases.hpp
OW_HTTPChunkerTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	-L$(top_builddir)/src/http/common -lowhttpcommon \
	$(Z_LIBS)


OW_ProviderManagerTestCases_SOURCES = \
	OW_ProviderManagerTestCases.cpp \
	OW_ProviderManagerTestCases.hpp \
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
	OW_SharedLibraryTestCases.hpp \
	UnitTestEnvironment.cpp \
	UnitTestEnvironment.hpp
OW_SharedLibraryTestCases_INCLUDES = $(PROVIDER_INCLUDES)
OW_SharedLibraryTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(PROVIDER_LIBS) \
	$(CIMOM_LIBS)


OW_SharedLibraryLoaderTestCases_SOURCES = \
	OW_SharedLibraryLoaderTestCases.cpp \
	OW_SharedLibraryLoaderTestCases.hpp \
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
	OW_ProviderMuxLoaderTestCases.hpp \
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
	OW_ProviderMuxTestCases.hpp \
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
	OW_LinuxSharedLibraryLoaderTestCases.hpp \
	testSharedLibraryLoader.hpp

OW_XMLUnescapeTestCases_SOURCES = \
	OW_XMLUnescapeTestCases.cpp \
	OW_XMLUnescapeTestCases.hpp
OW_XMLUnescapeTestCases_PROJECT_LIBS = $(COMMON_PROJECT_LIBS) $(XML_LIBS)


OW_XMLEscapeTestCases_SOURCES = \
	OW_XMLEscapeTestCases.cpp \
	OW_XMLEscapeTestCases.hpp
OW_XMLEscapeTestCases_PROJECT_LIBS = $(COMMON_PROJECT_LIBS) $(XML_LIBS)


OW_ListenerAuthenticatorTestCases_SOURCES = \
	OW_ListenerAuthenticatorTestCases.cpp \
	OW_ListenerAuthenticatorTestCases.hpp
OW_ListenerAuthenticatorTestCases_INCLUDES = -I$(top_srcdir)/src/cimxmllistener
OW_ListenerAuthenticatorTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	$(HTTP_CLIENT_LIBS) \
	$(XML_LIBS) \
	$(PRIVMAN_LIBS) \
	-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
	-L$(top_builddir)/src/client -lowclient \
	-L$(top_builddir)/src/services/http -lowservicehttp


OW_CIMUrlTestCases_SOURCES = \
	OW_CIMUrlTestCases.cpp \
	OW_CIMUrlTestCases.hpp

OW_CIMClassTestCases_SOURCES = \
	OW_CIMClassTestCases.cpp \
	OW_CIMClassTestCases.hpp

OW_CIMValueTestCases_SOURCES = \
	OW_CIMValueTestCases.cpp \
	OW_CIMValueTestCases.hpp

OW_StringArrayPropertyTestCases_SOURCES = \
	OW_StringArrayPropertyTestCases.cpp \
	OW_StringArrayPropertyTestCases.hpp

OW_CIMInstanceTestCases_SOURCES = \
	OW_CIMInstanceTestCases.cpp \
	OW_CIMInstanceTestCases.hpp

OW_CIMPropertyTestCases_SOURCES = \
	OW_CIMPropertyTestCases.cpp \
	OW_CIMPropertyTestCases.hpp
OW_CIMPropertyTestCases_PROJECT_LIBS = $(COMMON_PROJECT_LIBS) $(XML_LIBS)


OW_CIMDateTimeTestCases_SOURCES = \
	OW_CIMDateTimeTestCases.cpp \
	OW_CIMDateTimeTestCases.hpp

OW_HTTPUtilsTestCases_SOURCES = \
	OW_HTTPUtilsTestCases.cpp \
	OW_HTTPUtilsTestCases.hpp
OW_HTTPUtilsTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	-L$(top_builddir)/src/http/common -lowhttpcommon \
	$(Z_LIBS)


CIMRepository2TestCases_SOURCES =  \
	CIMRepository2TestCases.cpp \
	CIMRepository2TestCases.hpp

MOFCompilerTestCases_SOURCES = \
	MOFCompilerTestCases.cpp \
	MOFCompilerTestCases.hpp
MOFCompilerTestCases_INCLUDES = -I$(top_srcdir)/src/mof
MOFCompilerTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) \
	-L$(top_builddir)/src/mof -lowmofc


OperationContextTestCases_SOURCES = \
	OperationContextTestCases.cpp \
	OperationContextTestCases.hpp

OW_BinarySerializationTestCases_SOURCES = \
	OW_BinarySerializationTestCases.cpp \
	OW_BinarySerializationTestCases.hpp

URLTestCases_SOURCES = \
	URLTestCases.cpp \
	URLTestCases.hpp

PrivilegeMonitorParserTestCases_SOURCES = \
	PrivilegeMonitorParserTestCases.cpp \
	PrivilegeMonitorParserTestCases.hpp
PrivilegeMonitorParserTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) $(PRIVMAN_LIBS)


PrivilegeConfigTestCases_SOURCES = \
	PrivilegeConfigTestCases.cpp \
	PrivilegeConfigTestCases.hpp
PrivilegeConfigTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) $(PRIVMAN_LIBS)


XMLParserCoreTestCases_SOURCES = \
	XMLParserCoreTestCases.cpp \
	XMLParserCoreTestCases.hpp
XMLParserCoreTestCases_PROJECT_LIBS = $(COMMON_PROJECT_LIBS) $(XML_LIBS)


EnvironmentVariablePatternsTestCases_SOURCES = \
	EnvironmentVariablePatternsTestCases.cpp \
	EnvironmentVariablePatternsTestCases.hpp
EnvironmentVariablePatternsTestCases_PROJECT_LIBS = \
	$(COMMON_PROJECT_LIBS) $(PRIVMAN_LIBS)


DummyProvEnv_INCLUDES = $(PROVIDER_INCLUDES)

ConfigureCppIndicationExportXMLHTTPProviderTestCases_SOURCES = \
	ConfigureCppIndicationExportXMLHTTPProviderTestCases.cpp \
	ConfigureCppIndicationExportXMLHTTPProviderTestCases.hpp \
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


CppIndicationExportXMLHTTPProviderTestCases_SOURCES = \
	CppIndicationExportXMLHTTPProviderTestCases.cpp \
	CppIndicationExportXMLHTTPProviderTestCases.hpp
CppIndicationExportXMLHTTPProviderTestCases_INCLUDES = $(PROVIDER_INCLUDES)


SendIndicationBurstTaskTestCases_SOURCES = \
	SendIndicationBurstTaskTestCases.cpp \
	SendIndicationBurstTaskTestCases.hpp \
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
	IndicationBufferingThreadTestCases.hpp \
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

include $(top_srcdir)/Makefile-shared-lib.inc

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
