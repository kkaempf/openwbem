###########################################################################
# Begin auto-generated RPM spec header for openwbem
###########################################################################
Name: openwbem
Version: 3.1.0.1
Release: rh3WS
Group: Applications/System
Summary: OpenWBEM
Copyright: Copyright (C) 2005 Vintela, Inc.
Packager: support@vintela.com
URL: http://www.vintela.com
# "yes" is the default, but we put it here explicitly to avoid someone
# setting it to "no"
AutoReqProv: yes
# This is necessary to build the RPM as a non-root user.
BuildRoot: %{_tmppath}/%{name}-%{version}
Source0: openwbem.tgz
%description
The full collection of OpenWBEM packages

###########################################################################
# End auto-generated RPM spec header for openwbem
###########################################################################
###########################################################################
# Begin subpackage ow-cimomd
###########################################################################
%package ow-cimomd
Group: sys_daemon
Summary: OpenWBEM cimom daemon
Requires: openwbem-ow-libs4
%description ow-cimomd
The OpenWBEM CIMOM daemon and its required helper programs.

###########################################################################
# End subpackage ow-cimomd
###########################################################################

###########################################################################
# Begin subpackage ow-libs4
###########################################################################
%package ow-libs4
Group: sys_libs
Summary: OpenWBEM public libraries and header files
%description ow-libs4
The public libraries and header files for OpenWBEM.  Anything that will be
used within the CIMOM (such as providers) must be linked against this set
of libraries.

###########################################################################
# End subpackage ow-libs4
###########################################################################

###########################################################################
# Begin subpackage ow-schema
###########################################################################
%package ow-schema
Group: dev_system
Summary: CIM Schema as released by the DMTF
%description ow-schema
The CIM Schema as released by the Distributed Management Task Force (DMTF). 
See http://www.dmtf.org for more details.

###########################################################################
# End subpackage ow-schema
###########################################################################

###########################################################################
# Begin subpackage ow-tools
###########################################################################
%package ow-tools
Group: app_system
Summary: OpenWBEM tools
Requires: openwbem-ow-libs4, openwbem-ow-cimomd
%description ow-tools
Tools for compiling MOF and performing actions on the CIMOM.  This includes
tools for querying the CIMOM database (owexecwql).

###########################################################################
# End subpackage ow-tools
###########################################################################

###########################################################################
# Begin subpackage ow-devel
###########################################################################
%package ow-devel
Group: dev_system
Summary: OpenWBEM development header files
Requires: openwbem-ow-libs4
%description ow-devel
OpenWBEM development header files

###########################################################################
# End subpackage ow-devel
###########################################################################

%prep

# Untar the sources.
%setup -c

%build
# it's already built.

%Install
%{__rm} -rf $RPM_BUILD_ROOT
%{__mkdir} -p $RPM_BUILD_ROOT

cp -r * $RPM_BUILD_ROOT

%Clean
%{__rm} -rf $RPM_BUILD_ROOT

####################
# Begin files for openwbem from /tmp/file_list.openwbem.24652.1513.17548.0.3082
%Files 
%defattr(-,root,root)
# End files for openwbem from /tmp/file_list.openwbem.24652.1513.17548.0.3082
####################

