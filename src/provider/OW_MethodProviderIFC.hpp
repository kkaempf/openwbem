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

#ifndef OW_METHODPROVIDERIFC_HPP_
#define OW_METHODPROVIDERIFC_HPP_

#include "OW_config.h"
#include "OW_ProviderBaseIFC.hpp"
#include "OW_Reference.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"

/**
 * This is the interface implemented by method providers.  These providers are
 * used to provide implementation for all methods of CIM classes.
 * All calls to the derived provider will be serialized so that providers need
 * not worry about multi-threading issues.
 */
class OW_MethodProviderIFC: public OW_ProviderBaseIFC
{
public:
	virtual ~OW_MethodProviderIFC() {}

	/**
	 * The CIMOM calls this method when the method specified in the parameters
	 * is to be invoked.
	 *
	 * @param cop Contains the path to the instance whose method must be
	 * 	invoked.
	 * @param methodName The name of the method.
	 * @param inParams An array of OW_CIMValues which are the input parameters
	 * 	for this method.
	 * @param outParams An array of OW_CIMValues which are the output
	 * 	parameters for this method.
	 *
	 * @returns OW_CIMValue - The return value of the method.  Must be a
	 *    valid OW_CIMValue.
	 *
	 * @throws OW_CIMException
	 */
	virtual OW_CIMValue invokeMethod(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& path,
			const OW_String& methodName,
			const OW_CIMParamValueArray& in,
			OW_CIMParamValueArray& out ) = 0;
};

typedef OW_Reference< OW_MethodProviderIFC > OW_MethodProviderIFCRef;

#endif

