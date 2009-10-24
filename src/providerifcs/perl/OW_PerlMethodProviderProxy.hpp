/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef OW_Perl_METHOD_PROVIDER_PROXY_HPP_
#define OW_Perl_METHOD_PROVIDER_PROXY_HPP_
#include "OW_config.h"
#include "OW_MethodProviderIFC.hpp"
#include "OW_FTABLERef.hpp"

namespace OW_NAMESPACE
{

class PerlMethodProviderProxy : public MethodProviderIFC
{
public:
	PerlMethodProviderProxy(const FTABLERef& f)
		: m_ftable(f)
	{
	}
	virtual ~PerlMethodProviderProxy();
	/**
	 * The CIMOM calls this method when the method specified in the parameters
	 * is to be invoked.
	 *
	 * @param cop Contains the path to the instance whose method must be
	 * 	invoked.
	 * @param methodName The name of the method.
	 * @param inParams An array of CIMValues which are the input parameters
	 * 	for this method.
	 * @param outParams An array of CIMValues which are the output
	 * 	parameters for this method.
	 *
	 * @returns CIMValue - The return value of the method.  Must be a
	 *    valid CIMValue.
	 *
	 * @throws CIMException
	 */
	virtual CIMValue invokeMethod(const ProviderEnvironmentIFCRef &env,
		const String& ns,
		const CIMObjectPath& path,
		const String &methodName,
		const CIMParamValueArray &in, CIMParamValueArray &out);
private:
	FTABLERef m_ftable;
};


} // end namespace OW_NAMESPACE

#endif
