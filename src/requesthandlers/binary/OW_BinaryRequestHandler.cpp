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
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_BinaryRequestHandler.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_Format.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_SocketUtils.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_SocketException.hpp"
#include "OW_ThreadCancelledException.hpp"

extern "C"
{
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
}

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
	return new OW_BinaryRequestHandler(*this);
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

static OW_Mutex g_pwdLock;

//////////////////////////////////////////////////////////////////////////////
static OW_Bool
getUserId(const OW_String& userName, OW_UserId& userId)
{
	OW_MutexLock ml(g_pwdLock);
	OW_Bool rv = false;
	struct passwd* pwd;
	pwd = ::getpwnam(userName.c_str());
	if(pwd)
	{
		userId = pwd->pw_uid;
		rv = true;
	}
	return rv;
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
		if(!getUserId(userName, m_userId))
		{
			m_userId = OW_UserId(-1);
		}
	}

	OW_UInt8 funcNo = 0;

	OW_Bool doIndications = !(getEnvironment()->getConfigItem(
		OW_ConfigOpts::DISABLE_INDICATIONS_opt).equalsIgnoreCase("true"));

	OW_LoggerRef lgr = getEnvironment()->getLogger();

	try
	{
		OW_CIMOMHandleIFCRef chdl = getEnvironment()->getCIMOMHandle(userName, doIndications);
		OW_UInt32 version = 0;
		OW_BinarySerialization::read(*istrm, version);
		if (version != OW_BinaryProtocolVersion)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Incompatible version");
		}

		OW_BinarySerialization::read(*istrm, funcNo);

		try
		{
			switch(funcNo)
			{
				case OW_BIN_GETQUAL:
					lgr->logDebug("OW_BinaryRequestHandler get qualifier"
						" request");
					getQual(chdl, *ostrm, *istrm);
					break;

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
				case OW_BIN_SETQUAL:
					lgr->logDebug("OW_BinaryRequestHandler set qualifier"
						" request");
					setQual(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_DELETEQUAL:
					lgr->logDebug("OW_BinaryRequestHandler delete qualifier"
						" request");
					deleteQual(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_ENUMQUALS:
					lgr->logDebug("OW_BinaryRequestHandler enum qualifiers"
						" request");
					enumQualifiers(chdl, *ostrm, *istrm);
					break;
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

				case OW_BIN_GETCLS:
					lgr->logDebug("OW_BinaryRequestHandler get class"
						" request");
					getClass(chdl, *ostrm, *istrm);
					break;

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
				case OW_BIN_CREATECLS:
					lgr->logDebug("OW_BinaryRequestHandler create class"
						" request");
					createClass(chdl, *ostrm, *istrm);
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
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

				case OW_BIN_ENUMCLSS:
					lgr->logDebug("OW_BinaryRequestHandler enum classes"
						" request");
					enumClasses(chdl, *ostrm, *istrm);
					break;

				case OW_BIN_GETINST:
					lgr->logDebug("OW_BinaryRequestHandler get instance"
						" request");
					getInstance(chdl, *ostrm, *istrm);
					break;

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
				case OW_BIN_CREATEINST:
					lgr->logDebug("OW_BinaryRequestHandler create instance"
						" request");
					createInstance(chdl, *ostrm, *istrm);
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

				case OW_BIN_SETPROP:
					lgr->logDebug("OW_BinaryRequestHandler set property"
						" request");
					setProperty(chdl, *ostrm, *istrm);
					break;
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

				case OW_BIN_GETPROP:
					lgr->logDebug("OW_BinaryRequestHandler get property"
						" request");
					getProperty(chdl, *ostrm, *istrm);
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

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
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
#endif

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
			OW_BinarySerialization::write(*ostrError, OW_BIN_EXCEPTION);
			OW_BinarySerialization::write(*ostrError, OW_UInt16(e.getErrNo()));
			OW_BinarySerialization::write(*ostrError, e.getMessage());
			setError(e.getErrNo(), e.getMessage());
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
		setError(OW_CIMException::FAILED, e.getMessage());
		
	}
	catch(std::exception& e)
	{
		lgr->logError(format("Caught %1 exception in OW_BinaryRequestHandler",
			e.what()));
		writeError(*ostrError, format("OW_BinaryRequestHandler caught exception: %1", e.what()).c_str());
		setError(OW_CIMException::FAILED, e.what());
	}
	catch (OW_ThreadCancelledException&)
	{
		lgr->logError("Thread cancelled in OW_BinaryRequestHandler");
		writeError(*ostrError, "Thread cancelled");
		setError(OW_CIMException::FAILED, "Thread cancelled");
		throw;
	}
	catch(...)
	{
		lgr->logError("Unknown exception caught in OW_BinaryRequestHandler");
		writeError(*ostrError, "OW_BinaryRequestHandler caught unknown exception");
		setError(OW_CIMException::FAILED, "Caught unknown exception");
	}
}

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::createClass(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMClass cc(OW_BinarySerialization::readClass(istrm));
	chdl->createClass(ns, cc);
	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::modifyClass(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMClass cc(OW_BinarySerialization::readClass(istrm));
	chdl->modifyClass(ns, cc);
	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::deleteClass(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_String className(OW_BinarySerialization::readString(istrm));
	chdl->deleteClass(ns, className);
	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

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
			OW_BinarySerialization::writeClass(ostrm, c);
		}
	private:
		std::ostream& ostrm;
	};

	class BinaryCIMObjectPathWriter : public OW_CIMObjectPathResultHandlerIFC
	{
	public:
		BinaryCIMObjectPathWriter(std::ostream& ostrm_, const OW_String& host_)
		: ostrm(ostrm_)
		, m_host(host_)
		{}
	protected:
		virtual void doHandle(const OW_CIMObjectPath &cop_)
		{
			// Make sure all outgoing object paths have our host name, instead of 127.0.0.1
			OW_CIMObjectPath cop(cop_);
			if (cop.getFullNameSpace().isLocal())
			{
				try
				{
					cop.setHost(m_host);
				}
				catch (const OW_SocketException& e)
				{
				}
			}

			OW_BinarySerialization::writeObjectPath(ostrm, cop);
		}
	private:
		std::ostream& ostrm;
		OW_String m_host;
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
			OW_BinarySerialization::writeInstance(ostrm, cop);
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
			OW_BinarySerialization::writeQual(ostrm, cqt);
		}
	private:
		std::ostream& ostrm;
	};

	class BinaryStringWriter : public OW_StringResultHandlerIFC
	{
	public:
		BinaryStringWriter(std::ostream& ostrm_)
		: ostrm(ostrm_)
		{}
	protected:
		virtual void doHandle(const OW_String &name)
		{
			OW_BinarySerialization::writeString(ostrm, name);
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
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_String className(OW_BinarySerialization::readString(istrm));
	OW_Bool deep(OW_BinarySerialization::readBool(istrm));
	OW_Bool localOnly(OW_BinarySerialization::readBool(istrm));
	OW_Bool includeQualifiers(OW_BinarySerialization::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinarySerialization::readBool(istrm));

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::write(ostrm, OW_BINSIG_CLSENUM);

	BinaryCIMClassWriter handler(ostrm);
	chdl->enumClass(ns, className, handler, deep, localOnly,
		includeQualifiers, includeClassOrigin);
	
	OW_BinarySerialization::write(ostrm, OW_END_CLSENUM);
	OW_BinarySerialization::write(ostrm, OW_END_CLSENUM);

}

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::deleteQual(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_String qualName(OW_BinarySerialization::readString(istrm));
	chdl->deleteQualifierType(ns, qualName);
	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::setQual(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMQualifierType qt(OW_BinarySerialization::readQual(istrm));
	chdl->setQualifierType(ns, qt);
	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumQualifiers(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::write(ostrm, OW_BINSIG_QUALENUM);
	BinaryCIMQualifierTypeWriter handler(ostrm);
	chdl->enumQualifierTypes(ns, handler);

	OW_BinarySerialization::write(ostrm, OW_END_QUALENUM);
	OW_BinarySerialization::write(ostrm, OW_END_QUALENUM);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getClass(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_StringArray propList;
	OW_StringArray* propListPtr = 0;

	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_String className(OW_BinarySerialization::readString(istrm));
	OW_Bool localOnly(OW_BinarySerialization::readBool(istrm));
	OW_Bool includeQualifiers(OW_BinarySerialization::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinarySerialization::readBool(istrm));
	OW_Bool nullPropertyList(OW_BinarySerialization::readBool(istrm));
	if(!nullPropertyList)
	{
		propList = OW_BinarySerialization::readStringArray(istrm);
		propListPtr = &propList;
	}

	OW_CIMClass cc = chdl->getClass(ns, className, localOnly, includeQualifiers,
		includeClassOrigin, propListPtr);

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::writeClass(ostrm, cc);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getInstance(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMObjectPath op(OW_BinarySerialization::readObjectPath(istrm));
	OW_Bool localOnly(OW_BinarySerialization::readBool(istrm));
	OW_Bool includeQualifiers(OW_BinarySerialization::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinarySerialization::readBool(istrm));
	OW_StringArray propList;
	OW_StringArray* propListPtr = 0;
	OW_Bool nullPropertyList(OW_BinarySerialization::readBool(istrm));
	if(!nullPropertyList)
	{
		propList = OW_BinarySerialization::readStringArray(istrm);
		propListPtr = &propList;
	}

	OW_CIMInstance cimInstance = chdl->getInstance(ns, op, localOnly,
		includeQualifiers, includeClassOrigin, propListPtr);

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::writeInstance(ostrm, cimInstance);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getQual(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_String qualifierName(OW_BinarySerialization::readString(istrm));
	OW_CIMQualifierType qt = chdl->getQualifierType(ns, qualifierName);

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::writeQual(ostrm, qt);
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::createInstance(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMInstance cimInstance(OW_BinarySerialization::readInstance(istrm));

	OW_String className = cimInstance.getClassName();
	//OW_CIMObjectPath realPath(className, path.getNameSpace());

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

	/* This should be done in the CIM Server
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
	*/

	OW_CIMObjectPath newPath = chdl->createInstance(ns, cimInstance);

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::writeObjectPath(ostrm, newPath);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::deleteInstance(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMObjectPath op(OW_BinarySerialization::readObjectPath(istrm));
	chdl->deleteInstance(ns, op);
	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::modifyInstance(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMInstance ci(OW_BinarySerialization::readInstance(istrm));
	OW_Bool includeQualifiers(OW_BinarySerialization::readBool(istrm));
	OW_StringArray propList;
	OW_StringArray* propListPtr = 0;
	OW_Bool nullPropertyList(OW_BinarySerialization::readBool(istrm));
	if(!nullPropertyList)
	{
		propList = OW_BinarySerialization::readStringArray(istrm);
		propListPtr = &propList;
	}

	chdl->modifyInstance(ns, ci, includeQualifiers, propListPtr);
	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::setProperty(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMObjectPath op(OW_BinarySerialization::readObjectPath(istrm));
	OW_String propName(OW_BinarySerialization::readString(istrm));
	OW_Bool isValue(OW_BinarySerialization::readBool(istrm));
	OW_CIMValue cv(OW_CIMNULL);
	if(isValue)
	{
		cv = OW_BinarySerialization::readValue(istrm);
	}

	chdl->setProperty(ns, op, propName, cv);
	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getProperty(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns (OW_BinarySerialization::readString(istrm));
	OW_CIMObjectPath op(OW_BinarySerialization::readObjectPath(istrm));
	OW_String propName(OW_BinarySerialization::readString(istrm));

	OW_CIMValue cv = chdl->getProperty(ns, op, propName);
	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_Bool isValue = (cv) ? true : false;
	OW_BinarySerialization::writeBool(ostrm, isValue);
	if(isValue)
	{
		OW_BinarySerialization::writeValue(ostrm, cv);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumClassNames(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_String className(OW_BinarySerialization::readString(istrm));
	OW_Bool deep(OW_BinarySerialization::readBool(istrm));

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::write(ostrm, OW_BINSIG_STRINGENUM);
	BinaryStringWriter handler(ostrm);
	chdl->enumClassNames(ns, className, handler, deep);

	OW_BinarySerialization::write(ostrm, OW_END_STRINGENUM);
	OW_BinarySerialization::write(ostrm, OW_END_STRINGENUM);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumInstances(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_StringArray propList;
	OW_StringArray* propListPtr = 0;

	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_String className(OW_BinarySerialization::readString(istrm));
	OW_Bool deep(OW_BinarySerialization::readBool(istrm));
	OW_Bool localOnly(OW_BinarySerialization::readBool(istrm));
	OW_Bool includeQualifiers(OW_BinarySerialization::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinarySerialization::readBool(istrm));
	OW_Bool nullPropertyList(OW_BinarySerialization::readBool(istrm));
	if(!nullPropertyList)
	{
		propList = OW_BinarySerialization::readStringArray(istrm);
		propListPtr = &propList;
	}

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::write(ostrm, OW_BINSIG_INSTENUM);
	
	BinaryCIMInstanceWriter handler(ostrm);
	chdl->enumInstances(ns, className, handler, deep, localOnly,
		includeQualifiers, includeClassOrigin, propListPtr);

	OW_BinarySerialization::write(ostrm, OW_END_INSTENUM);
	OW_BinarySerialization::write(ostrm, OW_END_INSTENUM);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::enumInstanceNames(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_String className(OW_BinarySerialization::readString(istrm));

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::write(ostrm, OW_BINSIG_OPENUM);
	BinaryCIMObjectPathWriter handler(ostrm, getHost());
	chdl->enumInstanceNames(ns, className, handler);

	OW_BinarySerialization::write(ostrm, OW_END_OPENUM);
	OW_BinarySerialization::write(ostrm, OW_END_OPENUM);

}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::invokeMethod(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns (OW_BinarySerialization::readString(istrm));
	OW_CIMObjectPath path(OW_BinarySerialization::readObjectPath(istrm));
	OW_String methodName(OW_BinarySerialization::readString(istrm));
	OW_CIMParamValueArray inparms;
	OW_CIMParamValueArray outparms;

	// Get input params
	OW_BinarySerialization::verifySignature(istrm, OW_BINSIG_PARAMVALUEARRAY);
	OW_BinarySerialization::readArray(istrm, inparms);

	OW_CIMValue cv = chdl->invokeMethod(ns, path, methodName, inparms, outparms);
	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	if(cv)
	{
		OW_BinarySerialization::writeBool(ostrm, OW_Bool(true));
		OW_BinarySerialization::writeValue(ostrm, cv);
	}
	else
	{
		OW_BinarySerialization::writeBool(ostrm, OW_Bool(false));
	}

	OW_BinarySerialization::write(ostrm, OW_BINSIG_PARAMVALUEARRAY);
	OW_BinarySerialization::writeArray(ostrm, outparms);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::execQuery(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_String query(OW_BinarySerialization::readString(istrm));
	OW_String queryLang(OW_BinarySerialization::readString(istrm));

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::write(ostrm, OW_BINSIG_INSTENUM);
	BinaryCIMInstanceWriter handler(ostrm);
	chdl->execQuery(ns, handler, query, queryLang);

	OW_BinarySerialization::write(ostrm, OW_END_INSTENUM);
	OW_BinarySerialization::write(ostrm, OW_END_INSTENUM);
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::associators(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_StringArray propList;
	OW_StringArray* propListPtr = 0;

	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMObjectPath op(OW_BinarySerialization::readObjectPath(istrm));
	OW_String assocClass(OW_BinarySerialization::readString(istrm));
	OW_String resultClass(OW_BinarySerialization::readString(istrm));
	OW_String role(OW_BinarySerialization::readString(istrm));
	OW_String resultRole(OW_BinarySerialization::readString(istrm));
	OW_Bool includeQualifiers(OW_BinarySerialization::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinarySerialization::readBool(istrm));
	OW_Bool nullPropertyList(OW_BinarySerialization::readBool(istrm));
	if(!nullPropertyList)
	{
		propList = OW_BinarySerialization::readStringArray(istrm);
		propListPtr = &propList;
	}

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	if (op.isClassPath())
	{
		// class path
		OW_BinarySerialization::write(ostrm, OW_BINSIG_CLSENUM);
		BinaryCIMClassWriter handler(ostrm);
		op.setNameSpace(ns);
		chdl->associatorsClasses(ns, op, handler, assocClass, resultClass,
			role, resultRole, includeQualifiers, includeClassOrigin, propListPtr);
		OW_BinarySerialization::write(ostrm, OW_END_CLSENUM);
		OW_BinarySerialization::write(ostrm, OW_END_CLSENUM);
	}
	else
	{
		// instance path
		OW_BinarySerialization::write(ostrm, OW_BINSIG_INSTENUM);
		BinaryCIMInstanceWriter handler(ostrm);
		chdl->associators(ns, op, handler, assocClass, resultClass,
			role, resultRole, includeQualifiers, includeClassOrigin, propListPtr);
		OW_BinarySerialization::write(ostrm, OW_END_INSTENUM);
		OW_BinarySerialization::write(ostrm, OW_END_INSTENUM);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::associatorNames(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMObjectPath op(OW_BinarySerialization::readObjectPath(istrm));
	OW_String assocClass(OW_BinarySerialization::readString(istrm));
	OW_String resultClass(OW_BinarySerialization::readString(istrm));
	OW_String role(OW_BinarySerialization::readString(istrm));
	OW_String resultRole(OW_BinarySerialization::readString(istrm));

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::write(ostrm, OW_BINSIG_OPENUM);
	BinaryCIMObjectPathWriter handler(ostrm, getHost());
	chdl->associatorNames(ns, op, handler, assocClass,
		resultClass, role, resultRole);
	OW_BinarySerialization::write(ostrm, OW_END_OPENUM);
	OW_BinarySerialization::write(ostrm, OW_END_OPENUM);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::references(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_StringArray propList;
	OW_StringArray* propListPtr = 0;

	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMObjectPath op(OW_BinarySerialization::readObjectPath(istrm));
	OW_String resultClass(OW_BinarySerialization::readString(istrm));
	OW_String role(OW_BinarySerialization::readString(istrm));
	OW_Bool includeQualifiers(OW_BinarySerialization::readBool(istrm));
	OW_Bool includeClassOrigin(OW_BinarySerialization::readBool(istrm));
	OW_Bool nullPropertyList(OW_BinarySerialization::readBool(istrm));
	if(!nullPropertyList)
	{
		propList = OW_BinarySerialization::readStringArray(istrm);
		propListPtr = &propList;
	}

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	if (op.isClassPath())
	{
		// class path
		OW_BinarySerialization::write(ostrm, OW_BINSIG_CLSENUM);
		BinaryCIMClassWriter handler(ostrm);
		chdl->referencesClasses(ns, op, handler, resultClass,
			role, includeQualifiers, includeClassOrigin, propListPtr);
		OW_BinarySerialization::write(ostrm, OW_END_CLSENUM);
		OW_BinarySerialization::write(ostrm, OW_END_CLSENUM);
	}
	else
	{
		// instance path
		OW_BinarySerialization::write(ostrm, OW_BINSIG_INSTENUM);
		BinaryCIMInstanceWriter handler(ostrm);
		chdl->references(ns, op, handler, resultClass,
			role, includeQualifiers, includeClassOrigin, propListPtr);
		OW_BinarySerialization::write(ostrm, OW_END_INSTENUM);
		OW_BinarySerialization::write(ostrm, OW_END_INSTENUM);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::referenceNames(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& istrm)
{
	OW_String ns(OW_BinarySerialization::readString(istrm));
	OW_CIMObjectPath op(OW_BinarySerialization::readObjectPath(istrm));
	OW_String resultClass(OW_BinarySerialization::readString(istrm));
	OW_String role(OW_BinarySerialization::readString(istrm));

	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::write(ostrm, OW_BINSIG_OPENUM);
	BinaryCIMObjectPathWriter handler(ostrm, getHost());
	chdl->referenceNames(ns, op, handler, resultClass, role);
	OW_BinarySerialization::write(ostrm, OW_END_OPENUM);
	OW_BinarySerialization::write(ostrm, OW_END_OPENUM);
}
#endif

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::getServerFeatures(OW_CIMOMHandleIFCRef chdl,
	std::ostream& ostrm, std::istream& /*istrm*/)
{
	OW_CIMFeatures f = chdl->getServerFeatures();
	OW_BinarySerialization::write(ostrm, OW_BIN_OK);
	OW_BinarySerialization::write(ostrm, OW_Int32(f.cimProduct));
	OW_BinarySerialization::writeString(ostrm, f.extURL);
	OW_BinarySerialization::writeStringArray(ostrm, f.supportedGroups);
	OW_BinarySerialization::writeBool(ostrm, f.supportsBatch);
	OW_BinarySerialization::writeStringArray(ostrm, f.supportedQueryLanguages);
	OW_BinarySerialization::writeString(ostrm, f.validation);
	OW_BinarySerialization::writeString(ostrm, f.cimom);
	OW_BinarySerialization::writeString(ostrm, f.protocolVersion);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_BinaryRequestHandler::writeError(std::ostream& ostrm, const char* msg)
{
	OW_BinarySerialization::write(ostrm, OW_BIN_ERROR);
	OW_BinarySerialization::write(ostrm, msg);
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
		OW_BinarySerialization::write(ostrm, OW_Int32(-1));

		// Write file name
		OW_BinarySerialization::writeString(ostrm, fname);
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
OW_REQUEST_HANDLER_FACTORY(OW_BinaryRequestHandler);
