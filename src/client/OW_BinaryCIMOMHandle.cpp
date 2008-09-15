/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Jon Carey
 * @author Dan Nuffer
 */

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
#include "OW_BinarySerialization.hpp"
#include "OW_IOException.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMNameSpaceUtils.hpp"
#include "OW_CIMException.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_HTTPChunkedIStream.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

namespace OW_NAMESPACE
{

using std::ostream;
using std::iostream;
using std::istream;
using namespace WBEMFlags;

namespace // anonymous
{
inline void
checkError2(std::streambuf & istrm)
{
	UInt8 rc;
	BinarySerialization::read(istrm, rc);
	if (rc != BinarySerialization::BIN_OK)
	{
		switch (rc)
		{
			case BinarySerialization::BIN_ERROR:
			{
				String msg;
				BinarySerialization::read(istrm, msg);
				OW_THROW(IOException, msg.c_str());
			}
			case BinarySerialization::BIN_EXCEPTION:
			{
				UInt16 cimerrno;
				String cimMsg;
				BinarySerialization::read(istrm, cimerrno);
				BinarySerialization::read(istrm, cimMsg);
				OW_THROWCIMMSG(CIMException::ErrNoType(cimerrno), cimMsg.c_str());
			}
			default:
				OW_THROW(IOException,
					"Unexpected value received from server.");
		}
	}
}
inline void
checkError(std::istream & istr, const CIMProtocolIFCRef & protocol)
{
	try
	{
		checkError2(*istr.rdbuf());
	}
	catch (...)
	{
		protocol->endResponse(istr);
		throw;
	}
}
inline void
readCIMObject(std::streambuf & istr, CIMClass & cc)
{
	cc = BinarySerialization::readClass(istr);
}
inline void
readCIMObject(std::streambuf & istr, CIMClassResultHandlerIFC & result)
{
	BinarySerialization::readClassEnum(istr, result);
}
inline void
readCIMObject(std::streambuf & istr, CIMInstance& ci)
{
	ci = BinarySerialization::readInstance(istr);
}
inline void
readCIMObject(std::streambuf & istr, CIMObjectPath& cop)
{
	cop = BinarySerialization::readObjectPath(istr);
}
inline void
readCIMObject(std::streambuf & istr, CIMObjectPathResultHandlerIFC& result)
{
	BinarySerialization::readObjectPathEnum(istr, result);
}
inline void
readCIMObject(std::streambuf & istr, CIMInstanceResultHandlerIFC& result)
{
	BinarySerialization::readInstanceEnum(istr, result);
}
inline void
readCIMObject(std::streambuf & istr, CIMQualifierType& arg)
{
	arg = BinarySerialization::readQualType(istr);
}
inline void
readCIMObject(std::streambuf & istr, CIMQualifierTypeResultHandlerIFC& result)
{
	BinarySerialization::readQualifierTypeEnum(istr, result);
}
inline void
readCIMObject(std::streambuf & istr, StringResultHandlerIFC& result)
{
	BinarySerialization::readStringEnum(istr, result);
}
template<class T>
inline T
readCIMObject(std::istream & istr, const CIMProtocolIFCRef& protocol)
{
	T rval;
	checkError(istr, protocol);
	try
	{
		readCIMObject(*istr.rdbuf(), rval);
	}
	catch (IOException& e)
	{
		protocol->endResponse(istr);
		throw e;
	}
	protocol->endResponse(istr);
	return rval;
}

template<class T>
inline void
readAndDeliver(std::istream & istr, T& result, const CIMProtocolIFCRef& protocol)
{
	checkError(istr, protocol);
	try
	{
		readCIMObject(*istr.rdbuf(),result);
	}
	catch (IOException& e)
	{
		protocol->endResponse(istr);
		throw e;
	}
	protocol->endResponse(istr);
}

// We always send the lowest possible version. If 1.0 and 1.1 are the same, we send 1.0 so that 1.0 only clients will accept the request.
// If we're using a 1.1 only feature, then we have to send 1.1.
const String PROTOCOL_VERSION_1_0("1.0");
const String PROTOCOL_VERSION_1_1("1.1");

} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
BinaryCIMOMHandle::BinaryCIMOMHandle(const CIMProtocolIFCRef& prot)
: ClientCIMOMHandle(), m_protocol(prot)
{
	m_protocol->setContentType("application/x-owbinary");
}

//////////////////////////////////////////////////////////////////////////////
BinaryCIMOMHandle::~BinaryCIMOMHandle()
{
}

//////////////////////////////////////////////////////////////////////////////
CIMProtocolIFCRef
BinaryCIMOMHandle::getWBEMProtocolHandler() const
{
	return m_protocol;
}

//////////////////////////////////////////////////////////////////////////////
void BinaryCIMOMHandle::close() 
{
	m_protocol->close();
}

//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::enumClassNames(
	const String& ns_,
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"EnumerateClassNames", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_ENUMCLSNAMES);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeString(strm, className);
	BinarySerialization::writeBool(strm, deep);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"EnumerateClassNames", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::enumClass(const String& ns_,
	const String& className,
	CIMClassResultHandlerIFC& result,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"EnumerateClasses", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_ENUMCLSS);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeString(strm, className);
	BinarySerialization::writeBool(strm, deep);
	BinarySerialization::writeBool(strm, localOnly);
	BinarySerialization::writeBool(strm, includeQualifiers);
	BinarySerialization::writeBool(strm, includeClassOrigin);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"EnumerateClasses", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::enumInstanceNames(
	const String& ns_,
	const String& className,
	CIMObjectPathResultHandlerIFC& result)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"EnumerateInstanceNames", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_ENUMINSTNAMES);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeString(strm, className);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"EnumerateInstanceNames", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::enumInstances(
	const String& ns_,
	const String& className,
	CIMInstanceResultHandlerIFC& result, EDeepFlag deep,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"EnumerateInstances", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_ENUMINSTS);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeString(strm, className);
	BinarySerialization::writeBool(strm, deep);
	BinarySerialization::writeBool(strm, localOnly);
	BinarySerialization::writeBool(strm, includeQualifiers);
	BinarySerialization::writeBool(strm, includeClassOrigin);
	BinarySerialization::writeStringArray(strm, propertyList);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"EnumerateInstances", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
