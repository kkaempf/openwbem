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
 * @author Generated by codegen on Wed Jan 31 14:43:47 MST 2007
 */

#ifndef OW_GENERATED_OPENWBEM_OOPPROVIDERPROCESSCAPABILITIES_HPP_INCLUDE_GUARD_
#define OW_GENERATED_OPENWBEM_OOPPROVIDERPROCESSCAPABILITIES_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_CIMInstanceWrapperBase.hpp"

namespace OW_NAMESPACE
{
namespace OpenWBEM
{

class OOPProviderProcessCapabilities : public CIMInstanceWrapperBase
{
public:
	/**
	 * @throws CIMInstanceWrapperException if x.getClassName() != "OpenWBEM_OOPProviderProcessCapabilities"
	 */
	OOPProviderProcessCapabilities(const CIMInstance& x)
		: CIMInstanceWrapperBase(x, "OpenWBEM_OOPProviderProcessCapabilities")
	{
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderProcessCapabilities.Process property value.
	 *
	 * @return CIMObjectPath current Process property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	CIMObjectPath getProcess() const
	{
		return getCIMObjectPathProperty("Process");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderProcessCapabilities.Process property value.
	 *
	 * @param CIMObjectPath new Process property value
	 */
	void setProcess(const CIMObjectPath v)
	{
		setCIMObjectPathProperty("Process", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderProcessCapabilities.Process property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool ProcessIsNULL()
	{
		return propertyIsNULL("Process");
	}



	/**
	 * This function returns the OpenWBEM_OOPProviderProcessCapabilities.Capabilities property value.
	 *
	 * @return CIMObjectPath current Capabilities property value
	 * @exception NullValueException if the property value is NULL (uninitialized)
	 */
	CIMObjectPath getCapabilities() const
	{
		return getCIMObjectPathProperty("Capabilities");
	}

	/**
	 * This function sets the OpenWBEM_OOPProviderProcessCapabilities.Capabilities property value.
	 *
	 * @param CIMObjectPath new Capabilities property value
	 */
	void setCapabilities(const CIMObjectPath v)
	{
		setCIMObjectPathProperty("Capabilities", v);
	}

	/**
	 * This function tests whether the OpenWBEM_OOPProviderProcessCapabilities.Capabilities property value
	 * is NULL, meaning it has not been initialized to a value.
	 */
	bool CapabilitiesIsNULL()
	{
		return propertyIsNULL("Capabilities");
	}



};

}
} // end namespace OW_NAMESPACE::OpenWBEM

#endif


