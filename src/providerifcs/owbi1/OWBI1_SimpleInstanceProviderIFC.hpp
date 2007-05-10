/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#ifndef OWBI1_SIMPLE_INSTANCE_PROVIDER_IFC_HPP_
#define OWBI1_SIMPLE_INSTANCE_PROVIDER_IFC_HPP_
#include "OWBI1_config.h"
#include "OWBI1_InstanceProviderIFC.hpp"

namespace OWBI1
{

/**
 * A provider can derive from this class, and implement doSimpleEnumerateInstances(),
 * and BI1SimpleInstanceProviderIFC implements getInstance(),
 * enumInstanceNames(), and enumInstances()
 * This class can be part of a multiple-inerhitance diamond.  
 * Thus the "public virtual" inheritance.
 */
class OWBI1_OWBI1PROVIFC_API BI1SimpleInstanceProviderIFC : public virtual BI1InstanceProviderIFC
{
public:
	virtual CIMInstance getInstance(
		const ProviderEnvironmentIFCRef &env, 
		const String &ns, 
		const CIMObjectPath &instanceName, 
		const CIMPropertyList& propertyList, 
		const CIMClass &cimClass);
	virtual void enumInstanceNames(
		const ProviderEnvironmentIFCRef &env, 
		const String &ns, 
		const CIMName& className, 
		CIMObjectPathResultHandlerIFC &result, 
		const CIMClass &cimClass);
	virtual void enumInstances(
		const ProviderEnvironmentIFCRef &env, 
		const String &ns, 
		const CIMName& className, 
		CIMInstanceResultHandlerIFC &result, 
		const CIMPropertyList& propertyList, 
		const CIMClass &requestedClass, 
		const CIMClass &cimClass);
	enum EPropertiesFlag
	{
		E_ALL_PROPERTIES,
		E_KEY_PROPERTIES_ONLY
	};
	virtual void doSimpleEnumInstances(
		const ProviderEnvironmentIFCRef &env, 
		const String &ns, 
		const CIMClass &cimClass, 
		CIMInstanceResultHandlerIFC &result,
		EPropertiesFlag propertiesFlag) = 0;
};

} // end namespace OWBI1

#endif