BinaryCIMOMHandle::getClass(
	const String& ns_,
	const String& className,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"GetClass", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_GETCLS);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeString(strm, className);
	BinarySerialization::writeBool(strm, localOnly);
	BinarySerialization::writeBool(strm, includeQualifiers);
	BinarySerialization::writeBool(strm, includeClassOrigin);
	BinarySerialization::writeStringArray(strm, propertyList);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"GetClass", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	return readCIMObject<CIMClass>(*in, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
BinaryCIMOMHandle::getInstance(
	const String& ns_,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"GetInstance", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_GETINST);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeObjectPath(strm, instanceName);
	BinarySerialization::writeBool(strm, localOnly);
	BinarySerialization::writeBool(strm, includeQualifiers);
	BinarySerialization::writeBool(strm, includeClassOrigin);
	BinarySerialization::writeStringArray(strm, propertyList);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"GetInstance", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	return readCIMObject<CIMInstance>(*in, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
BinaryCIMOMHandle::invokeMethod(
	const String& ns_,
	const CIMObjectPath& path,
	const String& methodName,
	const CIMParamValueArray& inParams,
	CIMParamValueArray& outParams)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		methodName, ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_INVMETH);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeObjectPath(strm, path);
	BinarySerialization::writeString(strm, methodName);
	BinarySerialization::write(strm, BinarySerialization::BINSIG_PARAMVALUEARRAY);
	BinarySerialization::writeArray(strm, inParams);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		methodName, ns + ":" + path.modelPath(), CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	checkError(*in, m_protocol);
	CIMValue cv(CIMNULL);
	try
	{
		std::streambuf & inbuf = *in->rdbuf();
		cv = BinarySerialization::readValue(inbuf);
		BinarySerialization::verifySignature(inbuf, BinarySerialization::BINSIG_PARAMVALUEARRAY);
		BinarySerialization::readArray(inbuf, outParams);
	}
	catch(...)
	{
		m_protocol->endResponse(*in);
		throw;
	}
	m_protocol->endResponse(*in);
	return cv;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
BinaryCIMOMHandle::getQualifierType(const String& ns_,
		const String& qualifierName)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"GetQualifier", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_GETQUAL);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeString(strm, qualifierName);
	
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"GetQualifier", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	return readCIMObject<CIMQualifierType>(*in, m_protocol);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::setQualifierType(const String& ns_,
		const CIMQualifierType& qt)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"SetQualifier", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_SETQUAL);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeQualType(strm, qt);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"SetQualifier", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	checkError(*in, m_protocol);
	m_protocol->endResponse(*in);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::enumQualifierTypes(
	const String& ns_,
	CIMQualifierTypeResultHandlerIFC& result)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"EnumerateQualifiers", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_ENUMQUALS);
	BinarySerialization::writeString(strm, ns);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"EnumerateQualifiers", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::deleteQualifierType(const String& ns_, const String& qualName)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"DeleteQualifier", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_DELETEQUAL);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeString(strm, qualName);
	Reference<std::istream> in = m_protocol->endRequest(strmRef, "DeleteQualifier", ns,
		CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	checkError(*in, m_protocol);
	m_protocol->endResponse(*in);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::modifyClass(const String &ns_,
		const CIMClass& cc)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"ModifyClass", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_MODIFYCLS);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeClass(strm, cc);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"ModifyClass", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	checkError(*in, m_protocol);
	m_protocol->endResponse(*in);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::createClass(const String& ns_,
		const CIMClass& cc)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"CreateClass", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_CREATECLS);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeClass(strm, cc);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"CreateClass", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	checkError(*in, m_protocol);
	m_protocol->endResponse(*in);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::deleteClass(const String& ns_, const String& className)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"DeleteClass", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_DELETECLS);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeString(strm, className);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"DeleteClass", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	checkError(*in, m_protocol);
	m_protocol->endResponse(*in);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::modifyInstance(
	const String& ns_,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"ModifyInstance", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_MODIFYINST);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeInstance(strm, modifiedInstance);
	BinarySerialization::writeBool(strm, includeQualifiers);
	BinarySerialization::writeStringArray(strm, propertyList);
	
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"ModifyInstance", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_1);
	checkError(*in, m_protocol);
	m_protocol->endResponse(*in);
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
BinaryCIMOMHandle::createInstance(const String& ns_,
	const CIMInstance& ci)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"CreateInstance", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_CREATEINST);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeInstance(strm, ci);
	
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"CreateInstance", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	CIMObjectPath rval = readCIMObject<CIMObjectPath>(*in, m_protocol);
	rval.setNameSpace(ns);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::deleteInstance(const String& ns_, const CIMObjectPath& inst)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"DeleteInstance", ns);;
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_DELETEINST);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeObjectPath(strm, inst);
	Reference<std::istream> in = m_protocol->endRequest(strmRef, "DeleteInstance", ns,
		CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	checkError(*in, m_protocol);
	m_protocol->endResponse(*in);
}
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::setProperty(
	const String& ns_,
	const CIMObjectPath& path,
	const String& propName,
	const CIMValue& cv)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"SetProperty", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_SETPROP);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeObjectPath(strm, path);
	BinarySerialization::writeString(strm, propName);
	BinarySerialization::writeValue(strm, cv);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"SetProperty", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	checkError(*in, m_protocol);
	m_protocol->endResponse(*in);
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
CIMValue
BinaryCIMOMHandle::getProperty(
	const String& ns_,
	const CIMObjectPath& path,
	const String& propName)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"GetProperty", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_GETPROP);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeObjectPath(strm, path);
	BinarySerialization::writeString(strm, propName);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"GetProperty", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	checkError(*in, m_protocol);
	CIMValue cv(CIMNULL);
	try
	{
		cv = BinarySerialization::readValue(*in->rdbuf());
	}
	catch (...)
	{
		m_protocol->endResponse(*in);
		throw;
	}
	m_protocol->endResponse(*in);
	return cv;
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::associatorNames(
	const String& ns_,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"AssociatorNames", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_ASSOCNAMES);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeObjectPath(strm, path);
	BinarySerialization::writeString(strm, assocClass);
	BinarySerialization::writeString(strm, resultClass);
	BinarySerialization::writeString(strm, role);
	BinarySerialization::writeString(strm, resultRole);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"AssociatorNames", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::associators(
	const String& ns_,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	if (!path.isInstancePath())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"associators requires an instance path not a class path");
	}
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"Associators", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_ASSOCIATORS);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeObjectPath(strm, path);
	BinarySerialization::writeString(strm, assocClass);
	BinarySerialization::writeString(strm, resultClass);
	BinarySerialization::writeString(strm, role);
	BinarySerialization::writeString(strm, resultRole);
	BinarySerialization::writeBool(strm, includeQualifiers);
	BinarySerialization::writeBool(strm, includeClassOrigin);
	BinarySerialization::writeStringArray(strm, propertyList);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"Associators", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::associatorsClasses(
	const String& ns_,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	if (!path.isClassPath())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"associatorsClasses requires a class path not an instance path");
	}
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"Associators", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_ASSOCIATORS);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeObjectPath(strm, path);
	BinarySerialization::writeString(strm, assocClass);
	BinarySerialization::writeString(strm, resultClass);
	BinarySerialization::writeString(strm, role);
	BinarySerialization::writeString(strm, resultRole);
	BinarySerialization::writeBool(strm, includeQualifiers);
	BinarySerialization::writeBool(strm, includeClassOrigin);
	BinarySerialization::writeStringArray(strm, propertyList);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"Associators", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::referenceNames(
	const String& ns_,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& resultClass,
	const String& role)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"ReferenceNames", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_REFNAMES);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeObjectPath(strm, path);
	BinarySerialization::writeString(strm, resultClass);
	BinarySerialization::writeString(strm, role);
	
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"ReferenceNames", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::references(
	const String& ns_,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	if (!path.isInstancePath())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"references requires an instance path not a class path");
	}
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"ReferenceNames", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_REFERENCES);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeObjectPath(strm, path);
	BinarySerialization::writeString(strm, resultClass);
	BinarySerialization::writeString(strm, role);
	BinarySerialization::writeBool(strm, includeQualifiers);
	BinarySerialization::writeBool(strm, includeClassOrigin);
	BinarySerialization::writeStringArray(strm, propertyList);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"ReferenceNames", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::referencesClasses(
	const String& ns_,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	if (!path.isClassPath())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"referencesClasses requires a class path not an instance path");
	}
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"ReferenceNames", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_REFERENCES);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeObjectPath(strm, path);
	BinarySerialization::writeString(strm, resultClass);
	BinarySerialization::writeString(strm, role);
	BinarySerialization::writeBool(strm, includeQualifiers);
	BinarySerialization::writeBool(strm, includeClassOrigin);
	BinarySerialization::writeStringArray(strm, propertyList);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"ReferenceNames", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumeration
