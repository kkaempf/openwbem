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
#include "OW_IOException.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMFeatures.hpp"

#include <exception>

//////////////////////////////////////////////////////////////////////////////
OW_BinaryRequestHandler::OW_BinaryRequestHandler()
	: OW_RequestHandlerIFC()
	, m_isError(false)
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
OW_Bool
OW_BinaryRequestHandler::doHasError()
{
	return m_isError;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::setEnvironment(OW_ServiceEnvironmentIFCRef env)
{
	OW_RequestHandlerIFC::setEnvironment(env);
	OW_String userName = env->getConfigItem(OW_BINARY_USER_NAME_OPT);
	if(userName.length())
	{
		if(!OW_OS::getUserId(userName, m_userId))
		{
			m_userId = OW_UserId(-1);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::doOptions(OW_CIMFeatures& cf)
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
OW_String
OW_BinaryRequestHandler::doGetId() const
{
	return OW_BINARY_ID;
}
//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::doProcess(std::istream* istrm, std::ostream *ostrm,
	std::ostream* ostrError, const OW_String& userName)
{
	OW_Int32 funcNo = 0;

	OW_Bool doIndications = !(getEnvironment()->getConfigItem(
		OW_ConfigOpts::DISABLE_INDICATIONS_opt).equalsIgnoreCase("true"));

	OW_LoggerRef lgr = getEnvironment()->getLogger();

	try
	{
		OW_CIMOMHandleIFCRef chdl = getEnvironment()->getCIMOMHandle(userName, doIndications);
		OW_BinIfcIO::read(*istrm, funcNo, OW_Bool(true));

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

				case OW_BIN_SETCLS:
					lgr->logDebug("OW_BinaryRequestHandler set class"
						" request");
					setClass(chdl, *ostrm, *istrm);
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

				case OW_BIN_SETINST:
					lgr->logDebug("OW_BinaryRequestHandler get instance"
						" request");
					setInstance(chdl, *ostrm, *istrm);
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
			OW_BinIfcIO::write(*ostrError, OW_Int32(OW_BIN_EXCEPTION),
				OW_Bool(true));
			OW_BinIfcIO::write(*ostrError, OW_Int32(e.getErrNo()),
				OW_Bool(true));
			OW_BinIfcIO::write(*ostrError, e.getMessage(),
				OW_Bool(true));
			m_isError = true;
		}
	}
	catch(OW_Exception& e)
	{
		lgr->logError("OW_Exception caught in OW_BinaryRequestHandler");
		lgr->logError(format("Type: %1", e.type()));
		lgr->logError(format("File: %1", e.getFile()));
		lgr->logError(format("Line: %1", e.getLine()));
		lgr->logError(format("Msg: %1", e.getMessage()));
		m_isError = true;
		
	}
	catch(std::exception& e)
	{
		lgr->logError(format("Caught %1 exception in OW_BinaryRequestHandler",
			e.what()));
		m_isError = true;
	}
	catch(...)
	{
		lgr->logError("Unknown exception caught in OW_BinaryRequestHandler");
		m_isError = true;
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
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
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
	chdl->createInstance(realPath, cimInstance);
	OW_CIMObjectPath newPath(cimInstance.getClassName(),
		cimInstance.getKeyValuePairs());
	newPath.setNameSpace(path.getNameSpace());

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	OW_BinIfcIO::writeObjectPath(ostrm, newPath);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumClasses(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_Bool deep(OW_BinIfcIO::readBool(istrm));
	OW_Bool localOnly(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeQualifiers(OW_BinIfcIO::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinIfcIO::readBool(istrm));

	OW_CIMClassEnumeration ccenum = chdl->enumClass(op, deep, localOnly,
		includeQualifiers, includeClassOrigin);

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BINSIG_CLSENUM, OW_Bool(true));

	OW_Bool enumWritten = false;
	if(ccenum.usingTempFile() && m_userId != OW_UserId(-1))
	{
		OW_String tfileName = ccenum.releaseFile();
		if(!(enumWritten = writeFileName(ostrm, tfileName)))
		{
			ccenum = OW_CIMClassEnumeration(tfileName);
		}
	}

	if(!enumWritten)
	{
		OW_BinIfcIO::write(ostrm, OW_Int32(ccenum.numberOfElements()),
			OW_Bool(true));

		while(ccenum.hasMoreElements())
		{
			OW_BinIfcIO::writeClass(ostrm, ccenum.nextElement());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::deleteClass(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	chdl->deleteClass(op);
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::deleteInstance(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	chdl->deleteInstance(op);
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::deleteQual(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	chdl->deleteQualifierType(op);
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
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

	if(!cc)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
			format("Path=%1", op.toString()).c_str());
	}

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
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

	if(!cimInstance)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
			format("Path=%1", op.toString()).c_str());
	}

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
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

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
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
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::setClass(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_CIMClass cc(OW_BinIfcIO::readClass(istrm));
	chdl->setClass(op, cc);
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::setInstance(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_CIMInstance ci(OW_BinIfcIO::readInstance(istrm));
	chdl->setInstance(op, ci);
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
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
    OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getProperty(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_String propName(OW_BinIfcIO::readString(istrm));

	OW_CIMValue cv = chdl->getProperty(op, propName);
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
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

	OW_CIMObjectPathEnumeration en = chdl->enumClassNames(op, deep);
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	writeObjectPathEnum(ostrm, en);
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

	OW_CIMInstanceEnumeration enu = chdl->enumInstances(op, deep, localOnly,
		includeQualifiers, includeClassOrigin, propListPtr);

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	writeInstanceEnum(ostrm, enu);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumInstanceNames(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_Bool deep(OW_BinIfcIO::readBool(istrm));
	OW_CIMObjectPathEnumeration en = chdl->enumInstanceNames(op, deep);
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	writeObjectPathEnum(ostrm, en);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumQualifiers(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_CIMQualifierTypeEnumeration en = chdl->enumQualifierTypes(op);
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BINSIG_QUALENUM, OW_Bool(true));

	OW_Bool enumWritten = false;
	if(en.usingTempFile() && m_userId != OW_UserId(-1))
	{
		OW_String tfileName = en.releaseFile();
		if(!(enumWritten = writeFileName(ostrm, tfileName)))
		{
			en = OW_CIMQualifierTypeEnumeration(tfileName);
		}
	}

	if(!enumWritten)
	{
		OW_BinIfcIO::write(ostrm, OW_Int32(en.numberOfElements()),
			OW_Bool(true));

		while(en.hasMoreElements())
		{
			OW_BinIfcIO::writeQual(ostrm, en.nextElement());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::invokeMethod(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_String methodName(OW_BinIfcIO::readString(istrm));
	OW_CIMValueArray inparms;
	OW_CIMValueArray outparms;

	// Get input params
	OW_BinIfcIO::verifySignature(istrm, (OW_Int32)OW_BINSIG_VALUEARRAY);
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size, OW_Bool(true));

	while(size)
	{
		inparms.append(OW_BinIfcIO::readValue(istrm));
		size--;
	}

	OW_CIMValue cv = chdl->invokeMethod(op, methodName, inparms, outparms);
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	if(cv)
	{
		OW_BinIfcIO::writeBool(ostrm, OW_Bool(true));
		OW_BinIfcIO::writeValue(ostrm, cv);
	}
	else
	{
		OW_BinIfcIO::writeBool(ostrm, OW_Bool(false));
	}

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BINSIG_VALUEARRAY, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)outparms.size(), OW_Bool(true));
	for(size_t i = 0; i < outparms.size(); i++)
	{
		OW_BinIfcIO::writeValue(ostrm, outparms[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::execQuery(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMNameSpace path(OW_BinIfcIO::readNameSpace(istrm));
	OW_String query(OW_BinIfcIO::readString(istrm));
	OW_String queryLang(OW_BinIfcIO::readString(istrm));

	OW_CIMInstanceArray instra = chdl->execQuery(path, query, queryLang);

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BINSIG_INSTARRAY, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)instra.size(), OW_Bool(true));
	for(size_t i = 0; i < instra.size(); i++)
	{
		OW_BinIfcIO::writeInstance(ostrm, instra[i]);
	}
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

	OW_CIMInstanceEnumeration en = chdl->associators(op, assocClass, resultClass,
		role, resultRole, includeQualifiers, includeClassOrigin, propListPtr);

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	writeInstanceEnum(ostrm, en);
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

	OW_CIMObjectPathEnumeration en = chdl->associatorNames(op, assocClass,
		resultClass, role, resultRole);

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	writeObjectPathEnum(ostrm, en);
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

	OW_CIMInstanceEnumeration en = chdl->references(op, resultClass, role,
		includeQualifiers, includeClassOrigin, propListPtr);

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	writeInstanceEnum(ostrm, en);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::referenceNames(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_CIMObjectPath op(OW_BinIfcIO::readObjectPath(istrm));
	OW_String resultClass(OW_BinIfcIO::readString(istrm));
	OW_String role(OW_BinIfcIO::readString(istrm));

	OW_CIMObjectPathEnumeration en = chdl->referenceNames(op, resultClass, role);
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	writeObjectPathEnum(ostrm, en);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getServerFeatures(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& /*istrm*/)
{
	OW_CIMFeatures f = chdl->getServerFeatures();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, OW_Int32(f.cimProduct), OW_Bool(true));
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
	if(OW_BinIfcIO::write(ostrm, (OW_Int32)(OW_Int32)OW_BIN_ERROR,
		OW_Bool(true)))
	{
		OW_BinIfcIO::write(ostrm, msg, OW_Bool(true));
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::writeObjectPathEnum(std::ostream& ostrm,
	OW_CIMObjectPathEnumeration& en)
{
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BINSIG_OPENUM, OW_Bool(true));

	OW_Bool enumWritten = false;
	if(en.usingTempFile() && m_userId != OW_UserId(-1))
	{
		OW_String tfileName = en.releaseFile();
		if(!(enumWritten = writeFileName(ostrm, tfileName)))
		{
			en = OW_CIMObjectPathEnumeration(tfileName);
		}
	}

	if(!enumWritten)
	{
		OW_BinIfcIO::write(ostrm, OW_Int32(en.numberOfElements()),
			OW_Bool(true));

		while(en.hasMoreElements())
		{
			OW_BinIfcIO::writeObjectPath(ostrm, en.nextElement());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::writeInstanceEnum(std::ostream& ostrm,
	OW_CIMInstanceEnumeration& enu)
{
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BINSIG_INSTENUM, OW_Bool(true));

	OW_Bool enumWritten = false;
	if(enu.usingTempFile() && m_userId != OW_UserId(-1))
	{
		OW_String tfileName = enu.releaseFile();
		if(!(enumWritten = writeFileName(ostrm, tfileName)))
		{
			enu = OW_CIMInstanceEnumeration(tfileName);
		}
	}

	if(!enumWritten)
	{
		OW_BinIfcIO::write(ostrm, OW_Int32(enu.numberOfElements()),
			OW_Bool(true));

		while(enu.hasMoreElements())
		{
			OW_CIMInstance cimInstance = enu.nextElement();
			OW_BinIfcIO::writeInstance(ostrm, cimInstance);
		}
	}
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
		OW_BinIfcIO::write(ostrm, OW_Int32(-1), OW_Bool(true));

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
	rval.push_back("application/owbinary");
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_String 
OW_BinaryRequestHandler::getContentType() const
{
	return OW_String("application/owbinary");
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
OW_REQUEST_HANDLER_FACTORY(OW_BinaryRequestHandler);
