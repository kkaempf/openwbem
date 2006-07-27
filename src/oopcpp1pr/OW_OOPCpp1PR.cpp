/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Dan Nuffer
 */


#include "OW_config.h"
#include "OW_Format.hpp"
#include "OW_Logger.hpp"
#include "OW_LogAppender.hpp"
#include "OW_MultiAppender.hpp"
#include "OW_LogMessage.hpp"
#include "OW_LogLevel.hpp"
#include "OW_CerrAppender.hpp"
#include "OW_CppProviderBaseIFC.hpp"
#include "OW_CppProviderIFC.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_IOIFCStreamBuffer.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_Assertion.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_CppIndicationProviderIFC.hpp"
#include "OW_CppIndicationExportProviderIFC.hpp"
#include "OW_CIMException.hpp"
#include "OW_OperationContext.hpp"
#include "OW_UserInfo.hpp"
#include "OW_IOException.hpp"
#include "OW_CIMProtocolIFC.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_FileAppender.hpp"
#include "OW_RepositoryIFC.hpp"
#include "OW_Timeout.hpp"

#include <iostream>
#include <csignal>
#include <cstdio>
#include <cerrno>

using namespace std;
using namespace OpenWBEM;
using namespace OpenWBEM::WBEMFlags;

namespace OW_NAMESPACE
{

namespace
{


CppProviderBaseIFCRef loadProvider(const String& provider);

const String COMPONENT_NAME("ow.owoopcpp1pr");

class BinaryCIMInstanceWriter : public CIMInstanceResultHandlerIFC
{
public:
	BinaryCIMInstanceWriter(std::streambuf & obuf_)
	: obuf(obuf_)
	{}
protected:
	virtual void doHandle(const CIMInstance &cop)
	{
		BinarySerialization::write(obuf, BinarySerialization::BINSIG_INST);
		BinarySerialization::writeInstance(obuf, cop);
	}
private:
	std::streambuf & obuf;
};

class BinaryCIMObjectPathWriter : public CIMObjectPathResultHandlerIFC
{
public:
	BinaryCIMObjectPathWriter(std::streambuf & obuf_)
	: obuf(obuf_)
	{}
protected:
	virtual void doHandle(const CIMObjectPath &cop)
	{
		BinarySerialization::write(obuf, BinarySerialization::BINSIG_OP);
		BinarySerialization::writeObjectPath(obuf, cop);
	}
private:
	std::streambuf & obuf;
};

class OOPcpp1LogAppender : public LogAppender
{
public:
	OOPcpp1LogAppender(std::streambuf & obuf_, StringArray const & loglevels)
		: LogAppender(ALL_COMPONENTS, loglevels, "") // TODO: propagate the categories & components down from the caller?
		, obuf(obuf_)
	{
	}

	virtual void doProcessLogMessage(const String& formattedMessage, const LogMessage& message) const
	{
		BinarySerialization::write(obuf, BinarySerialization::BIN_LOG_MESSAGE);
		BinarySerialization::writeString(obuf, message.component);
		BinarySerialization::writeString(obuf, message.category);
		BinarySerialization::writeString(obuf, message.message);
		BinarySerialization::writeString(obuf, message.filename);
		BinarySerialization::write(obuf, message.fileline);
		BinarySerialization::writeString(obuf, message.methodname);
	}
private:
	std::streambuf & obuf;
};

class OOPOperationContext : public OperationContext
{
public:
	OOPOperationContext(std::streambuf & ibuf, std::streambuf & outbuf)
		: m_inbuf(ibuf)
		, m_outbuf(outbuf)
	{
	}

private:
	virtual bool doGetData(const String& key, DataRef& data) const
	{
		if (!data)
		{
			return false;
		}
		BinarySerialization::write(m_outbuf, BinarySerialization::OPERATION_CONTEXT_GET_DATA);
		BinarySerialization::writeString(m_outbuf, key);
		bool found = BinarySerialization::readBool(m_inbuf);
		if (found)
		{
			data->readObject(m_inbuf);
		}
		return found;
	}
	virtual void doSetData(const String& key, const DataRef& data)
	{
		OW_ASSERT(data);
		BinarySerialization::write(m_outbuf, BinarySerialization::OPERATION_CONTEXT_SET_DATA);
		BinarySerialization::writeString(m_outbuf, key);
		BinarySerialization::writeString(m_outbuf, data->getType());
		// use a chunked stream so the other end can read it all even if it doesn't know the type
		std::ostream ostr(&m_outbuf);
		HTTPChunkedOStream chunkedOStr(ostr);
		data->writeObject(*chunkedOStr.rdbuf());
		chunkedOStr.termOutput();

		bool sent = BinarySerialization::readBool(m_inbuf);
		if (!sent)
		{
			OW_THROW(ContextDataNotFoundException,
				Format("Out of process provider failed to set OperationContext data with key = %1", key).c_str());
		}
	}
	virtual void doRemoveData(const String& key)
	{
		BinarySerialization::write(m_outbuf, BinarySerialization::OPERATION_CONTEXT_REMOVE_DATA);
		BinarySerialization::writeString(m_outbuf, key);
		bool success = BinarySerialization::readBool(m_inbuf);
		if (!success)
		{
			OW_THROW(ContextDataNotFoundException,
				Format("Out of process provider cannot remove OperationContext data with key = %1", key).c_str());
		}
	}
	virtual bool doKeyHasData(const String& key) const
	{
		BinarySerialization::write(m_outbuf, BinarySerialization::OPERATION_CONTEXT_KEY_HAS_DATA);
		BinarySerialization::writeString(m_outbuf, key);
		return BinarySerialization::readBool(m_inbuf);
	}

