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

static inline void
checkError(std::istream& istrm)
{
	OW_Int32 rc;
	OW_BinIfcIO::read(istrm, rc);
	if(rc != OW_BIN_OK)
	{
		switch(rc)
		{
			case OW_BIN_ERROR:
			{
				OW_String msg;
				OW_BinIfcIO::read(istrm, msg);
				OW_THROW(OW_IOException, msg.c_str());
			}
			case OW_BIN_EXCEPTION:
			{
				OW_Int32 cimerrno;
				OW_String cimMsg;
				OW_BinIfcIO::read(istrm, cimerrno);
				OW_BinIfcIO::read(istrm, cimMsg);
				OW_THROWCIMMSG(OW_CIMException::ErrNoType(cimerrno), cimMsg.c_str());
			}
			default:
				OW_THROW(OW_IOException,
					"Unexpected value received from server.");
		}
	}
}

static inline void
checkError(OW_CIMProtocolIStreamIFCRef istr)
{
	try
	{
		checkError(*istr);
	}
	catch (OW_IOException& e)
	{
		while(*istr) istr->get();
		if (istr->getError().length() == 0)
		{
			throw e;
		}
		else
		{
			OW_TempFileStream error(500);
			istr->getError(error);
			checkError(error);
		}
	}
}



static inline void
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMClass& cc)
{
	cc = OW_BinIfcIO::readClass(*istr);
}
static inline void
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMClassResultHandlerIFC& result)
{
	OW_BinIfcIO::readClassEnum(*istr, result);
}
static inline void
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMInstance& ci)
{
	ci = OW_BinIfcIO::readInstance(*istr);
}
static inline void
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMObjectPath& cop)
{
	cop = OW_BinIfcIO::readObjectPath(*istr);
}
static inline void
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMObjectPathEnumeration& enu)
{
	// TODO: remove me
	enu = OW_BinIfcIO::readObjectPathEnum(*istr);
}
static inline void
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMObjectPathResultHandlerIFC& result)
{
	OW_BinIfcIO::readObjectPathEnum(*istr, result);
}
static inline void
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMInstanceEnumeration& arg)
{
	// TODO: remove me
	arg = OW_BinIfcIO::readInstanceEnum(*istr);
}
static inline void
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMInstanceResultHandlerIFC& result)
{
	OW_BinIfcIO::readInstanceEnum(*istr, result);
}
static inline void
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMQualifierType& arg)
{
	arg = OW_BinIfcIO::readQual(*istr);
}
static inline void
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMQualifierTypeEnumeration& arg)
{
	arg = OW_BinIfcIO::readQualifierTypeEnum(*istr);
}

template<class T>
static inline T
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr)
{
	T rval;
	try
	{
		checkError(istr);
		readCIMObject(istr, rval);
	}
	catch (OW_IOException& e)
	{
		while(*istr) istr->get();
		if (istr->getError().length() == 0)
		{
			throw e;
		}
	}
	while(*istr) istr->get();
	OW_String errorStr = istr->getError();
	if (istr->getError().length() > 0)
	{
		OW_TempFileStream error(500);
		istr->getError(error);
		checkError(error);
	}
	return rval;
}

template<class T>
static inline void
readAndDeliver(OW_CIMProtocolIStreamIFCRef& istr, T& result)
{
	try
	{
		checkError(istr);
		readCIMObject(istr,result);
	}
	catch (OW_IOException& e)
	{
		while(*istr) istr->get();
		if (istr->getError().length() == 0)
		{
			throw e;
		}
	}
	while(*istr) istr->get();
	OW_String errorStr = istr->getError();
	if (istr->getError().length() > 0)
	{
		OW_TempFileStream error(500);
		istr->getError(error);
		checkError(error);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_BinaryCIMOMHandle::OW_BinaryCIMOMHandle(OW_CIMProtocolIFCRef prot)
: OW_ClientCIMOMHandle(), m_protocol(prot)
{
	m_protocol->setContentType("application/x-owbinary");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::deleteClass(const OW_CIMObjectPath& path)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"DeleteClass", path.getNameSpace());
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_DELETECLS);
	OW_BinIfcIO::writeObjectPath(strm, path);
	
	checkError(m_protocol->endRequest(strmRef, "DeleteClass", path.getNameSpace()));
}


