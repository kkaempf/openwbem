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

#include "OW_config.h"
#include "OW_SimpleCppInstanceProviderIFC.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class instanceToObjectPathFilter : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		instanceToObjectPathFilter(
			OW_CIMObjectPathResultHandlerIFC& result_,
			const OW_String& ns_)
		: result(result_)
		, ns(ns_)
		{}

	protected:
		virtual void doHandle(const OW_CIMInstance &i)
		{
			OW_CIMObjectPath cop(i.getClassName(), ns);
			cop.setKeys(i);
			result.handle(cop);
		}
	private:
		OW_CIMObjectPathResultHandlerIFC& result;
		const OW_String& ns;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SimpleCppInstanceProviderIFC::enumInstanceNames(
	const OW_ProviderEnvironmentIFCRef& env,
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_CIMClass& cimClass )
{
	instanceToObjectPathFilter handler(result, ns);
	enumInstances(env, ns, className, handler, cimClass);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SimpleCppInstanceProviderIFC::enumInstances(
	const OW_ProviderEnvironmentIFCRef&,
	const OW_String&,
	const OW_String&,
	OW_CIMInstanceResultHandlerIFC&,
	const OW_CIMClass&)
{
	OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_SimpleCppInstanceProviderIFC::getInstance(
	const OW_ProviderEnvironmentIFCRef&,
	const OW_CIMObjectPath&,
	const OW_CIMClass&)
{
	OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_SimpleCppInstanceProviderIFC::createInstance(
	const OW_ProviderEnvironmentIFCRef&,
	const OW_CIMObjectPath&,
	const OW_CIMInstance&)
{
	OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SimpleCppInstanceProviderIFC::modifyInstance(
	const OW_ProviderEnvironmentIFCRef&,
	const OW_CIMObjectPath&,
	const OW_CIMInstance&)
{
	OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SimpleCppInstanceProviderIFC::deleteInstance(
	const OW_ProviderEnvironmentIFCRef&,
	const OW_String&,
	const OW_CIMObjectPath&)
{
	OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}