	std::streambuf & m_inbuf;
	std::streambuf & m_outbuf;
};

class OOPProtocol : public CIMProtocolIFC
{
public:
	OOPProtocol(const UnnamedPipeRef& conn)
		: m_conn(conn)
		, m_ibuf(conn.getPtr())
		, m_istr(&m_ibuf)
		, m_obuf(conn.getPtr())
		, m_ostr(&m_obuf)
	{
		m_istr.tie(&m_ostr);
	}

	virtual ~OOPProtocol()
	{
	}

	virtual Reference<std::ostream> beginRequest(
			const String& methodName, const String& nameSpace)
	{

		return Reference<std::ostream>(new HTTPChunkedOStream(m_ostr));
	}

	virtual Reference<std::istream> endRequest(
		const Reference<std::ostream>& request,
		const String& methodName,
		const String& cimObject,
		ERequestType requestType,
		const String& cimProtocolVersion)
	{
		request.cast_to<HTTPChunkedOStream>()->termOutput();
		request->flush();
		m_ostr.flush();

		return Reference<std::istream>(new HTTPChunkedIStream(m_istr));
	}

	virtual void endResponse(std::istream & istr)
	{
		// finish off the response if the caller didn't.
		HTTPUtils::eatEntity(istr);

		std::streambuf & ib = *m_istr.rdbuf();
		// now check for success or error
		UInt8 op;
		BinarySerialization::read(ib, op);

		switch (op)
		{
			case BinarySerialization::BIN_ERROR:
			{
				String msg;
				BinarySerialization::read(ib, msg);
				OW_THROW(IOException, msg.c_str());
			}
			break;
			case BinarySerialization::BIN_EXCEPTION:
			{
				UInt16 cimerrno;
				String cimMsg;
				BinarySerialization::read(ib, cimerrno);
				BinarySerialization::read(ib, cimMsg);
				OW_THROWCIMMSG(CIMException::ErrNoType(cimerrno), cimMsg.c_str());
			}
			break;
			case BinarySerialization::BIN_OK:
			{
				// do nothing
			}
			break;
			default:
				OW_THROW(IOException,
					"Unexpected value received from server.");
		}

	}

	virtual CIMFeatures getFeatures()
	{
		return CIMFeatures();
	}

	virtual SocketAddress getLocalAddress() const
	{
   		return SocketAddress();
	}
	virtual SocketAddress getPeerAddress()  const
	{
   		return SocketAddress();
	}

	virtual void close()
	{

	}

