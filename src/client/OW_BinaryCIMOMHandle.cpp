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
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_Format.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_BinIfcIO.hpp"
#include "OW_IOException.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMInstanceEnumeration.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

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
		istr->checkForError();
		throw e;
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
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMObjectPathResultHandlerIFC& result)
{
	OW_BinIfcIO::readObjectPathEnum(*istr, result);
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
readCIMObject(OW_CIMProtocolIStreamIFCRef& istr, OW_CIMQualifierTypeResultHandlerIFC& result)
{
	OW_BinIfcIO::readQualifierTypeEnum(*istr, result);
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
		istr->checkForError();
		throw e;
	}
	while(*istr) istr->get();
	istr->checkForError();
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
		istr->checkForError();
		throw e;
	}
	while(*istr) istr->get();
	istr->checkForError();
}

//////////////////////////////////////////////////////////////////////////////
OW_BinaryCIMOMHandle::OW_BinaryCIMOMHandle(OW_CIMProtocolIFCRef prot)
: OW_ClientCIMOMHandle(), m_protocol(prot)
{
	m_protocol->setContentType("application/x-owbinary");
}


//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::deleteClass(const OW_String& ns_, const OW_String& className)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"DeleteClass", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_DELETECLS);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeString(strm, className);
	
	checkError(m_protocol->endRequest(strmRef, "DeleteClass", ns));
}


