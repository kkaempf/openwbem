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
#include "OW_NameSpaceProvider.hpp"
#include "OW_Exception.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_CIMClass.hpp"

//////////////////////////////////////////////////////////////////////////////
OW_NameSpaceProvider::~OW_NameSpaceProvider()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_NameSpaceProvider::deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop)
{
	OW_CIMPropertyArray pra = cop.getKeys();
	if(pra.size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE,
			"root namespace cannot be deleted");
	}

	OW_CIMNameSpace ns = cop.getFullNameSpace();
	OW_String nsName;

	for(size_t i = 0; i < pra.size(); i++)
	{
		if(pra[i].getName().equalsIgnoreCase(OW_CIMProperty::NAME_PROPERTY))
		{
			OW_CIMValue cv = pra[i].getValue();
			if(!cv)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED,
					"Name property doesn't have a value");
			}
			cv.get(nsName);
			break;
		}
	}

	if(nsName.length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"Name property contains an empty value");
	}

	nsName = ns.getNameSpace() + "/" + nsName;
	ns.setNameSpace(nsName);

	env->getCIMOMHandle()->deleteNameSpace(ns);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_NameSpaceProvider::enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_Bool& deep,
		const OW_CIMClass& cimClass)
{
	OW_String className = cimClass.getName();
	OW_CIMObjectPathEnumeration openum;
	OW_CIMInstanceEnumeration ienum = enumInstances(env, cop, deep, cimClass, false);

	while(ienum.hasMoreElements())
	{
		OW_CIMInstance ci = ienum.nextElement();
		openum.addElement(OW_CIMObjectPath(className, ci.getKeyValuePairs()));
	}

	return openum;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_NameSpaceProvider::enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_Bool& /*deep*/,
		const OW_CIMClass& cimClass,
		const OW_Bool& /*localOnly*/)
{
	OW_CIMInstanceEnumeration cienum;
	OW_StringArray nsra = env->getCIMOMHandle()->enumNameSpace(
		cop.getFullNameSpace(), false);

	for(size_t i = 0; i < nsra.size(); i++)
	{
		OW_String nameSpaceName = nsra[i];
		int ndx = nameSpaceName.lastIndexOf('/');
		if(ndx != -1)
		{
			nameSpaceName = nameSpaceName.substring(ndx+1);
		}

		OW_CIMInstance ci = cimClass.newInstance();
		ci.setProperty("Name", OW_CIMValue(nameSpaceName));
		cienum.addElement(ci);
	}

	return cienum;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_NameSpaceProvider::getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_CIMClass& cimClass,
		const OW_Bool& /*localOnly*/)
{
	OW_CIMProperty cp = cop.getKey(OW_CIMProperty::NAME_PROPERTY);
	OW_CIMValue nsVal;
	if (cp)
	{
		nsVal = cp.getValue();
	}

	if (nsVal && nsVal.getType() == OW_CIMDataType::STRING)
	{
		OW_CIMInstanceEnumeration cie = enumInstances(env,cop,false,cimClass,false);
		
		while (cie.hasMoreElements())
		{
			OW_CIMInstance ci = cie.nextElement();
			if (ci)
			{
				OW_CIMProperty cp = ci.getProperty(OW_CIMProperty::NAME_PROPERTY);
				if (cp)
				{
					OW_CIMValue v = cp.getValue();
					if (v && v.getType() == OW_CIMDataType::STRING)
					{
						OW_String vval;
						v.get(vval);
						OW_String nsValStr;
						nsVal.get(nsValStr);
						if (vval.equalsIgnoreCase(nsValStr))
						{
							return ci;
						}
					}
				}
			}
		}
	}

	OW_THROWCIM(OW_CIMException::NOT_FOUND);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_NameSpaceProvider::createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_CIMInstance& cimInstance)
{
	OW_CIMProperty cp = cimInstance.getProperty(OW_CIMProperty::NAME_PROPERTY);
	if(!cp)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE,
			"Instance \"Name\" property is not set");
	}

	OW_CIMValue cv = cp.getValue();
	OW_String newName;
	cv.get(newName);

	newName = newName.substring(newName.indexOf('=') + 1);
	OW_String newNameSpace = cop.getNameSpace();
	newNameSpace += "/";
	newNameSpace += newName;

	env->getLogger()->logDebug(format("OW_NameSpaceProvider::createInstance calling"
			" createNameSpace with %1", newNameSpace));

	env->getCIMOMHandle()->createNameSpace(OW_CIMNameSpace(OW_CIMUrl(), newNameSpace));

	return cop;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_NameSpaceProvider::modifyInstance(
		const OW_ProviderEnvironmentIFCRef& /*env*/,
		const OW_CIMObjectPath& /*cop*/,
		const OW_CIMInstance& /*cimInstance*/)
{
	OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_NameSpaceProvider::initialize(const OW_ProviderEnvironmentIFCRef& env)
{
	env->getLogger()->logDebug("OW_NameSpaceProvider initialize called");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_NameSpaceProvider::cleanup()
{
}


