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
#include "OW_ProviderManager.hpp"
#include "OW_Format.hpp"
#include "OW_ProxyProvider.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_CppIndicationExportProviderIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifier.hpp"

//////////////////////////////////////////////////////////////////////////////
void OW_ProviderManager::init(const OW_ProviderIFCLoaderRef& IFCLoader)
{
	IFCLoader->loadIFCs(m_IFCArray);
	OW_Reference<OW_ProviderIFCBaseIFC> pi(new OW_InternalProviderIFC);
	m_internalIFC = pi.cast_to<OW_InternalProviderIFC>();
	// 0 because there is no shared library.
	m_IFCArray.push_back(OW_ProviderIFCBaseIFCRef(OW_SharedLibraryRef(0), pi));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderManager::addCIMOMProvider(const OW_CppProviderBaseIFCRef& pProv)
{
	m_internalIFC->addCIMOMProvider(pProv);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderManager::addCIMOMProvider(const OW_String& providerNameArg,
	const OW_CppProviderBaseIFCRef& pProv)
{
	m_internalIFC->addCIMOMProvider(providerNameArg, pProv);
}

//////////////////////////////////////////////////////////////////////////////
OW_InstanceProviderIFCRef
OW_ProviderManager::getInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual) const
{
	OW_String provStr;
	OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
	if(theIFC.isNull())
	{
		return OW_InstanceProviderIFCRef(0);
	}

	return theIFC->getInstanceProvider(env, provStr.c_str());
}

//////////////////////////////////////////////////////////////////////////////
OW_MethodProviderIFCRef
OW_ProviderManager::getMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual) const
{
	OW_String provStr;
	OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
	if(theIFC.isNull())
	{
		return OW_MethodProviderIFCRef(0);
	}

	return theIFC->getMethodProvider(env, provStr.c_str());
}

//////////////////////////////////////////////////////////////////////////////
OW_PropertyProviderIFCRef
OW_ProviderManager::getPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual) const
{
	OW_String provStr;
	OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
	if(theIFC.isNull())
	{
		return OW_PropertyProviderIFCRef(0);
	}

	return theIFC->getPropertyProvider(env, provStr.c_str());
}

//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_ProviderManager::getAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual) const
{
	OW_String provStr;
	OW_ProviderIFCBaseIFCRef theIFC = getProviderIFC(env, qual, provStr);
	if(theIFC.isNull())
	{
		return OW_AssociatorProviderIFCRef(0);
	}

	return theIFC->getAssociatorProvider(env, provStr.c_str());
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationExportProviderIFCRefArray
OW_ProviderManager::getIndicationExportProviders(const OW_ProviderEnvironmentIFCRef& env
	)
{
	OW_IndicationExportProviderIFCRefArray rv;

	for(size_t i = 0; i < m_IFCArray.size(); i++)
	{
		OW_IndicationExportProviderIFCRefArray pra =
				m_IFCArray[i]->getIndicationExportProviders(env);
		if(pra.size() > 0)
		{
			rv.appendArray(pra);
		}
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
OW_PolledProviderIFCRefArray
OW_ProviderManager::getPolledProviders(const OW_ProviderEnvironmentIFCRef& env
	)
{
	OW_PolledProviderIFCRefArray rv;

	for(size_t i = 0; i < m_IFCArray.size(); i++)
	{
		OW_PolledProviderIFCRefArray pra =
				m_IFCArray[i]->getPolledProviders(env);

		if(pra.size() > 0)
		{
			rv.appendArray(pra);
		}
	}

	return rv;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderManager::unloadProviders(const OW_ProviderEnvironmentIFCRef& env)
{
	for(size_t i = 0; i < m_IFCArray.size(); i++)
	{
		try
		{
			m_IFCArray[i]->unloadProviders(env);
		}
		catch (const OW_Exception& e)
		{
			env->getLogger()->logError(format("Caught exception while calling unloadProviders for provider interface %1: %2", m_IFCArray[i]->getName(), e));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_ProviderIFCBaseIFCRef
OW_ProviderManager::getProviderIFC(const OW_ProviderEnvironmentIFCRef& env,
	const OW_CIMQualifier& qual,
	OW_String& provStr) const
{
	OW_ProviderIFCBaseIFCRef rref;

	provStr = OW_String();
	if(!qual.getName().equalsIgnoreCase(OW_CIMQualifier::CIM_QUAL_PROVIDER))
	{
		env->getLogger()->logError(format("Provider Manager - invalid provider qualifier: %1",
			qual.getName()));
		return rref;
	}

	OW_CIMValue cv = qual.getValue();
	if(cv.getType() != OW_CIMDataType::STRING || cv.isArray())
	{
		OW_CIMDataType dt(cv.getType());
		if(cv.isArray())
		{
			dt.setToArrayType(cv.getArraySize());
		}

		env->getLogger()->logError(format(
			"Provider Manager - qualifier has incompatible data type: %1",
			dt.toString()));

		return rref;
	}

	OW_String qvstr;
	cv.get(qvstr);
	int ndx = qvstr.indexOf("::");
	if(ndx == -1)
	{
		env->getLogger()->logError(format(
			"Provider Manager - Invalid format for provider string: %1", qvstr));
		return rref;
	}

	OW_String ifcStr = qvstr.substring(0, ndx);
	provStr = qvstr.substring(ndx+2);

	for (size_t i = 0; i < m_IFCArray.size(); i++)
	{
		if(ifcStr.equalsIgnoreCase(m_IFCArray[i]->getName()))
		{
			rref = m_IFCArray[i];
			break;
		}
	}

	if(rref.isNull())
	{
		env->getLogger()->logError(format(
			"Provider Manager - Invalid provider interface identifier: %1",
			ifcStr));
	}

	return rref;
}