BinaryCIMOMHandle::execQuery(
	const String& ns,
	const String& query, int wqlLevel)
{
	return CIMOMHandleIFC::execQueryE(ns, query,
		String("WQL") + String(wqlLevel));
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::execQuery(
	const String& ns_,
	CIMInstanceResultHandlerIFC& result,
	const String& query, const String& queryLanguage)
{
	String ns(CIMNameSpaceUtils::prepareNamespace(ns_));
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"ExecQuery", ns);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::BIN_EXECQUERY);
	BinarySerialization::writeString(strm, ns);
	BinarySerialization::writeString(strm, query);
	BinarySerialization::writeString(strm, queryLanguage);
	Reference<std::istream> in = m_protocol->endRequest(strmRef,
		"ExecQuery", ns, CIMProtocolIFC::E_CIM_OPERATION_REQUEST, PROTOCOL_VERSION_1_0);
	readAndDeliver(*in, result, m_protocol);
}

//////////////////////////////////////////////////////////////////////////////
CIMFeatures
BinaryCIMOMHandle::getServerFeatures()
{
	return m_protocol->getFeatures();
}

//////////////////////////////////////////////////////////////////////////////
void
BinaryCIMOMHandle::exportIndication(const CIMInstance& instance, const String& instNS)
{
	Reference<std::ostream> strmRef = m_protocol->beginRequest(
		"ExportIndication", instNS);
	std::streambuf & strm = *strmRef->rdbuf();
	BinarySerialization::write(strm, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(strm, BinarySerialization::EXPORT_INDICATION);
	BinarySerialization::writeInstance(strm, instance);
	BinarySerialization::writeString(strm, instNS);
	Reference<std::istream> in = m_protocol->endRequest(strmRef, "ExportIndication", instNS,
		CIMProtocolIFC::E_CIM_EXPORT_REQUEST, PROTOCOL_VERSION_1_1);
	checkError(*in, m_protocol);
	m_protocol->endResponse(*in);
}

} // end namespace OW_NAMESPACE