####################
# Begin files for ow-cimomd from /tmp/file_list.ow-cimomd.23164.25638.880.0.3082
%Files ow-cimomd
%defattr(-,root,root)
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/sbin
%defattr(0755,root,root)
/opt/vintela/openwbem/sbin/*owcimomd
%defattr(0755,root,root)
%dir /var/opt/vintela/openwbem
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/man
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/man/man8
%defattr(0644,root,root)
/opt/vintela/openwbem/man/man8/*owcimomd.8
%defattr(0755,root,root)
%dir /etc/opt/vintela/openwbem
%defattr(0755,root,root)
%dir /etc/opt/vintela/openwbem/openwbem
%defattr(0644,root,root)
%config /etc/opt/vintela/openwbem/openwbem/openwbem.conf
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/libexec
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/libexec/openwbem
%defattr(0755,root,root)
/opt/vintela/openwbem/libexec/openwbem/owlocalhelper
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/share
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/share/openwbem
%defattr(0644,root,root)
/opt/vintela/openwbem/share/openwbem/*.mof
%defattr(0755,root,root)
%dir /etc/pam.d
%defattr(0644,root,root)
/etc/pam.d/vintelaopenwbem
# End files for ow-cimomd from /tmp/file_list.ow-cimomd.23164.25638.880.0.3082
####################

####################
# Begin files for ow-libs4 from /tmp/file_list.ow-libs4.13245.3217.18515.0.3082
%Files ow-libs4
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/lib
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/lib/openwbem
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/lib/openwbem/services
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/lib/openwbem/authentication
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/lib/openwbem/requesthandlers
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/lib/openwbem/provifcs
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/lib/openwbem/c++providers
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/services/libowservicehttp.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/authentication/libsimpleauthentication.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/authentication/libnonauthenticatingauthentication.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/requesthandlers/libowrequesthandlercimxml.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/requesthandlers/libowrequesthandlerbinary.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/libowsimpleauthorizer.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/libowsimpleauthorizer2.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/provifcs/libowremoteprovifc.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/provifcs/libnpiprovifc.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/c++providers/libcppindicationexportxmlhttpprovider.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/c++providers/libowprovpollUnloader.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/c++providers/libowprovCIM_IndicationSubscription.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/c++providers/libowprovinstCIM_IndicationFilter.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/c++providers/libowprovinstOpenWBEM_ConfigSettingData.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/c++providers/libowprovinstOpenWBEM_ObjectManager.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/c++providers/libowprovinstOpenWBEM_UnitaryComputerSystem.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/c++providers/libowprovinstCIM_Namespace.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/c++providers/libowprovinstCIM_NamespaceInManager.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/c++providers/libowprovinstOW_NameSpace.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/c++providers/libowprovindIndicationRepLayer.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/libowindicationserver.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowdb.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libopenwbem.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowservicehttp.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libsimpleauthentication.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libnonauthenticatingauthentication.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowrequesthandlercimxml.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowrequesthandlerbinary.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libnpisupport.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowmofc.*
%defattr(0644,root,root)
%dir /opt/vintela/openwbem/share/aclocal
%defattr(0644,root,root)
/opt/vintela/openwbem/share/aclocal/openwbem.m4
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/sbin
%defattr(0755,root,root)
/opt/vintela/openwbem/sbin/owprovideragent
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowhttpcommon.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowhttpclient.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowxml.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowclient.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/authentication/libpamauthentication.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/authentication/libpamclauthentication.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/openwbem/libowindicationreplayer.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowhttpxmllistener.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowprovider.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowprovideragent.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowwqlcommon.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowwql.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libpamclauthentication.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowrepositoryhdb.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowcppprovifc.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowserver.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowcimomcommon.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libowembeddedcimom.*
%defattr(0755,root,root)
/opt/vintela/openwbem/lib/libpamauthentication.*
# End files for ow-libs4 from /tmp/file_list.ow-libs4.13245.3217.18515.0.3082
####################

####################
# Begin files for ow-schema from /tmp/file_list.ow-schema.22975.15687.16499.0.3082
%Files ow-schema
# End files for ow-schema from /tmp/file_list.ow-schema.22975.15687.16499.0.3082
####################

####################
# Begin files for ow-tools from /tmp/file_list.ow-tools.21797.14768.30314.0.3082
%Files ow-tools
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/bin
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/sbin
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/man
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/man/man1
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/man/man8
%defattr(0755,root,root)
/opt/vintela/openwbem/bin/owcimindicationlistener
%defattr(0755,root,root)
/opt/vintela/openwbem/bin/owcreatenamespace
%defattr(0755,root,root)
/opt/vintela/openwbem/bin/owdeletenamespace
%defattr(0755,root,root)
/opt/vintela/openwbem/bin/owenumclasses
%defattr(0755,root,root)
/opt/vintela/openwbem/bin/owenumclassnames
%defattr(0755,root,root)
/opt/vintela/openwbem/bin/owenumnamespace
%defattr(0755,root,root)
/opt/vintela/openwbem/bin/owenumqualifiers
%defattr(0755,root,root)
/opt/vintela/openwbem/bin/owexecwql
%defattr(0755,root,root)
/opt/vintela/openwbem/bin/owmofc
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/sbin
%defattr(0755,root,root)
/opt/vintela/openwbem/sbin/owdigestgenpass
%defattr(0755,root,root)
/opt/vintela/openwbem/sbin/owrepositorydump
%defattr(0644,root,root)
/opt/vintela/openwbem/man/man1/owexecwql.1
%defattr(0644,root,root)
/opt/vintela/openwbem/man/man1/owmofc.1
%defattr(0644,root,root)
/opt/vintela/openwbem/man/man8/owdigestgenpass.8
# End files for ow-tools from /tmp/file_list.ow-tools.21797.14768.30314.0.3082
####################

####################
# Begin files for ow-devel from /tmp/file_list.ow-devel.30827.13471.8082.0.3082
%Files ow-devel
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/include
%defattr(0755,root,root)
%dir /opt/vintela/openwbem/include/openwbem
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_AppenderLogger.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Array.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ArrayFwd.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ArrayImpl.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Assertion.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_AssocDb.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_AssociatorProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_AssociatorProviderInfo.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_AtomicOps.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_AuthenticationException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_AuthenticatorIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Authorizer2IFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_AuthorizerIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_AutoPtr.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Base64FormatException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_BaseStreamBuffer.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_BinaryCIMOMHandle.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_BinarySerialization.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Bool.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ByteSwap.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIM.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMBase.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMClass.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMClassEnumeration.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMClient.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMDataType.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMDateTime.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMElement.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMErrorException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMFeatures.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMFlavor.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMFwd.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMInstance.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMInstanceEnumeration.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMListenerCallback.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMMethod.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMNULL.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMName.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMNameSpace.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMNameSpaceUtils.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMOMHandleIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMOMInfo.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMOMLocator.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMOMLocatorSLP.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMObjectPath.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMObjectPathEnumeration.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMParamValue.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMParameter.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMProperty.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMProtocolIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMProtocolIStreamIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMQualifier.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMQualifierEnumeration.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMQualifierType.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMRepository.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMScope.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMUrl.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMValue.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMValueCast.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMXMLCIMOMHandle.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMXMLListenerFwd.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMXMLParser.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CIMtoXML.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_COWIntrusiveCountableBase.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_COWIntrusiveReference.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_COWReference.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_COWReferenceBase.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Cache.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CerrAppender.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CerrLogger.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Char16.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ClientAuthCBIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ClientCIMOMHandle.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ClientCIMOMHandleConnectionPool.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ClientFwd.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CmdLineParser.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CommonFwd.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Condition.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ConfigException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ConfigFile.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ConfigOpts.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppAssociatorProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppIndicationExportProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppIndicationProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppInstanceProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppMethodProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppPolledProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppProviderBaseIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppProviderIncludes.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppReadOnlyInstanceProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppSecondaryInstanceProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppSimpleAssociatorProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CppSimpleInstanceProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_CryptographicRandomNumber.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_DataStreams.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_DateTime.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Enumeration.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_EnumerationException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_EnvVars.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Exception.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ExceptionIds.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Exec.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_File.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_FileAppender.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_FileSystem.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Format.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_FuncNamePrinter.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_GenericHDBRepository.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_GetPass.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HDB.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HDBCommon.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HDBNode.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPChunkException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPChunkedIStream.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPChunkedOStream.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPClient.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPCounter.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPDeflateIStream.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPDeflateOStream.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPLenLimitIStream.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPStatusCodes.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPUtils.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HTTPXMLCIMListener.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HashMap.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HashMultiMap.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_HttpCommonFwd.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_IOException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_IOIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_IOIFCStreamBuffer.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_IfcsFwd.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Index.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_IndicationExportProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_IndicationProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_IndicationProviderInfo.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_InstanceProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_InstanceProviderInfo.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_InstanceRepository.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_IntrusiveCountableBase.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_IntrusiveReference.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_IstreamBufIterator.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_List.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ListenerAuthenticator.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_LogAppender.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_LogLevel.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_LogMessage.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_LogMessagePatternFormatter.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Logger.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MD5.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MOFCIMOMVisitor.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MOFCompiler.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MOFGrammar.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MOFLineInfo.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MOFParserDecls.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MOFParserErrorHandlerIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MOFVisitor.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Map.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MemTracer.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MetaRepository.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MethodProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MethodProviderInfo.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Mutex.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MutexImpl.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_MutexLock.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NULLValueException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NetwareIdentity.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NetworkTypes.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NoSuchPropertyException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NoSuchProviderException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NoSuchQualifierException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NonRecursiveMutex.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NonRecursiveMutexImpl.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NonRecursiveMutexLock.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NullAppender.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NullLogger.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_NwIface.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_OperationContext.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Param.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_PolledProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_PosixUnnamedPipe.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ProviderAgent.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ProviderAgentFwd.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ProviderAgentLifecycleCallbackIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ProviderAgentLockerIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ProviderBaseIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ProviderEnvironmentIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ProviderFwd.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ProviderIFCBaseIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ProviderInfoBase.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_RWLocker.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_RandomNumber.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_RefCount.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Reference.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ReferenceBase.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_RepositoryCIMOMHandle.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_RepositoryIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_RequestHandlerIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_RequestHandlerIFCXML.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ResultHandlerIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ResultHandlers.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Runnable.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SSLCtxMgr.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SSLException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SSLSocketImpl.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SafeLibCreate.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SecondaryInstanceProviderIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SecondaryInstanceProviderInfo.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Select.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SelectEngine.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SelectableCallbackIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SelectableIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Semaphore.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SerializableIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ServerSocket.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ServerSocketImpl.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ServiceEnvironmentIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ServiceIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ServiceIFCNames.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ServicesHttpFwd.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SessionLanguage.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SharedLibrary.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SharedLibraryException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SharedLibraryLoader.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SharedLibraryReference.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SignalScope.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Socket.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SocketAddress.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SocketBaseImpl.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SocketException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SocketFlags.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SocketImpl.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SocketStreamBuffer.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SocketUtils.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SortedVectorMap.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SortedVectorSet.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Stack.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_StackTrace.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_StrictWeakOrdering.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_String.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_StringBuffer.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_StringEnumeration.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_StringStream.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_SyslogAppender.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_System.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_TempFileEnumerationImplBase.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_TempFileStream.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Thread.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ThreadBarrier.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ThreadCancelledException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ThreadCounter.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ThreadDoneCallback.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ThreadImpl.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ThreadOnce.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ThreadPool.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_ThreadTypes.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_TimeoutException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_TmpFile.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_Types.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_URL.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_UTF8Utils.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_UUID.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_UnnamedPipe.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_UserInfo.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_UserUtils.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WBEMFlags.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WQLAst.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WQLCompile.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WQLIFC.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WQLImpl.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WQLInstancePropertySource.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WQLOperand.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WQLOperation.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WQLPropertySource.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WQLScanUtils.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WQLSelectStatement.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_WQLVisitor.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLAttribute.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLCIMFactory.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLClass.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLEscape.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLFwd.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLListener.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLNode.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLOperationGeneric.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLParseException.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLParserCore.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLParserDOM.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLParserSAX.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLPrettyPrint.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLPullParser.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLQualifier.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_XMLUnescape.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_config.h
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_dlSharedLibrary.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_dlSharedLibraryLoader.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_dyldSharedLibrary.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_dyldSharedLibraryLoader.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_shlSharedLibrary.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_shlSharedLibraryLoader.hpp
%defattr(0644,root,root)
/opt/vintela/openwbem/include/openwbem/OW_vector.hpp
# End files for ow-devel from /tmp/file_list.ow-devel.30827.13471.8082.0.3082
####################

