/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_NPI_INSTANCE_PROVIDER_PROXY_HPP_
#define OW_NPI_INSTANCE_PROVIDER_PROXY_HPP_

#include "OW_config.h"
#include "OW_InstanceProviderIFC.hpp"
#include "OW_FTABLERef.hpp"

class OW_NPIInstanceProviderProxy : public OW_InstanceProviderIFC
{
public:

	OW_NPIInstanceProviderProxy(const OW_FTABLERef& f)
		: m_ftable(f)
	{
	}

	virtual ~OW_NPIInstanceProviderProxy() {}


	/**
	 * This method enumerates
	 * all instances of the class which is specified in cop.  The entire
	 * instances and not just the names are returned.  Deep or shallow
	 * enumeration is possible.
	 *
	 * @param cop The object path specifies the class that must be
	 * 	enumerated.
	 *
	 * @param deep If true, deep enumeration must be done, otherwise shallow.
	 *
	 * @param cimClass The class reference.
	 *
	 * @param localOnly If true, only the non-inherited properties are to be
	 * 	returned, otherwise all properties are required.
	 *
	 * @returns An array of OW_CIMInstance containing names of the enumerated
	 * 	instances.
	 *
	 * @throws OW_CIMException - thrown if cop is incorrect or does not exist.
	 */
	virtual OW_CIMObjectPathEnumeration enumInstances(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath cop,
			OW_Bool deep,
			OW_CIMClass cimClass );

	virtual OW_CIMInstanceEnumeration enumInstances(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath cop,
			OW_Bool deep,
			OW_CIMClass cimClass,
			OW_Bool localOnly );
	
	/**
	 * This method deletes the instance specified in the object path
	 *
	 * @param cop The instance to be deleted
	 *
	 * @throws OW_CIMException
	 */
	virtual void deleteInstance(const OW_ProviderEnvironmentIFCRef &env,
		OW_CIMObjectPath cop);

	/**
	 * This method retrieves the instance specified in the object path.
	 *
	 * @param cop The name of the instance to be retrieved.
	 *
	 * @param cimClass The class to which the instance belongs.  This is
	 * 	useful for providers which do not want to create instances from
	 * 	scratch.  They can call the class newInstance() routine to create
	 * 	a template for the new instance.
	 *
	 * @param localOnly If true, only the non-inherited properties are to be
	 * 	returned, otherwise all properties are required.
	 *
	 * @returns The retrieved instance
	 *
	 * @throws OW_CIMException - thrown if cop is incorrect or does not exist
	 */
	
	virtual OW_CIMInstance getInstance(const OW_ProviderEnvironmentIFCRef &env,
		OW_CIMObjectPath cop, OW_CIMClass cimClass, OW_Bool localOnly);

	/**
	 * This method creates the instance specified in the object path.  If the
	 * instance does exist an OW_CIMException with ID CIM_ERR_ALREADY_EXISTS
	 * must be thrown.  The parameter should be the instance name.
	 *
	 * @param cop The path to the instance to be set.  The import part in
	 * 	this parameter is the namespace component.
	 *
	 * @param cimInstance The instance to be set
	 *
	 * @returns A CIM ObjectPath of the instance that was created.
	 *
	 * @throws OW_CIMException
	 */
	
	virtual OW_CIMObjectPath createInstance(
		const OW_ProviderEnvironmentIFCRef &env, OW_CIMObjectPath cop,
		OW_CIMInstance cimInstance);

	/**
	 * This method sets the instance specified in the object path.  If the
	 * instance does not exist an OW_CIMException with ID CIM_ERR_NOT_FOUND
	 * must be thrown.  The parameter should be the instance name.
	 *
	 * @param cop The path of the instance to be set.  The important part in
	 * 	this parameter is the namespace component.
	 *
	 * @param cimInstance The instance to be set.
	 *
	 * @throws OW_CIMException
	 */
	virtual void setInstance(const OW_ProviderEnvironmentIFCRef &env,
		OW_CIMObjectPath cop, OW_CIMInstance cimInstance);

private:
	OW_FTABLERef m_ftable;
};
										
#endif