	virtual void setReceiveTimeout(const Timeout&)
	{

	}
	virtual Timeout getReceiveTimeout() const
	{
		return Timeout::infinite;
	}
	virtual void setSendTimeout(const Timeout&)
	{

	}
	virtual Timeout getSendTimeout() const
	{
		return Timeout::infinite;
	}
	virtual void setConnectTimeout(const Timeout&)
	{

	}
	virtual Timeout getConnectTimeout() const
	{
		return Timeout::infinite;
	}
	virtual void setTimeouts(const Timeout&)
	{

	}

private:
	UnnamedPipeRef m_conn;
	IOIFCStreamBuffer m_ibuf;
	std::istream m_istr;
	IOIFCStreamBuffer m_obuf;
	std::ostream m_ostr;
};

class OOPProviderEnvironment : public ProviderEnvironmentIFC
{
public:
	OOPProviderEnvironment(std::streambuf& ibuf, std::streambuf& obuf, const UnnamedPipeRef& inpipe)
		: m_context(ibuf, obuf)
		, m_inbuf(ibuf)
		, m_outbuf(obuf)
		, m_inpipe(inpipe)
	{}

	virtual CIMOMHandleIFCRef commonGetCIMOMHandle() const
	{
		if (m_outbuf.pubsync() == -1)
		{
			OW_THROWCIMMSG(CIMException::FAILED, "sync failed");
		}
		UInt8 op;
		if (m_inpipe->read(&op, sizeof(op)) != sizeof(op))
		{
			OW_THROW_ERRNO_MSG(IOException, "OOPProviderEnvironment::commonGetCIMOMHandle() reading op failed");
		}
		if (op == BinarySerialization::BIN_OK)
		{
			//OW_LOG_DEBUG(m_logger, "OOPProviderEnvironment::commonGetCIMOMHandle() got BIN_OK");
			AutoDescriptor inputDescriptor = m_inpipe->receiveDescriptor();
			//OW_LOG_DEBUG(m_logger, Format("OOPProviderEnvironment::commonGetCIMOMHandle() got input descriptor: %1", inputDescriptor));
			AutoDescriptor outputDescriptor = m_inpipe->receiveDescriptor();
			//OW_LOG_DEBUG(m_logger, Format("OOPProviderEnvironment::commonGetCIMOMHandle() got output descriptor: %1", outputDescriptor));
			UnnamedPipeRef newConn = UnnamedPipe::createUnnamedPipeFromDescriptor(inputDescriptor, outputDescriptor);
			// Setting the timeouts to be infinite won't cause a problem here.  The OOP interface enforces a timeout for each operation and
			// will kill us if we exceed it.
			newConn->setTimeouts(Timeout::infinite);
			//OW_LOG_DEBUG(m_logger, "OOPProviderEnvironment::commonGetCIMOMHandle() received descriptors, returning a BinaryCIMOMHandle");

			return CIMOMHandleIFCRef(new BinaryCIMOMHandle(CIMProtocolIFCRef(new OOPProtocol(newConn))));
		}
		else if (op == BinarySerialization::BIN_ERROR)
		{
			Logger logger(COMPONENT_NAME);
			OW_LOG_DEBUG(logger, "OOPProviderEnvironment::commonGetCIMOMHandle() got BIN_ERROR");
			String msg;
			BinarySerialization::read(m_inbuf, msg);
			OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
		}
		OW_THROWCIMMSG(CIMException::FAILED, Format("Received unknown value from CIMOMHandle request: %1", static_cast<unsigned>(op)).c_str());
	}

	virtual CIMOMHandleIFCRef getCIMOMHandle() const
	{
		BinarySerialization::write(m_outbuf, BinarySerialization::CIMOM_HANDLE_REQUEST);
		return commonGetCIMOMHandle();
	}
	virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const
	{
		BinarySerialization::write(m_outbuf, BinarySerialization::REPOSITORY_CIMOM_HANDLE_REQUEST);
		return commonGetCIMOMHandle();

	}
	virtual RepositoryIFCRef getRepository() const
	{
		OW_ASSERTMSG(0, "Not implemented");
		return RepositoryIFCRef();
	}
	virtual RepositoryIFCRef getAuthorizingRepository() const
	{
		OW_ASSERTMSG(0, "Not implemented");
		return RepositoryIFCRef();
	}
	virtual String getConfigItem(const String &name, const String& defRetVal="") const
	{
		BinarySerialization::write(m_outbuf, BinarySerialization::GET_CONFIG_ITEM);
		BinarySerialization::writeString(m_outbuf, name);
		BinarySerialization::writeString(m_outbuf, defRetVal);
		return BinarySerialization::readString(m_inbuf);
	}
	virtual StringArray getMultiConfigItem(const String &itemName,
		const StringArray& defRetVal, const char* tokenizeSeparator) const
	{
		BinarySerialization::write(m_outbuf, BinarySerialization::GET_MULTI_CONFIG_ITEM);
		BinarySerialization::writeString(m_outbuf, itemName);
		BinarySerialization::writeStringArray(m_outbuf, defRetVal);
		BinarySerialization::writeString(m_outbuf, tokenizeSeparator);
		return BinarySerialization::readStringArray(m_inbuf);
	}