//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::deleteInstance(const OW_CIMObjectPath& path)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"DeleteInstance", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_DELETEINST);
	OW_BinIfcIO::writeObjectPath(strm, path);

	checkError(m_protocol->endRequest(strmRef, "DeleteInstance", path.getNameSpace()));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::deleteQualifierType(const OW_CIMObjectPath& path)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"DeleteQualifier", path.getNameSpace());
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_DELETEQUAL);
	OW_BinIfcIO::writeObjectPath(strm, path);
	checkError(m_protocol->endRequest(strmRef, "DeleteQualifier", path.getNameSpace()));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::enumClassNames(const OW_CIMObjectPath& path,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"EnumerateClassNames", path.getNameSpace());
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ENUMCLSNAMES);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeBool(strm, deep);
	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "EnumerateClassNames", path.getNameSpace());

	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::enumClass(const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"EnumerateClasses", path.getNameSpace());
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ENUMCLSS);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeBool(strm, deep);
	OW_BinIfcIO::writeBool(strm, localOnly);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);

	OW_CIMProtocolIStreamIFCRef in = m_protocol->endRequest(strmRef,
		"EnumerateClasses", path.getNameSpace());

	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::enumInstanceNames(const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"EnumerateInstanceNames", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ENUMINSTNAMES);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeBool(strm, deep);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "EnumerateInstanceNames", path.getNameSpace());
	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::enumInstances(const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"EnumerateInstances", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ENUMINSTS);
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

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "EnumerateInstances", path.getNameSpace());
	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierTypeEnumeration
