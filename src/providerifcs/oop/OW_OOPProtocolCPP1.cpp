/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_OOPProtocolCPP1.hpp"
#include "OW_OOPProtocolCPP1Impl.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_BinarySerialization.hpp"
#include "blocxx/IOException.hpp"
#include "OW_CIMException.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/LogMessage.hpp"
#include "OW_OperationContext.hpp"
#include "OW_BinaryRequestHandler.hpp"
#include "OW_Assertion.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "blocxx/TempFileStream.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_CIMParamValue.hpp"
#include "blocxx/DateTime.hpp"
#include "blocxx/UnnamedPipe.hpp"
#include "blocxx/DataStreams.hpp"
#include "blocxx/Runnable.hpp"
#include "blocxx/IOIFCStreamBuffer.hpp"
#include "blocxx/ThreadPool.hpp"
#include "blocxx/NonRecursiveMutex.hpp"
#include "blocxx/NonRecursiveMutexLock.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_OOPProviderBase.hpp"
#include "OW_OOPDataOStreamBuf.hpp"
#include "OW_OOPCallbackServiceEnv.hpp"
#include "OW_OOPCIMOMHandleConnectionRunner.hpp"
#include "blocxx/SelectEngine.hpp"

#include <deque>
#include <iosfwd>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

#define OW_OOP_LOG_DEBUG4(logger, message)

// use this version if you need to debug this code
//#define OW_OOP_LOG_DEBUG4(logger, message) OW_LOG_DEBUG(logger, message)

namespace OW_NAMESPACE
{

using namespace OOPProtocolCPP1Impl;


OOPProtocolCPP1::OOPProtocolCPP1(OOPProviderBase* pprov)
	: OOPProtocolIFC(pprov)
{
}

OOPProtocolCPP1::~OOPProtocolCPP1()
{

}

namespace
{
	const String COMPONENT_NAME("ow.provider.OOP.ifc");

	enum EReadWriteFlag
	{
		E_READ_WRITE_UNTIL_FINISHED,
		E_WRITE_ONLY
	};

	void end(Array<unsigned char>& outputBuf,
		const UnnamedPipeRef& inputPipe,
		const UnnamedPipeRef& outputPipe,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		OperationResultHandler& result,
		OOPProviderBase* pprov,
		EReadWriteFlag readWriteFlag = E_READ_WRITE_UNTIL_FINISHED)
	{
		const int MAX_CALLBACK_THREADS = 10;

		Logger logger(COMPONENT_NAME);

		ThreadPool threadPool(ThreadPool::DYNAMIC_SIZE_NO_QUEUE, MAX_CALLBACK_THREADS, MAX_CALLBACK_THREADS, logger, "OOPProtocolCPP1");
		SelectEngine selectEngine;
		Array<unsigned char> inputBuf;
		std::deque<OutputEntry> outputEntries;
		inputPipe->setTimeouts(Timeout::relative(0));
		outputPipe->setTimeouts(Timeout::relative(0));
		outputPipe->setWriteBlocking(UnnamedPipe::E_NONBLOCKING);
		bool finishedSuccessfully = false;
		ShutdownThreadPool shutdownThreadPool(threadPool);

		SelectableCallbackIFCRef callback(new OOPSelectableCallback(
			inputBuf, outputEntries, inputPipe, outputPipe, env, result, selectEngine, 
			finishedSuccessfully, threadPool, pprov));

		if (readWriteFlag == E_READ_WRITE_UNTIL_FINISHED)
		{
			selectEngine.addSelectableObject(inputPipe->getReadSelectObj(), callback, SelectableCallbackIFC::E_READ_EVENT);
		}

		if (!outputBuf.empty())
		{
			outputEntries.push_back(OutputEntry(outputBuf));
			selectEngine.addSelectableObject(outputPipe->getWriteSelectObj(), callback, SelectableCallbackIFC::E_WRITE_EVENT);
		}

		OW_LOG_DEBUG3(logger, "end() about to run the select engine");
		try
		{
			selectEngine.go(timeout);
		}
		catch (SelectException& e)
		{
			OW_THROW(OOPProtocolCPP1Exception, "Timeout expired, provider terminated");
		}

		if (readWriteFlag == E_READ_WRITE_UNTIL_FINISHED && !finishedSuccessfully)
		{
			OW_THROW(OOPProtocolCPP1Exception, "pipe closed without sending a BIN_END");
		}
	}



	class CIMObjectPathOperationResultHandler : public OperationResultHandler
	{
	public:
		CIMObjectPathOperationResultHandler(CIMObjectPathResultHandlerIFC& result)
		: m_result(result)
		{}

