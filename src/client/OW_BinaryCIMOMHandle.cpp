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
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_Format.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_BinIfcIO.hpp"
#include "OW_IOException.hpp"

#include <iostream>

using std::ostream;
using std::iostream;
using std::istream;


//////////////////////////////////////////////////////////////////////////////
OW_BinaryCIMOMHandle::OW_BinaryCIMOMHandle(OW_Reference<OW_CIMProtocol> prot)
: OW_CIMOMHandle(), m_protocol(prot)
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::createNameSpace(const OW_CIMNameSpace& ns)
{
	OW_String nameSpace = ns.getNameSpace();
	int index = nameSpace.lastIndexOf('/');

	if(index==-1)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
			"A Namespace must only be created in an existing Namespace");
	}

	OW_String parentPath = nameSpace.substring(0, index);
	OW_String newNameSpace = nameSpace.substring(index + 1);
	OW_CIMObjectPath path(OW_CIMClass::NAMESPACECLASS, parentPath);

	OW_CIMClass cimClass = getClass(path, false);
	if(!cimClass)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
			format("Could not find internal class %1",
				OW_CIMClass::NAMESPACECLASS).c_str());
	}

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
OW_BinaryCIMOMHandle::deleteNameSpace(const OW_CIMNameSpace& ns)
{
	OW_String parentPath;
	OW_String nameSpace = ns.getNameSpace();
	int index = nameSpace.lastIndexOf('/');

	if(index == -1)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
			"A Namespace must only be created in an existing Namespace");
	}

	parentPath = nameSpace.substring(0,index);
	OW_String newNameSpace = nameSpace.substring(index + 1);

	OW_CIMPropertyArray v;
	OW_CIMValue cv(newNameSpace);
	OW_CIMProperty cp("Name", cv);
	cp.setDataType(OW_CIMDataType(OW_CIMDataType::STRING));
	v.push_back(cp);

	OW_CIMObjectPath path(OW_CIMClass::NAMESPACECLASS, v);
	path.setNameSpace(parentPath);
	deleteInstance(path);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::enumNameSpaceAux(const OW_CIMObjectPath& path,
	OW_StringArray& rval, OW_Bool deep)
{
	OW_CIMInstanceEnumeration e = enumInstances(path, deep);
	while(e.hasMoreElements())
	{
		OW_CIMProperty nameProp;

		OW_CIMPropertyArray keys = e.nextElement().getKeyValuePairs();
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
		rval.push_back(path.getNameSpace() + "/" + tmp);
		if(deep)
		{
			OW_CIMObjectPath newObjPath(OW_CIMClass::NAMESPACECLASS,
				path.getNameSpace()+ "/" + tmp);
			enumNameSpaceAux(newObjPath, rval, deep);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_BinaryCIMOMHandle::enumNameSpace(const OW_CIMNameSpace& ns, OW_Bool deep)
{
	OW_CIMObjectPath cop(OW_CIMClass::NAMESPACECLASS, ns.getNameSpace());
	OW_StringArray rval;
	rval.push_back(ns.getNameSpace());
	enumNameSpaceAux(cop, rval, deep);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::deleteClass(const OW_CIMObjectPath& path)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_DELETECLS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	
	checkError(m_protocol->sendRequest(strm, "DeleteClass", path.getNameSpace()));
}


//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::deleteInstance(const OW_CIMObjectPath& path)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_DELETEINST, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);

	checkError(m_protocol->sendRequest(strm, "DeleteInstance", path.getNameSpace()));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::deleteQualifierType(const OW_CIMObjectPath& path)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_DELETEQUAL, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	checkError(m_protocol->sendRequest(strm, "DeleteQualifier", path.getNameSpace()));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_BinaryCIMOMHandle::enumClassNames(const OW_CIMObjectPath& path,
		OW_Bool deep)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_ENUMCLSNAMES, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeBool(strm, deep);
	std::istream& in = m_protocol->sendRequest(strm, "EnumerateClassNames", path.getNameSpace());
	checkError(in);
	return OW_BinIfcIO::readObjectPathEnum(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_BinaryCIMOMHandle::enumClass(const OW_CIMObjectPath& path, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_ENUMCLSS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeBool(strm, deep);
	OW_BinIfcIO::writeBool(strm, localOnly);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);

	std::istream& in = m_protocol->sendRequest(strm, "EnumerateClasses", path.getNameSpace());
	checkError(in);

	return OW_BinIfcIO::readClassEnum(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_BinaryCIMOMHandle::enumInstanceNames(const OW_CIMObjectPath& path,
	OW_Bool deep)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_ENUMINSTNAMES, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeBool(strm, deep);

	std::istream& in = m_protocol->sendRequest(strm, "EnumerateInstanceNames", path.getNameSpace());
	checkError(in);
	return OW_BinIfcIO::readObjectPathEnum(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_BinaryCIMOMHandle::enumInstances(const OW_CIMObjectPath& path, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_ENUMINSTS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeBool(strm, deep);
	OW_BinIfcIO::writeBool(strm, localOnly);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_Bool nullPropList = (propertyList == 0);
	OW_BinIfcIO::writeBool(strm, nullPropList);
	if(!nullPropList)
	{
		OW_BinIfcIO::writeStringArray(strm, *propertyList);
	}

	std::istream& in = m_protocol->sendRequest(strm, "EnumerateInstances", path.getNameSpace());
	checkError(in);
	return OW_BinIfcIO::readInstanceEnum(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierTypeEnumeration
OW_BinaryCIMOMHandle::enumQualifierTypes(const OW_CIMObjectPath& path)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_ENUMQUALS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);

	std::istream& in = m_protocol->sendRequest(strm, "EnumerateQualifiers", path.getNameSpace());
	checkError(in);

	return OW_BinIfcIO::readQualifierTypeEnum(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_BinaryCIMOMHandle::getClass(const OW_CIMObjectPath& path, OW_Bool localOnly,
    OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_GETCLS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeBool(strm, localOnly);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_Bool nullPropertyList = (propertyList == 0);
	OW_BinIfcIO::writeBool(strm, nullPropertyList);
	if(!nullPropertyList)
	{
		OW_BinIfcIO::writeStringArray(strm, *propertyList);
	}

	std::istream& in = m_protocol->sendRequest(strm, "GetClass", path.getNameSpace());
	checkError(in);

	return OW_BinIfcIO::readClass(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_BinaryCIMOMHandle::getInstance(const OW_CIMObjectPath& path,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_GETINST, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeBool(strm, localOnly);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_Bool nullPropertyList = (propertyList == 0);
	OW_BinIfcIO::writeBool(strm, nullPropertyList);
	if(!nullPropertyList)
	{
		OW_BinIfcIO::writeStringArray(strm, *propertyList);
	}

	std::istream& in = m_protocol->sendRequest(strm, "GetClass", path.getNameSpace());
	checkError(in);

	return OW_BinIfcIO::readInstance(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_BinaryCIMOMHandle::invokeMethod(const OW_CIMObjectPath& name,
											  const OW_String& methodName,
											  const OW_CIMValueArray& inParams,
											  OW_CIMValueArray& outParams)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_INVMETH, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, name);
	OW_BinIfcIO::writeString(strm, methodName);

	OW_BinIfcIO::write(strm, OW_Int32(OW_BINSIG_VALUEARRAY), OW_Bool(true));
	OW_BinIfcIO::write(strm, OW_Int32(inParams.size()), OW_Bool(true));
	for(size_t i = 0; i < inParams.size(); i++)
	{
		OW_BinIfcIO::writeValue(strm, inParams[i]);
	}

	std::istream& in = m_protocol->sendRequest(strm, methodName, name.getNameSpace());
	checkError(in);

	OW_CIMValue cv;
	OW_Bool isrv(OW_BinIfcIO::readBool(in));
	if(isrv)
	{
		cv = OW_BinIfcIO::readValue(in);
	}

	outParams.clear();
	OW_BinIfcIO::verifySignature(in, OW_BINSIG_VALUEARRAY);
	OW_Int32 size;
	OW_BinIfcIO::read(in, size, OW_Bool(true));
	while(size)
	{
		outParams.append(OW_BinIfcIO::readValue(in));
		size--;
	}

	return cv;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_BinaryCIMOMHandle::getQualifierType(const OW_CIMObjectPath& path)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_GETQUAL, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	
	std::istream& in = m_protocol->sendRequest(strm, "GetQualifier", path.getNameSpace());
	checkError(in);

	return OW_BinIfcIO::readQual(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::setQualifierType(const OW_CIMObjectPath& path,
								   		const OW_CIMQualifierType& qt)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_SETQUAL, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeQual(strm, qt);

	std::istream& in = m_protocol->sendRequest(strm, "SetQualifier", path.getNameSpace());
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::setClass(const OW_CIMObjectPath& path,
										 const OW_CIMClass& cc)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_SETCLS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeClass(strm, cc);

	std::istream& in = m_protocol->sendRequest(strm, "ModifyClass", path.getNameSpace());
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::createClass(const OW_CIMObjectPath& path,
											 const OW_CIMClass& cc)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_CREATECLS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeClass(strm, cc);

	std::istream& in = m_protocol->sendRequest(strm, "CreateClass", path.getNameSpace());
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::setInstance(const OW_CIMObjectPath& path,
											 const OW_CIMInstance& ci)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_SETINST, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeInstance(strm, ci);
	
	std::istream& in = m_protocol->sendRequest(strm, "ModifyInstance", path.getNameSpace());
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_BinaryCIMOMHandle::createInstance(const OW_CIMObjectPath& path,
	const OW_CIMInstance& ci)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_CREATEINST, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeInstance(strm, ci);
	
	std::istream& in = m_protocol->sendRequest(strm, "CreateInstance", path.getNameSpace());
	checkError(in);
	return OW_BinIfcIO::readObjectPath(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_BinaryCIMOMHandle::getProperty(const OW_CIMObjectPath& path,
											 const OW_String& propName)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_GETPROP, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, propName);

	std::istream& in = m_protocol->sendRequest(strm, "GetProperty", path.getNameSpace());
	checkError(in);

	OW_CIMValue cv;
	OW_Bool isValue(OW_BinIfcIO::readBool(in));
	if(isValue)
	{
		cv = OW_BinIfcIO::readValue(in);
	}

	return cv;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::setProperty(const OW_CIMObjectPath& path,
											 const OW_String& propName,
											 const OW_CIMValue& cv)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_SETPROP, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, propName);
	OW_Bool isValue = (cv) ? true : false;
	OW_BinIfcIO::writeBool(strm, isValue);
	if(isValue)
	{
		OW_BinIfcIO::writeValue(strm, cv);
	}

	std::istream& in = m_protocol->sendRequest(strm, "SetProperty", path.getNameSpace());
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_BinaryCIMOMHandle::associatorNames(const OW_CIMObjectPath& path,
				const OW_String& assocClass, const OW_String& resultClass,
				const OW_String& role, const OW_String& resultRole)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_ASSOCNAMES, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, assocClass);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	OW_BinIfcIO::writeString(strm, resultRole);

	std::istream& in = m_protocol->sendRequest(strm, "AssociatorNames", path.getNameSpace());
	checkError(in);

	return OW_BinIfcIO::readObjectPathEnum(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_BinaryCIMOMHandle::associators(const OW_CIMObjectPath& path,
		 	 			 const OW_String& assocClass, const OW_String& resultClass,
		 	 			 const OW_String& role, const OW_String& resultRole,
		 	 			 OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		 				 const OW_StringArray* propertyList)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_ASSOCIATORS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, assocClass);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	OW_BinIfcIO::writeString(strm, resultRole);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_Bool nullPropertyList = (propertyList == 0);
	OW_BinIfcIO::writeBool(strm, nullPropertyList);
	if(!nullPropertyList)
	{
		OW_BinIfcIO::writeStringArray(strm, *propertyList);
	}

	std::istream& in = m_protocol->sendRequest(strm, "Associators", path.getNameSpace());
	checkError(in);

	return OW_BinIfcIO::readInstanceEnum(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_BinaryCIMOMHandle::referenceNames(const OW_CIMObjectPath& path,
												 const OW_String& resultClass,
												 const OW_String& role)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_REFNAMES, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	
	std::istream& in = m_protocol->sendRequest(strm, "ReferenceNames", path.getNameSpace());
	checkError(in);

	return OW_BinIfcIO::readObjectPathEnum(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_BinaryCIMOMHandle::references(const OW_CIMObjectPath& path,
		 						const OW_String& resultClass, const OW_String& role,
		 						OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
								const OW_StringArray* propertyList)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_REFERENCES, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_Bool nullPropertyList = (propertyList == 0);
	OW_BinIfcIO::writeBool(strm, nullPropertyList);
	if(!nullPropertyList)
	{
		OW_BinIfcIO::writeStringArray(strm, *propertyList);
	}

	std::istream& in = m_protocol->sendRequest(strm, "ReferenceNames", path.getNameSpace());
	checkError(in);
	
	return OW_BinIfcIO::readInstanceEnum(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceArray
OW_BinaryCIMOMHandle::execQuery(const OW_CIMNameSpace& path,
	const OW_String& query, int wqlLevel)
{
	return execQuery(path, query, OW_String("WQL") + OW_String(wqlLevel));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceArray
OW_BinaryCIMOMHandle::execQuery(const OW_CIMNameSpace& path,
	const OW_String& query, const OW_String& queryLanguage)
{
	OW_Reference<std::iostream> strmRef = m_protocol->getStream();
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(strm, (OW_Int32)OW_BIN_EXECQUERY, OW_Bool(true));
	OW_BinIfcIO::writeNameSpace(strm, path);// writeObjectPath(strm, cop);
	OW_BinIfcIO::writeString(strm, query);
	OW_BinIfcIO::writeString(strm, queryLanguage);

	std::istream& in = m_protocol->sendRequest(strm, "ReferenceNames", path.getNameSpace());
	checkError(in);

	OW_BinIfcIO::verifySignature(in, OW_BINSIG_INSTARRAY);
	OW_Int32 size;
	OW_BinIfcIO::read(in, size, OW_Bool(true));
	OW_CIMInstanceArray ira;
	while(size)
	{
		ira.append(OW_BinIfcIO::readInstance(in));
		size--;
	}

	return ira;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMFeatures
OW_BinaryCIMOMHandle::getServerFeatures()
{
	return m_protocol->getFeatures();
}


//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::checkError(std::istream& istrm)
{
	OW_Int32 rc;
	OW_BinIfcIO::read(istrm, rc, OW_Bool(true));
	if(rc != OW_BIN_OK)
	{
		switch(rc)
		{
			case OW_BIN_ERROR:
			{
				OW_String msg;
				OW_BinIfcIO::read(istrm, msg, OW_Bool(true));
				OW_THROW(OW_IOException, msg.c_str());
			}
			case OW_BIN_EXCEPTION:
			{
				OW_Int32 cimerrno;
				OW_String cimMsg;
				OW_BinIfcIO::read(istrm, cimerrno, OW_Bool(true));
				OW_BinIfcIO::read(istrm, cimMsg, OW_Bool(true));
				OW_THROWCIMMSG(cimerrno, cimMsg.c_str());
			}
			default:
				OW_THROW(OW_IOException,
					"Unexpected value received from server.");
		}
	}
}