	virtual String getUserName() const
	{
		return m_context.getUserInfo().getUserName();
	}

	virtual OperationContext& getOperationContext()
	{
		return m_context;
	}
	virtual ProviderEnvironmentIFCRef clone() const
	{
		// don't actually implement this, because we don't support threads in providers, which is basically the only reason someone would call clone().
		return ProviderEnvironmentIFCRef();
		//return ProviderEnvironmentIFCRef(new OOPProviderEnvironment(m_logger, m_inbuf, m_outbuf, m_inpipe));
	}
private:
	OOPOperationContext m_context;
	std::streambuf& m_inbuf;
	std::streambuf& m_outbuf;
	UnnamedPipeRef m_inpipe;
};

int callInstanceProvider(UInt8 op, CppProviderBaseIFCRef * provider, std::streambuf& inbuf, std::streambuf& outbuf, const UnnamedPipeRef& stdinout)
{
	CppInstanceProviderIFC* instProvider = (*provider)->getInstanceProvider();
	Logger logger(COMPONENT_NAME);
	if (!instProvider)
	{
		OW_LOG_ERROR(logger, "provider is not an instance provider");
		return 3;
	}
	ProviderEnvironmentIFCRef provenv(new OOPProviderEnvironment(inbuf, outbuf, stdinout));

	switch (op)
	{
		case BinarySerialization::BIN_ENUMINSTS:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got BIN_ENUMINSTS command");
			String ns = BinarySerialization::readString(inbuf);
			String className = BinarySerialization::readString(inbuf);
			WBEMFlags::ELocalOnlyFlag localOnly(
				BinarySerialization::readBool(inbuf) ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY);
			WBEMFlags::EDeepFlag deep(
				BinarySerialization::readBool(inbuf) ? E_DEEP : E_SHALLOW);
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers(
				BinarySerialization::readBool(inbuf) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS);
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin(
				BinarySerialization::readBool(inbuf) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN);
			StringArray propertyList;
			StringArray* propertyListPtr;
			BinarySerialization::readPropertyList(inbuf, propertyList, propertyListPtr);
			CIMClass requestedClass = BinarySerialization::readClass(inbuf);
			CIMClass cimClass = BinarySerialization::readClass(inbuf);

			BinaryCIMInstanceWriter handler(outbuf);
			instProvider->enumInstances(provenv, ns, className, handler, localOnly, deep,
				includeQualifiers, includeClassOrigin, propertyListPtr, requestedClass, cimClass);
		}
		break;

		case BinarySerialization::BIN_ENUMINSTNAMES:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got BIN_ENUMINSTNAMES command");
			String ns = BinarySerialization::readString(inbuf);
			String className = BinarySerialization::readString(inbuf);
			CIMClass cimClass = BinarySerialization::readClass(inbuf);

			BinaryCIMObjectPathWriter handler(outbuf);
			instProvider->enumInstanceNames(provenv, ns, className, handler, cimClass);
		}
		break;

		case BinarySerialization::BIN_GETINST:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got BIN_GETINST command");
			String ns = BinarySerialization::readString(inbuf);
			CIMObjectPath instanceName = BinarySerialization::readObjectPath(inbuf);
			WBEMFlags::ELocalOnlyFlag localOnly(
				BinarySerialization::readBool(inbuf) ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY);
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers(
				BinarySerialization::readBool(inbuf) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS);
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin(
				BinarySerialization::readBool(inbuf) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN);
			StringArray propertyList;
			StringArray* propertyListPtr;
			BinarySerialization::readPropertyList(inbuf, propertyList, propertyListPtr);
			CIMClass cimClass = BinarySerialization::readClass(inbuf);

