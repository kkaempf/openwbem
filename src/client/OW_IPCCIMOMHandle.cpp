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
#include "OW_IPCCIMOMHandle.hpp"
#include "OW_BinIfcIO.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_IOException.hpp"
#include "OW_URL.hpp"

#include <iostream>
using std::ostream;
using std::iostream;
using std::istream;

//////////////////////////////////////////////////////////////////////////////
OW_IPCCIMOMHandle::OW_IPCCIMOMHandle(const OW_String& url)
	: OW_CIMOMHandle()
	, m_conn()
	, m_url(url)
	, m_authenticated(false)
	, m_loginCB()
{
	m_conn.connect();
}

//////////////////////////////////////////////////////////////////////////////
OW_IPCCIMOMHandle::~OW_IPCCIMOMHandle()
{
	try
	{
		OW_BinIfcIO::write(m_conn.getOutputStream(),
			(OW_Int32) OW_IPC_CLOSECONN);
		m_conn.disconnect();
	}
	catch(...)
	{
		// Ignore?
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_IPCCIMOMHandle::authenticate(const OW_String& userName,
	const OW_String& passWord, OW_Bool throwIfFailure)
{
	OW_String details;
	return authenticate(userName, passWord, details, throwIfFailure);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_IPCCIMOMHandle::authenticate(const OW_String& userName,
	const OW_String& info, OW_String& details, OW_Bool throwIfFailure)
{
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_AUTHENTICATE, OW_Bool(true));
	OW_BinIfcIO::writeString(ostrm, userName);
	OW_BinIfcIO::writeString(ostrm, info);

	checkError();
	std::istream& istrm = m_conn.getInputStream();
	OW_Bool authenticated = OW_BinIfcIO::readBool(istrm);
	details = OW_BinIfcIO::readString(istrm);
	if(!authenticated && throwIfFailure)
	{
		OW_THROWCIMMSG(OW_CIMException::ACCESS_DENIED,
			format("Authentication failed: %1", details).c_str());
	}

	return authenticated;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCCIMOMHandle::createNameSpace(const OW_CIMNameSpace& ns)
{
	authenticateIfNeeded();
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
OW_IPCCIMOMHandle::deleteNameSpace(const OW_CIMNameSpace& ns)
{
	authenticateIfNeeded();
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
OW_IPCCIMOMHandle::enumNameSpaceAux(const OW_CIMObjectPath& path,
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
OW_IPCCIMOMHandle::enumNameSpace(const OW_CIMNameSpace& ns,
	OW_Bool deep)
{
	authenticateIfNeeded();
	OW_CIMObjectPath cop(OW_CIMClass::NAMESPACECLASS, ns.getNameSpace());
	OW_StringArray rval;
	rval.push_back(ns.getNameSpace());
	enumNameSpaceAux(cop, rval, deep);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCCIMOMHandle::deleteClass(const OW_CIMObjectPath& path)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_DELETECLS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	checkError();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCCIMOMHandle::deleteInstance(const OW_CIMObjectPath& path)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_DELETEINST, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	checkError();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCCIMOMHandle::deleteQualifierType(const OW_CIMObjectPath& path)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_DELETEQUAL, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	checkError();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_IPCCIMOMHandle::enumClassNames(const OW_CIMObjectPath& path,
	OW_Bool deep)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_ENUMCLSNAMES, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeBool(ostrm, deep);
	checkError();
	return readObjectPathEnum();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_IPCCIMOMHandle::enumClass(const OW_CIMObjectPath& path, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_ENUMCLSS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeBool(ostrm, deep);
	OW_BinIfcIO::writeBool(ostrm, localOnly);
	OW_BinIfcIO::writeBool(ostrm, includeQualifiers);
	OW_BinIfcIO::writeBool(ostrm, includeClassOrigin);

	checkError();

	return readClassEnum();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_IPCCIMOMHandle::enumInstanceNames(const OW_CIMObjectPath& path,
	OW_Bool deep)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_ENUMINSTNAMES, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeBool(ostrm, deep);
	checkError();
	return readObjectPathEnum();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_IPCCIMOMHandle::enumInstances(const OW_CIMObjectPath& path, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_ENUMINSTS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeBool(ostrm, deep);
	OW_BinIfcIO::writeBool(ostrm, localOnly);
	OW_BinIfcIO::writeBool(ostrm, includeQualifiers);
	OW_BinIfcIO::writeBool(ostrm, includeClassOrigin);
	OW_Bool nullPropList = (propertyList == 0);
	OW_BinIfcIO::writeBool(ostrm, nullPropList);
	if(!nullPropList)
	{
		OW_BinIfcIO::writeStringArray(ostrm, *propertyList);
	}

	checkError();
	return readInstanceEnum();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierTypeEnumeration
OW_IPCCIMOMHandle::enumQualifierTypes(const OW_CIMObjectPath& path)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_ENUMQUALS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);

	checkError();

	std::istream& istrm = m_conn.getInputStream();
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_QUALENUM);
	OW_CIMQualifierTypeEnumeration en;
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size, OW_Bool(true));
	if(size == -1)
	{
		OW_String fname = OW_BinIfcIO::readString(istrm);
		en = OW_CIMQualifierTypeEnumeration(fname);
	}
	else
	{
		while(size)
		{
			en.addElement(OW_BinIfcIO::readQual(istrm));
			size--;
		}
	}

	return en;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_IPCCIMOMHandle::getClass(const OW_CIMObjectPath& path,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
    const OW_StringArray* propertyList)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_GETCLS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeBool(ostrm, localOnly);
	OW_BinIfcIO::writeBool(ostrm, includeQualifiers);
	OW_BinIfcIO::writeBool(ostrm, includeClassOrigin);
	OW_Bool nullPropertyList = (propertyList == 0);
	OW_BinIfcIO::writeBool(ostrm, nullPropertyList);
	if(!nullPropertyList)
	{
		OW_BinIfcIO::writeStringArray(ostrm, *propertyList);
	}

	checkError();
	return OW_BinIfcIO::readClass(m_conn.getInputStream());
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_IPCCIMOMHandle::getInstance(const OW_CIMObjectPath& path,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_GETINST, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeBool(ostrm, localOnly);
	OW_BinIfcIO::writeBool(ostrm, includeQualifiers);
	OW_BinIfcIO::writeBool(ostrm, includeClassOrigin);
	OW_Bool nullPropertyList = (propertyList == 0);
	OW_BinIfcIO::writeBool(ostrm, nullPropertyList);
	if(!nullPropertyList)
	{
		OW_BinIfcIO::writeStringArray(ostrm, *propertyList);
	}

	checkError();
	return OW_BinIfcIO::readInstance(m_conn.getInputStream());
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_IPCCIMOMHandle::invokeMethod(const OW_CIMObjectPath& name,
	const OW_String& methodName, const OW_CIMValueArray& inParams,
	OW_CIMValueArray& outParams)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_INVMETH, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, name);
	OW_BinIfcIO::writeString(ostrm, methodName);

	OW_BinIfcIO::write(ostrm, OW_Int32(OW_BINSIG_VALUEARRAY), OW_Bool(true));
	OW_BinIfcIO::write(ostrm, OW_Int32(inParams.size()), OW_Bool(true));
	for(size_t i = 0; i < inParams.size(); i++)
	{
		OW_BinIfcIO::writeValue(ostrm, inParams[i]);
	}

	checkError();

	std::istream& istrm = m_conn.getInputStream();
	OW_CIMValue cv;
	OW_Bool isrv(OW_BinIfcIO::readBool(istrm));
	if(isrv)
	{
		cv = OW_BinIfcIO::readValue(istrm);
	}

	outParams.clear();
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_VALUEARRAY);
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size, OW_Bool(true));
	while(size)
	{
		outParams.append(OW_BinIfcIO::readValue(istrm));
		size--;
	}

	return cv;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_IPCCIMOMHandle::getQualifierType(const OW_CIMObjectPath& path)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_GETQUAL, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	checkError();
	return OW_BinIfcIO::readQual(m_conn.getInputStream());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCCIMOMHandle::setQualifierType(const OW_CIMObjectPath& path,
	const OW_CIMQualifierType& qt)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_SETQUAL, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeQual(ostrm, qt);
	checkError();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCCIMOMHandle::setClass(const OW_CIMObjectPath& path,
	const OW_CIMClass& cc)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_SETCLS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeClass(ostrm, cc);
	checkError();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCCIMOMHandle::createClass(const OW_CIMObjectPath& path,
	const OW_CIMClass& cc)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_CREATECLS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeClass(ostrm, cc);
	checkError();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCCIMOMHandle::setInstance(const OW_CIMObjectPath& path,
	const OW_CIMInstance& ci)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_SETINST, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeInstance(ostrm, ci);
	checkError();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_IPCCIMOMHandle::createInstance(const OW_CIMObjectPath& path,
	const OW_CIMInstance& ci)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_CREATEINST, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeInstance(ostrm, ci);
	checkError();
	return OW_BinIfcIO::readObjectPath(m_conn.getInputStream());
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_IPCCIMOMHandle::getProperty(const OW_CIMObjectPath& path,
	const OW_String& propName)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_GETPROP, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeString(ostrm, propName);

	checkError();

	std::istream& istrm = m_conn.getInputStream();
	OW_CIMValue cv;
	OW_Bool isValue(OW_BinIfcIO::readBool(istrm));
	if(isValue)
	{
		cv = OW_BinIfcIO::readValue(istrm);
	}

	return cv;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCCIMOMHandle::setProperty(const OW_CIMObjectPath& path,
	const OW_String& propName, const OW_CIMValue& cv)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_SETPROP, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeString(ostrm, propName);
	OW_Bool isValue = (cv) ? true : false;
	OW_BinIfcIO::writeBool(ostrm, isValue);
	if(isValue)
	{
		OW_BinIfcIO::writeValue(ostrm, cv);
	}

	checkError();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_IPCCIMOMHandle::associatorNames(const OW_CIMObjectPath& path,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_ASSOCNAMES, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeString(ostrm, assocClass);
	OW_BinIfcIO::writeString(ostrm, resultClass);
	OW_BinIfcIO::writeString(ostrm, role);
	OW_BinIfcIO::writeString(ostrm, resultRole);

	checkError();
	return readObjectPathEnum();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_IPCCIMOMHandle::associators(const OW_CIMObjectPath& path,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_ASSOCIATORS, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeString(ostrm, assocClass);
	OW_BinIfcIO::writeString(ostrm, resultClass);
	OW_BinIfcIO::writeString(ostrm, role);
	OW_BinIfcIO::writeString(ostrm, resultRole);
	OW_BinIfcIO::writeBool(ostrm, includeQualifiers);
	OW_BinIfcIO::writeBool(ostrm, includeClassOrigin);
	OW_Bool nullPropertyList = (propertyList == 0);
	OW_BinIfcIO::writeBool(ostrm, nullPropertyList);
	if(!nullPropertyList)
	{
		OW_BinIfcIO::writeStringArray(ostrm, *propertyList);
	}

	checkError();
	return readInstanceEnum();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_IPCCIMOMHandle::referenceNames(const OW_CIMObjectPath& path,
	const OW_String& resultClass, const OW_String& role)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_REFNAMES, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeString(ostrm, resultClass);
	OW_BinIfcIO::writeString(ostrm, role);
	checkError();
	return readObjectPathEnum();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_IPCCIMOMHandle::references(const OW_CIMObjectPath& path,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_REFERENCES, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, path);
	OW_BinIfcIO::writeString(ostrm, resultClass);
	OW_BinIfcIO::writeString(ostrm, role);
	OW_BinIfcIO::writeBool(ostrm, includeQualifiers);
	OW_BinIfcIO::writeBool(ostrm, includeClassOrigin);
	OW_Bool nullPropertyList = (propertyList == 0);
	OW_BinIfcIO::writeBool(ostrm, nullPropertyList);
	if(!nullPropertyList)
	{
		OW_BinIfcIO::writeStringArray(ostrm, *propertyList);
	}

	checkError();
	return readInstanceEnum();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceArray
OW_IPCCIMOMHandle::execQuery(const OW_CIMNameSpace& path,
	const OW_String& query, int wqlLevel)
{
	return execQuery(path, query, OW_String("WQL") + OW_String(wqlLevel));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceArray
OW_IPCCIMOMHandle::execQuery(const OW_CIMNameSpace& ns,
	const OW_String& query, const OW_String& queryLanguage)
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_EXECQUERY, OW_Bool(true));
	OW_BinIfcIO::writeNameSpace(ostrm, ns);
	OW_BinIfcIO::writeString(ostrm, query);
	OW_BinIfcIO::writeString(ostrm, queryLanguage);

	checkError();

	std::istream& istrm = m_conn.getInputStream();
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_INSTARRAY);
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size, OW_Bool(true));
	OW_CIMInstanceArray ira;
	while(size)
	{
		ira.append(OW_BinIfcIO::readInstance(istrm));
		size--;
	}

	return ira;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMFeatures
OW_IPCCIMOMHandle::getServerFeatures()
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_GETSVRFEATURES, OW_Bool(true));

	checkError();

	OW_CIMFeatures f;
	OW_Int32 val;

	std::istream& istrm = m_conn.getInputStream();
	OW_BinIfcIO::read(istrm, val, OW_Bool(true));
	f.cimProduct = (OW_CIMFeatures::OW_CIMProduct_t) val;
	f.extURL = OW_BinIfcIO::readString(istrm);
	f.supportedGroups = OW_BinIfcIO::readStringArray(istrm);
	f.supportsBatch = OW_BinIfcIO::readBool(istrm);
	f.supportedQueryLanguages = OW_BinIfcIO::readStringArray(istrm);
	f.validation = OW_BinIfcIO::readString(istrm);
	f.cimom = OW_BinIfcIO::readString(istrm);
	f.protocolVersion = OW_BinIfcIO::readString(istrm);

	return f;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCCIMOMHandle::checkError()
{
	std::istream& istrm = m_conn.getInputStream();
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
				m_conn.disconnect();
				OW_THROW(OW_IOException,
					"Unexpected value received from server. disconnecting");
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_IPCCIMOMHandle::readObjectPathEnum()
{
	std::istream& istrm = m_conn.getInputStream();
	OW_CIMObjectPathEnumeration en;
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_OPENUM);
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size, OW_Bool(true));

	if(size == -1)
	{
		OW_String fname = OW_BinIfcIO::readString(istrm);
		en = OW_CIMObjectPathEnumeration(fname);
	}
	else
	{
		while(size)
		{
			en.addElement(OW_BinIfcIO::readObjectPath(istrm));
			size--;
		}
	}

	return en;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_IPCCIMOMHandle::readInstanceEnum()
{
	std::istream& istrm = m_conn.getInputStream();
	OW_CIMInstanceEnumeration en;
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_INSTENUM);
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size, OW_Bool(true));
	if(size == -1)
	{
		OW_String fname = OW_BinIfcIO::readString(istrm);
		en = OW_CIMInstanceEnumeration(fname);
	}
	else
	{
		while(size)
		{
			en.addElement(OW_BinIfcIO::readInstance(istrm));
			size--;
		}
	}

	return en;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_IPCCIMOMHandle::readClassEnum()
{
	std::istream& istrm = m_conn.getInputStream();
	OW_CIMClassEnumeration en;
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_CLSENUM);
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size, OW_Bool(true));
	if(size == -1)
	{
		OW_String fname = OW_BinIfcIO::readString(istrm);
		en = OW_CIMClassEnumeration(fname);
	}
	else
	{
		while(size)
		{
			en.addElement(OW_BinIfcIO::readClass(istrm));
			size--;
		}
	}

	return en;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCCIMOMHandle::authenticateIfNeeded()
{
	if (!m_authenticated)
	{
		int trycount = 0;
		OW_URL url(m_url);
		while (trycount < 3)
		{
			OW_String details;
			if(!authenticate(url.username, url.password, details))
			{
				if (!m_loginCB)
				{
					OW_THROWCIMMSG(OW_CIMException::ACCESS_DENIED,
						format("Authentication failed: %1", details).c_str());
				}
				else
				{
					if (!m_loginCB->getCredentials(m_url, url.username, url.password))
					{
						OW_THROWCIMMSG(OW_CIMException::ACCESS_DENIED,
							"Authentication failed: Unable to get valid credentials");
					}
				}
			}
			else
			{
				// sucessfully authenticated
				m_authenticated = true;
				return;
			}
		}
		// couldn't authenticate after 3 tries
		OW_THROWCIMMSG(OW_CIMException::ACCESS_DENIED,
			"Authentication failed: Unable to get valid credentials");
	}
}

