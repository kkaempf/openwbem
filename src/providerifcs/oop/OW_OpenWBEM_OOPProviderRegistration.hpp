/*******************************************************************************
 * Copyright (C) 2005 Vintela, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  - Neither the name of Vintela, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/**
 * @author Generated by codegen on Fri Jul 01 14:26:36 MDT 2005
 */

#ifndef OW_GENERATED_OPENWBEM_OOPPROVIDERREGISTRATION_HPP_INCLUDE_GUARD_
#define OW_GENERATED_OPENWBEM_OOPPROVIDERREGISTRATION_HPP_INCLUDE_GUARD_

#include "OW_CIMInstanceWrapperBase.hpp"

namespace OW_NAMESPACE
{ 
namespace OpenWBEM 
{

class OOPProviderRegistration : public CIMInstanceWrapperBase
{
public:
	/**
	 * @throws CIMInstanceWrapperException if x.getClassName() != "OpenWBEM_OOPProviderRegistration"
	 */
	OOPProviderRegistration(const CIMInstance& x)
		: CIMInstanceWrapperBase(x, "OpenWBEM_OOPProviderRegistration")
	{
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.InstanceID property value. 
	 * This property is described as follows: 
	 * 
	 *  Within the scope of the instantiating Namespace, InstanceID opaquely and uniquely identifies an instance of this class. In order to ensure uniqueness within the NameSpace, the value of InstanceID SHOULD be constructed using the following 'preferred' algorithm: <OrgID>:<LocalID>Where <OrgID> and <LocalID> are separated by a colon ':', and where <OrgID> MUST include a copyrighted, trademarked or otherwise unique name that is owned by the business entity creating/defining the InstanceID, or is a registered ID that is assigned to the business entity by a recognized global authority. (This is similar to the <Schema Name>_<Class Name> structure of Schema class names.) In addition, to ensure uniqueness <OrgID> MUST NOT contain a colon (':'). When using this algorithm, the first colon to appear in InstanceID MUST appear between <OrgID> and <LocalID>. <LocalID> is chosen by the business entity and SHOULD not be re-used to identify different underlying (real-world) elements. If the above 'preferred' algorithm is not used, the defining entity MUST assure that the resultant InstanceID is not re-used across any InstanceIDs produced by this or other providers for this instance's NameSpace. For DMTF defined instances, the 'preferred' algorithm MUST be used with the <OrgID> set to 'CIM'.
	 * 
	 * @return String current InstanceID property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	String getInstanceID() const
	{
		return getStringProperty("InstanceID");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.InstanceID property value.
	 * This property is described as follows: 
	 * 
	 *  Within the scope of the instantiating Namespace, InstanceID opaquely and uniquely identifies an instance of this class. In order to ensure uniqueness within the NameSpace, the value of InstanceID SHOULD be constructed using the following 'preferred' algorithm: <OrgID>:<LocalID>Where <OrgID> and <LocalID> are separated by a colon ':', and where <OrgID> MUST include a copyrighted, trademarked or otherwise unique name that is owned by the business entity creating/defining the InstanceID, or is a registered ID that is assigned to the business entity by a recognized global authority. (This is similar to the <Schema Name>_<Class Name> structure of Schema class names.) In addition, to ensure uniqueness <OrgID> MUST NOT contain a colon (':'). When using this algorithm, the first colon to appear in InstanceID MUST appear between <OrgID> and <LocalID>. <LocalID> is chosen by the business entity and SHOULD not be re-used to identify different underlying (real-world) elements. If the above 'preferred' algorithm is not used, the defining entity MUST assure that the resultant InstanceID is not re-used across any InstanceIDs produced by this or other providers for this instance's NameSpace. For DMTF defined instances, the 'preferred' algorithm MUST be used with the <OrgID> set to 'CIM'.
	 * 
	 * @param String new InstanceID property value
	 */
	void setInstanceID(const String& v)
	{
		setStringProperty("InstanceID", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.InstanceID property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool InstanceIDIsNULL()
	{
		return propertyIsNULL("InstanceID");
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.NamespaceNames property value. 
	 * This property is described as follows: 
	 * 
	 *  The namespaces for this provider registration. If this property is empty or NULL, all namespaces are implied.
	 * 
	 * @return StringArray current NamespaceNames property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	StringArray getNamespaceNames() const
	{
		return getStringArrayProperty("NamespaceNames");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.NamespaceNames property value.
	 * This property is described as follows: 
	 * 
	 *  The namespaces for this provider registration. If this property is empty or NULL, all namespaces are implied.
	 * 
	 * @param StringArray new NamespaceNames property value
	 */
	void setNamespaceNames(const StringArray& v)
	{
		setStringArrayProperty("NamespaceNames", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.NamespaceNames property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool NamespaceNamesIsNULL()
	{
		return propertyIsNULL("NamespaceNames");
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.ClassName property value. 
	 * This property is described as follows: 
	 * 
	 *  The CIM class name for the provider
	 * 
	 * @return String current ClassName property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	String getClassName() const
	{
		return getStringProperty("ClassName");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.ClassName property value.
	 * This property is described as follows: 
	 * 
	 *  The CIM class name for the provider
	 * 
	 * @param String new ClassName property value
	 */
	void setClassName(const String& v)
	{
		setStringProperty("ClassName", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.ClassName property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool ClassNameIsNULL()
	{
		return propertyIsNULL("ClassName");
	}


	// This enum is to be used with getProviderTypes() and setProviderTypes()
	// These values are distinct values and can *NOT* be and'd and or'd together.
	enum
	{
		E_PROVIDERTYPES_INSTANCE = 1,
		E_PROVIDERTYPES_SECONDARY_INSTANCE = 2,
		E_PROVIDERTYPES_ASSOCIATION = 3,
		E_PROVIDERTYPES_LIFECYCLE_INDICATION = 4,
		E_PROVIDERTYPES_ALERT_INDICATION = 5,
		E_PROVIDERTYPES_METHOD = 6,
		E_PROVIDERTYPES_INDICATION_EXPORT = 7,
		E_PROVIDERTYPES_POLLED = 8
	};


	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.ProviderTypes property value. 
	 * This property is described as follows: 
	 * 
	 *  ProviderTypes identifies the kind of provider.
	 * 
	 * @return UInt16Array current ProviderTypes property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	UInt16Array getProviderTypes() const
	{
		return getUInt16ArrayProperty("ProviderTypes");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.ProviderTypes property value.
	 * This property is described as follows: 
	 * 
	 *  ProviderTypes identifies the kind of provider.
	 * 
	 * @param UInt16Array new ProviderTypes property value
	 */
	void setProviderTypes(const UInt16Array& v)
	{
		setUInt16ArrayProperty("ProviderTypes", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.ProviderTypes property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool ProviderTypesIsNULL()
	{
		return propertyIsNULL("ProviderTypes");
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.Process property value. 
	 * This property is described as follows: 
	 * 
	 *  Process to launch.
	 * 
	 * @return String current Process property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	String getProcess() const
	{
		return getStringProperty("Process");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.Process property value.
	 * This property is described as follows: 
	 * 
	 *  Process to launch.
	 * 
	 * @param String new Process property value
	 */
	void setProcess(const String& v)
	{
		setStringProperty("Process", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.Process property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool ProcessIsNULL()
	{
		return propertyIsNULL("Process");
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.Args property value. 
	 * This property is described as follows: 
	 * 
	 *  Command line arguments to use when launching Process.
	 * 
	 * @return StringArray current Args property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	StringArray getArgs() const
	{
		return getStringArrayProperty("Args");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.Args property value.
	 * This property is described as follows: 
	 * 
	 *  Command line arguments to use when launching Process.
	 * 
	 * @param StringArray new Args property value
	 */
	void setArgs(const StringArray& v)
	{
		setStringArrayProperty("Args", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.Args property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool ArgsIsNULL()
	{
		return propertyIsNULL("Args");
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.Protocol property value. 
	 * This property is described as follows: 
	 * 
	 *  Protocol to use.  Valid values are: owcpp1.
	 * 
	 * @return String current Protocol property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	String getProtocol() const
	{
		return getStringProperty("Protocol");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.Protocol property value.
	 * This property is described as follows: 
	 * 
	 *  Protocol to use.  Valid values are: owcpp1.
	 * 
	 * @param String new Protocol property value
	 */
	void setProtocol(const String& v)
	{
		setStringProperty("Protocol", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.Protocol property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool ProtocolIsNULL()
	{
		return propertyIsNULL("Protocol");
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.Timeout property value. 
	 * This property is described as follows: 
	 * 
	 *  An interval (an absolute time is NOT valid) describing how long the provider process will be allowed to run before being terminated.
	 * 
	 * @return CIMDateTime current Timeout property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	CIMDateTime getTimeout() const
	{
		return getCIMDateTimeProperty("Timeout");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.Timeout property value.
	 * This property is described as follows: 
	 * 
	 *  An interval (an absolute time is NOT valid) describing how long the provider process will be allowed to run before being terminated.
	 * 
	 * @param CIMDateTime new Timeout property value
	 */
	void setTimeout(const CIMDateTime& v)
	{
		setCIMDateTimeProperty("Timeout", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.Timeout property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool TimeoutIsNULL()
	{
		return propertyIsNULL("Timeout");
	}


	// This enum is to be used with getUserContext() and setUserContext()
	// These values are distinct values and can *NOT* be and'd and or'd together.
	enum
	{
		E_USERCONTEXT_SUPER_USER = 1,
		E_USERCONTEXT_OPERATION = 2,
		E_USERCONTEXT_UNPRIVILEGED = 3,
		E_USERCONTEXT_MONITORED = 4
	};


	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.UserContext property value. 
	 * This property is described as follows: 
	 * 
	 *  The user context that will be used to run the provider process super user means that the provider process will be run as root on POSIX systems, and as SYSTEM on win32. BE VERY CAUTIOUS WITH THIS SETTING!
     *  operation means that the provider process will be run as the authenticated username who initiated the operation.
     *  unprivileged means that the provider process will be run as an unprivileged user named owcimomd.
	 * 
	 * @return UInt16 current UserContext property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	UInt16 getUserContext() const
	{
		return getUInt16Property("UserContext");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.UserContext property value.
	 * This property is described as follows: 
	 * 
	 *  The user context that will be used to run the provider process super user means that the provider process will be run as root on POSIX systems, and as SYSTEM on win32. BE VERY CAUTIOUS WITH THIS SETTING!
     *  operation means that the provider process will be run as the authenticated username who initiated the operation.
     *  unprivileged means that the provider process will be run as an unprivileged user named owcimomd.
	 * 
	 * @param v new UserContext property value
	 */
	void setUserContext(UInt16 v)
	{
		setUInt16Property("UserContext", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.UserContext property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool UserContextIsNULL()
	{
		return propertyIsNULL("UserContext");
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.MonitorPrivilegesFile property value. 
	 * This property is described as follows: 
	 * 
	 *  If UserContext = 3 (unprivileged), a monitor can be created for the provider process. The monitor can give the process special permissions. By default no monitor is created. If one is needed, this property must be set to the file name of the provider's privileges configuration file. The file must be in the directory identified by the owcimomd.privileges_config_dir config item.
	 * 
	 * @return String current MonitorPrivilegesFile property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	String getMonitorPrivilegesFile() const
	{
		return getStringProperty("MonitorPrivilegesFile");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.MonitorPrivilegesFile property value.
	 * This property is described as follows: 
	 * 
	 *  If UserContext = 3 (unprivileged), a monitor can be created for the provider process. The monitor can give the process special permissions. By default no monitor is created. If one is needed, this property must be set to the file name of the provider's privileges configuration file. The file must be in the directory identified by the owcimomd.privileges_config_dir config item.
	 * 
	 * @param String new MonitorPrivilegesFile property value
	 */
	void setMonitorPrivilegesFile(const String& v)
	{
		setStringProperty("MonitorPrivilegesFile", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.MonitorPrivilegesFile property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool MonitorPrivilegesFileIsNULL()
	{
		return propertyIsNULL("MonitorPrivilegesFile");
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.IndicationExportHandlerClassNames property value. 
	 * This property is described as follows: 
	 * 
	 *  Command line arguments to use when launching Process.
	 * 
	 * @return StringArray current IndicationExportHandlerClassNames property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	StringArray getIndicationExportHandlerClassNames() const
	{
		return getStringArrayProperty("IndicationExportHandlerClassNames");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.IndicationExportHandlerClassNames property value.
	 * This property is described as follows: 
	 * 
	 *  Command line arguments to use when launching Process.
	 * 
	 * @param StringArray new IndicationExportHandlerClassNames property value
	 */
	void setIndicationExportHandlerClassNames(const StringArray& v)
	{
		setStringArrayProperty("IndicationExportHandlerClassNames", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.IndicationExportHandlerClassNames property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool IndicationExportHandlerClassNamesIsNULL()
	{
		return propertyIsNULL("IndicationExportHandlerClassNames");
	}



};

}
} // end namespace OW_NAMESPACE::OpenWBEM

#endif