			CIMInstance rval = instProvider->getInstance(provenv, ns, instanceName, localOnly,
				includeQualifiers, includeClassOrigin, propertyListPtr, cimClass);

			BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
			BinarySerialization::writeInstance(outbuf, rval);

		}
		break;

		case BinarySerialization::BIN_CREATEINST:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got BIN_CREATEINST command");
			String ns = BinarySerialization::readString(inbuf);
			CIMInstance instance = BinarySerialization::readInstance(inbuf);

			CIMObjectPath rval = instProvider->createInstance(provenv, ns, instance);
			if (!rval)
			{
				OW_THROWCIMMSG(CIMException::FAILED, "Provider returned a NULL CIMObjectPath");
			}

			BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
			BinarySerialization::writeObjectPath(outbuf, rval);
		}
		break;

		case BinarySerialization::BIN_MODIFYINST:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got BIN_MODIFYINST command");
			String ns = BinarySerialization::readString(inbuf);
			CIMInstance modifiedInstance = BinarySerialization::readInstance(inbuf);
			CIMInstance previousInstance = BinarySerialization::readInstance(inbuf);
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers(
				BinarySerialization::readBool(inbuf) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS);
			StringArray propertyList;
			StringArray* propertyListPtr;
			BinarySerialization::readPropertyList(inbuf, propertyList, propertyListPtr);
			CIMClass cimClass = BinarySerialization::readClass(inbuf);

			instProvider->modifyInstance(provenv, ns, modifiedInstance, previousInstance,
				includeQualifiers, propertyListPtr, cimClass);

			BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
		}
		break;

		case BinarySerialization::BIN_DELETEINST:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got BIN_DELETEINST command");
			String ns = BinarySerialization::readString(inbuf);
			CIMObjectPath cop = BinarySerialization::readObjectPath(inbuf);

			instProvider->deleteInstance(provenv, ns, cop);

			BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
		}
		break;
	}
	return 0;
}

int callPolledProvider(UInt8 op, CppProviderBaseIFCRef * provider, std::streambuf& inbuf, std::streambuf& outbuf, const UnnamedPipeRef& stdinout)
{
	CppPolledProviderIFC* polledProvider = (*provider)->getPolledProvider();
	Logger logger(COMPONENT_NAME);
	if (!polledProvider)
	{
		OW_LOG_ERROR(logger, "provider is not a polled provider");
		return 3;
	}

	ProviderEnvironmentIFCRef provenv(new OOPProviderEnvironment(inbuf, outbuf, stdinout));

	switch (op)
	{
		case BinarySerialization::POLL:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got POLL command");
			Int32 rval = polledProvider->poll(provenv);
			BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
			BinarySerialization::write(outbuf, rval);
		}
		break;

		case BinarySerialization::GET_INITIAL_POLLING_INTERVAL:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got GET_INITIAL_POLLING_INTERVAL command");
			Int32 rval = polledProvider->getInitialPollingInterval(provenv);
			BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
			BinarySerialization::write(outbuf, rval);
		}
		break;

	}
	return 0;
}

