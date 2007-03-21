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
 * @author Generated by codegen on Wed Jan 31 14:40:28 MST 2007
 */

#ifndef OW_GENERATED_OPENWBEM_OOPLIFECYCLEINDICATIONPROVIDERCAPABILITIES_HPP_INCLUDE_GUARD_
#define OW_GENERATED_OPENWBEM_OOPLIFECYCLEINDICATIONPROVIDERCAPABILITIES_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_CIMInstanceWrapperBase.hpp"

namespace OW_NAMESPACE
{ 
namespace OpenWBEM 
{

class OOPLifecycleIndicationProviderCapabilities : public CIMInstanceWrapperBase
{
public:
	/**
	 * @throws CIMInstanceWrapperException if x.getClassName() != "OpenWBEM_OOPLifecycleIndicationProviderCapabilities"
	 */
	OOPLifecycleIndicationProviderCapabilities(const CIMInstance& x)
		: CIMInstanceWrapperBase(x, "OpenWBEM_OOPLifecycleIndicationProviderCapabilities")
	{
	}



	/**
	 * This function returns the OpenWBEM_OOPLifecycleIndicationProviderCapabilities.NamespaceNames property value. 
	 * This property is described as follows: 
	 * 
	 *  The namespaces for this provider registration. If this property is empty or NULL, all namespaces are implied. These are the source namespace for the indication, not the namespace of the registration.
	 * 
	 * @return StringArray current NamespaceNames property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	StringArray getNamespaceNames() const
	{
		return getStringArrayProperty("NamespaceNames");
	}

	/**
	 * This function sets the OpenWBEM_OOPLifecycleIndicationProviderCapabilities.NamespaceNames property value.
	 * This property is described as follows: 
	 * 
	 *  The namespaces for this provider registration. If this property is empty or NULL, all namespaces are implied. These are the source namespace for the indication, not the namespace of the registration.
	 * 
	 * @param StringArray new NamespaceNames property value
	 */
	void setNamespaceNames(const StringArray& v)
	{
		setStringArrayProperty("NamespaceNames", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPLifecycleIndicationProviderCapabilities.NamespaceNames property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool NamespaceNamesIsNULL()
	{
		return propertyIsNULL("NamespaceNames");
	}



	/**
	 * This function returns the OpenWBEM_OOPLifecycleIndicationProviderCapabilities.ClassNames property value. 
	 * This property is described as follows: 
	 * 
	 *  The CIM class names for the provider. These are the names of the classes that are monitored for changes. e.g. CIM_DiskDrive.CIM_InstIndication and subclasses should not be listed here, even though those are the actual classes of the indications the provider generates.
	 * 
	 * @return StringArray current ClassNames property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	StringArray getClassNames() const
	{
		return getStringArrayProperty("ClassNames");
	}

	/**
	 * This function sets the OpenWBEM_OOPLifecycleIndicationProviderCapabilities.ClassNames property value.
	 * This property is described as follows: 
	 * 
	 *  The CIM class names for the provider. These are the names of the classes that are monitored for changes. e.g. CIM_DiskDrive.CIM_InstIndication and subclasses should not be listed here, even though those are the actual classes of the indications the provider generates.
	 * 
	 * @param StringArray new ClassNames property value
	 */
	void setClassNames(const StringArray& v)
	{
		setStringArrayProperty("ClassNames", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPLifecycleIndicationProviderCapabilities.ClassNames property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool ClassNamesIsNULL()
	{
		return propertyIsNULL("ClassNames");
	}



	/**
	 * This function returns the OpenWBEM_OOPLifecycleIndicationProviderCapabilities.InstanceID property value. 
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
	 * This function sets the OpenWBEM_OOPLifecycleIndicationProviderCapabilities.InstanceID property value.
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
	 * This function tests whether the OpenWBEM_OOPLifecycleIndicationProviderCapabilities.InstanceID property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool InstanceIDIsNULL()
	{
		return propertyIsNULL("InstanceID");
	}



};

}
} // end namespace OW_NAMESPACE::OpenWBEM

#endif


