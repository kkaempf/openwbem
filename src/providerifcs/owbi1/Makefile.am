MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


owstatlib_LIBRARIES = libowowbi1provifc.a
OW_SHARED_LIBS = libowowbi1provifc.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

#create links here
OW_EXTRA_LINK_DIRS=$(pkglibdir)/provifcs

libowowbi1provifc_a_SOURCES = \
cim/OWBI1_CIMParamValue.cpp \
common/OWBI1_SessionLanguage.cpp \
common/OWBI1_Logger.cpp \
common/OWBI1_OperationContext.cpp \
common/OWBI1_WQLCompile.cpp \
common/OWBI1_WQLPropertySource.cpp \
common/OWBI1_WQLOperand.cpp \
common/OWBI1_WQLSelectStatement.cpp \
provider/OWBI1_MethodProviderInfo.cpp \
provider/OWBI1_ProviderInfoBase.cpp \
cim/OWBI1_CIMParameter.cpp \
cim/OWBI1_CIMMethod.cpp \
common/OWBI1_NoSuchPropertyException.cpp \
cim/OWBI1_CIMQualifier.cpp \
common/OWBI1_NoSuchQualifierException.cpp \
common/OWBI1_NULLValueException.cpp \
cim/OWBI1_CIMUrl.cpp \
common/OWBI1_StringBuffer.cpp \
cim/OWBI1_CIMFlavor.cpp \
cim/OWBI1_CIMScope.cpp \
cim/OWBI1_CIMDateTime.cpp \
cim/OWBI1_CIMValueCast.cpp \
cim/OWBI1_CIMQualifierType.cpp \
common/OWBI1_ResultHandlers.cpp \
common/OWBI1_EnumerationException.cpp \
common/OWBI1_TempFileEnumerationImplBase.cpp \
cim/OWBI1_CIMException.cpp \
cim/OWBI1_CIMBase.cpp \
common/OWBI1_SerializableIFC.cpp \
cim/OWBI1_CIMName.cpp \
cim/OWBI1_CIMDataType.cpp \
cim/OWBI1_CIMElement.cpp \
common/OWBI1_CIMOMHandleIFC.cpp \
cim/OWBI1_CIMNameSpace.cpp \
cim/OWBI1_CIMValue.cpp \
cim/OWBI1_CIMProperty.cpp \
cim/OWBI1_CIMObjectPath.cpp \
cim/OWBI1_CIMClass.cpp \
cim/OWBI1_CIMInstance.cpp \
cim/OWBI1_CIMPropertyList.cpp \
common/OWBI1_IntrusiveCountableBase.cpp \
common/OWBI1_COWIntrusiveCountableBase.cpp \
common/OWBI1_AtomicOps.cpp \
common/OWBI1_COWReferenceBase.cpp \
common/OWBI1_Array.cpp \
common/OWBI1_Bool.cpp \
common/OWBI1_Char16.cpp \
common/OWBI1_COWIntrusiveReference.cpp \
common/OWBI1_String.cpp \
common/OWBI1_Exception.cpp \
common/OWBI1_DateTime.cpp \
provider/OWBI1_ProviderEnvironmentIFC.cpp \
OWBI1_SimpleAssociatorProviderIFC.cpp \
OWBI1_AssociatorProviderIFC.cpp \
OWBI1_ProviderBaseIFC.cpp \
OWBI1_ProviderIFC.cpp \
OWBI1_ProxyProvider.cpp \
OWBI1_ProxyProvider.hpp \
OWBI1_ProxyProviderDetail.cpp \
OWBI1_ProxyProviderDetail.hpp \
OWBI1_ProviderIFC.hpp \
OWBI1_IndicationExportProviderIFC.cpp \
OWBI1_IndicationProviderIFC.cpp \
OWBI1_MethodProviderIFC.cpp \
OWBI1_PolledProviderIFC.cpp \
OWBI1_SimpleInstanceProviderIFC.cpp \
OWBI1_InstanceProviderIFC.cpp \
OWBI1_SecondaryInstanceProviderIFC.cpp \
OWBI1_ReadOnlyInstanceProviderIFC.cpp