int callIndicationProvider(UInt8 op, CppProviderBaseIFCRef * provider, std::streambuf& inbuf, std::streambuf& outbuf, const UnnamedPipeRef& stdinout)
{
	CppIndicationProviderIFC* indicationProvider = (*provider)->getIndicationProvider();
	Logger logger(COMPONENT_NAME);
	if (!indicationProvider)
	{
		OW_LOG_ERROR(logger, "provider is not an indication provider");
		return 3;
	}

	ProviderEnvironmentIFCRef provenv(new OOPProviderEnvironment(inbuf, outbuf, stdinout));

	switch (op)
	{
		case BinarySerialization::MUST_POLL:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got MUST_POLL command");
			WQLSelectStatement filter = BinarySerialization::readWQLSelectStatement(inbuf);
			String eventType = BinarySerialization::readString(inbuf);
			String nameSpace = BinarySerialization::readString(inbuf);
			StringArray classes = BinarySerialization::readStringArray(inbuf);
			Int32 rval = indicationProvider->mustPoll(provenv, filter, eventType, nameSpace, classes);
			BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
			BinarySerialization::write(outbuf, rval);
		}
		break;

		case BinarySerialization::AUTHORIZE_FILTER:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got AUTHORIZE_FILTER command");
			WQLSelectStatement filter = BinarySerialization::readWQLSelectStatement(inbuf);
			String eventType = BinarySerialization::readString(inbuf);
			String nameSpace = BinarySerialization::readString(inbuf);
			StringArray classes = BinarySerialization::readStringArray(inbuf);
			String owner = BinarySerialization::readString(inbuf);
			indicationProvider->authorizeFilter(provenv, filter, eventType, nameSpace, classes, owner);
			BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
		}
		break;

		case BinarySerialization::ACTIVATE_FILTER:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got ACTIVATE_FILTER command");
			WQLSelectStatement filter = BinarySerialization::readWQLSelectStatement(inbuf);
			String eventType = BinarySerialization::readString(inbuf);
			String nameSpace = BinarySerialization::readString(inbuf);
			StringArray classes = BinarySerialization::readStringArray(inbuf);
			Bool firstActivation;
			firstActivation.readObject(inbuf);
			indicationProvider->activateFilter(provenv, filter, eventType, nameSpace, classes, firstActivation);
			BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
		}
		break;

		case BinarySerialization::DEACTIVATE_FILTER:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got DEACTIVATE_FILTER command");
			WQLSelectStatement filter = BinarySerialization::readWQLSelectStatement(inbuf);
			String eventType = BinarySerialization::readString(inbuf);
			String nameSpace = BinarySerialization::readString(inbuf);
			StringArray classes = BinarySerialization::readStringArray(inbuf);
			Bool lastActivation;
			lastActivation.readObject(inbuf);
			indicationProvider->deActivateFilter(provenv, filter, eventType, nameSpace, classes, lastActivation);
			BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
		}
		break;

	}
	return 0;
}

int callIndicationExportProvider(UInt8 op, CppProviderBaseIFCRef * provider, std::streambuf& inbuf, std::streambuf& outbuf, const UnnamedPipeRef& stdinout)
{
	CppIndicationExportProviderIFC* indicationExportProvider = (*provider)->getIndicationExportProvider();
	Logger logger(COMPONENT_NAME);
	if (!indicationExportProvider)
	{
		OW_LOG_ERROR(logger, "provider is not a indicationExport provider");
		return 3;
	}

	ProviderEnvironmentIFCRef provenv(new OOPProviderEnvironment(inbuf, outbuf, stdinout));

	switch (op)
	{
		case BinarySerialization::EXPORT_INDICATION:
		{
			//OW_LOG_DEBUG(logger, "owoopcpp1pr Got EXPORT_INDICATION command");
			String ns = BinarySerialization::readString(inbuf);
			CIMInstance indHandlerInst = BinarySerialization::readInstance(inbuf);
			CIMInstance indicationInst = BinarySerialization::readInstance(inbuf);
			indicationExportProvider->exportIndication(provenv, ns, indHandlerInst, indicationInst);
			BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
		}
		break;

	}
	return 0;
}

int callMethodProvider(UInt8 op, CppProviderBaseIFCRef * provider, std::streambuf& inbuf, std::streambuf& outbuf, const UnnamedPipeRef& stdinout)
{
	//OW_LOG_DEBUG(logger, "owoopcpp1pr Got BIN_INVMETH command");
	Logger logger(COMPONENT_NAME);
	CppMethodProviderIFC* methProvider = (*provider)->getMethodProvider();
	if (!methProvider)
	{
		OW_LOG_ERROR(logger, "provider is not a method provider");
		return 3;
	}

	String ns (BinarySerialization::readString(inbuf));
	CIMObjectPath path(BinarySerialization::readObjectPath(inbuf));
	String methodName(BinarySerialization::readString(inbuf));
	CIMParamValueArray inparms;
	// Get input params
	BinarySerialization::verifySignature(inbuf, BinarySerialization::BINSIG_PARAMVALUEARRAY);
	BinarySerialization::readArray(inbuf, inparms);

	ProviderEnvironmentIFCRef provenv(new OOPProviderEnvironment(inbuf, outbuf, stdinout));
	CIMParamValueArray outparms;
	CIMValue cv = methProvider->invokeMethod(provenv, ns, path, methodName, inparms, outparms);
	//OW_LOG_DEBUG(logger, "invokeMethod done");
	BinarySerialization::write(outbuf, BinarySerialization::BIN_OK);
	BinarySerialization::writeValue(outbuf, cv);
	BinarySerialization::write(outbuf, BinarySerialization::BINSIG_PARAMVALUEARRAY);
	BinarySerialization::writeArray(outbuf, outparms);

	return 0;
}

