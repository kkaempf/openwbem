/*******************************************************************************
* Copyright (C) 2006 Novell, Inc. All rights reserved.
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
*  - Neither the name of Novell, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_OOPClonedProviderEnv.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_IOException.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_Format.hpp"
#include "OW_LogMessage.hpp"
#include "OW_OperationContext.hpp"
#include "OW_BinaryRequestHandler.hpp"
#include "OW_Assertion.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_SelectEngine.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_OOPProviderBase.hpp"
#include "OW_OOPDataOStreamBuf.hpp"

#include <deque>
#include <iosfwd>

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(ClonedOOPProtocolCPP1);
OW_DEFINE_EXCEPTION(ClonedOOPProtocolCPP1);

namespace
{

const String COMPONENT_NAME("ow.provider.OOP.ifc");
enum { CONTINUE, FINISHED };

//////////////////////////////////////////////////////////////////////////////
struct OutputEntry
{
	enum EEntryType { E_PASS_PIPE, E_BUFF };
	enum EDirectionIndicator { E_INPUT, E_OUTPUT };

	OutputEntry(const Array<unsigned char>& b) : type(E_BUFF) , buf(b)
	{
		OW_ASSERT(buf.size() > 0);
	}

	OutputEntry(EDirectionIndicator d, const UnnamedPipeRef& p) : type(E_PASS_PIPE) , direction(d) , pipe(p)
	{
	}

	EEntryType type;
	EDirectionIndicator direction;
	UnnamedPipeRef pipe;
	Array<unsigned char> buf;
};

//////////////////////////////////////////////////////////////////////////////
struct ShutdownThreadPool
{
	ShutdownThreadPool(ThreadPool& tp) : m_tp(tp) {}
	~ShutdownThreadPool()
	{
		// use an infinite timeout so the threads don't get definitively cancelled, 
		// but immediately cooperatively cancel them.
		m_tp.shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, Timeout::relative(0.1), Timeout::infinite);
	}
	ThreadPool& m_tp;
};

//////////////////////////////////////////////////////////////////////////////
int clonedEvnProcessOneRequest(std::streambuf & in,
	std::streambuf & out,
	std::deque<OutputEntry>& outputEntries,
	const ProviderEnvironmentIFCRef& env,
	ThreadPool& threadPool,
	OOPProviderBase* pprov);

int clonedEnvProcess(Array<unsigned char>& in,
	std::deque<OutputEntry>& outputEntries,
	const ProviderEnvironmentIFCRef& env,
	const Logger& logger, 
	ThreadPool& threadPool,
	OOPProviderBase* pprov)
{
	OW_LOG_DEBUG3(logger, Format("in clonedEnvProcess(). in.size() = %1", in.size()));
	if (in.size() == 0)
	{
		return FINISHED;
	}

	std::streambuf::pos_type numRead = std::streambuf::pos_type(std::streambuf::off_type(-1));
	DataIStreamBuf inbuf(in.size(), &in[0]);
	try
	{
		// this loop can end once all the data from in has been read, then 
		// clonedEvnProcessOneRequest() will throw an IOException.
		int processrv = CONTINUE;
		while (processrv == CONTINUE && static_cast<size_t>(numRead) != in.size())
		{
			Array<unsigned char> out;
			OOPDataOStreamBuf outbuf(out);
			processrv = clonedEvnProcessOneRequest(inbuf, outbuf, outputEntries, env, threadPool, pprov);
			if (!out.empty())
			{
				OW_LOG_DEBUG3(logger, Format("clonedEvnProcessOneRequest() found %1 bytes of out. Adding to outputEntries.", out.size()));
				outputEntries.push_back(OutputEntry(out));
			}
			// successful request, store how much was read so it can be removed when we're done.
			numRead = inbuf.pubseekoff(0, std::ios::cur);
			OW_LOG_DEBUG3(logger, Format("clonedEvnProcessOneRequest() done. numRead = %1", numRead));
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
		OW_LOG_DEBUG2(logger, Format("clonedEvnProcessOneRequest() threw IOException: %1", e));

		// check that the exception happened because of reaching the end of the stream.
		// Otherwise, re-throw. This will happen if something is corrupted and we can't parse the input.
		if (static_cast<size_t>(inbuf.pubseekoff(0, std::ios::cur)) != in.size())
		{
			OW_LOG_DEBUG(logger, "IOException not caused by reaching end of input.  clonedEnvProcess() Re-throwing");
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

//////////////////////////////////////////////////////////////////////////////
class ClonedProvEnvSelectableCallback : public SelectableCallbackIFC
{
public:
	ClonedProvEnvSelectableCallback(
		Array<unsigned char>& inputBuf,
		std::deque<OutputEntry>& outputEntries,
		const UnnamedPipeRef& conn,
		const ProviderEnvironmentIFCRef& env,
		SelectEngine& selectEngine,
		bool &finishedSuccessfully,
		ThreadPool& threadPool,
		OOPProviderBase* pprov
		)
		: m_inputBuf(inputBuf)
		, m_outputEntries(outputEntries)
		, m_conn(conn)
		, m_env(env)
		, m_selectEngine(selectEngine)
		, m_finishedSuccessfully(finishedSuccessfully)
		, m_threadPool(threadPool)
		, m_pprov(pprov)
	{
	}

	void doSelected(Select_t& selectedObject, EEventType eventType)
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
				OW_LOG_DEBUG3(logger, Format("reading some data into offset %1", oldSize));
				ssize_t numRead = m_conn->read(&m_inputBuf[oldSize], INCREMENT, IOIFC::E_RETURN_ON_ERROR);
				int lerrno = errno;
				OW_LOG_DEBUG3(logger, Format("read() returned %1" , numRead));
				if (numRead == -1)
				{
					m_inputBuf.resize(oldSize, 0xFB);
					if (lerrno == ETIMEDOUT)
					{
						 moreDataToRead = false; // end the loop
						 OW_LOG_DEBUG3(logger, "Detected ETIMEDOUT");
					}
					else
					{
						OW_THROW_ERRNO_MSG(ClonedOOPProtocolCPP1Exception, "read error");
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
			if (clonedEnvProcess(m_inputBuf, m_outputEntries, m_env, logger, m_threadPool, m_pprov) == FINISHED)
			{
				OW_LOG_DEBUG2(logger, "clonedEnvProcess() returned FINISHED, telling the select engine to stop");
				m_selectEngine.stop();
				m_finishedSuccessfully = true;
			}
			else
			{
				OW_LOG_DEBUG3(logger, Format("doSelected(): clonedEnvProcess() returned CONTINUE. m_outputEntries.size() = %1", m_outputEntries.size()));
				if (!m_outputEntries.empty())
				{
					// add it
					m_selectEngine.addSelectableObject(m_conn->getWriteSelectObj(), this, SelectableCallbackIFC::E_WRITE_EVENT);
				}
			}
		}
		else if (eventType == E_WRITE_EVENT)
		{
			OW_LOG_DEBUG3(logger, "doSelected() got a write event");
			if (m_outputEntries.empty())
			{
				OW_LOG_DEBUG3(logger, "m_outputEntries.empty(), removing from select engine");
				m_selectEngine.removeSelectableObject(m_conn->getWriteSelectObj(), SelectableCallbackIFC::E_WRITE_EVENT);
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
						m_conn->passDescriptor(entry.pipe->getInputDescriptor(), m_conn);
					}
					else
					{
						OW_LOG_DEBUG3(logger, Format("doSelected() attempting to pass a output descriptor: %1", entry.pipe->getOutputDescriptor()));
						m_conn->passDescriptor(entry.pipe->getOutputDescriptor());
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
						numWrote = m_conn->write(&entry.buf[0], entry.buf.size(), IOIFC::E_THROW_ON_ERROR);
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
				m_selectEngine.removeSelectableObject(m_conn->getWriteSelectObj(), SelectableCallbackIFC::E_WRITE_EVENT);
				return;
			}
		}
	}

	Array<unsigned char>& m_inputBuf;
	std::deque<OutputEntry>& m_outputEntries;
	UnnamedPipeRef m_conn;
	ProviderEnvironmentIFCRef m_env;
	SelectEngine& m_selectEngine;
	bool& m_finishedSuccessfully;
	ThreadPool& m_threadPool;
	OOPProviderBase* m_pprov;
};

//////////////////////////////////////////////////////////////////////////////
class ClonedProvEnvCallbackSeriveEnv : public ServiceEnvironmentIFC
{
public:
	ClonedProvEnvCallbackSeriveEnv(UInt8 type, const ProviderEnvironmentIFCRef& provEnv)
		: m_type(type)
		, m_provEnv(provEnv)
	{
	}

	virtual String getConfigItem(const String& item, const String& defRetVal) const
	{
		return m_provEnv->getConfigItem(item, defRetVal);
	}

	virtual CIMOMHandleIFCRef getCIMOMHandle(OperationContext& context,
		EBypassProvidersFlag bypassProviders) const
	{
		if (m_type == BinarySerialization::CIMOM_HANDLE_REQUEST)
			return m_provEnv->getCIMOMHandle();
		else if (m_type == BinarySerialization::REPOSITORY_CIMOM_HANDLE_REQUEST)
			return m_provEnv->getRepositoryCIMOMHandle();
		else
			OW_ASSERT(0);
		return CIMOMHandleIFCRef();
	}
	
private:
	UInt8 m_type;
	ProviderEnvironmentIFCRef m_provEnv;
};

///////////////////////////////////////////////////////////////////////////
class CloneCIMOMHandleConnectionRunner : public Runnable
{
public:
	CloneCIMOMHandleConnectionRunner(const UnnamedPipeRef& conn, UInt8 op,
			const ProviderEnvironmentIFCRef& provEnv)
		: m_conn(conn)
		, m_binaryRH(new BinaryRequestHandler(BinaryRequestHandler::E_SUPPORT_EXPORT_INDICATION))
		, m_inbuf(m_conn.getPtr())
		, m_instr(&m_inbuf)
		, m_outbuf(m_conn.getPtr())
		, m_outstr(&m_outbuf)
		, m_env(provEnv)
		, m_cancelled(false)
	{
		m_binaryRH->init(ServiceEnvironmentIFCRef(
			new ClonedProvEnvCallbackSeriveEnv(op, provEnv)));
		m_instr.tie(&m_outstr);
	}

	virtual void run()
	{
		Logger logger(COMPONENT_NAME);
		while (m_instr.good() && !cancelled())
		{
			UInt8 op;
			BinarySerialization::read(*m_instr.rdbuf(), op);
			if (op == BinarySerialization::BIN_END)
			{
				OW_LOG_DEBUG3(logger, "CloneCIMOMHandleConnectionRunner::run() received BIN_END request. shutting down cimom handle");
				return;
			}
			else if (op != BinarySerialization::BIN_OK)
			{
				return;
			}

			HTTPChunkedIStream operationIstr(m_instr);
			HTTPChunkedOStream operationOstr(m_outstr);
			TempFileStream operationErrstr;
			OW_LOG_DEBUG2(logger, "CloneCIMOMHandleConnectionRunner::run() calling m_binaryRH->process");
			m_binaryRH->process(&operationIstr, &operationOstr, &operationErrstr, m_env->getOperationContext());
			HTTPUtils::eatEntity(operationIstr);
			if (m_binaryRH->hasError())
			{
				OW_LOG_DEBUG2(logger, "CloneCIMOMHandleConnectionRunner::run() m_binaryRH->hasError()");
				operationOstr.termOutput(HTTPChunkedOStream::E_DISCARD_LAST_CHUNK);
				operationErrstr.rewind();
				m_outstr << operationErrstr.rdbuf();
			}
			else
			{
				OW_LOG_DEBUG2(logger, "CloneCIMOMHandleConnectionRunner::run() sending result and BIN_OK");
				operationOstr.termOutput(HTTPChunkedOStream::E_SEND_LAST_CHUNK);
				BinarySerialization::write(m_outbuf, BinarySerialization::BIN_OK);
			}

			if (m_outbuf.pubsync() == -1)
				OW_LOG_ERROR(logger, "CloneCIMOMHandleConnectionRunner::run() failed to flush output");
		}
		OW_LOG_DEBUG2(logger, "CloneCIMOMHandleConnectionRunner::run() finished");
	}
	bool cancelled()
	{
		NonRecursiveMutexLock lock(m_guard);
		return m_cancelled;
	}
	virtual void doShutdown()
	{
		Logger logger(COMPONENT_NAME);
		NonRecursiveMutexLock lock(m_guard);
		m_cancelled = true;
	}

private:
	UnnamedPipeRef m_conn;
	IntrusiveReference<BinaryRequestHandler> m_binaryRH;
	IOIFCStreamBuffer m_inbuf;
	std::istream m_instr;
	IOIFCStreamBuffer m_outbuf;
	std::ostream m_outstr;
	ProviderEnvironmentIFCRef m_env;
	bool m_cancelled;
	NonRecursiveMutex m_guard;
};

//////////////////////////////////////////////////////////////////////////////
int clonedEvnProcessOneRequest(std::streambuf & inbuf,
	std::streambuf & outbuf,
	std::deque<OutputEntry>& outputEntries,
	const ProviderEnvironmentIFCRef& env,
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
	OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest about to read the code.");
	BinarySerialization::read(inbuf, op);
	OW_LOG_DEBUG3(logger, Format("clonedEvnProcessOneRequest read code: %1", static_cast<int>(op)));
	switch (op)
	{
		case BinarySerialization::BIN_OK:
		{
			OW_LOG_DEBUG2(logger, "clonedEvnProcessOneRequest got BIN_OK. Not implemented");
		}
		break;

		case BinarySerialization::BIN_ERROR:
		{
			OW_LOG_DEBUG2(logger, "clonedEvnProcessOneRequest got BIN_ERROR");
			String msg;
			BinarySerialization::read(inbuf, msg);
			OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
		}
		break;

		case BinarySerialization::BIN_END:
		{
			OW_LOG_DEBUG2(logger, "clonedEvnProcessOneRequest got BIN_END");
			return FINISHED;
		}

		case BinarySerialization::BIN_LOG_MESSAGE:
		{
			OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest got BIN_LOG_MESSAGE");
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
			OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest got OPERATION_CONTEXT_GET_DATA");
			String key = BinarySerialization::readString(inbuf);
			OW_LOG_DEBUG3(logger, Format("clonedEvnProcessOneRequest read key: %1", key));
			OperationContext::DataRef data = env->getOperationContext().getData(key);
			if (data)
			{
				OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest found it, writing true");
				BinarySerialization::writeBool(outbuf, true);
				data->writeObject(outbuf);
			}
			else
			{
				OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest didn't find it, writing false");
				BinarySerialization::writeBool(outbuf, false);
			}
			if (outbuf.pubsync() == -1)
			{
				OW_LOG_DEBUG(logger, "clonedEvnProcessOneRequest flush failed!");
				OW_THROWCIMMSG(CIMException::FAILED, "Writing to process failed");
			}
		}
		break;

		case BinarySerialization::OPERATION_CONTEXT_SET_DATA:
		{
			OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest got OPERATION_CONTEXT_SET_DATA");
			String key = BinarySerialization::readString(inbuf);
			OW_LOG_DEBUG3(logger, Format("clonedEvnProcessOneRequest read key: %1", key));
			String type = BinarySerialization::readString(inbuf);
			OW_LOG_DEBUG3(logger, Format("clonedEvnProcessOneRequest read type: %1", type));
			std::istream instr(&inbuf);
			HTTPChunkedIStreamBuffer chunkedIBuf(instr);
			OperationContext::DataRef data = env->getOperationContext().getData(key);
			if (data)
			{
				// replace the existing object
				data->readObject(chunkedIBuf);

				OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest found it, writing true");
				BinarySerialization::writeBool(outbuf, true);
			}
			else if (type == OperationContext::StringData().getType())
			{
				OperationContext::DataRef data(new OperationContext::StringData());
				data->readObject(chunkedIBuf);
				env->getOperationContext().setData(key, data);
				OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest didn't find it, but it was a string type, writing true");
				BinarySerialization::writeBool(outbuf, true);
			}
			else
			{
				OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest didn't find it, writing false");
				BinarySerialization::writeBool(outbuf, false);
			}
			
			// need to consume all the chunking
			HTTPUtils::eatEntity(chunkedIBuf);
		}
		break;

		case BinarySerialization::OPERATION_CONTEXT_REMOVE_DATA:
		{
			OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest got OPERATION_CONTEXT_REMOVE_DATA");
			String key = BinarySerialization::readString(inbuf);
			OW_LOG_DEBUG3(logger, Format("clonedEvnProcessOneRequest read key: %1", key));
			if (env->getOperationContext().keyHasData(key))
			{
				OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest found it, writing true");
				BinarySerialization::writeBool(outbuf, true);
				if (outbuf.pubsync() == -1)
				{
					OW_LOG_DEBUG(logger, "clonedEvnProcessOneRequest flush failed!");
					OW_THROWCIMMSG(CIMException::FAILED, "Writing to process failed");
				}
				env->getOperationContext().removeData(key);
			}
			else
			{
				OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest didn't find it, writing false");
				BinarySerialization::writeBool(outbuf, false);
			}
		}
		break;

		case BinarySerialization::OPERATION_CONTEXT_KEY_HAS_DATA:
		{
			OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest got OPERATION_CONTEXT_KEY_HAS_DATA");
			String key = BinarySerialization::readString(inbuf);
			OW_LOG_DEBUG3(logger, Format("clonedEvnProcessOneRequest read key: %1", key));
			BinarySerialization::writeBool(
				outbuf, env->getOperationContext().keyHasData(key));
			if (outbuf.pubsync() == -1)
			{
				OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest flush failed!");
				OW_THROWCIMMSG(CIMException::FAILED, "Writing to process failed");
			}
		}
		break; 

		case BinarySerialization::OPERATION_CONTEXT_GET_OPERATION_ID:
		{
			OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest got OPERATION_CONTEXT_GET_OPERATION_ID");
			BinarySerialization::write(outbuf, env->getOperationContext().getOperationId());
			if (outbuf.pubsync() == -1)
			{
				OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest flush failed!");
				OW_THROWCIMMSG(CIMException::FAILED, "Writing to process failed");
			}
		}
		break; 

		case BinarySerialization::PROVIDER_ENVIRONMENT_REQUEST:
		{
			OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest got PROVIDER_ENVIRONMENT_REQUEST");
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
				OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest PROVIDER_ENVIRONMENT_REQUEST returning BIN_ERROR");
				BinarySerialization::write(outbuf, BinarySerialization::BIN_ERROR);
				BinarySerialization::write(outbuf, String("thread limit reached"));
			}
		}
		break;

		case BinarySerialization::CIMOM_HANDLE_REQUEST:
		case BinarySerialization::REPOSITORY_CIMOM_HANDLE_REQUEST:
		{
			OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest got CIMOM_HANDLE_REQUEST or REPOSITORY_CIMOM_HANDLE_REQUEST");
			UnnamedPipeRef connToKeep;
			UnnamedPipeRef connToSend;
			UnnamedPipe::createConnectedPipes(connToKeep, connToSend);
			// Setting the timeouts to be infinite won't cause a problem here.  The OOP interface enforces a timeout for each operation and
			// will kill the provider if we exceeds it. That will cause the other side of the pipe to close and the thread will then exit.
			connToKeep->setTimeouts(Timeout::infinite);
			// Pass a timeout to tryAddWork, because there could be threads in the pool that haven't terminated yet, but
			// just need a chance to run and detect a closed connection. This can happen if the provider creates/destroys
			// a number of CIMOMHandle instances in quick succession.
			if (threadPool.tryAddWork(RunnableRef(new CloneCIMOMHandleConnectionRunner(connToKeep, op, env)),
				Timeout::relative(10)))
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
		break;

		case BinarySerialization::GET_CONFIG_ITEM:
		{
			OW_LOG_DEBUG3(logger, "clonedEvnProcessOneRequest got GET_CONFIG_ITEM");
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


}	// End of unnamed namespace

//////////////////////////////////////////////////////////////////////////////
OOPClonedProviderEnv::OOPClonedProviderEnv(
	OOPProviderBase* pprov, 
	const UnnamedPipeRef& conn,
	const ProviderEnvironmentIFCRef& provEnv)
	: Runnable()
	, m_pprov(pprov)
	, m_conn(conn)
	, m_env(provEnv)
{
	m_conn->setTimeouts(Timeout::relative(0));
	m_conn->setWriteBlocking(UnnamedPipe::E_NONBLOCKING);
}

//////////////////////////////////////////////////////////////////////////////
void 
OOPClonedProviderEnv::run()
{
	Logger logger(COMPONENT_NAME);
	const int MAX_CALLBACK_THREADS = 10;
	ThreadPool threadPool(ThreadPool::DYNAMIC_SIZE_NO_QUEUE, 10, 10, "ClonedOOPProtocolCPP1");
	ShutdownThreadPool stp(threadPool);
	SelectEngine selectEngine;
	Array<unsigned char> inputBuf;
	std::deque<OutputEntry> outputEntries;

	bool finishedSuccessfully = false;
	ShutdownThreadPool shutdownThreadPool(threadPool);

	SelectableCallbackIFCRef callback(new ClonedProvEnvSelectableCallback(
		inputBuf, outputEntries, m_conn, m_env, selectEngine,
		finishedSuccessfully, threadPool, m_pprov));

	selectEngine.addSelectableObject(m_conn->getReadSelectObj(), callback, 
		SelectableCallbackIFC::E_READ_EVENT);

	OW_LOG_DEBUG3(logger, "OOPClonedProviderEnv::run() about to run the select engine");
	try
	{
		selectEngine.go(Timeout::infinite);
	}
	catch (Exception& e)
	{
		OW_LOG_DEBUG(logger, Format("OOPClonedProviderEnv::run() caught select exception: %1", e));
	}
	catch(...)
	{
		OW_LOG_DEBUG(logger, "OOPClonedProviderEnv::run() caught UNKNOWN exception");
	}

	if (!finishedSuccessfully)
	{
		OW_LOG_DEBUG3(logger, "ClonedProvEnv::run pipe closed without sending a BIN_END");
	}

	OW_LOG_DEBUG3(logger, "OOPClonedProviderEnv::run returning");
}

//////////////////////////////////////////////////////////////////////////////
void
OOPClonedProviderEnv::doShutdown()
{
}

} // end namespace OW_NAMESPACE







