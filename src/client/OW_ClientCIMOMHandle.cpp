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
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMException.hpp"
#include "OW_String.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_Format.hpp"

//////////////////////////////////////////////////////////////////////////////
void
OW_ClientCIMOMHandle::createNameSpace(const OW_String& ns)
{
	int index = ns.lastIndexOf('/');

	if(index==-1)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
			"A Namespace must only be created in an existing Namespace");
	}

	OW_String parentPath = ns.substring(0, index);
	OW_String newNameSpace = ns.substring(index + 1);

	OW_CIMClass cimClass = getClass(parentPath, OW_CIMClass::NAMESPACECLASS, false);

	OW_CIMInstance cimInstance = cimClass.newInstance();
	OW_CIMValue cv(newNameSpace);
	cimInstance.setProperty("Name", cv);

	OW_CIMObjectPath cimPath(OW_CIMClass::NAMESPACECLASS,
		cimInstance.getKeyValuePairs());

	cimPath.setNameSpace(parentPath);
	createInstance(cimPath, cimInstance);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ClientCIMOMHandle::deleteNameSpace(const OW_String& ns)
{
	OW_String parentPath;
	int index = ns.lastIndexOf('/');

	if(index == -1)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
			"A Namespace must only be created in an existing Namespace");
	}

	parentPath = ns.substring(0,index);
	OW_String newNameSpace = ns.substring(index + 1);

	OW_CIMPropertyArray v;
	OW_CIMValue cv(newNameSpace);
	OW_CIMProperty cp("Name", cv);
	cp.setDataType(OW_CIMDataType(OW_CIMDataType::STRING));
	v.push_back(cp);

	OW_CIMObjectPath path(OW_CIMClass::NAMESPACECLASS, v);
	deleteInstance(parentPath, path);
}

//////////////////////////////////////////////////////////////////////////////
static void
enumNameSpaceAux(OW_CIMOMHandleIFC* hdl,
	const OW_String& ns,
	OW_StringResultHandlerIFC& result, OW_Bool deep)
{
	// can't use the callback version of enumInstances, because the recursion
	// throws a wrench in the works.  Each CIM Method call has to finish
	// before another one can begin.
	OW_CIMInstanceEnumeration en = hdl->enumInstancesE(ns,
		OW_CIMClass::NAMESPACECLASS, false, true);
	while (en.hasMoreElements())
	{
		OW_CIMInstance i = en.nextElement();
		OW_CIMProperty nameProp;

		OW_CIMPropertyArray keys = i.getKeyValuePairs();
		if(keys.size() == 1)
		{
			nameProp = keys[0];
		}
		else
		{
			for(size_t i = 0; i < keys.size(); i++)
			{
				if(keys[i].getName().equalsIgnoreCase("Name"))
				{
					nameProp = keys[i];
					break;
				}
			}

			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Name of namespace not found");
		}

		OW_String tmp;
		nameProp.getValue().get(tmp);
		result.handle(ns + "/" + tmp);
		if(deep)
		{
			enumNameSpaceAux(hdl, ns + "/" + tmp, result, deep);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ClientCIMOMHandle::enumNameSpace(const OW_String& ns,
	OW_StringResultHandlerIFC &result, OW_Bool deep)
{
	result.handle(ns);
	enumNameSpaceAux(this, ns, result, deep);
}
				