		virtual void handleResult(std::streambuf & instr, UInt8 op)
		{
			if (op == BinarySerialization::BINSIG_OP)
			{
				CIMObjectPath ci = BinarySerialization::readObjectPath(instr);
				m_result.handle(ci);
			}
			else
			{
				OW_THROW(OOPProtocolCPP1Exception, Format("Invalid op, expected BINSIG_OP, got: %1", static_cast<int>(op)).c_str());
			}
		}
	private:
		CIMObjectPathResultHandlerIFC& m_result;
	};

	class CIMInstanceOperationResultHandler : public OperationResultHandler
	{
	public:
		CIMInstanceOperationResultHandler(CIMInstanceResultHandlerIFC& result)
		: m_result(result)
		{}

		virtual void handleResult(std::streambuf & instr, UInt8 op)
		{
			if (op == BinarySerialization::BINSIG_INST)
			{
				CIMInstance ci = BinarySerialization::readInstance(instr);
				m_result.handle(ci);
			}
			else
			{
				OW_THROW(OOPProtocolCPP1Exception, Format("Invalid op, expected BINSIG_INST, got: %1", static_cast<int>(op)).c_str());
			}
		}
	private:
		CIMInstanceResultHandlerIFC& m_result;
	};

	class GetInstanceOperationResultHandler : public OperationResultHandler
	{
	public:
		GetInstanceOperationResultHandler(CIMInstance& returnValue)
			: m_returnValue(returnValue)
		{
		}

		virtual void handleResult(std::streambuf & instr, UInt8 op)
		{
			if (op == BinarySerialization::BIN_OK)
			{
				m_returnValue = BinarySerialization::readInstance(instr);
			}
			else
			{
				OW_THROW(OOPProtocolCPP1Exception, Format("Invalid op, expected BINSIG_OK, got: %1", static_cast<int>(op)).c_str());
			}
		}
	private:
		CIMInstance& m_returnValue;
	};

	class CreateInstanceOperationResultHandler : public OperationResultHandler
	{
	public:
		CreateInstanceOperationResultHandler(CIMObjectPath& returnValue)
			: m_returnValue(returnValue)
		{
		}

		virtual void handleResult(std::streambuf & instr, UInt8 op)
		{
			if (op == BinarySerialization::BIN_OK)
			{
				m_returnValue = BinarySerialization::readObjectPath(instr);
			}
			else
			{
				OW_THROW(OOPProtocolCPP1Exception, Format("Invalid op, expected BINSIG_OK, got: %1", static_cast<int>(op)).c_str());
			}
		}
	private:
		CIMObjectPath& m_returnValue;
	};

	class VoidOperationResultHandler : public OperationResultHandler
	{
	public:
		VoidOperationResultHandler(bool& gotOK)
			: m_gotOK(gotOK)
		{
		}

		virtual void handleResult(std::streambuf & instr, UInt8 op)
		{
			if (op != BinarySerialization::BIN_OK)
			{
				OW_THROW(OOPProtocolCPP1Exception, Format("Invalid op, expected BINSIG_OK, got: %1", static_cast<int>(op)).c_str());
			}
			m_gotOK = true;
		}
	private:
		bool& m_gotOK;
	};

	class InvokeMethodOperationResultHandler : public OperationResultHandler
	{
	public:
		InvokeMethodOperationResultHandler(CIMValue& returnValue, CIMParamValueArray& outParams, bool& gotOK)
			: m_returnValue(returnValue)
			, m_outParams(outParams)
			, m_gotOK(gotOK)
		{
		}

		virtual void handleResult(std::streambuf & instr, UInt8 op)
		{
			if (op == BinarySerialization::BIN_OK)
			{
				m_returnValue = BinarySerialization::readValue(instr);
				BinarySerialization::verifySignature(instr, BinarySerialization::BINSIG_PARAMVALUEARRAY);
				BinarySerialization::readArray(instr, m_outParams);
				m_gotOK = true;
			}
			else
			{
				OW_THROW(OOPProtocolCPP1Exception, Format("Invalid op, expected BINSIG_OK, got: %1", static_cast<int>(op)).c_str());
			}
		}
	private:
		CIMValue& m_returnValue;
		CIMParamValueArray& m_outParams;
		bool& m_gotOK;
	};

	class Int32OperationResultHandler : public OperationResultHandler
	{
	public:
		Int32OperationResultHandler(Int32& returnValue, bool& gotOK)
			: m_returnValue(returnValue)
			, m_gotOK(gotOK)
		{
		}

