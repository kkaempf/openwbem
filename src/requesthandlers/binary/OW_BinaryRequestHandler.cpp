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
#include "OW_ConfigOpts.hpp"
#include "blocxx/FileSystem.hpp"
#include "OW_BinaryRequestHandler.hpp"
#include "OW_BinarySerialization.hpp"
#include "blocxx/Format.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMParamValue.hpp"
#include "blocxx/SocketUtils.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMQualifier.hpp"
#include "blocxx/Mutex.hpp"
#include "blocxx/MutexLock.hpp"
#include "blocxx/SocketException.hpp"
#include "blocxx/ThreadCancelledException.hpp"
#include "OW_Logger.hpp"
#include "OW_OperationContext.hpp"
#include "blocxx/UserUtils.hpp"
#include "OW_ServiceIFCNames.hpp"
#include "OW_ResultHandlerIFC.hpp"

#include <exception>

namespace OW_NAMESPACE
{

namespace
{
	const String COMPONENT_NAME("ow.requesthandler.owbinary");
}

using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
BinaryRequestHandler::BinaryRequestHandler()
	: RequestHandlerIFC()
	, m_supportExportIndication(false)
{
}
BinaryRequestHandler::BinaryRequestHandler(ESupportExportIndicationFlag supportExportIndicationFlag)
	: RequestHandlerIFC()
	, m_supportExportIndication(supportExportIndicationFlag == E_SUPPORT_EXPORT_INDICATION)
{

}
//////////////////////////////////////////////////////////////////////////////
RequestHandlerIFC*
BinaryRequestHandler::clone() const
{
	return new BinaryRequestHandler(*this);
}
//////////////////////////////////////////////////////////////////////////////
String
BinaryRequestHandler::getName() const
{
	return ServiceIFCNames::BinaryRequestHandler;
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::doOptions(CIMFeatures& cf,
	OperationContext&)
{
	cf.cimom = "openwbem";
	cf.cimProduct = CIMFeatures::SERVER;
	cf.extURL = "local_binary";
	cf.protocolVersion = OW_VERSION;
	cf.supportedGroups.clear();
	cf.supportedQueryLanguages.clear();
	cf.supportedQueryLanguages.append("WQL");
	cf.supportsBatch = false;
	cf.validation.erase();
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::doProcess(std::istream * istr, std::ostream * ostrEntity,
	std::ostream * ostrError, OperationContext & context)
{
	this->doProcess(
		istr->rdbuf(), ostrEntity->rdbuf(), ostrError->rdbuf(), context);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::doProcess(std::streambuf * istrm, std::streambuf * ostrm,
	std::streambuf * ostrError, OperationContext& context)
{
	clearError();
	UInt8 funcNo = 0;
	Logger lgr(COMPONENT_NAME);
	try
	{
		CIMOMHandleIFCRef chdl = getEnvironment()->getCIMOMHandle(context, ServiceEnvironmentIFC::E_USE_PROVIDERS);
		UInt32 version = 0;
		BinarySerialization::read(*istrm, version);
		if (version < BinarySerialization::MinBinaryProtocolVersion || version > BinarySerialization::BinaryProtocolVersion)
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Incompatible version");
		}
		BinarySerialization::read(*istrm, funcNo);
		try
		{
			switch (funcNo)
			{
				case BinarySerialization::BIN_GETQUAL:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler get qualifier"
						" request");
					getQual(chdl, *ostrm, *istrm);
					break;
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
				case BinarySerialization::BIN_SETQUAL:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler set qualifier"
						" request");
					setQual(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_DELETEQUAL:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler delete qualifier"
						" request");
					deleteQual(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_ENUMQUALS:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler enum qualifiers"
						" request");
					enumQualifiers(chdl, *ostrm, *istrm);
					break;
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
				case BinarySerialization::BIN_GETCLS:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler get class"
						" request");
					getClass(chdl, *ostrm, *istrm);
					break;
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
				case BinarySerialization::BIN_CREATECLS:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler create class"
						" request");
					createClass(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_MODIFYCLS:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler modify class"
						" request");
					modifyClass(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_DELETECLS:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler delete class"
						" request");
					deleteClass(chdl, *ostrm, *istrm);
					break;
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
				case BinarySerialization::BIN_ENUMCLSS:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler enum classes"
						" request");
					enumClasses(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_GETINST:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler get instance"
						" request");
					getInstance(chdl, *ostrm, *istrm);
					break;
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
				case BinarySerialization::BIN_CREATEINST:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler create instance"
						" request");
					createInstance(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_MODIFYINST:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler get instance"
						" request");
					modifyInstance(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_DELETEINST:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler delete instance"
						" request");
					deleteInstance(chdl, *ostrm, *istrm);
					break;
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
				case BinarySerialization::BIN_SETPROP:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler set property"
						" request");
					setProperty(chdl, *ostrm, *istrm);
					break;
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
				case BinarySerialization::BIN_GETPROP:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler get property"
						" request");
					getProperty(chdl, *ostrm, *istrm);
					break;
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
				case BinarySerialization::BIN_ENUMCLSNAMES:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler enum class names"
						" request");
					enumClassNames(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_ENUMINSTS:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler enum instances"
						" request");
					enumInstances(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_ENUMINSTNAMES:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler enum instance"
						" names request");
					enumInstanceNames(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_INVMETH:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler invoke method"
						" request");
					invokeMethod(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_EXECQUERY:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler exec query"
						" request");
					execQuery(chdl, *ostrm, *istrm);
					break;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
				case BinarySerialization::BIN_ASSOCIATORS:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler associators"
						" request");
					associators(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_ASSOCNAMES:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler associator names"
						" request");
					associatorNames(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_REFERENCES:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler references"
						" request");
					references(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::BIN_REFNAMES:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler reference names"
						" request");
					referenceNames(chdl, *ostrm, *istrm);
					break;
