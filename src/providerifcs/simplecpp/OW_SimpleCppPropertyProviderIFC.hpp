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

#ifndef OW_SIMPLECPP_PROPERTYPROVIDERIFC_HPP_
#define OW_SIMPLECPP_PROPERTYPROVIDERIFC_HPP_

#include "OW_config.h"
#include "OW_SimpleCppProviderBaseIFC.hpp"
#include "OW_SharedLibraryReference.hpp"

/**
 * This is the interface implemented by property providers.  Property providers
 * are used to retrieve and update properties which are dynamic in nature, i.e.
 * their values are not stored in the CIMOM data store.
 * All calls to the derived provider will be serialized so that providers need
 * not worry about multi-threading issues.
 */
class OW_SimpleCppPropertyProviderIFC: public virtual OW_SimpleCppProviderBaseIFC
{
public:
	virtual ~OW_SimpleCppPropertyProviderIFC() {}

	/**
	 * This method retrieves the value of the property specified by the
	 * parameters.
	 *
	 * @param cop Contains the path to the instance whose property must be
	 * 	retrieved.
	 * @param propertyName The name of the property.
	 *
	 * @returns OW_CIMValue - The value of the property.
	 *
	 * @throws OW_CIMException
	 */
	virtual OW_CIMValue getPropertyValue(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_String& propertyName);

	/**
	 * This method sets the value of the property specified by the
	 * parameters.
	 *
	 * @param cop Contains the path to the instance whose property must be
	 * 	set.
	 * @param propertyName The name of the property.
	 * @param val The value to set the property to.
	 *
	 * @throws OW_CIMException
	 */
	virtual void setPropertyValue(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_String& propertyName,
			const OW_CIMValue& val );

	virtual OW_SimpleCppPropertyProviderIFC* getPropertyProvider() { return this; }
};

typedef OW_SharedLibraryReference< OW_SimpleCppPropertyProviderIFC > OW_SimpleCppPropertyProviderIFCRef;

#endif