pkginclude_HEADERS = \
common/OWBI1_SortedVectorSet.hpp \
cim/OWBI1_CIMParamValue.hpp \
common/OWBI1_SessionLanguage.hpp \
common/OWBI1_LogMessage.hpp \
common/OWBI1_LogLevel.hpp \
common/OWBI1_Logger.hpp \
provider/OWBI1_AssociatorProviderInfo.hpp \
common/OWBI1_OperationContext.hpp \
common/OWBI1_WQLCompile.hpp \
common/OWBI1_WQLPropertySource.hpp \
common/OWBI1_WQLOperand.hpp \
common/OWBI1_WQLOperation.hpp \
common/OWBI1_WQLSelectStatement.hpp \
provider/OWBI1_MethodProviderInfo.hpp \
provider/OWBI1_IndicationProviderInfo.hpp \
provider/OWBI1_ProviderInfoBase.hpp \
provider/OWBI1_SecondaryInstanceProviderInfo.hpp \
cim/OWBI1_CIMParameter.hpp \
cim/OWBI1_CIMMethod.hpp \
common/OWBI1_NoSuchPropertyException.hpp \
cim/OWBI1_CIMQualifier.hpp \
common/OWBI1_NoSuchQualifierException.hpp \
common/OWBI1_NULLValueException.hpp \
cim/OWBI1_CIMUrl.hpp \
common/OWBI1_StringBuffer.hpp \
cim/OWBI1_CIMFlavor.hpp \
cim/OWBI1_CIMScope.hpp \
cim/OWBI1_CIMDateTime.hpp \
cim/OWBI1_CIMValueCast.hpp \
cim/OWBI1_CIMQualifierType.hpp \
common/OWBI1_ResultHandlers.hpp \
common/OWBI1_EnumerationException.hpp \
common/OWBI1_TempFileEnumerationImplBase.hpp \
common/OWBI1_Enumeration.hpp \
cim/OWBI1_CIMException.hpp \
cim/OWBI1_CIMNULL.hpp \
cim/OWBI1_CIMBase.hpp \
common/OWBI1_SerializableIFC.hpp \
cim/OWBI1_CIMName.hpp \
cim/OWBI1_CIMDataType.hpp \
cim/OWBI1_CIMElement.hpp \
common/OWBI1_CIMOMHandleIFC.hpp \
cim/OWBI1_CIMNameSpace.hpp \
cim/OWBI1_CIMValue.hpp \
cim/OWBI1_CIMProperty.hpp \
cim/OWBI1_CIMObjectPath.hpp \
cim/OWBI1_CIMClass.hpp \
cim/OWBI1_CIMInstance.hpp \
cim/OWBI1_CIMPropertyList.hpp \
common/OWBI1_ResultHandlerIFC.hpp \
common/OWBI1_WBEMFlags.hpp \
provider/OWBI1_InstanceProviderInfo.hpp \
provider/OWBI1_ProviderFwd.hpp \
common/OWBI1_IntrusiveCountableBase.hpp \
common/OWBI1_COWIntrusiveCountableBase.hpp \
common/OWBI1_ArrayImpl.hpp \
common/OWBI1_AtomicOps.hpp \
common/OWBI1_RefCount.hpp \
common/OWBI1_COWReferenceBase.hpp \
common/OWBI1_vector.hpp \
common/OWBI1_COWReference.hpp \
common/OWBI1_Array.hpp \
common/OWBI1_Bool.hpp \
common/OWBI1_Char16.hpp \
common/OWBI1_COWIntrusiveReference.hpp \
common/OWBI1_String.hpp \
common/OWBI1_AutoPtr.hpp \
common/OWBI1_Types.hpp \
common/OWBI1_Exception.hpp \
common/OWBI1_DateTime.hpp \
provider/OWBI1_ProviderEnvironmentIFC.hpp \
common/OWBI1_IntrusiveReference.hpp \
common/OWBI1_CommonFwd.hpp \
common/OWBI1_ArrayFwd.hpp \
cim/OWBI1_CIMFwd.hpp \
OWBI1_ProviderBaseIFC.hpp \
OWBI1_SimpleAssociatorProviderIFC.hpp \
OWBI1_AssociatorProviderIFC.hpp \
OWBI1_IndicationExportProviderIFC.hpp \
OWBI1_InstanceProviderIFC.hpp \
OWBI1_SecondaryInstanceProviderIFC.hpp \
OWBI1_MethodProviderIFC.hpp \
OWBI1_PolledProviderIFC.hpp \
OWBI1_IndicationProviderIFC.hpp \
OWBI1_SimpleInstanceProviderIFC.hpp \
OWBI1_ReadOnlyInstanceProviderIFC.hpp \
OWBI1_ProviderIncludes.hpp \
OWBI1_OW_Fwd.hpp \
OWBI1_config.h \
OWBI1_AuxConfig.h

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/owbi1/cim \
-I$(top_srcdir)/src/providerifcs/owbi1/common \
-I$(top_srcdir)/src/providerifcs/owbi1/provider