#endif
				case BinarySerialization::BIN_GETSVRFEATURES:
					OW_LOG_DEBUG(lgr, "BinaryRequestHandler get server features request");
					getServerFeatures(chdl, *ostrm, *istrm);
					break;
				case BinarySerialization::EXPORT_INDICATION:
					if (m_supportExportIndication)
					{
						OW_LOG_DEBUG(lgr, "BinaryRequestHandler get export indication request");
						exportIndication(chdl, *ostrm, *istrm);
					}
					else
					{
						OW_LOG_DEBUG(lgr, "Ignoring export indication request");
						writeError(*ostrError, "Export indication not supported.");
					}
					break;
				default:
					OW_LOG_INFO(lgr, Format("BinaryRequestHandler: Received invalid function number: %1", static_cast<int>(funcNo)));
					writeError(*ostrError, "Invalid function number");
					break;
			}
		}
		catch(CIMException& e)
		{
			OW_LOG_INFO(lgr, Format("CIM Exception caught in"
				" BinaryRequestHandler: %1", e));
			BinarySerialization::write(*ostrError, BinarySerialization::BIN_EXCEPTION);
			BinarySerialization::write(*ostrError, UInt16(e.getErrNo()));
			BinarySerialization::write(*ostrError, e.getMessage());
			setError(e.getErrNo(), e.getDescription());
		}
	}
	catch(Exception& e)
	{
		OW_LOG_INFO(lgr, "Exception caught in BinaryRequestHandler");
		OW_LOG_INFO(lgr, Format("Type: %1", e.type()));
		OW_LOG_INFO(lgr, Format("File: %1", e.getFile()));
		OW_LOG_INFO(lgr, Format("Line: %1", e.getLine()));
		OW_LOG_INFO(lgr, Format("Msg: %1", e.getMessage()));
		writeError(*ostrError, Format("BinaryRequestHandler caught exception: %1", e).c_str());
		setError(CIMException::FAILED, e.getMessage());
		
	}
	catch(std::exception& e)
	{
		OW_LOG_INFO(lgr, Format("Caught %1 exception in BinaryRequestHandler",
			e.what()));
		writeError(*ostrError, Format("BinaryRequestHandler caught exception: %1", e.what()).c_str());
		setError(CIMException::FAILED, e.what());
	}
	catch (ThreadCancelledException&)
	{
		OW_LOG_INFO(lgr, "Thread cancelled in BinaryRequestHandler");
		writeError(*ostrError, "Thread cancelled");
		setError(CIMException::FAILED, "Thread cancelled");
		throw;
	}
	catch(...)
	{
		OW_LOG_ERROR(lgr, "Unknown exception caught in BinaryRequestHandler");
		writeError(*ostrError, "BinaryRequestHandler caught unknown exception");
		setError(CIMException::FAILED, "Caught unknown exception");
	}
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::createClass(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	CIMClass cc(BinarySerialization::readClass(istrm));
	chdl->createClass(ns, cc);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::modifyClass(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	CIMClass cc(BinarySerialization::readClass(istrm));
	chdl->modifyClass(ns, cc);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::deleteClass(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	String className(BinarySerialization::readString(istrm));
	chdl->deleteClass(ns, className);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class BinaryCIMClassWriter : public CIMClassResultHandlerIFC
	{
	public:
		BinaryCIMClassWriter(std::streambuf & ostrm_)
		: ostrm(ostrm_)
		{}
	protected:
		virtual void doHandle(const CIMClass &c)
		{
			BinarySerialization::writeClass(ostrm, c);
		}
	private:
		std::streambuf & ostrm;
	};
	class BinaryCIMObjectPathWriter : public CIMObjectPathResultHandlerIFC
	{
	public:
		BinaryCIMObjectPathWriter(std::streambuf & ostrm_, const String& host_)
		: ostrm(ostrm_)
		, m_host(host_)
		{}
	protected:
		virtual void doHandle(const CIMObjectPath &cop_)
		{
			// Make sure all outgoing object paths have our host name, instead of 127.0.0.1
			CIMObjectPath cop(cop_);
			if (cop.getFullNameSpace().isLocal())
			{
				try
				{
					cop.setHost(m_host);
				}
				catch (const SocketException& e)
				{
				}
			}
			BinarySerialization::writeObjectPath(ostrm, cop);
		}
	private:
		std::streambuf & ostrm;
		String m_host;
	};
	class BinaryCIMInstanceWriter : public CIMInstanceResultHandlerIFC
	{
	public:
		BinaryCIMInstanceWriter(std::streambuf & ostrm_, const String& ns_)
		: ostrm(ostrm_)
		, ns(ns_)
		{}
	protected:
		virtual void doHandle(const CIMInstance &cop)
		{
			if (cop.getNameSpace().empty())
			{
				CIMInstance ci(cop);
				ci.setNameSpace(ns);
				BinarySerialization::writeInstance(ostrm, ci);
			}
			else
			{
				BinarySerialization::writeInstance(ostrm, cop);
			}
		}
	private:
		std::streambuf & ostrm;
		String ns;
	};
	class BinaryCIMQualifierTypeWriter : public CIMQualifierTypeResultHandlerIFC
	{
	public:
		BinaryCIMQualifierTypeWriter(std::streambuf & ostrm_)
		: ostrm(ostrm_)
		{}
	protected:
		virtual void doHandle(const CIMQualifierType &cqt)
		{
			BinarySerialization::writeQualType(ostrm, cqt);
		}
	private:
		std::streambuf & ostrm;
	};
	class BinaryStringWriter : public StringResultHandlerIFC
	{
	public:
		BinaryStringWriter(std::streambuf & ostrm_)
		: ostrm(ostrm_)
		{}
	protected:
		virtual void doHandle(const String &name)
		{
			BinarySerialization::writeString(ostrm, name);
		}
	private:
		std::streambuf & ostrm;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::enumClasses(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	String className(BinarySerialization::readString(istrm));
	EDeepFlag deep(BinarySerialization::readBool(istrm) ? E_DEEP : E_SHALLOW);
	ELocalOnlyFlag localOnly(
		BinarySerialization::readBool(istrm) ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY);
	EIncludeQualifiersFlag includeQualifiers(
		BinarySerialization::readBool(istrm) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS);
	EIncludeClassOriginFlag includeClassOrigin(
		BinarySerialization::readBool(istrm) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::write(ostrm, BinarySerialization::BINSIG_CLSENUM);
	BinaryCIMClassWriter handler(ostrm);
	chdl->enumClass(ns, className, handler, deep, localOnly,
		includeQualifiers, includeClassOrigin);
	
	BinarySerialization::write(ostrm, BinarySerialization::END_CLSENUM);
	BinarySerialization::write(ostrm, BinarySerialization::END_CLSENUM);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::deleteQual(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	String qualName(BinarySerialization::readString(istrm));
	chdl->deleteQualifierType(ns, qualName);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::setQual(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	CIMQualifierType qt(BinarySerialization::readQualType(istrm));
	chdl->setQualifierType(ns, qt);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::enumQualifiers(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::write(ostrm, BinarySerialization::BINSIG_QUAL_TYPEENUM);
	BinaryCIMQualifierTypeWriter handler(ostrm);
	chdl->enumQualifierTypes(ns, handler);
	BinarySerialization::write(ostrm, BinarySerialization::END_QUALENUM);
	BinarySerialization::write(ostrm, BinarySerialization::END_QUALENUM);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::getClass(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	StringArray propList;
	StringArray* propListPtr = 0;
	String ns(BinarySerialization::readString(istrm));
	String className(BinarySerialization::readString(istrm));
	ELocalOnlyFlag localOnly(
		BinarySerialization::readBool(istrm) ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY);
	EIncludeQualifiersFlag includeQualifiers(
		BinarySerialization::readBool(istrm) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS);
	EIncludeClassOriginFlag includeClassOrigin(
		BinarySerialization::readBool(istrm) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN);
	BinarySerialization::readPropertyList(istrm, propList, propListPtr);
	CIMClass cc = chdl->getClass(ns, className, localOnly, includeQualifiers,
		includeClassOrigin, propListPtr);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::writeClass(ostrm, cc);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::getInstance(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	CIMObjectPath op(BinarySerialization::readObjectPath(istrm));
	ELocalOnlyFlag localOnly(BinarySerialization::readBool(istrm) ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY);
	EIncludeQualifiersFlag includeQualifiers(BinarySerialization::readBool(istrm) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS);
	EIncludeClassOriginFlag includeClassOrigin(BinarySerialization::readBool(istrm) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN);
	StringArray propList;
	StringArray* propListPtr = 0;
	BinarySerialization::readPropertyList(istrm, propList, propListPtr);
	CIMInstance cimInstance = chdl->getInstance(ns, op, localOnly,
		includeQualifiers, includeClassOrigin, propListPtr);
	if (cimInstance.getNameSpace().empty())
	{
		cimInstance.setNameSpace(ns);
	}
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::writeInstance(ostrm, cimInstance);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::getQual(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	String qualifierName(BinarySerialization::readString(istrm));
	CIMQualifierType qt = chdl->getQualifierType(ns, qualifierName);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::writeQualType(ostrm, qt);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::createInstance(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	CIMInstance cimInstance(BinarySerialization::readInstance(istrm));
	CIMName className = cimInstance.getClassName();
	//CIMObjectPath realPath(className, path.getNameSpace());
	// Special treatment for __Namespace class
	if (className == "__Namespace")
	{
		CIMProperty prop = cimInstance.getProperty(CIMProperty::NAME_PROPERTY);
		// Need the name property since it contains the name of the new
		// name space
		if (!prop)
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				"Name property not specified for new namespace");
		}
		// If the name property didn't come acrossed as a key, then
		// set the name property as the key
		if (!prop.isKey())
		{
			prop.addQualifier(CIMQualifier::createKeyQualifier());
		}
		cimInstance.setProperty(prop);
	}
	/* This should be done in the CIM Server
	CIMPropertyArray keys = cimInstance.getKeyValuePairs();
	if (keys.size() == 0)
	{
		OW_THROWCIMMSG(CIMException::FAILED,"Instance doesn't have keys");
	}
	for (size_t i = 0; i < keys.size(); ++i)
	{
		CIMProperty key = keys[i];
		if (!key.getValue())
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				Format("Key must be provided!  Property \"%1\" does not have a "
					"valid value.", key.getName()).c_str());
		}
	}
	realPath.setKeys(keys);
	*/
	CIMObjectPath newPath = chdl->createInstance(ns, cimInstance);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::writeObjectPath(ostrm, newPath);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::deleteInstance(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	CIMObjectPath op(BinarySerialization::readObjectPath(istrm));
	chdl->deleteInstance(ns, op);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::modifyInstance(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	CIMInstance ci(BinarySerialization::readInstance(istrm));
	EIncludeQualifiersFlag includeQualifiers(
		BinarySerialization::readBool(istrm) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS);
	StringArray propList;
	StringArray* propListPtr = 0;
	BinarySerialization::readPropertyList(istrm, propList, propListPtr);
	chdl->modifyInstance(ns, ci, includeQualifiers, propListPtr);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
}
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::setProperty(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	CIMObjectPath op(BinarySerialization::readObjectPath(istrm));
	String propName(BinarySerialization::readString(istrm));
	CIMValue cv = BinarySerialization::readValue(istrm);
	chdl->setProperty(ns, op, propName, cv);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::getProperty(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns (BinarySerialization::readString(istrm));
	CIMObjectPath op(BinarySerialization::readObjectPath(istrm));
	String propName(BinarySerialization::readString(istrm));
	CIMValue cv = chdl->getProperty(ns, op, propName);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::writeValue(ostrm, cv);
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::enumClassNames(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	String className(BinarySerialization::readString(istrm));
	EDeepFlag deep(BinarySerialization::readBool(istrm) ? E_DEEP : E_SHALLOW);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::write(ostrm, BinarySerialization::BINSIG_STRINGENUM);
	BinaryStringWriter handler(ostrm);
	chdl->enumClassNames(ns, className, handler, deep);
	BinarySerialization::write(ostrm, BinarySerialization::END_STRINGENUM);
	BinarySerialization::write(ostrm, BinarySerialization::END_STRINGENUM);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::enumInstances(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	StringArray propList;
	StringArray* propListPtr = 0;
	String ns(BinarySerialization::readString(istrm));
	String className(BinarySerialization::readString(istrm));
	EDeepFlag deep(BinarySerialization::readBool(istrm) ? E_DEEP : E_SHALLOW);
	ELocalOnlyFlag localOnly(
		BinarySerialization::readBool(istrm) ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY);
	EIncludeQualifiersFlag includeQualifiers(
		BinarySerialization::readBool(istrm) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS);
	EIncludeClassOriginFlag includeClassOrigin(
		BinarySerialization::readBool(istrm) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN);
	BinarySerialization::readPropertyList(istrm, propList, propListPtr);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::write(ostrm, BinarySerialization::BINSIG_INSTENUM);
	
	BinaryCIMInstanceWriter handler(ostrm, ns);
	chdl->enumInstances(ns, className, handler, deep, localOnly,
		includeQualifiers, includeClassOrigin, propListPtr);
	BinarySerialization::write(ostrm, BinarySerialization::END_INSTENUM);
	BinarySerialization::write(ostrm, BinarySerialization::END_INSTENUM);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::enumInstanceNames(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	String className(BinarySerialization::readString(istrm));
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::write(ostrm, BinarySerialization::BINSIG_OPENUM);
	BinaryCIMObjectPathWriter handler(ostrm, getHost());
	chdl->enumInstanceNames(ns, className, handler);
	BinarySerialization::write(ostrm, BinarySerialization::END_OPENUM);
	BinarySerialization::write(ostrm, BinarySerialization::END_OPENUM);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::invokeMethod(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns (BinarySerialization::readString(istrm));
	CIMObjectPath path(BinarySerialization::readObjectPath(istrm));
	String methodName(BinarySerialization::readString(istrm));
	CIMParamValueArray inparms;
	CIMParamValueArray outparms;
	// Get input params
	BinarySerialization::verifySignature(istrm, BinarySerialization::BINSIG_PARAMVALUEARRAY);
	BinarySerialization::readArray(istrm, inparms);
	CIMValue cv = chdl->invokeMethod(ns, path, methodName, inparms, outparms);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::writeValue(ostrm, cv);
	BinarySerialization::write(ostrm, BinarySerialization::BINSIG_PARAMVALUEARRAY);
	BinarySerialization::writeArray(ostrm, outparms);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::execQuery(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	String query(BinarySerialization::readString(istrm));
	String queryLang(BinarySerialization::readString(istrm));
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::write(ostrm, BinarySerialization::BINSIG_INSTENUM);
	BinaryCIMInstanceWriter handler(ostrm, ns);
	chdl->execQuery(ns, handler, query, queryLang);
	BinarySerialization::write(ostrm, BinarySerialization::END_INSTENUM);
	BinarySerialization::write(ostrm, BinarySerialization::END_INSTENUM);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::associators(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	StringArray propList;
	StringArray* propListPtr = 0;
	String ns(BinarySerialization::readString(istrm));
	CIMObjectPath op(BinarySerialization::readObjectPath(istrm));
	String assocClass(BinarySerialization::readString(istrm));
	String resultClass(BinarySerialization::readString(istrm));
	String role(BinarySerialization::readString(istrm));
	String resultRole(BinarySerialization::readString(istrm));
	EIncludeQualifiersFlag includeQualifiers(
		BinarySerialization::readBool(istrm) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS);
	EIncludeClassOriginFlag includeClassOrigin(
		BinarySerialization::readBool(istrm) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN);
	BinarySerialization::readPropertyList(istrm, propList, propListPtr);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	if (op.isClassPath())
	{
		// class path
		BinarySerialization::write(ostrm, BinarySerialization::BINSIG_CLSENUM);
		BinaryCIMClassWriter handler(ostrm);
		op.setNameSpace(ns);
		chdl->associatorsClasses(ns, op, handler, assocClass, resultClass,
			role, resultRole, includeQualifiers, includeClassOrigin, propListPtr);
		BinarySerialization::write(ostrm, BinarySerialization::END_CLSENUM);
		BinarySerialization::write(ostrm, BinarySerialization::END_CLSENUM);
	}
	else
	{
		// instance path
		BinarySerialization::write(ostrm, BinarySerialization::BINSIG_INSTENUM);
		BinaryCIMInstanceWriter handler(ostrm, ns);
		chdl->associators(ns, op, handler, assocClass, resultClass,
			role, resultRole, includeQualifiers, includeClassOrigin, propListPtr);
		BinarySerialization::write(ostrm, BinarySerialization::END_INSTENUM);
		BinarySerialization::write(ostrm, BinarySerialization::END_INSTENUM);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::associatorNames(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	CIMObjectPath op(BinarySerialization::readObjectPath(istrm));
	String assocClass(BinarySerialization::readString(istrm));
	String resultClass(BinarySerialization::readString(istrm));
	String role(BinarySerialization::readString(istrm));
	String resultRole(BinarySerialization::readString(istrm));
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::write(ostrm, BinarySerialization::BINSIG_OPENUM);
	BinaryCIMObjectPathWriter handler(ostrm, getHost());
	chdl->associatorNames(ns, op, handler, assocClass,
		resultClass, role, resultRole);
	BinarySerialization::write(ostrm, BinarySerialization::END_OPENUM);
	BinarySerialization::write(ostrm, BinarySerialization::END_OPENUM);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::references(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	StringArray propList;
	StringArray* propListPtr = 0;
	String ns(BinarySerialization::readString(istrm));
	CIMObjectPath op(BinarySerialization::readObjectPath(istrm));
	String resultClass(BinarySerialization::readString(istrm));
	String role(BinarySerialization::readString(istrm));
	EIncludeQualifiersFlag includeQualifiers(
		BinarySerialization::readBool(istrm) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS);
	EIncludeClassOriginFlag includeClassOrigin(
		BinarySerialization::readBool(istrm) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN);
	BinarySerialization::readPropertyList(istrm, propList, propListPtr);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	if (op.isClassPath())
	{
		// class path
		BinarySerialization::write(ostrm, BinarySerialization::BINSIG_CLSENUM);
		BinaryCIMClassWriter handler(ostrm);
		chdl->referencesClasses(ns, op, handler, resultClass,
			role, includeQualifiers, includeClassOrigin, propListPtr);
		BinarySerialization::write(ostrm, BinarySerialization::END_CLSENUM);
		BinarySerialization::write(ostrm, BinarySerialization::END_CLSENUM);
	}
	else
	{
		// instance path
		BinarySerialization::write(ostrm, BinarySerialization::BINSIG_INSTENUM);
		BinaryCIMInstanceWriter handler(ostrm, ns);
		chdl->references(ns, op, handler, resultClass,
			role, includeQualifiers, includeClassOrigin, propListPtr);
		BinarySerialization::write(ostrm, BinarySerialization::END_INSTENUM);
		BinarySerialization::write(ostrm, BinarySerialization::END_INSTENUM);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::referenceNames(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & istrm)
{
	String ns(BinarySerialization::readString(istrm));
	CIMObjectPath op(BinarySerialization::readObjectPath(istrm));
	String resultClass(BinarySerialization::readString(istrm));
	String role(BinarySerialization::readString(istrm));
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::write(ostrm, BinarySerialization::BINSIG_OPENUM);
	BinaryCIMObjectPathWriter handler(ostrm, getHost());
	chdl->referenceNames(ns, op, handler, resultClass, role);
	BinarySerialization::write(ostrm, BinarySerialization::END_OPENUM);
	BinarySerialization::write(ostrm, BinarySerialization::END_OPENUM);
}
#endif
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::getServerFeatures(const CIMOMHandleIFCRef & chdl,
	std::streambuf & ostrm, std::streambuf & /*istrm*/)
{
	CIMFeatures f = chdl->getServerFeatures();
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
	BinarySerialization::write(ostrm, Int32(f.cimProduct));
	BinarySerialization::writeString(ostrm, f.extURL);
	BinarySerialization::writeStringArray(ostrm, f.supportedGroups);
	BinarySerialization::writeBool(ostrm, f.supportsBatch);
	BinarySerialization::writeStringArray(ostrm, f.supportedQueryLanguages);
	BinarySerialization::writeString(ostrm, f.validation);
	BinarySerialization::writeString(ostrm, f.cimom);
	BinarySerialization::writeString(ostrm, f.protocolVersion);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::exportIndication(const CIMOMHandleIFCRef & chdl,
	std::streambuf& ostrm, std::streambuf& istrm)
{
	CIMInstance indication(BinarySerialization::readInstance(istrm));
	String instNS(BinarySerialization::readString(istrm));
	chdl->exportIndication(indication, instNS);
	BinarySerialization::write(ostrm, BinarySerialization::BIN_OK);
}
//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::writeError(std::streambuf & ostrm, const char * msg)
{
	BinarySerialization::write(ostrm, BinarySerialization::BIN_ERROR);
	BinarySerialization::write(ostrm, msg);
}
//////////////////////////////////////////////////////////////////////////////
StringArray
BinaryRequestHandler::getSupportedContentTypes() const
{
	StringArray rval;
	rval.push_back("application/x-owbinary");
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
String
BinaryRequestHandler::getContentType() const
{
	return String("application/x-owbinary");
}

//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::init(const ServiceEnvironmentIFCRef& env)
{
	setEnvironment(env);
}

//////////////////////////////////////////////////////////////////////////////
void
BinaryRequestHandler::shutdown()
{
}

} // end namespace OW_NAMESPACE

//////////////////////////////////////////////////////////////////////////////
OW_REQUEST_HANDLER_FACTORY(OW_NAMESPACE::BinaryRequestHandler,BinaryRequest);