		virtual void handleResult(std::streambuf & instr, UInt8 op)
		{
			if (op == BinarySerialization::BIN_OK)
			{
				BinarySerialization::read(instr, m_returnValue);
				m_gotOK = true;
			}
			else
			{
				OW_THROW(OOPProtocolCPP1Exception, Format("Invalid op, expected BINSIG_OK, got: %1", static_cast<int>(op)).c_str());
			}
		}
	private:
		Int32& m_returnValue;
		bool& m_gotOK;
	};

} // end unnamed namespace

void 
OOPProtocolCPP1::enumInstanceNames(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result,
	const CIMClass& cimClass)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::enumInstanceNames about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_ENUMINSTNAMES);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeString(obuf, className);
	BinarySerialization::writeClass(obuf, cimClass);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::enumInstanceNames finished filling buffer");

	CIMObjectPathOperationResultHandler operationResult(result);
	end(buf, in, out, timeout, env, operationResult, m_pprov);

}

void 
OOPProtocolCPP1::enumInstances(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result,
	WBEMFlags::ELocalOnlyFlag localOnly, 
	WBEMFlags::EDeepFlag deep, 
	WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
	WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	const CIMClass& requestedClass,
	const CIMClass& cimClass)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::enumInstances about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_ENUMINSTS);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeString(obuf, className);
	BinarySerialization::writeBool(obuf, localOnly);
	BinarySerialization::writeBool(obuf, deep);
	BinarySerialization::writeBool(obuf, includeQualifiers);
	BinarySerialization::writeBool(obuf, includeClassOrigin);
	BinarySerialization::writeStringArray(obuf, propertyList);
	BinarySerialization::writeClass(obuf, requestedClass);
	BinarySerialization::writeClass(obuf, cimClass);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::enumInstances finished filling buffer");

	CIMInstanceOperationResultHandler operationResult(result);
	end(buf, in, out, timeout, env, operationResult, m_pprov);

}

CIMInstance 
OOPProtocolCPP1::getInstance(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMObjectPath& instanceName,
	WBEMFlags::ELocalOnlyFlag localOnly,
	WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
	WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, 
	const CIMClass& cimClass)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::getInstance about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_GETINST);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeObjectPath(obuf, instanceName);
	BinarySerialization::writeBool(obuf, localOnly);
	BinarySerialization::writeBool(obuf, includeQualifiers);
	BinarySerialization::writeBool(obuf, includeClassOrigin);
	BinarySerialization::writeStringArray(obuf, propertyList);
	BinarySerialization::writeClass(obuf, cimClass);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::getInstance finished filling buffer");

	CIMInstance rval(CIMNULL);
	GetInstanceOperationResultHandler operationResult(rval);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
	if (!rval)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to getInstance");
	}
	return rval;
}

CIMObjectPath
OOPProtocolCPP1::createInstance(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMInstance& cimInstance)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::createInstance about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_CREATEINST);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeInstance(obuf, cimInstance);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::createInstance finished filling buffer");

	CIMObjectPath rval(CIMNULL);
	CreateInstanceOperationResultHandler operationResult(rval);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
	if (!rval)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to createInstance");
	}
	return rval;
}

void 
OOPProtocolCPP1::modifyInstance(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMInstance& modifiedInstance,
	const CIMInstance& previousInstance,
	WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	const CIMClass& theClass)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::modifyInstance about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_MODIFYINST);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeInstance(obuf, modifiedInstance);
	BinarySerialization::writeInstance(obuf, previousInstance);
	BinarySerialization::writeBool(obuf, includeQualifiers);
	BinarySerialization::writeStringArray(obuf, propertyList);
	BinarySerialization::writeClass(obuf, theClass);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::modifyInstance finished filling buffer");

	bool gotOK = false;
	VoidOperationResultHandler operationResult(gotOK);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
	if (!gotOK)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to modifyInstance");
	}
}

void 
OOPProtocolCPP1::deleteInstance(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMObjectPath& cop)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::deleteInstance about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_DELETEINST);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeObjectPath(obuf, cop);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::deleteInstance finished filling buffer");

	bool gotOK = false;
	VoidOperationResultHandler operationResult(gotOK);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
	if (!gotOK)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to deleteInstance");
	}
}

void
OOPProtocolCPP1::associators(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	CIMInstanceResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole,
	WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
	WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::associators about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_ASSOCIATORS);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeObjectPath(obuf, objectName);
	BinarySerialization::writeString(obuf, assocClass);
	BinarySerialization::writeString(obuf, resultClass);
	BinarySerialization::writeString(obuf, role);
	BinarySerialization::writeString(obuf, resultRole);
	BinarySerialization::writeBool(obuf, includeQualifiers);
	BinarySerialization::writeBool(obuf, includeClassOrigin);
	BinarySerialization::writeStringArray(obuf, propertyList);

	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::associators finished filling buffer");

	CIMInstanceOperationResultHandler operationResult(result);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
}

