/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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
#include "OW_CIMNameSpaceUtils.hpp"



//////////////////////////////////////////////////////////////////////////////
void
OW_ClientCIMOMHandle::createNameSpace(const OW_String& ns)
{
    OW_CIMNameSpaceUtils::create__Namespace(OW_CIMOMHandleIFCRef(this, true), ns);
    /*
    OW_String ns(prepareNamespace(ns_));

	int index = ns.lastIndexOf('/');

	//if (index==-1)
	//{
	//	OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
	//		"A Namespace must only be created in an existing Namespace");
	//}

	OW_String parentPath = ns.substring(0, index);
	OW_String newNameSpace = ns.substring(index + 1);

	//OW_CIMClass cimClass = getClass(parentPath, 
	//	OW_String(OW_CIMClass::NAMESPACECLASS), false);
	if (!nsClass)
	{
		nsClass = OW_CIMClass("__Namespace");

		OW_CIMProperty cimProp(OW_CIMProperty::NAME_PROPERTY);
		cimProp.setDataType(OW_CIMDataType::STRING);
		cimProp.addQualifier(OW_CIMQualifier::createKeyQualifier());
		nsClass.addProperty(cimProp);
	}

	OW_CIMInstance cimInstance = nsClass.newInstance();
	OW_CIMValue cv(newNameSpace);
	cimInstance.setProperty("Name", cv);

	createInstance(parentPath, cimInstance);
    */
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ClientCIMOMHandle::deleteNameSpace(const OW_String& ns_)
{
    OW_CIMNameSpaceUtils::delete__Namespace(OW_CIMOMHandleIFCRef(this, true), ns_);
    /*
    OW_String ns(prepareNamespace(ns_));

	int index = ns.lastIndexOf('/');

	//if (index == -1)
	//{
	//	OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
	//		"A Namespace must only be created in an existing Namespace");
	//}

	OW_String parentPath = ns.substring(0,index);
	OW_String newNameSpace = ns.substring(index + 1);

	OW_CIMPropertyArray v;
	OW_CIMValue cv(newNameSpace);
	OW_CIMProperty cp("Name", cv);
	cp.setDataType(OW_CIMDataType::STRING);
	v.push_back(cp);

	OW_CIMObjectPath path(OW_CIMClass::NAMESPACECLASS, v);
	deleteInstance(parentPath, path);
    */
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ClientCIMOMHandle::enumNameSpace(const OW_String& ns_,
	OW_StringResultHandlerIFC &result, OW_Bool deep)
{
    OW_CIMNameSpaceUtils::enum__Namespace(OW_CIMOMHandleIFCRef(this, true), ns_, result, deep);
    /*
    OW_String ns(prepareNamespace(ns_));

	result.handle(ns);
	enumNameSpaceAux(this, ns, result, deep);
    */
}