OW_BinaryCIMOMHandle::enumQualifierTypes(const OW_CIMObjectPath& path)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"EnumerateQualifiers", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ENUMQUALS);
	OW_BinIfcIO::writeObjectPath(strm, path);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "EnumerateQualifiers", path.getNameSpace());

	return readCIMObject<OW_CIMQualifierTypeEnumeration>(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_BinaryCIMOMHandle::getClass(const OW_CIMObjectPath& path, OW_Bool localOnly,
    OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"GetClass", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_GETCLS);
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

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "GetClass", path.getNameSpace());

	return readCIMObject<OW_CIMClass>(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_BinaryCIMOMHandle::getInstance(const OW_CIMObjectPath& path,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"GetInstance", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_GETINST);
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

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "GetInstance", path.getNameSpace());

	return readCIMObject<OW_CIMInstance>(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_BinaryCIMOMHandle::invokeMethod(const OW_CIMObjectPath& name,
											  const OW_String& methodName,
											  const OW_CIMValueArray& inParams,
											  OW_CIMValueArray& outParams)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		methodName, name.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_INVMETH);
	OW_BinIfcIO::writeObjectPath(strm, name);
	OW_BinIfcIO::writeString(strm, methodName);

	OW_BinIfcIO::write(strm, OW_Int32(OW_BINSIG_VALUEARRAY));
	OW_BinIfcIO::write(strm, OW_Int32(inParams.size()));
	for(size_t i = 0; i < inParams.size(); i++)
	{
		OW_BinIfcIO::writeValue(strm, inParams[i]);
	}

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, methodName, name.getNameSpace());
	checkError(in);

	OW_CIMValue cv;
	try
	{
		OW_Bool isrv(OW_BinIfcIO::readBool(*in));
		if(isrv)
		{
			cv = OW_BinIfcIO::readValue(*in);
		}

		outParams.clear();
		OW_BinIfcIO::verifySignature(*in, OW_BINSIG_VALUEARRAY);
		OW_Int32 size;
		OW_BinIfcIO::read(*in, size);
		while(size)
		{
			outParams.append(OW_BinIfcIO::readValue(*in));
			size--;
		}

	}
	catch(OW_IOException& e)
	{
		while(*in) in->get();
		if (in->getError().length() > 0)
		{
			checkError(in);
		}
		else
		{
			throw e;
		}
	}
	return cv;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_BinaryCIMOMHandle::getQualifierType(const OW_CIMObjectPath& path)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"GetQualifier", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_GETQUAL);
	OW_BinIfcIO::writeObjectPath(strm, path);
	
	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"GetQualifier", path.getNameSpace());

	return readCIMObject<OW_CIMQualifierType>(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::setQualifierType(const OW_CIMObjectPath& path,
								   		const OW_CIMQualifierType& qt)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"SetQualifier", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_SETQUAL);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeQual(strm, qt);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"SetQualifier", path.getNameSpace());
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::modifyClass(const OW_CIMObjectPath& path,
										 const OW_CIMClass& cc)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"ModifyClass", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_MODIFYCLS);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeClass(strm, cc);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"ModifyClass", path.getNameSpace());
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::createClass(const OW_CIMObjectPath& path,
											 const OW_CIMClass& cc)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"CreateClass", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_CREATECLS);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeClass(strm, cc);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"CreateClass", path.getNameSpace());
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::modifyInstance(const OW_CIMObjectPath& path,
											 const OW_CIMInstance& ci)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"ModifyInstance", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_MODIFYINST);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeInstance(strm, ci);
	
	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"ModifyInstance", path.getNameSpace());
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_BinaryCIMOMHandle::createInstance(const OW_CIMObjectPath& path,
	const OW_CIMInstance& ci)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"CreateInstance", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_CREATEINST);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeInstance(strm, ci);
	
	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"CreateInstance", path.getNameSpace());
	return readCIMObject<OW_CIMObjectPath>(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_BinaryCIMOMHandle::getProperty(const OW_CIMObjectPath& path,
											 const OW_String& propName)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"GetProperty", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_GETPROP);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, propName);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "GetProperty", path.getNameSpace());
	checkError(in);

	OW_CIMValue cv;
	try
	{
		OW_Bool isValue(OW_BinIfcIO::readBool(*in));
		if(isValue)
		{
			cv = OW_BinIfcIO::readValue(*in);
		}

	}
	catch (OW_IOException& e)
	{
		while(*in) in->get();
		if (in->getError().length() > 0)
		{
			checkError(in);
		}
		else
		{
			throw e;
		}
	}
	return cv;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::setProperty(const OW_CIMObjectPath& path,
											 const OW_String& propName,
											 const OW_CIMValue& cv)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"SetProperty", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_SETPROP);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, propName);
	OW_Bool isValue = (cv) ? true : false;
	OW_BinIfcIO::writeBool(strm, isValue);
	if(isValue)
	{
		OW_BinIfcIO::writeValue(strm, cv);
	}

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "SetProperty", path.getNameSpace());
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_BinaryCIMOMHandle::associatorNames(const OW_CIMObjectPath& path,
				const OW_String& assocClass, const OW_String& resultClass,
				const OW_String& role, const OW_String& resultRole)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"AssociatorNames", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ASSOCNAMES);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, assocClass);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	OW_BinIfcIO::writeString(strm, resultRole);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "AssociatorNames", path.getNameSpace());

	return readCIMObject<OW_CIMObjectPathEnumeration>(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_BinaryCIMOMHandle::associators(const OW_CIMObjectPath& path,
		 	 			 const OW_String& assocClass, const OW_String& resultClass,
		 	 			 const OW_String& role, const OW_String& resultRole,
		 	 			 OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		 				 const OW_StringArray* propertyList)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"Associators", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ASSOCIATORS);
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

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "Associators", path.getNameSpace());

	return readCIMObject<OW_CIMInstanceEnumeration>(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_BinaryCIMOMHandle::referenceNames(const OW_CIMObjectPath& path,
												 const OW_String& resultClass,
												 const OW_String& role)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"ReferenceNames", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_REFNAMES);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	
	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "ReferenceNames", path.getNameSpace());

	return readCIMObject<OW_CIMObjectPathEnumeration>(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_BinaryCIMOMHandle::references(const OW_CIMObjectPath& path,
		 						const OW_String& resultClass, const OW_String& role,
		 						OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
								const OW_StringArray* propertyList)
{
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"ReferenceNames", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_REFERENCES);
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

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "ReferenceNames", path.getNameSpace());
	
	return readCIMObject<OW_CIMInstanceEnumeration>(in);
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
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"ExecQuery", path.getNameSpace());;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_EXECQUERY);
	OW_BinIfcIO::writeNameSpace(strm, path);
	OW_BinIfcIO::writeString(strm, query);
	OW_BinIfcIO::writeString(strm, queryLanguage);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef, "ExecQuery", path.getNameSpace());
	checkError(in);

	OW_CIMInstanceArray ira;
	try
	{
		OW_BinIfcIO::verifySignature(*in, OW_BINSIG_INSTARRAY);
		OW_Int32 size;
		OW_BinIfcIO::read(*in, size);
		while(size)
		{
			ira.append(OW_BinIfcIO::readInstance(*in));
			size--;
		}
	}
	catch(OW_IOException& e)
	{
		while(*in) in->get();
		if (in->getError().length() > 0)
		{
			checkError(in);
		}
		else
		{
			throw e;
		}
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