void
OOPProtocolCPP1::associatorNames(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	CIMObjectPathResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::associatorNames about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_ASSOCNAMES);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeObjectPath(obuf, objectName);
	BinarySerialization::writeString(obuf, assocClass);
	BinarySerialization::writeString(obuf, resultClass);
	BinarySerialization::writeString(obuf, role);
	BinarySerialization::writeString(obuf, resultRole);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::associatorNames finished filling buffer");

	CIMObjectPathOperationResultHandler operationResult(result);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
}

void
OOPProtocolCPP1::references(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	CIMInstanceResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& resultClass,
	const String& role,
	WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
	WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::references about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_REFERENCES);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeObjectPath(obuf, objectName);
	BinarySerialization::writeString(obuf, resultClass);
	BinarySerialization::writeString(obuf, role);
	BinarySerialization::writeBool(obuf, includeQualifiers);
	BinarySerialization::writeBool(obuf, includeClassOrigin);
	BinarySerialization::writeStringArray(obuf, propertyList);

	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::references finished filling buffer");

	CIMInstanceOperationResultHandler operationResult(result);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
}

void
OOPProtocolCPP1::referenceNames(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	CIMObjectPathResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& resultClass,
	const String& role)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::referenceNames about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_REFNAMES);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeObjectPath(obuf, objectName);
	BinarySerialization::writeString(obuf, resultClass);
	BinarySerialization::writeString(obuf, role);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::referenceNames finished filling buffer");

	CIMObjectPathOperationResultHandler operationResult(result);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
}

CIMValue
OOPProtocolCPP1::invokeMethod(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName,
	const CIMParamValueArray& inparams,
	CIMParamValueArray& outparams)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::invokeMethod about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_INVMETH);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeObjectPath(obuf, path);
	BinarySerialization::writeString(obuf, methodName);
	BinarySerialization::write(obuf, BinarySerialization::BINSIG_PARAMVALUEARRAY);
	BinarySerialization::writeArray(obuf, inparams);
	BinarySerialization::write(obuf, BinarySerialization::BINSIG_PARAMVALUEARRAY);
	BinarySerialization::writeArray(obuf, outparams);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::invokeMethod finished writing.");

	CIMValue returnValue(CIMNULL);
	bool gotOK = false;
	InvokeMethodOperationResultHandler operationResult(returnValue, outparams, gotOK);
	end(buf, in, out, timeout, env, operationResult, m_pprov);

	if (!gotOK)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to invokeMethod");
	}
	return returnValue;
}

Int32
OOPProtocolCPP1::poll(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::poll about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::POLL);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::poll finished writing.");

	Int32 returnValue(-1);
	bool gotOK = false;
	Int32OperationResultHandler operationResult(returnValue, gotOK);
	end(buf, in, out, timeout, env, operationResult, m_pprov);

	if (!gotOK)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to poll");
	}
	return returnValue;
}

Int32
OOPProtocolCPP1::getInitialPollingInterval(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::getInitialPollingInterval about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::GET_INITIAL_POLLING_INTERVAL);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::getInitialPollingInterval finished writing.");

	Int32 returnValue(-1);
	bool gotOK = false;
	Int32OperationResultHandler operationResult(returnValue, gotOK);
	end(buf, in, out, timeout, env, operationResult, m_pprov);

	if (!gotOK)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to getInitialPollingInterval");
	}
	return returnValue;
}

void
OOPProtocolCPP1::setPersistent(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	bool persistent)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, Format("OOPProtocolCPP1::setPersistent about to start writing: %1", persistent));

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::SET_PERSISTENT);
	BinarySerialization::writeBool(obuf, persistent);

	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::setPersistent finished writing.");

	NoResultHandler noResultHandler;
	end(buf, in, out, timeout, env, noResultHandler, m_pprov, E_WRITE_ONLY);
}

void
OOPProtocolCPP1::setLogLevel(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	ELogLevel logLevel)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, Format("OOPProtocolCPP1::setLogLevel about to start writing: %1", static_cast<int>(logLevel)));

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::SET_LOG_LEVEL);
	BinarySerialization::write(obuf, static_cast<UInt8>(logLevel));

	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::setLogLevel finished writing.");

	NoResultHandler noResultHandler;
	end(buf, in, out, timeout, env, noResultHandler, m_pprov, E_WRITE_ONLY);
}