//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::deleteInstance(const OW_String& ns_, const OW_CIMObjectPath& inst)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"DeleteInstance", ns);;
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_DELETEINST);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeObjectPath(strm, inst);

	checkError(m_protocol->endRequest(strmRef, "DeleteInstance", ns));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::deleteQualifierType(const OW_String& ns_, const OW_String& qualName)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"DeleteQualifier", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_DELETEQUAL);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeString(strm, qualName);
	checkError(m_protocol->endRequest(strmRef, "DeleteQualifier", ns));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::enumClassNames(
	const OW_String& ns_,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"EnumerateClassNames", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ENUMCLSNAMES);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeString(strm, className);
	OW_BinIfcIO::writeBool(strm, deep);
	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"EnumerateClassNames", ns);

	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::enumClass(const OW_String& ns_,
	const OW_String& className,
	OW_CIMClassResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"EnumerateClasses", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ENUMCLSS);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeString(strm, className);
	OW_BinIfcIO::writeBool(strm, deep);
	OW_BinIfcIO::writeBool(strm, localOnly);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);

	OW_CIMProtocolIStreamIFCRef in = m_protocol->endRequest(strmRef,
		"EnumerateClasses", ns);

	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::enumInstanceNames(
	const OW_String& ns_,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"EnumerateInstanceNames", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ENUMINSTNAMES);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeString(strm, className);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"EnumerateInstanceNames", ns);
	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::enumInstances(
	const OW_String& ns_,
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"EnumerateInstances", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ENUMINSTS);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeString(strm, className);
	OW_BinIfcIO::writeBool(strm, deep);
	OW_BinIfcIO::writeBool(strm, localOnly);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_BinIfcIO::writeStringArray(strm, propertyList);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"EnumerateInstances", ns);
	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::enumQualifierTypes(
	const OW_String& ns_,
	OW_CIMQualifierTypeResultHandlerIFC& result)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"EnumerateQualifiers", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ENUMQUALS);
	OW_BinIfcIO::writeString(strm, ns);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"EnumerateQualifiers", ns);

	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_BinaryCIMOMHandle::getClass(
	const OW_String& ns_,
	const OW_String& className,
	OW_Bool localOnly,
    OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"GetClass", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_GETCLS);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeString(strm, className);
	OW_BinIfcIO::writeBool(strm, localOnly);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_BinIfcIO::writeStringArray(strm, propertyList);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"GetClass", ns);

	return readCIMObject<OW_CIMClass>(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_BinaryCIMOMHandle::getInstance(
	const OW_String& ns_,
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"GetInstance", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_GETINST);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeObjectPath(strm, instanceName);
	OW_BinIfcIO::writeBool(strm, localOnly);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_BinIfcIO::writeStringArray(strm, propertyList);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"GetInstance", ns);

	return readCIMObject<OW_CIMInstance>(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_BinaryCIMOMHandle::invokeMethod(
	const OW_String& ns_,
	const OW_CIMObjectPath& path,
	const OW_String& methodName,
	const OW_CIMParamValueArray& inParams,
	OW_CIMParamValueArray& outParams)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		methodName, ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_INVMETH);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, methodName);

	OW_BinIfcIO::write(strm, OW_Int32(OW_BINSIG_PARAMVALUEARRAY));
	inParams.writeObject(strm);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		methodName, ns);
	checkError(in);

	OW_CIMValue cv(OW_CIMNULL);
	try
	{
		OW_Bool isrv(OW_BinIfcIO::readBool(*in));
		if(isrv)
		{
			cv = OW_BinIfcIO::readValue(*in);
		}

		OW_BinIfcIO::verifySignature(*in, OW_BINSIG_PARAMVALUEARRAY);
		outParams.readObject(*in);
	}
	catch(OW_IOException& e)
	{
		while(*in) in->get();
		in->checkForError();
		throw e;
	}
	return cv;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_BinaryCIMOMHandle::getQualifierType(const OW_String& ns_,
		const OW_String& qualifierName)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"GetQualifier", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_GETQUAL);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeString(strm, qualifierName);
	
	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"GetQualifier", ns);

	return readCIMObject<OW_CIMQualifierType>(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::setQualifierType(const OW_String& ns_,
		const OW_CIMQualifierType& qt)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"SetQualifier", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_SETQUAL);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeQual(strm, qt);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"SetQualifier", ns);
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::modifyClass(const OW_String &ns_,
		const OW_CIMClass& cc)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"ModifyClass", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_MODIFYCLS);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeClass(strm, cc);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"ModifyClass", ns);
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::createClass(const OW_String& ns_,
		const OW_CIMClass& cc)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"CreateClass", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_CREATECLS);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeClass(strm, cc);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"CreateClass", ns);
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::modifyInstance(
	const OW_String& ns_,
	const OW_CIMInstance& modifiedInstance,
	OW_Bool includeQualifiers,
	const OW_StringArray* propertyList)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"ModifyInstance", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_MODIFYINST);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeInstance(strm, modifiedInstance);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeStringArray(strm, propertyList);
	
	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"ModifyInstance", ns);
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_BinaryCIMOMHandle::createInstance(const OW_String& ns_,
	const OW_CIMInstance& ci)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"CreateInstance", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_CREATEINST);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeInstance(strm, ci);
	
	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"CreateInstance", ns);
	OW_CIMObjectPath rval = readCIMObject<OW_CIMObjectPath>(in);
	rval.setNameSpace(ns);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_BinaryCIMOMHandle::getProperty(
	const OW_String& ns_,
	const OW_CIMObjectPath& path,
	const OW_String& propName)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"GetProperty", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_GETPROP);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, propName);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"GetProperty", ns);
	checkError(in);

	OW_CIMValue cv(OW_CIMNULL);
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
		in->checkForError();
		throw e;
	}
	return cv;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::setProperty(
	const OW_String& ns_,
	const OW_CIMObjectPath& path,
	const OW_String& propName,
	const OW_CIMValue& cv)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"SetProperty", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_SETPROP);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, propName);
	OW_Bool isValue = (cv) ? true : false;
	OW_BinIfcIO::writeBool(strm, isValue);
	if(isValue)
	{
		OW_BinIfcIO::writeValue(strm, cv);
	}

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"SetProperty", ns);
	checkError(in);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::associatorNames(
	const OW_String& ns_,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"AssociatorNames", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ASSOCNAMES);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, assocClass);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	OW_BinIfcIO::writeString(strm, resultRole);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"AssociatorNames", ns);

	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::associators(
	const OW_String& ns_,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
    OW_String ns(prepareNamespace(ns_));
	if (path.getKeys().size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"associators requires an instance path not a class path");
	}

	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"Associators", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ASSOCIATORS);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, assocClass);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	OW_BinIfcIO::writeString(strm, resultRole);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_BinIfcIO::writeStringArray(strm, propertyList);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"Associators", ns);

	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::associatorsClasses(
	const OW_String& ns_,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
    OW_String ns(prepareNamespace(ns_));
	if (path.getKeys().size() > 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"associatorsClasses requires a class path not an instance path");
	}
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"Associators", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_ASSOCIATORS);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, assocClass);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	OW_BinIfcIO::writeString(strm, resultRole);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_BinIfcIO::writeStringArray(strm, propertyList);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"Associators", ns);

	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::referenceNames(
	const OW_String& ns_,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& resultClass,
	const OW_String& role)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"ReferenceNames", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_REFNAMES);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	
	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"ReferenceNames", ns);

	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::references(
	const OW_String& ns_,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
    OW_String ns(prepareNamespace(ns_));
	if (path.getKeys().size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"references requires an instance path not a class path");
	}

	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"ReferenceNames", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_REFERENCES);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_BinIfcIO::writeStringArray(strm, propertyList);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"ReferenceNames", ns);
	
	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::referencesClasses(
	const OW_String& ns_,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
    OW_String ns(prepareNamespace(ns_));
	if (path.getKeys().size() > 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"referencesClasses requires a class path not an instance path");
	}
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"ReferenceNames", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_REFERENCES);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeObjectPath(strm, path);
	OW_BinIfcIO::writeString(strm, resultClass);
	OW_BinIfcIO::writeString(strm, role);
	OW_BinIfcIO::writeBool(strm, includeQualifiers);
	OW_BinIfcIO::writeBool(strm, includeClassOrigin);
	OW_BinIfcIO::writeStringArray(strm, propertyList);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"ReferenceNames", ns);
	
	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_BinaryCIMOMHandle::execQuery(
	const OW_String& ns,
	const OW_String& query, int wqlLevel)
{
	return OW_CIMOMHandleIFC::execQueryE(ns, query, 
		OW_String("WQL") + OW_String(wqlLevel));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryCIMOMHandle::execQuery(
	const OW_String& ns_,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& query, const OW_String& queryLanguage)
{
    OW_String ns(prepareNamespace(ns_));
	OW_Reference<std::iostream> strmRef = m_protocol->beginRequest(
		"ExecQuery", ns);
	std::iostream& strm = *strmRef;
	OW_BinIfcIO::write(strm, OW_BIN_EXECQUERY);
	OW_BinIfcIO::writeString(strm, ns);
	OW_BinIfcIO::writeString(strm, query);
	OW_BinIfcIO::writeString(strm, queryLanguage);

	OW_Reference<OW_CIMProtocolIStreamIFC> in = m_protocol->endRequest(strmRef,
		"ExecQuery", ns);

	readAndDeliver(in, result);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMFeatures
OW_BinaryCIMOMHandle::getServerFeatures()
{
	return m_protocol->getFeatures();
}


//////////////////////////////////////////////////////////////////////////////

