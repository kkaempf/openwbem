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
#include "OW_ConfigOpts.hpp"
#include "OW_OS.hpp"
#include "OW_FileSystem.hpp"
#include "OW_BinaryRequestHandler.hpp"
#include "OW_BinIfcIO.hpp"
#include "OW_Format.hpp"
#include "OW_CIM.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMParamValue.hpp"

#include <exception>

//////////////////////////////////////////////////////////////////////////////
OW_BinaryRequestHandler::OW_BinaryRequestHandler()
	: OW_RequestHandlerIFC()
	, m_userId(OW_UserId(-1))
{
}

//////////////////////////////////////////////////////////////////////////////
OW_RequestHandlerIFC*
OW_BinaryRequestHandler::clone() const
{
	return new OW_BinaryRequestHandler();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::setEnvironment(OW_ServiceEnvironmentIFCRef env)
{
	OW_RequestHandlerIFC::setEnvironment(env);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::doOptions(OW_CIMFeatures& cf,
	const OW_SortedVectorMap<OW_String, OW_String>& /*handlerVars*/)
{
	cf.cimom = "openwbem";
	cf.cimProduct = OW_CIMFeatures::SERVER;
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
OW_BinaryRequestHandler::doProcess(std::istream* istrm, std::ostream *ostrm,
	std::ostream* ostrError, const OW_SortedVectorMap<OW_String, OW_String>& handlerVars)
{
	OW_String userName;
	OW_SortedVectorMap<OW_String, OW_String>::const_iterator i = handlerVars.find(OW_ConfigOpts::USER_NAME_opt);
	if (i != handlerVars.end())
	{
		userName = (*i).second;
	}
	if(!userName.empty())
	{
		if(!OW_OS::getUserId(userName, m_userId))
		{
			m_userId = OW_UserId(-1);
		}
	}

	OW_Int32 funcNo = 0;

	OW_Bool doIndications = !(getEnvironment()->getConfigItem(
		OW_ConfigOpts::DISABLE_INDICATIONS_opt).equalsIgnoreCase("true"));

	OW_LoggerRef lgr = getEnvironment()->getLogger();

	try
	{
		OW_CIMOMHandleIFCRef chdl = getEnvironment()->getCIMOMHandle(userName, doIndications);
		OW_BinIfcIO::read(*istrm, funcNo);

		try
		{
			switch(funcNo)
			{
				case OW_BIN_SETQUAL:
					lgr->logDebug("OW_BinaryRequestHandler set qualifier"
						" request");
					setQual(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_GETQUAL:
					lgr->logDebug("OW_BinaryRequestHandler get qualifier"
						" request");
					getQual(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_DELETEQUAL:
					lgr->logDebug("OW_BinaryRequestHandler delete qualifier"
						" request");
					deleteQual(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_CREATECLS:
					lgr->logDebug("OW_BinaryRequestHandler create class"
						" request");
					createClass(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_GETCLS:
					lgr->logDebug("OW_BinaryRequestHandler get class"
						" request");
					getClass(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_MODIFYCLS:
					lgr->logDebug("OW_BinaryRequestHandler modify class"
						" request");
					modifyClass(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_DELETECLS:
					lgr->logDebug("OW_BinaryRequestHandler delete class"
						" request");
					deleteClass(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_ENUMCLSS:
					lgr->logDebug("OW_BinaryRequestHandler enum classes"
						" request");
					enumClasses(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_CREATEINST:
					lgr->logDebug("OW_BinaryRequestHandler create instance"
						" request");
					createInstance(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_GETINST:
					lgr->logDebug("OW_BinaryRequestHandler get instance"
						" request");
					getInstance(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_MODIFYINST:
					lgr->logDebug("OW_BinaryRequestHandler get instance"
						" request");
					modifyInstance(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_DELETEINST:
					lgr->logDebug("OW_BinaryRequestHandler delete instance"
						" request");
					deleteInstance(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_GETPROP:
					lgr->logDebug("OW_BinaryRequestHandler get property"
						" request");
					getProperty(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_SETPROP:
					lgr->logDebug("OW_BinaryRequestHandler set property"
						" request");
					setProperty(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_ENUMCLSNAMES:
					lgr->logDebug("OW_BinaryRequestHandler enum class names"
						" request");
					enumClassNames(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_ENUMINSTS:
					lgr->logDebug("OW_BinaryRequestHandler enum instances"
						" request");
					enumInstances(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_ENUMINSTNAMES:
					lgr->logDebug("OW_BinaryRequestHandler enum instance"
						" names request");
					enumInstanceNames(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_ENUMQUALS:
					lgr->logDebug("OW_BinaryRequestHandler enum qualifiers"
						" request");
					enumQualifiers(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_INVMETH:
					lgr->logDebug("OW_BinaryRequestHandler invoke method"
						" request");
					invokeMethod(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_EXECQUERY:
					lgr->logDebug("OW_BinaryRequestHandler exec query"
						" request");
					execQuery(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_ASSOCIATORS:
					lgr->logDebug("OW_BinaryRequestHandler associators"
						" request");
					associators(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_ASSOCNAMES:
					lgr->logDebug("OW_BinaryRequestHandler associator names"
						" request");
					associatorNames(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_REFERENCES:
					lgr->logDebug("OW_BinaryRequestHandler references"
						" request");
					references(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_REFNAMES:
					lgr->logDebug("OW_BinaryRequestHandler reference names"
						" request");
					referenceNames(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_GETSVRFEATURES:
					lgr->logDebug("OW_BinaryRequestHandler get server"
						" features request");
					getServerFeatures(chdl, *ostrm, *istrm);
					break;

				default:
					lgr->logDebug(format("OW_BinaryRequestHandler: Received"
						" invalid function number: %1", funcNo));
					writeError(*ostrError, "Invalid function number");
					break;
			}
		}
		catch(OW_CIMException& e)
		{
			lgr->logDebug(format("CIM Exception caught in"
				" OW_BinaryRequestHandler: %1", e));
			OW_BinIfcIO::write(*ostrError, OW_BIN_EXCEPTION);
			OW_BinIfcIO::write(*ostrError, OW_Int32(e.getErrNo()));
			OW_BinIfcIO::write(*ostrError, e.getMessage());
			m_errorCode = e.getErrNo();
			m_errorDescription = e.getMessage();
			m_hasError = true;
		}
	}
	catch(OW_Exception& e)
	{
		lgr->logError("OW_Exception caught in OW_BinaryRequestHandler");
		lgr->logError(format("Type: %1", e.type()));
		lgr->logError(format("File: %1", e.getFile()));
		lgr->logError(format("Line: %1", e.getLine()));
		lgr->logError(format("Msg: %1", e.getMessage()));
		writeError(*ostrError, format("OW_BinaryRequestHandler caught exception: %1", e).c_str());
		m_errorCode = OW_CIMException::FAILED;
		m_errorDescription = e.getMessage();
		m_hasError = true;
		
	}
	catch(std::exception& e)
	{
		lgr->logError(format("Caught %1 exception in OW_BinaryRequestHandler",
			e.what()));
		writeError(*ostrError, format("OW_BinaryRequestHandler caught exception: %1", e.what()).c_str());
		m_errorCode = OW_CIMException::FAILED;
		m_errorDescription = e.what();
		m_hasError = true;
	}
	catch(...)
	{
		lgr->logError("Unknown exception caught in OW_BinaryRequestHandler");
		writeError(*ostrError, "OW_BinaryRequestHandler caught unknown exception");
		m_errorCode = OW_CIMException::FAILED;
		m_errorDescription = "Caught unknown exception";
		m_hasError = true;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::createClass(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_CIMClass cc(OW_BinIfcIO::readClass(istrm));
	chdl->createClass(op, cc);
	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::createInstance(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath path(OW_BinIfcIO::readObjectPath(istrm));
	OW_CIMInstance cimInstance(OW_BinIfcIO::readInstance(istrm));

	OW_String className = cimInstance.getClassName();
	OW_CIMObjectPath realPath(className, path.getNameSpace());

	// Special treatment for __Namespace class
	if(className.equals(OW_CIMClass::NAMESPACECLASS))
	{
		OW_CIMProperty prop = cimInstance.getProperty(
			OW_CIMProperty::NAME_PROPERTY);

		// Need the name property since it contains the name of the new
		// name space
		if (!prop)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Name property not specified for new namespace");
		}

		// If the name property didn't come acrossed as a key, then
		// set the name property as the key
		if(!prop.isKey())
		{
			prop.addQualifier(OW_CIMQualifier::createKeyQualifier());
		}

		cimInstance.setProperty(prop);
	}

	OW_CIMPropertyArray keys = cimInstance.getKeyValuePairs();
	if (keys.size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,"Instance doesn't have keys");
	}

	for (size_t i = 0; i < keys.size(); ++i)
	{
		OW_CIMProperty key = keys[i];
		if (!key.getValue())
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("Key must be provided!  Property \"%1\" does not have a "
					"valid value.", key.getName()).c_str());
		}
	}

	realPath.setKeys(keys);
	OW_CIMObjectPath newPath = chdl->createInstance(realPath, cimInstance);

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::writeObjectPath(ostrm, newPath);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class BinaryCIMClassWriter : public OW_CIMClassResultHandlerIFC
	{
	public:
		BinaryCIMClassWriter(std::ostream& ostrm_)
		: ostrm(ostrm_)
		{}
	protected:
		virtual void doHandle(const OW_CIMClass &c)
		{
			OW_BinIfcIO::writeClass(ostrm, c);
		}
	private:
		std::ostream& ostrm;
	};

	class BinaryCIMObjectPathWriter : public OW_CIMObjectPathResultHandlerIFC
	{
	public:
		BinaryCIMObjectPathWriter(std::ostream& ostrm_)
		: ostrm(ostrm_)
		{}
	protected:
		virtual void doHandle(const OW_CIMObjectPath &cop)
		{
			OW_BinIfcIO::writeObjectPath(ostrm, cop);
		}
	private:
		std::ostream& ostrm;
	};

	class BinaryCIMInstanceWriter : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		BinaryCIMInstanceWriter(std::ostream& ostrm_)
		: ostrm(ostrm_)
		{}
	protected:
		virtual void doHandle(const OW_CIMInstance &cop)
		{
			OW_BinIfcIO::writeInstance(ostrm, cop);
		}
	private:
		std::ostream& ostrm;
	};

	class BinaryCIMQualifierTypeWriter : public OW_CIMQualifierTypeResultHandlerIFC
	{
	public:
		BinaryCIMQualifierTypeWriter(std::ostream& ostrm_)
		: ostrm(ostrm_)
		{}
	protected:
		virtual void doHandle(const OW_CIMQualifierType &cqt)
		{
			OW_BinIfcIO::writeQual(ostrm, cqt);
		}
	private:
		std::ostream& ostrm;
	};

}
//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumClasses(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinIfcIO::readString(istrm));
	OW_String className(OW_BinIfcIO::readString(istrm));
	OW_Bool deep(OW_BinIfcIO::readBool(istrm));
	OW_Bool localOnly(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeQualifiers(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinIfcIO::readBool(istrm));

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::write(ostrm, OW_BINSIG_CLSENUM);

	BinaryCIMClassWriter handler(ostrm);
	chdl->enumClass(ns, className, handler, deep, localOnly,
		includeQualifiers, includeClassOrigin);
	
	OW_BinIfcIO::write(ostrm, OW_END_CLSENUM);
	OW_BinIfcIO::write(ostrm, OW_END_CLSENUM);

}

//////////////////////////////////////////////////////////////////////////////

void
OW_BinaryRequestHandler::deleteClass(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinIfcIO::readString(istrm));
	OW_String className(OW_BinIfcIO::readString(istrm));
	chdl->deleteClass(ns, className);
	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::deleteInstance(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinIfcIO::readString(istrm));
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	chdl->deleteInstance(ns, op);
	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::deleteQual(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinIfcIO::readString(istrm));
	OW_String qualName(OW_BinIfcIO::readString(istrm));
	chdl->deleteQualifierType(ns, qualName);
	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getClass(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_StringArray propList;
	OW_StringArray* propListPtr = 0;

	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_Bool localOnly(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeQualifiers(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinIfcIO::readBool(istrm));
	OW_Bool nullPropertyList(OW_BinIfcIO::readBool(istrm));
	if(!nullPropertyList)
	{
		propList = OW_BinIfcIO::readStringArray(istrm);
		propListPtr = &propList;
	}

	OW_CIMClass cc = chdl->getClass(op, localOnly, includeQualifiers,
		includeClassOrigin, propListPtr);

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::writeClass(ostrm, cc);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getInstance(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_StringArray propList;
	OW_StringArray* propListPtr = 0;

	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_Bool localOnly(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeQualifiers(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinIfcIO::readBool(istrm));
	OW_Bool nullPropertyList(OW_BinIfcIO::readBool(istrm));
	if(!nullPropertyList)
	{
		propList = OW_BinIfcIO::readStringArray(istrm);
		propListPtr = &propList;
	}

	OW_CIMInstance cimInstance = chdl->getInstance(op, localOnly,
		includeQualifiers, includeClassOrigin, propListPtr);

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::writeInstance(ostrm, cimInstance);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getQual(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_CIMQualifierType qt = chdl->getQualifierType(op);
	if(!qt)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
			format("Path=%1", op.toString()).c_str());
	}

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::writeQual(ostrm, qt);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::setQual(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_CIMQualifierType qt(OW_BinIfcIO::readQual(istrm));
	chdl->setQualifierType(op, qt);
	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::modifyClass(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_CIMClass cc(OW_BinIfcIO::readClass(istrm));
	chdl->modifyClass(op, cc);
	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::modifyInstance(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_CIMInstance ci(OW_BinIfcIO::readInstance(istrm));
	chdl->modifyInstance(op, ci);
	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::setProperty(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_String propName(OW_BinIfcIO::readString(istrm));
	OW_Bool isValue(OW_BinIfcIO::readBool(istrm));
	OW_CIMValue cv;
	if(isValue)
	{
		cv = OW_BinIfcIO::readValue(istrm);
	}

	chdl->setProperty(op, propName, cv);
    OW_BinIfcIO::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getProperty(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_String propName(OW_BinIfcIO::readString(istrm));

	OW_CIMValue cv = chdl->getProperty(op, propName);
	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_Bool isValue = (cv) ? true : false;
	OW_BinIfcIO::writeBool(ostrm, isValue);
	if(isValue)
	{
		OW_BinIfcIO::writeValue(ostrm, cv);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumClassNames(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_Bool deep(OW_BinIfcIO::readBool(istrm));

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::write(ostrm, OW_BINSIG_OPENUM);
	BinaryCIMObjectPathWriter handler(ostrm);
	chdl->enumClassNames(op, handler, deep);

	OW_BinIfcIO::write(ostrm, OW_END_OPENUM);
	OW_BinIfcIO::write(ostrm, OW_END_OPENUM);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumInstances(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_StringArray propList;
	OW_StringArray* propListPtr = 0;

	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_Bool deep(OW_BinIfcIO::readBool(istrm));
	OW_Bool localOnly(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeQualifiers(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinIfcIO::readBool(istrm));
	OW_Bool nullPropertyList(OW_BinIfcIO::readBool(istrm));
	if(!nullPropertyList)
	{
		propList = OW_BinIfcIO::readStringArray(istrm);
		propListPtr = &propList;
	}

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::write(ostrm, OW_BINSIG_INSTENUM);
	
	BinaryCIMInstanceWriter handler(ostrm);
	chdl->enumInstances(op, handler, deep, localOnly,
		includeQualifiers, includeClassOrigin, propListPtr);

	OW_BinIfcIO::write(ostrm, OW_END_INSTENUM);
	OW_BinIfcIO::write(ostrm, OW_END_INSTENUM);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumInstanceNames(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::write(ostrm, OW_BINSIG_OPENUM);
	BinaryCIMObjectPathWriter handler(ostrm);
	chdl->enumInstanceNames(op, handler);

	OW_BinIfcIO::write(ostrm, OW_END_OPENUM);
	OW_BinIfcIO::write(ostrm, OW_END_OPENUM);

}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumQualifiers(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::write(ostrm, OW_BINSIG_QUALENUM);
	BinaryCIMQualifierTypeWriter handler(ostrm);
	chdl->enumQualifierTypes(op, handler);

	OW_BinIfcIO::write(ostrm, OW_END_QUALENUM);
	OW_BinIfcIO::write(ostrm, OW_END_QUALENUM);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::invokeMethod(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_String methodName(OW_BinIfcIO::readString(istrm));
	OW_CIMParamValueArray inparms;
	OW_CIMParamValueArray outparms;

	// Get input params
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_PARAMVALUEARRAY);
	inparms.readObject(istrm);

	OW_CIMValue cv = chdl->invokeMethod(op, methodName, inparms, outparms);
	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	if(cv)
	{
		OW_BinIfcIO::writeBool(ostrm, OW_Bool(true));
		OW_BinIfcIO::writeValue(ostrm, cv);
	}
	else
	{
		OW_BinIfcIO::writeBool(ostrm, OW_Bool(false));
	}

	OW_BinIfcIO::write(ostrm, OW_BINSIG_PARAMVALUEARRAY);
	outparms.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::execQuery(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMNameSpace path(OW_BinIfcIO::readNameSpace(istrm));
	OW_String query(OW_BinIfcIO::readString(istrm));
	OW_String queryLang(OW_BinIfcIO::readString(istrm));

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::write(ostrm, OW_BINSIG_INSTENUM);
	BinaryCIMInstanceWriter handler(ostrm);
	chdl->execQuery(path, handler, query, queryLang);

	OW_BinIfcIO::write(ostrm, OW_END_INSTENUM);
	OW_BinIfcIO::write(ostrm, OW_END_INSTENUM);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::associators(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_StringArray propList;
	OW_StringArray* propListPtr = 0;

	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_String assocClass(OW_BinIfcIO::readString(istrm));
	OW_String resultClass(OW_BinIfcIO::readString(istrm));
	OW_String role(OW_BinIfcIO::readString(istrm));
	OW_String resultRole(OW_BinIfcIO::readString(istrm));
	OW_Bool includeQualifiers(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinIfcIO::readBool(istrm));
	OW_Bool nullPropertyList(OW_BinIfcIO::readBool(istrm));
	if(!nullPropertyList)
	{
		propList = OW_BinIfcIO::readStringArray(istrm);
		propListPtr = &propList;
	}

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	if (op.getKeys().size() == 0)
	{
		// class path
		OW_BinIfcIO::write(ostrm, OW_BINSIG_CLSENUM);
		BinaryCIMClassWriter handler(ostrm);
		chdl->associatorsClasses(op, handler, assocClass, resultClass,
			role, resultRole, includeQualifiers, includeClassOrigin, propListPtr);
		OW_BinIfcIO::write(ostrm, OW_END_CLSENUM);
		OW_BinIfcIO::write(ostrm, OW_END_CLSENUM);
	}
	else
	{
		// instance path
		OW_BinIfcIO::write(ostrm, OW_BINSIG_INSTENUM);
		BinaryCIMInstanceWriter handler(ostrm);
		chdl->associators(op, handler, assocClass, resultClass,
			role, resultRole, includeQualifiers, includeClassOrigin, propListPtr);
		OW_BinIfcIO::write(ostrm, OW_END_INSTENUM);
		OW_BinIfcIO::write(ostrm, OW_END_INSTENUM);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::associatorNames(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_String assocClass(OW_BinIfcIO::readString(istrm));
	OW_String resultClass(OW_BinIfcIO::readString(istrm));
	OW_String role(OW_BinIfcIO::readString(istrm));
	OW_String resultRole(OW_BinIfcIO::readString(istrm));

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::write(ostrm, OW_BINSIG_OPENUM);
	BinaryCIMObjectPathWriter handler(ostrm);
	chdl->associatorNames(op, handler, assocClass,
		resultClass, role, resultRole);
	OW_BinIfcIO::write(ostrm, OW_END_OPENUM);
	OW_BinIfcIO::write(ostrm, OW_END_OPENUM);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::references(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_StringArray propList;
	OW_StringArray* propListPtr = 0;

	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_String resultClass(OW_BinIfcIO::readString(istrm));
	OW_String role(OW_BinIfcIO::readString(istrm));
	OW_Bool includeQualifiers(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinIfcIO::readBool(istrm));
	OW_Bool nullPropertyList(OW_BinIfcIO::readBool(istrm));
	if(!nullPropertyList)
	{
		propList = OW_BinIfcIO::readStringArray(istrm);
		propListPtr = &propList;
	}

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	if (op.getKeys().size() == 0)
	{
		// class path
		OW_BinIfcIO::write(ostrm, OW_BINSIG_CLSENUM);
		BinaryCIMClassWriter handler(ostrm);
		chdl->referencesClasses(op, handler, resultClass,
			role, includeQualifiers, includeClassOrigin, propListPtr);
		OW_BinIfcIO::write(ostrm, OW_END_CLSENUM);
		OW_BinIfcIO::write(ostrm, OW_END_CLSENUM);
	}
	else
	{
		// instance path
		OW_BinIfcIO::write(ostrm, OW_BINSIG_INSTENUM);
		BinaryCIMInstanceWriter handler(ostrm);
		chdl->references(op, handler, resultClass,
			role, includeQualifiers, includeClassOrigin, propListPtr);
		OW_BinIfcIO::write(ostrm, OW_END_INSTENUM);
		OW_BinIfcIO::write(ostrm, OW_END_INSTENUM);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::referenceNames(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_String resultClass(OW_BinIfcIO::readString(istrm));
	OW_String role(OW_BinIfcIO::readString(istrm));

	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::write(ostrm, OW_BINSIG_OPENUM);
	BinaryCIMObjectPathWriter handler(ostrm);
	chdl->referenceNames(op, handler, resultClass, role);
	OW_BinIfcIO::write(ostrm, OW_END_OPENUM);
	OW_BinIfcIO::write(ostrm, OW_END_OPENUM);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getServerFeatures(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& /*istrm*/)
{
	OW_CIMFeatures f = chdl->getServerFeatures();
	OW_BinIfcIO::write(ostrm, OW_BIN_OK);
	OW_BinIfcIO::write(ostrm, OW_Int32(f.cimProduct));
	OW_BinIfcIO::writeString(ostrm, f.extURL);
	OW_BinIfcIO::writeStringArray(ostrm, f.supportedGroups);
	OW_BinIfcIO::writeBool(ostrm, f.supportsBatch);
	OW_BinIfcIO::writeStringArray(ostrm, f.supportedQueryLanguages);
	OW_BinIfcIO::writeString(ostrm, f.validation);
	OW_BinIfcIO::writeString(ostrm, f.cimom);
	OW_BinIfcIO::writeString(ostrm, f.protocolVersion);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::writeError(std::ostream& ostrm, const char* msg)
{
	OW_BinIfcIO::write(ostrm, OW_BIN_ERROR);
	OW_BinIfcIO::write(ostrm, msg);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_BinaryRequestHandler::writeFileName(std::ostream& ostrm,
	const OW_String& fname)
{
	OW_LoggerRef lgr = getEnvironment()->getLogger();
	if(m_userId == OW_UserId(-1))
	{
		lgr->logError("Binary request handler cannot change file ownership:"
			" Owner unknown");
		return false;
	}

	try
	{
		if(OW_FileSystem::changeFileOwner(fname, m_userId) != 0)
		{
			lgr->logError(format("Binary request handler failed changing"
				" ownership on file %1", fname));
			return false;
		}

		// Write -1 to indicate file name follows
		OW_BinIfcIO::write(ostrm, OW_Int32(-1));

		// Write file name
		OW_BinIfcIO::writeString(ostrm, fname);
	}
	catch(...)
	{
		OW_FileSystem::removeFile(fname);
		throw;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_BinaryRequestHandler::getSupportedContentTypes() const
{
	OW_StringArray rval;
	rval.push_back("application/x-owbinary");
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_BinaryRequestHandler::getContentType() const
{
	return OW_String("application/x-owbinary");
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
OW_REQUEST_HANDLER_FACTORY(OW_BinaryRequestHandler);
