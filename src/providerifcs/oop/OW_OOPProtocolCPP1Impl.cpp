/*******************************************************************************
* Copyright (C) 2007 Quest Software, Inc. All rights reserved.
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
#include "OW_OOPProtocolCPP1Impl.hpp"
#include "blocxx/Format.hpp"
#include "OW_Logger.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_CIMException.hpp"
#include "blocxx/LogMessage.hpp"
#include "OW_OperationContext.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_OOPProviderBase.hpp"
#include "OW_OOPCIMOMHandleConnectionRunner.hpp"
#include "blocxx/DataStreams.hpp"
#include "OW_OOPDataOStreamBuf.hpp"
#include "blocxx/IOException.hpp"
#include "blocxx/SelectEngine.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

#define OW_OOP_LOG_DEBUG4(logger, message)

// use this version if you need to debug this code
//#define OW_OOP_LOG_DEBUG4(logger, message) OW_LOG_DEBUG(logger, message)

namespace OW_NAMESPACE
{

namespace OOPProtocolCPP1Impl
{
namespace
{
	const String COMPONENT_NAME("ow.provider.OOP.ifc");
}

OW_DEFINE_EXCEPTION(OOPProtocolCPP1);

int processOneRequest(std::streambuf & inbuf,
	std::streambuf & outbuf,
	std::deque<OutputEntry>& outputEntries,
	const ProviderEnvironmentIFCRef& env,
	OperationResultHandler& result,
	ThreadPool& threadPool,
	OOPProviderBase* pprov)

{
	Logger logger(COMPONENT_NAME);
	// Either out or outputEntries can be used.  If both are used in the same call, the data inserted
	// into out will be appended to outputEntries after the call returns.
	//std::ostream outstr(out);
	//std::istream instr(in);
	//instr.tie(&outstr);
	UInt8 op = 0xFE;
	OW_OOP_LOG_DEBUG4(logger, "processOneRequest about to read the code.");
	BinarySerialization::read(inbuf, op);
	OW_OOP_LOG_DEBUG4(logger, Format("processOneRequest read code: %1", static_cast<int>(op)));
	switch (op)
	{
		case BinarySerialization::BIN_OK:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got BIN_OK");
			result.handleResult(inbuf, op);
		}
		break;
		case BinarySerialization::BIN_ERROR:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got BIN_ERROR");
			String msg;
			BinarySerialization::read(inbuf, msg);
			OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
		}
		break;

		case BinarySerialization::BIN_EXCEPTION:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got BIN_EXCEPTION");
			UInt16 cimerrno;
			String cimMsg;
			BinarySerialization::read(inbuf, cimerrno);
			BinarySerialization::read(inbuf, cimMsg);
			OW_THROWCIMMSG(CIMException::ErrNoType(cimerrno), cimMsg.c_str());
		}
		break;

		case BinarySerialization::BINSIG_INST:
		case BinarySerialization::BINSIG_OP:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got BINSIG_INST or BINSIG_OP");
			result.handleResult(inbuf, op);
		}
		break;

		case BinarySerialization::BIN_END:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got BIN_END");
			return FINISHED;
		}

		case BinarySerialization::BIN_LOG_MESSAGE:
		{
			OW_OOP_LOG_DEBUG4(logger, "processOneRequest got BIN_LOG_MESSAGE");
			String component = BinarySerialization::readString(inbuf);
			String category = BinarySerialization::readString(inbuf);
			String message = BinarySerialization::readString(inbuf);
			String filename = BinarySerialization::readString(inbuf);
			int fileline;
			BinarySerialization::read(inbuf, fileline);
			String methodname = BinarySerialization::readString(inbuf);
			LogMessage logMessage(component, category, message, filename.c_str(), fileline, methodname.c_str());
			logger.logMessage(logMessage);
		}
		break;

		case BinarySerialization::OPERATION_CONTEXT_GET_DATA:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got OPERATION_CONTEXT_GET_DATA");
			String key = BinarySerialization::readString(inbuf);
			OW_LOG_DEBUG3(logger, Format("processOneRequest read key: %1", key));
			OperationContext::DataRef data = env->getOperationContext().getData(key);
			if (data)
			{
				OW_LOG_DEBUG3(logger, "processOneRequest found it, writing true");
				BinarySerialization::writeBool(outbuf, true);
				data->writeObject(outbuf);
			}
			else
			{
				OW_LOG_DEBUG3(logger, "processOneRequest didn't find it, writing false");
				BinarySerialization::writeBool(outbuf, false);
			}
			if (outbuf.pubsync() == -1)
			{
				OW_LOG_ERROR(logger, "processOneRequest flush failed!");
				OW_THROWCIMMSG(CIMException::FAILED, "Writing to process failed");
			}
		}
		break;

		case BinarySerialization::OPERATION_CONTEXT_SET_DATA:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got OPERATION_CONTEXT_SET_DATA");
			String key = BinarySerialization::readString(inbuf);
			OW_LOG_DEBUG3(logger, Format("processOneRequest read key: %1", key));
			String type = BinarySerialization::readString(inbuf);
			OW_LOG_DEBUG3(logger, Format("processOneRequest read type: %1", type));
			std::istream instr(&inbuf);
			HTTPChunkedIStreamBuffer chunkedIBuf(instr);
			OperationContext::DataRef data = env->getOperationContext().getData(key);
			if (data)
			{
				// replace the existing object
				data->readObject(chunkedIBuf);

				OW_LOG_DEBUG3(logger, "processOneRequest found it, writing true");
				BinarySerialization::writeBool(outbuf, true);
			}
			else if (type == OperationContext::StringData().getType())
			{
				OperationContext::DataRef data(new OperationContext::StringData());
				data->readObject(chunkedIBuf);
				env->getOperationContext().setData(key, data);
				OW_LOG_DEBUG3(logger, "processOneRequest didn't find it, but it was a string type, writing true");
				BinarySerialization::writeBool(outbuf, true);
			}
			else
			{
				OW_LOG_DEBUG3(logger, "processOneRequest didn't find it, not a string type, writing false");
				BinarySerialization::writeBool(outbuf, false);
			}

			// need to consume all the chunking
			HTTPUtils::eatEntity(chunkedIBuf);
		}
		break;

		case BinarySerialization::OPERATION_CONTEXT_REMOVE_DATA:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got OPERATION_CONTEXT_REMOVE_DATA");
			String key = BinarySerialization::readString(inbuf);
			OW_LOG_DEBUG3(logger, Format("processOneRequest read key: %1", key));
			if (env->getOperationContext().keyHasData(key))
			{
				OW_LOG_DEBUG3(logger, "processOneRequest found it, writing true");
				BinarySerialization::writeBool(outbuf, true);
				if (outbuf.pubsync() == -1)
				{
					OW_LOG_DEBUG3(logger, "processOneRequest flush failed!");
					OW_THROWCIMMSG(CIMException::FAILED, "Writing to process failed");
				}
				env->getOperationContext().removeData(key);
			}
			else
			{
				OW_LOG_DEBUG3(logger, "processOneRequest didn't find it, writing false");
				BinarySerialization::writeBool(outbuf, false);
			}
		}
		break;

		case BinarySerialization::OPERATION_CONTEXT_KEY_HAS_DATA:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got OPERATION_CONTEXT_KEY_HAS_DATA");
			String key = BinarySerialization::readString(inbuf);
			OW_LOG_DEBUG3(logger, Format("processOneRequest read key: %1", key));
			BinarySerialization::writeBool(
				outbuf, env->getOperationContext().keyHasData(key));
			if (outbuf.pubsync() == -1)
			{
				OW_LOG_DEBUG(logger, "processOneRequest flush failed!");
				OW_THROWCIMMSG(CIMException::FAILED, "Writing to process failed");
			}
		}
		break;

		case BinarySerialization::OPERATION_CONTEXT_GET_OPERATION_ID:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got OPERATION_CONTEXT_GET_OPERATION_ID");
			BinarySerialization::write(outbuf, env->getOperationContext().getOperationId());
			if (outbuf.pubsync() == -1)
			{
				OW_LOG_DEBUG(logger, "processOneRequest flush failed!");
				OW_THROWCIMMSG(CIMException::FAILED, "Writing to process failed");
			}
		}
		break;

		case BinarySerialization::PROVIDER_ENVIRONMENT_REQUEST:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got PROVIDER_ENVIRONMENT_REQUEST");
			UnnamedPipeRef envconn = pprov->startClonedProviderEnv(env);
			if (envconn)
			{
				// Provider environment thread is now running in the
				// OOPProviderBase thread pool.
				outputEntries.push_back(OutputEntry(Array<unsigned char>(1, BinarySerialization::BIN_OK)));
				outputEntries.push_back(OutputEntry(OutputEntry::E_INPUT, envconn));
				outputEntries.push_back(OutputEntry(OutputEntry::E_OUTPUT, envconn));
			}
			else
			{
				// Assume OOPProviderBase was unable to start the thread
				OW_LOG_DEBUG(logger, "processOneRequest PROVIDER_ENVIRONMENT_REQUEST returning BIN_ERROR");
				BinarySerialization::write(outbuf, BinarySerialization::BIN_ERROR);
				BinarySerialization::write(outbuf, String("thread limit reached"));
			}
		}
		break;

		case BinarySerialization::CIMOM_HANDLE_REQUEST:
		case BinarySerialization::REPOSITORY_CIMOM_HANDLE_REQUEST:
		case BinarySerialization::LOCKED_CIMOM_HANDLE_REQUEST:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got CIMOM_HANDLE_REQUEST, REPOSITORY_CIMOM_HANDLE_REQUEST or LOCKED_CIMOM_HANDLE_REQUEST");
			UInt8 lockTypeArg = 0;
			if (op == BinarySerialization::LOCKED_CIMOM_HANDLE_REQUEST)
			{
				BinarySerialization::read(inbuf, lockTypeArg);
			}
			UnnamedPipeRef connToKeep;
			UnnamedPipeRef connToSend;
			UnnamedPipe::createConnectedPipes(connToKeep, connToSend);
			// Setting the timeouts to be infinite won't cause a problem here.  The OOP interface enforces a timeout for each operation and
			// will kill the provider if we exceeds it. That will cause the other side of the pipe to close and the thread will then exit.
			connToKeep->setTimeouts(Timeout::infinite);
			// Pass a timeout to tryAddWork, because there could be threads in the pool that haven't terminated yet, but
			// just need a chance to run and detect a closed connection. This can happen if the provider creates/destroys
			// a number of CIMOMHandle instances in quick succession.
			try
			{
				if (threadPool.tryAddWork(RunnableRef(new OOPCIMOMHandleConnectionRunner(connToKeep, op, lockTypeArg, env)), Timeout::relative(10)))
				{
					outputEntries.push_back(OutputEntry(Array<unsigned char>(1, BinarySerialization::BIN_OK)));
					outputEntries.push_back(OutputEntry(OutputEntry::E_INPUT, connToSend));
					outputEntries.push_back(OutputEntry(OutputEntry::E_OUTPUT, connToSend));
				}
				else
				{
					BinarySerialization::write(outbuf, BinarySerialization::BIN_ERROR);
					BinarySerialization::write(outbuf, String("thread limit reached"));
				}
			}
			catch (TimeoutException& e)
			{
				// This may happen for a LOCKED_CIMOM_HANDLE_REQUEST
				BinarySerialization::write(outbuf, BinarySerialization::BIN_ERROR);
				BinarySerialization::write(outbuf, String(Format("Timed out: %1", e)));
			}
		}
		break;

		case BinarySerialization::GET_CONFIG_ITEM:
		{
			OW_LOG_DEBUG3(logger, "processOneRequest got GET_CONFIG_ITEM");
			String name = BinarySerialization::readString(inbuf);
			String defRetVal = BinarySerialization::readString(inbuf);
			String rv = env->getConfigItem(name, defRetVal);
			BinarySerialization::writeString(outbuf, rv);
		}
		break;

		case BinarySerialization::GET_MULTI_CONFIG_ITEM:
		{
			String itemName = BinarySerialization::readString(inbuf);
			StringArray defRetVal = BinarySerialization::readStringArray(inbuf);
			String tokenizeSeparator = BinarySerialization::readString(inbuf);
			StringArray rv = env->getMultiConfigItem(itemName, defRetVal, tokenizeSeparator.c_str());
			BinarySerialization::writeStringArray(outbuf, rv);
		}
		break;

		default:
			OW_LOG_ERROR(logger, Format("Unexpected value received from provider: %1", static_cast<int>(op)));
			OW_THROWCIMMSG(CIMException::FAILED, Format("Unexpected value received from provider: %1", static_cast<int>(op)).c_str());
	}
	return CONTINUE;
}

void
NoResultHandler::handleResult(std:: streambuf &instr, UInt8 op)
{
	OW_THROW(OOPProtocolCPP1Exception, Format("Invalid op, expected nothing, got: %1", static_cast<int>(op)).c_str());
}

ShutdownThreadPool::ShutdownThreadPool(ThreadPool& tp)
	: m_tp(tp)
{
}

ShutdownThreadPool::~ShutdownThreadPool()
{
	// use an infinite timeout so the threads don't get definitively cancelled, but immediately cooperatively cancel them.
	m_tp.shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, Timeout::relative(0.1), Timeout::infinite);
}

int process(Array<unsigned char>& in,
	std::deque<OutputEntry>& outputEntries,
	const ProviderEnvironmentIFCRef& env,
	const Logger& logger,
	OperationResultHandler& result,
	ThreadPool& threadPool,
	OOPProviderBase* pprov)
{
	OW_LOG_DEBUG3(logger, Format("in process(). in.size() = %1", in.size()));
	if (in.size() == 0)
	{
		return FINISHED;
	}

	std::streambuf::pos_type numRead = std::streambuf::pos_type(std::streambuf::off_type(-1));
	DataIStreamBuf inbuf(in.size(), &in[0]);
	try
	{
		// this loop can end once all the data from in has been read, then processOneRequest() will throw an IOException.
		int processrv = CONTINUE;
		while (processrv == CONTINUE && static_cast<size_t>(numRead) != in.size())
		{
			Array<unsigned char> out;
			OOPDataOStreamBuf outbuf(out);
			processrv = processOneRequest(inbuf, outbuf, outputEntries, env, result, threadPool, pprov);
			if (!out.empty())
			{
				OW_LOG_DEBUG3(logger, Format("processOneRequest() found %1 bytes of out. Adding to outputEntries.", out.size()));
				outputEntries.push_back(OutputEntry(out));
			}
			// successful request, store how much was read so it can be removed when we're done.
			numRead = inbuf.pubseekoff(0, std::ios::cur);
			OW_OOP_LOG_DEBUG4(logger, Format("processOneRequest() done. numRead = %1", numRead));
		}

		// if the loop finished by returning FINISHED
		if (numRead != std::streambuf::pos_type(std::streambuf::off_type(-1)))
		{
			in.erase(in.begin(), in.begin() + numRead);
		}
		return processrv;
	}
	catch (IOException& e)
	{
		OW_LOG_DEBUG3(logger, Format("processOneRequest() threw IOException: %1", e));

		// check that the exception happened because of reaching the end of the stream.
		// Otherwise, re-throw. This will happen if something is corrupted and we can't parse the input.
		if (static_cast<size_t>(inbuf.pubseekoff(0, std::ios::cur)) != in.size())
		{
			OW_LOG_INFO(logger, "IOException not caused by reaching end of input.  process() Re-throwing");
			throw;
		}

		// loop terminated with a partial request
		if (numRead != std::streambuf::pos_type(std::streambuf::off_type(-1)))
		{
			in.erase(in.begin(), in.begin() + numRead);
		}
	}
	return CONTINUE;
}

OOPSelectableCallback::OOPSelectableCallback(
	Array<unsigned char>& inputBuf,
	std::deque<OutputEntry>& outputEntries,
	const UnnamedPipeRef& inputPipe,
	const UnnamedPipeRef& outputPipe,
	const ProviderEnvironmentIFCRef& env,
	OperationResultHandler& result,
	SelectEngine& selectEngine,
	bool &finishedSuccessfully,
	ThreadPool& threadPool,
	OOPProviderBase* pprov
	)
	: m_inputBuf(inputBuf)
	, m_outputEntries(outputEntries)
	, m_inputPipe(inputPipe)
	, m_outputPipe(outputPipe)
	, m_env(env)
	, m_result(result)
	, m_selectEngine(selectEngine)
	, m_finishedSuccessfully(finishedSuccessfully)
	, m_threadPool(threadPool)
	, m_pprov(pprov)
{

}

void
OOPSelectableCallback::doSelected(Select_t& selectedObject, EEventType eventType)
{
	Logger logger(COMPONENT_NAME);
	if (eventType == E_READ_EVENT)
	{
		OW_LOG_DEBUG3(logger, "doSelected() got a read event");
		// read all the data out of the pipe and append it to m_inputBuf
		size_t oldSize = m_inputBuf.size();
		const unsigned int INCREMENT = 1024;
		bool moreDataToRead = true;
		while (moreDataToRead)
		{
			m_inputBuf.resize(oldSize + INCREMENT, 0xFA);
			OW_OOP_LOG_DEBUG4(logger, Format("reading some data into offset %1", oldSize));
			ssize_t numRead = m_inputPipe->read(&m_inputBuf[oldSize], INCREMENT, IOIFC::E_RETURN_ON_ERROR);
			int lerrno = errno;
			OW_OOP_LOG_DEBUG4(logger, Format("read() returned %1" , numRead));
			if (numRead == -1)
			{
				m_inputBuf.resize(oldSize, 0xFB);
				if (lerrno == ETIMEDOUT)
				{
					 moreDataToRead = false; // end the loop
					 OW_OOP_LOG_DEBUG4(logger, "Detected ETIMEDOUT");
				}
				else
				{
					OW_THROW_ERRNO_MSG(OOPProtocolCPP1Exception, "read error");
				}
			}
			else
			{
				m_inputBuf.resize(oldSize + numRead, 0xFC);
			}

			if (numRead == 0)
			{
				// closed, so stop the select engine
				moreDataToRead = false;
				m_selectEngine.stop();
			}

			oldSize = m_inputBuf.size();
		}
		// now that we've read the data, see if we can do anything with it.
		if (process(m_inputBuf, m_outputEntries, m_env, logger, m_result, m_threadPool, m_pprov) == FINISHED)
		{
			OW_LOG_DEBUG3(logger, "process() returned FINISHED, telling the select engine to stop");
			m_selectEngine.stop();
			m_finishedSuccessfully = true;
		}
		else
		{
			OW_OOP_LOG_DEBUG4(logger, Format("doSelected(): process() returned CONTINUE. m_outputEntries.size() = %1", m_outputEntries.size()));
			if (!m_outputEntries.empty())
			{
				// add it
				m_selectEngine.addSelectableObject(m_outputPipe->getWriteSelectObj(), this, SelectableCallbackIFC::E_WRITE_EVENT);
			}
		}
	}
	else if (eventType == E_WRITE_EVENT)
	{
		OW_LOG_DEBUG3(logger, "doSelected() got a write event");
		if (m_outputEntries.empty())
		{
			OW_LOG_DEBUG3(logger, "m_outputEntries.empty(), removing from select engine");
			m_selectEngine.removeSelectableObject(m_outputPipe->getWriteSelectObj(), SelectableCallbackIFC::E_WRITE_EVENT);
			return;
		}

		try
		{
			OutputEntry& entry(m_outputEntries[0]);
			if (entry.type == OutputEntry::E_PASS_PIPE)
			{
				if (entry.direction == OutputEntry::E_INPUT)
				{
					OW_LOG_DEBUG3(logger, Format("doSelected() attempting to pass a input descriptor: %1", entry.pipe->getInputDescriptor()));
					m_outputPipe->passDescriptor(entry.pipe->getInputDescriptor(), m_inputPipe);
				}
				else
				{
					OW_LOG_DEBUG3(logger, Format("doSelected() attempting to pass a output descriptor: %1", entry.pipe->getOutputDescriptor()));
					m_outputPipe->passDescriptor(entry.pipe->getOutputDescriptor(), m_inputPipe);
				}
				m_outputEntries.erase(m_outputEntries.begin());
			}
			else // entry.type == E_BUFFER
			{
				OW_LOG_DEBUG3(logger, Format("doSelected() attempting to write %1 bytes", entry.buf.size()));
				OW_ASSERT(entry.buf.size() > 0);
				ssize_t numWrote = 0;
				try
				{
					numWrote = m_outputPipe->write(&entry.buf[0], entry.buf.size(), IOIFC::E_THROW_ON_ERROR);
				}
				catch (IOException& e)
				{
					// EAGAIN isn't an error, we just need to try again later.
					if (e.getErrorCode() != EAGAIN)
					{
						throw;
					}
					OW_LOG_DEBUG3(logger, "doSelected() got EAGAIN while attempting to write.");
				}
				OW_LOG_DEBUG3(logger, Format("write returned %1", numWrote));
				if (static_cast<size_t>(numWrote) == entry.buf.size())
				{
					m_outputEntries.erase(m_outputEntries.begin());
				}
				else
				{
					entry.buf.erase(entry.buf.begin(), entry.buf.begin() + numWrote);
				}
			}
		}
		catch (IOException& e)
		{
			OW_LOG_ERROR(logger, Format("doSelected() error while writing: %1", e));
			// Something went horribly wrong.  Just clear out the output queue, so we stop trying.
			// Don't propagate the exception so that the select engine can keep going, so that
			// log messages or other input from the provider can be processed.
			m_outputEntries.clear();
		}

		if (m_outputEntries.empty())
		{
			m_selectEngine.removeSelectableObject(m_outputPipe->getWriteSelectObj(), SelectableCallbackIFC::E_WRITE_EVENT);
			return;
		}
	}
}


} // end namespace OOPProtocolCPP1
} // end namespace OW_NAMESPACE