void
OOPProtocolCPP1::activateFilter(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const WQLSelectStatement& filter,
	const String& eventType,
	const String& nameSpace,
	const StringArray& classes,
	bool firstActivation)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::activateFilter about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::ACTIVATE_FILTER);
	BinarySerialization::writeWQLSelectStatement(obuf, filter);
	BinarySerialization::writeString(obuf, eventType);
	BinarySerialization::writeString(obuf, nameSpace);
	BinarySerialization::writeStringArray(obuf, classes);
	Bool(firstActivation).writeObject(obuf);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::activateFilter finished writing.");

	bool gotOK = false;
	VoidOperationResultHandler operationResult(gotOK);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
	if (!gotOK)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to activateFilter");
	}
}

void
OOPProtocolCPP1::authorizeFilter(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const WQLSelectStatement& filter,
	const String& eventType,
	const String& nameSpace,
	const StringArray& classes,
	const String& owner)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::authorizeFilter about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::AUTHORIZE_FILTER);
	BinarySerialization::writeWQLSelectStatement(obuf, filter);
	BinarySerialization::writeString(obuf, eventType);
	BinarySerialization::writeString(obuf, nameSpace);
	BinarySerialization::writeStringArray(obuf, classes);
	BinarySerialization::writeString(obuf, owner);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::authorizeFilter finished writing.");

	bool gotOK = false;
	VoidOperationResultHandler operationResult(gotOK);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
	if (!gotOK)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to authorizeFilter");
	}
}

void
OOPProtocolCPP1::deActivateFilter(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const WQLSelectStatement& filter,
	const String& eventType,
	const String& nameSpace,
	const StringArray& classes,
	bool lastActivation)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::deActivateFilter about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::DEACTIVATE_FILTER);
	BinarySerialization::writeWQLSelectStatement(obuf, filter);
	BinarySerialization::writeString(obuf, eventType);
	BinarySerialization::writeString(obuf, nameSpace);
	BinarySerialization::writeStringArray(obuf, classes);
	Bool(lastActivation).writeObject(obuf);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::deActivateFilter finished writing.");

	bool gotOK = false;
	VoidOperationResultHandler operationResult(gotOK);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
	if (!gotOK)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to deActivateFilter");
	}
}

int
OOPProtocolCPP1::mustPoll(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const WQLSelectStatement& filter,
	const String& eventType,
	const String& nameSpace,
	const StringArray& classes)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::mustPoll about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::MUST_POLL);
	BinarySerialization::writeWQLSelectStatement(obuf, filter);
	BinarySerialization::writeString(obuf, eventType);
	BinarySerialization::writeString(obuf, nameSpace);
	BinarySerialization::writeStringArray(obuf, classes);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::mustPoll finished writing.");

	Int32 returnValue(-1);
	bool gotOK = false;
	Int32OperationResultHandler operationResult(returnValue, gotOK);
	end(buf, in, out, timeout, env, operationResult, m_pprov);

	if (!gotOK)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to mustPoll");
	}
	return returnValue;

}

void
OOPProtocolCPP1::exportIndication(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMInstance& indHandlerInst, 
	const CIMInstance& indicationInst)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::exportIndication about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::EXPORT_INDICATION);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeInstance(obuf, indHandlerInst);
	BinarySerialization::writeInstance(obuf, indicationInst);
	
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::exportIndication finished writing.");

	bool gotOK = false;
	VoidOperationResultHandler operationResult(gotOK);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
	if (!gotOK)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to exportIndication");
	}
}

void
OOPProtocolCPP1::shuttingDown(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::shuttingDown about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::SHUTTING_DOWN);

	bool gotOK = false;
	VoidOperationResultHandler operationResult(gotOK);
	end(buf, in, out, timeout, env, operationResult, m_pprov);
	if (!gotOK)
	{
		OW_THROW(OOPProtocolCPP1Exception, "OOPProtocolCPP1: No result from call to shuttingDown");
	}
}

void
OOPProtocolCPP1::queryInstances(
	const UnnamedPipeRef& in,
	const UnnamedPipeRef& out,
	const Timeout& timeout,
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const WQLSelectStatement& query,
	CIMInstanceResultHandlerIFC& result,
	const CIMClass& cimClass)
{
	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG3(logger, "OOPProtocolCPP1::queryInstances about to start filling request buffer");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::QUERY_INSTANCES);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeWQLSelectStatement(obuf, query);
	BinarySerialization::writeClass(obuf, cimClass);

	CIMInstanceOperationResultHandler operationResult(result);
	end(buf, in, out, timeout, env, operationResult, m_pprov);

}

} // end namespace OW_NAMESPACE







