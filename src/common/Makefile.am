
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

OW_SHARED_LIBS = libopenwbem.$(LIB_EXT)
libopenwbem_EXTERNAL_LIBS = \
$(DL_LIB) $(SSL_LIBS) $(BLOCXX_LIBS)

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
OW_Exception.cpp \
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
OW_URL.cpp \
OW_UserInfo.cpp \
OW_WQLSelectStatement.cpp \
OW_WQLCompile.cpp \
OW_WQLOperand.cpp \
OW_WQLPropertySource.cpp \
OW_WQLInstancePropertySource.cpp

pkginclude_HEADERS =	\
OW_BinarySerialization.hpp \
OW_Cache.hpp \
OW_CIMErrorException.hpp \
OW_CIMFeatures.hpp \
OW_CommonFwd.hpp \
OW_ConfigException.hpp \
OW_ConfigFile.hpp \
OW_ConfigOpts.hpp \
OW_Exception.hpp \
OW_ExceptionIds.hpp \
OW_LocalOperationContext.hpp \
OW_NoSuchPropertyException.hpp \
OW_NoSuchProviderException.hpp \
OW_NoSuchQualifierException.hpp \
OW_NULLValueException.hpp \
OW_OperationContext.hpp \
OW_OperationRWLock.hpp \
OW_RepositoryCIMOMHandle.hpp \
OW_ResultHandlers.hpp \
OW_SafeLibCreate.hpp \
OW_SessionLanguage.hpp \
OW_Types.hpp \
OW_URL.hpp \
OW_UserInfo.hpp \
OW_WBEMFlags.hpp \
OW_WQLSelectStatement.hpp \
OW_WQLCompile.hpp \
OW_WQLOperand.hpp \
OW_WQLOperation.hpp \
OW_WQLPropertySource.hpp \
OW_WQLInstancePropertySource.hpp


libopenwbem_a_LIBADD = \
../cim/libOW_cim.a \
../ifcs/libOW_ifcs.a