bool opRequiresEndReply(UInt8 op)
{
	return op != BinarySerialization::SET_PERSISTENT;
}

bool doPlatformInitialization()
{
	if (::signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		::perror("signal(SIGPIPE, SIG_IGN) failed");
		return false;
	}

	return true;
}

String where(String const & provider_lib, UInt8 op, bool got_op)
{
	return got_op ? provider_lib + ", op=" + String(int(op)) : provider_lib;
}

} // end unnamed namespace

namespace OOPCpp1
{

int runOOPProvider(
	const String& providerLib, const String& logfile, const String & loglevel)
{
	int rval = 0;
	UnnamedPipeRef stdinout = UnnamedPipe::createStdinStdout();
	IOIFCStreamBuffer inbuf(stdinout.getPtr());
	IOIFCStreamBuffer outbuf(stdinout.getPtr());
	inbuf.tie(&outbuf);

	if (!doPlatformInitialization())
	{
		return 1;
	}

	// set up a file log as well so we can see what happened even if the communication back to the provider interface is broken.
	Array<LogAppenderRef> appenders;
	StringArray categories = loglevel.tokenize(",");
	appenders.push_back(new OOPcpp1LogAppender(outbuf, categories));

	const char LOG_FORMAT[] = "%-3r [%t] %-5p %F:%L %c - %m";
	if (!logfile.empty())
	{
		appenders.push_back(new FileAppender(LogAppender::ALL_COMPONENTS, LogAppender::ALL_CATEGORIES, logfile.c_str(),
			LOG_FORMAT, FileAppender::NO_MAX_LOG_SIZE, 9999));
	}
	LogAppender::setDefaultLogAppender(LogAppenderRef(new MultiAppender(appenders)));

	Logger logger(COMPONENT_NAME);

	bool got_op = false;
	UInt8 op;
	try
	{
		//OW_LOG_DEBUG(logger, Format("about to load %1", providerLib));
		// static and a pointer so it never gets unloaded (or deleted)
		static CppProviderBaseIFCRef * provider = new CppProviderBaseIFCRef(
			CppProviderIFC::loadProvider(providerLib)
		);
		if (!*provider)
		{
			OW_LOG_ERROR(logger, Format("provider %1 did not load", providerLib));
			rval = 1;
			goto main_end;
		}

		//OW_LOG_DEBUG(logger, "loaded provider");

		bool persistent = false;
		do
		{
			got_op = false;
			UInt32 binaryProtocolVersion = 0;
			BinarySerialization::read(inbuf, binaryProtocolVersion);
			//OW_LOG_DEBUG(logger, Format("Read protocol version %1", binaryProtocolVersion));
			if (binaryProtocolVersion < BinarySerialization::MinBinaryProtocolVersion
				|| binaryProtocolVersion > BinarySerialization::BinaryProtocolVersion)
			{
				OW_LOG_ERROR(logger, Format("Unknown version: %1. This implementation can only handle version %2-%3",
					binaryProtocolVersion, BinarySerialization::MinBinaryProtocolVersion, BinarySerialization::BinaryProtocolVersion));
				rval = 2;
				goto main_end;
			}

			BinarySerialization::read(inbuf, op);
			got_op = true;
			switch (op)
			{
				case BinarySerialization::BIN_ENUMINSTS:
				case BinarySerialization::BIN_ENUMINSTNAMES:
				case BinarySerialization::BIN_GETINST:
				case BinarySerialization::BIN_CREATEINST:
				case BinarySerialization::BIN_MODIFYINST:
				case BinarySerialization::BIN_DELETEINST:
				{
					rval = callInstanceProvider(op, provider, inbuf, outbuf, stdinout);
				}
				break;

				case BinarySerialization::BIN_INVMETH:
				{
					rval = callMethodProvider(op, provider, inbuf, outbuf, stdinout);
				}
				break;

				case BinarySerialization::SET_PERSISTENT:
				{
					//OW_LOG_DEBUG(logger, "owoopcpp1pr Got SET_PERSISTENT command");
					persistent = BinarySerialization::readBool(inbuf);
					if (persistent)
					{
						stdinout->setTimeouts(Timeout::infinite);
					}
					else
					{
						stdinout->setTimeouts(Timeout::relative(60 * 10.0));
					}
				}
				break;

				case BinarySerialization::POLL:
				case BinarySerialization::GET_INITIAL_POLLING_INTERVAL:
				{
					rval = callPolledProvider(op, provider, inbuf, outbuf, stdinout);
				}
				break;

				case BinarySerialization::MUST_POLL:
				case BinarySerialization::AUTHORIZE_FILTER:
				case BinarySerialization::ACTIVATE_FILTER:
				case BinarySerialization::DEACTIVATE_FILTER:
				{
					rval = callIndicationProvider(op, provider, inbuf, outbuf, stdinout);
				}
				break;

				case BinarySerialization::EXPORT_INDICATION:
				{
					rval = callIndicationExportProvider(op, provider, inbuf, outbuf, stdinout);
				}
				break;


				default:
					OW_LOG_ERROR(logger, "Invalid operation");
					rval = 4;
					goto main_end;
			}
			if (persistent && opRequiresEndReply(op))
			{
				BinarySerialization::write(outbuf, BinarySerialization::BIN_END);
				if (outbuf.pubsync() == -1)
				{
					cerr << "owoopcpp1pr main() outbuf.pubsync() failed" << endl;
					rval = 7;
					goto main_end;
				}

			}
		} while (persistent);
	}
	catch (CIMException& e)
	{
		OW_LOG_INFO(logger, Format("CIMException in owoopcpp1pr (%2): %1", e, where(providerLib, op, got_op)));
		BinarySerialization::write(outbuf, BinarySerialization::BIN_EXCEPTION);
		BinarySerialization::write(outbuf, UInt16(e.getErrNo()));
		BinarySerialization::write(outbuf, e.getMessage());
	}
	catch (const IOException& e)
	{
		String msg = Format("Exception in owoopcpp1pr (%2): %1", e, where(providerLib, op, got_op));
		OW_LOG_ERROR(logger, msg);
		// Odds are that the output pipe was closed.  We can't use the
		// BinarySerialization::write functions without catching the exceptions
		// that will result from logging these errors.
		try
		{
			BinarySerialization::write(outbuf, BinarySerialization::BIN_ERROR);
			BinarySerialization::write(outbuf, msg);
		}
		catch (...)
		{
			// We can't do anything else here.
			OW_LOG_ERROR(logger, "Failed writing error message to output pipe.");
			return 9;
		}
	}
	catch (Exception& e)
	{
		String msg = Format("Exception in owoopcpp1pr (%2): %1", e, where(providerLib, op, got_op));
		OW_LOG_INFO(logger, msg);
		BinarySerialization::write(outbuf, BinarySerialization::BIN_ERROR);
		BinarySerialization::write(outbuf, msg);
	}
	catch (std::exception& e)
	{
		String msg = Format("std::exception in owoopcpp1pr (%2): %1", e.what(), where(providerLib, op, got_op));
		OW_LOG_ERROR(logger, msg);
		BinarySerialization::write(outbuf, BinarySerialization::BIN_ERROR);
		BinarySerialization::write(outbuf, msg);
	}
	catch(...)
	{
		String msg = "* UNKNOWN EXCEPTION CAUGHT IN owoopcpp1pr MAIN! ("
			+ where(providerLib, op, got_op) + ")";
		OW_LOG_ERROR(logger, msg);
		BinarySerialization::write(outbuf, BinarySerialization::BIN_ERROR);
		BinarySerialization::write(outbuf, msg);
	}

main_end:

	BinarySerialization::write(outbuf, BinarySerialization::BIN_END);
	if (outbuf.pubsync() == -1)
	{
		cerr << "owoopcpp1pr main() outbuf.pubsync() failed" << endl;
		rval = 7;
	}
	return rval;
}

} // end namespace OOPCpp1

} // end namespace OW_NAMESPACE



