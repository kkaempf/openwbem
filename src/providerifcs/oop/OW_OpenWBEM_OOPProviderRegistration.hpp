/*******************************************************************************
 * Copyright (C) 2007 Quest Software All rights reserved.
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
 *  - Neither the name of Quest Software nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL Quest Software OR THE CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/**
 * @author Generated by codegen on Wed Jan 31 14:07:23 MST 2007
 */

#ifndef OW_GENERATED_OPENWBEM_OOPPROVIDERREGISTRATION_HPP_INCLUDE_GUARD_
#define OW_GENERATED_OPENWBEM_OOPPROVIDERREGISTRATION_HPP_INCLUDE_GUARD_

#include "OW_config.h"
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
	blocxx::String getInstanceID() const
	{
		return getStringProperty("InstanceID");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.InstanceID property value.
	 * This property is described as follows:
	 *
	 *  Within the scope of the instantiating Namespace, InstanceID opaquely and uniquely identifies an instance of this class. In order to ensure uniqueness within the NameSpace, the value of InstanceID SHOULD be constructed using the following 'preferred' algorithm: <OrgID>:<LocalID>Where <OrgID> and <LocalID> are separated by a colon ':', and where <OrgID> MUST include a copyrighted, trademarked or otherwise unique name that is owned by the business entity creating/defining the InstanceID, or is a registered ID that is assigned to the business entity by a recognized global authority. (This is similar to the <Schema Name>_<Class Name> structure of Schema class names.) In addition, to ensure uniqueness <OrgID> MUST NOT contain a colon (':'). When using this algorithm, the first colon to appear in InstanceID MUST appear between <OrgID> and <LocalID>. <LocalID> is chosen by the business entity and SHOULD not be re-used to identify different underlying (real-world) elements. If the above 'preferred' algorithm is not used, the defining entity MUST assure that the resultant InstanceID is not re-used across any InstanceIDs produced by this or other providers for this instance's NameSpace. For DMTF defined instances, the 'preferred' algorithm MUST be used with the <OrgID> set to 'CIM'.
	 *
	 * @param blocxx::String new InstanceID property value
	 */
	void setInstanceID(const blocxx::String& v)
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
	 * @return blocxx::StringArray current NamespaceNames property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	blocxx::StringArray getNamespaceNames() const
	{
		return getStringArrayProperty("NamespaceNames");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.NamespaceNames property value.
	 * This property is described as follows:
	 *
	 *  The namespaces for this provider registration. If this property is empty or NULL, all namespaces are implied.
	 *
	 * @param blocxx::StringArray new NamespaceNames property value
	 */
	void setNamespaceNames(const blocxx::StringArray& v)
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
	 * @return blocxx::String current ClassName property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	blocxx::String getClassName() const
	{
		return getStringProperty("ClassName");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.ClassName property value.
	 * This property is described as follows:
	 *
	 *  The CIM class name for the provider
	 *
	 * @param blocxx::String new ClassName property value
	 */
	void setClassName(const blocxx::String& v)
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
		E_PROVIDERTYPES_POLLED = 8,
		E_PROVIDERTYPES_QUERY = 9
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
	blocxx::UInt16Array getProviderTypes() const
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
	void setProviderTypes(const blocxx::UInt16Array& v)
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
	 * @return blocxx::String current Process property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	blocxx::String getProcess() const
	{
		return getStringProperty("Process");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.Process property value.
	 * This property is described as follows:
	 *
	 *  Process to launch.
	 *
	 * @param blocxx::String new Process property value
	 */
	void setProcess(const blocxx::String& v)
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
	 * @return blocxx::StringArray current Args property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	blocxx::StringArray getArgs() const
	{
		return getStringArrayProperty("Args");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.Args property value.
	 * This property is described as follows:
	 *
	 *  Command line arguments to use when launching Process.
	 *
	 * @param blocxx::StringArray new Args property value
	 */
	void setArgs(const blocxx::StringArray& v)
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
	 * @return blocxx::String current Protocol property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	blocxx::String getProtocol() const
	{
		return getStringProperty("Protocol");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.Protocol property value.
	 * This property is described as follows:
	 *
	 *  Protocol to use.  Valid values are: owcpp1.
	 *
	 * @param blocxx::String new Protocol property value
	 */
	void setProtocol(const blocxx::String& v)
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
	 *  An interval (an absolute time is NOT valid) describing how long the provider process will be allowed to run before being terminated. Care must be taken in choosing this value for persistent providers (polled & indication), because if a timeout occurs, the provider process will be killed and will not be called again.
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
	 *  An interval (an absolute time is NOT valid) describing how long the provider process will be allowed to run before being terminated. Care must be taken in choosing this value for persistent providers (polled & indication), because if a timeout occurs, the provider process will be killed and will not be called again.
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
		E_USERCONTEXT_MONITORED = 4,
		E_USERCONTEXT_OPERATION_MONITORED = 5
	};


	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.UserContext property value.
	 * This property is described as follows:
	 *
	 *  The user context that will be used to run the provider process super user means that the provider process will be run as root on POSIX systems, and as SYSTEM on win32. BE VERY CAUTIOUS WITH THIS SETTING!operation means that the provider process will be run as the authenticated username who initiated the operation.unprivileged means that the provider process will be run as an unprivileged user named owcimomd.monitored means that the provider process will be run as the user specified in the monitor privileges configuration file, and the monitor will run as root.  BE VERY CAUTIOUS WITH THIS SETTING!  Operation monitored is preferred to monitored because with monitored, non-root users can run code as root if the CIMOM is configured to allow non-root users to log in.  Monitored is similar to super user in this regard.operation monitored means that the provider process will be run as the user specified in the monitor privileges configuration file, and the monitor wil run as as the authenticated username who initiated the operation.If this setting is "monitored" or "operation monitored", MonitorPrivilegesFile must be set.
	 *
	 * @return UInt16 current UserContext property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	blocxx::UInt16 getUserContext() const
	{
		return getUInt16Property("UserContext");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.UserContext property value.
	 * This property is described as follows:
	 *
	 *  The user context that will be used to run the provider process super user means that the provider process will be run as root on POSIX systems, and as SYSTEM on win32. BE VERY CAUTIOUS WITH THIS SETTING!operation means that the provider process will be run as the authenticated username who initiated the operation.unprivileged means that the provider process will be run as an unprivileged user named owcimomd.monitored means that the provider process will be run as the user specified in the monitor privileges configuration file, and the monitor will run as root.  BE VERY CAUTIOUS WITH THIS SETTING!  Operation monitored is preferred to monitored because with monitored, non-root users can run code as root if the CIMOM is configured to allow non-root users to log in.  Monitored is similar to super user in this regard.operation monitored means that the provider process will be run as the user specified in the monitor privileges configuration file, and the monitor wil run as as the authenticated username who initiated the operation.If this setting is "monitored" or "operation monitored", MonitorPrivilegesFile must be set.
	 *
	 * @param UInt16 new UserContext property value
	 */
	void setUserContext(const blocxx::UInt16 v)
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
	 *  If UserContext = 4 or 5 (monitored), a monitor will be created for the provider process. The monitor can give the process special permissions. This property must be set to the file name of the provider's privileges configuration file. The file must be in the directory identified by the owcimomd.privileges_config_dir config item.
	 *
	 * @return blocxx::String current MonitorPrivilegesFile property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	blocxx::String getMonitorPrivilegesFile() const
	{
		return getStringProperty("MonitorPrivilegesFile");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.MonitorPrivilegesFile property value.
	 * This property is described as follows:
	 *
	 *  If UserContext = 4 or 5 (monitored), a monitor will be created for the provider process. The monitor can give the process special permissions. This property must be set to the file name of the provider's privileges configuration file. The file must be in the directory identified by the owcimomd.privileges_config_dir config item.
	 *
	 * @param blocxx::String new MonitorPrivilegesFile property value
	 */
	void setMonitorPrivilegesFile(const blocxx::String& v)
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
	 *  List of class names (subclasses of CIM_ListenerDestination) that the provider handles. Only applicable for Indication Export providers.
	 *
	 * @return blocxx::blocxx::StringArray current IndicationExportHandlerClassNames property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	blocxx::StringArray getIndicationExportHandlerClassNames() const
	{
		return getStringArrayProperty("IndicationExportHandlerClassNames");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.IndicationExportHandlerClassNames property value.
	 * This property is described as follows:
	 *
	 *  List of class names (subclasses of CIM_ListenerDestination) that the provider handles. Only applicable for Indication Export providers.
	 *
	 * @param blocxx::blocxx::StringArray new IndicationExportHandlerClassNames property value
	 */
	void setIndicationExportHandlerClassNames(const blocxx::StringArray& v)
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



	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.Persistent property value.
	 * This property is described as follows:
	 *
	 *  Setting this property to true means that the provider process will not be terminated and sent the SET_PERSISTENT(true) command. For polled, indication export or indication providers it defaults to true. For other provider types it defaults to false.
	 *
	 * @return bool current Persistent property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	bool getPersistent() const
	{
		return getBoolProperty("Persistent");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.Persistent property value.
	 * This property is described as follows:
	 *
	 *  Setting this property to true means that the provider process will not be terminated and sent the SET_PERSISTENT(true) command. For polled, indication export or indication providers it defaults to true. For other provider types it defaults to false.
	 *
	 * @param bool new Persistent property value
	 */
	void setPersistent(const bool v)
	{
		setBoolProperty("Persistent", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.Persistent property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool PersistentIsNULL()
	{
		return propertyIsNULL("Persistent");
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.UnloadTimeout property value.
	 * This property is described as follows:
	 *
	 *  If a provider is not persistent, then it can stay running for the period of time specified by this property. The value must be an interval, an absolute datetime is not allowed.
	 *
	 * @return CIMDateTime current UnloadTimeout property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	CIMDateTime getUnloadTimeout() const
	{
		return getCIMDateTimeProperty("UnloadTimeout");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.UnloadTimeout property value.
	 * This property is described as follows:
	 *
	 *  If a provider is not persistent, then it can stay running for the period of time specified by this property. The value must be an interval, an absolute datetime is not allowed.
	 *
	 * @param CIMDateTime new UnloadTimeout property value
	 */
	void setUnloadTimeout(const CIMDateTime& v)
	{
		setCIMDateTimeProperty("UnloadTimeout", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.UnloadTimeout property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool UnloadTimeoutIsNULL()
	{
		return propertyIsNULL("UnloadTimeout");
	}



	// This enum is to be used with getMethodProviderLockType() and setMethodProviderLockType()
	// These values are distinct values and can *NOT* be and'd and or'd together.
	enum
	{
		E_METHODPROVIDERLOCKTYPE_NO_LOCK = 0,
		E_METHODPROVIDERLOCKTYPE_READ_LOCK = 1,
		E_METHODPROVIDERLOCKTYPE_WRITE_LOCK = 2
	};


	/**
	 * This function returns the OpenWBEM_OOPProviderRegistration.MethodProviderLockType property value.
	 * This property is described as follows:
	 *
	 *  Specify which type of lock is required by a method provider. This property is ignored for non-method providers.
	 *
	 * @return UInt16 current MethodProviderLockType property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	blocxx::UInt16 getMethodProviderLockType() const
	{
		return getUInt16Property("MethodProviderLockType");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderRegistration.MethodProviderLockType property value.
	 * This property is described as follows:
	 *
	 *  Specify which type of lock is required by a method provider. This property is ignored for non-method providers.
	 *
	 * @param UInt16 new MethodProviderLockType property value
	 */
	void setMethodProviderLockType(const blocxx::UInt16 v)
	{
		setUInt16Property("MethodProviderLockType", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderRegistration.MethodProviderLockType property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool MethodProviderLockTypeIsNULL()
	{
		return propertyIsNULL("MethodProviderLockType");
	}



};

}
} // end namespace OW_NAMESPACE::OpenWBEM

#endif


