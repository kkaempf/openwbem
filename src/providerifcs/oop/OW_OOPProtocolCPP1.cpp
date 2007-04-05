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
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_IOException.hpp"
#include "OW_CIMException.hpp"
#include "OW_ResultHandlerIFC.hpp"
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
#include "OW_CIMParamValue.hpp"
#include "OW_DateTime.hpp"
#include "OW_SelectEngine.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_DataStreams.hpp"
#include "OW_Runnable.hpp"
#include "OW_IOIFCStreamBuffer.hpp"
#include "OW_ThreadPool.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_OOPProviderBase.hpp"
#include "OW_OOPDataOStreamBuf.hpp"

#include <deque>
#include <iosfwd>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(OOPProtocolCPP1);
OW_DEFINE_EXCEPTION(OOPProtocolCPP1);

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

	class OOPCallbackServiceEnv : public ServiceEnvironmentIFC
	{
	public:
		OOPCallbackServiceEnv(UInt8 type, const ProviderEnvironmentIFCRef& provEnv)
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
			{
				return m_provEnv->getCIMOMHandle();
			}
			else if (m_type == BinarySerialization::REPOSITORY_CIMOM_HANDLE_REQUEST)
			{
				return m_provEnv->getRepositoryCIMOMHandle();
			}
			else
			{
				OW_ASSERT(0);
			}
			return CIMOMHandleIFCRef();
		}
		
	private:
		UInt8 m_type;
		ProviderEnvironmentIFCRef m_provEnv;
	};

	class OperationResultHandler
	{
	public:
		virtual ~OperationResultHandler()
		{
		}

		virtual void handleResult(std::streambuf & instr, UInt8 op) = 0;
	};

	struct OutputEntry
	{
		enum EEntryType
		{
			E_PASS_PIPE,
			E_BUFF
		};
		enum EDirectionIndicator
		{
			E_INPUT,
			E_OUTPUT
		};


		OutputEntry(const Array<unsigned char>& b)
			: type(E_BUFF)
			, buf(b)
		{
			OW_ASSERT(buf.size() > 0);
		}

		OutputEntry(EDirectionIndicator d, const UnnamedPipeRef& p)
			: type(E_PASS_PIPE)
			, direction(d)
			, pipe(p)
		{
		}

		EEntryType type;

		EDirectionIndicator direction;
		UnnamedPipeRef pipe;
		Array<unsigned char> buf;

	};


	static const int CONTINUE = 0;
	static const int FINISHED = 1;

	int processOneRequest(std::streambuf & in,
		std::streambuf & out,
		std::deque<OutputEntry>& outputEntries,
		const ProviderEnvironmentIFCRef& env,
		OperationResultHandler& result,
		ThreadPool& threadPool,
		OOPProviderBase* pprov);

	int process(Array<unsigned char>& in,
		std::deque<OutputEntry>& outputEntries,
		const ProviderEnvironmentIFCRef& env,
		const Logger& logger, 
		OperationResultHandler& result,
		ThreadPool& threadPool,
		OOPProviderBase* pprov)
	{
		//OW_LOG_DEBUG(logger, Format("in process(). in.size() = %1", in.size()));
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
					//OW_LOG_DEBUG(logger, Format("processOneRequest() found %1 bytes of out. Adding to outputEntries.", out.size()));
					outputEntries.push_back(OutputEntry(out));
				}
				// successful request, store how much was read so it can be removed when we're done.
				numRead = inbuf.pubseekoff(0, std::ios::cur);
				//OW_LOG_DEBUG(logger, Format("processOneRequest() done. numRead = %1", numRead));
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
			//OW_LOG_DEBUG(logger, Format("processOneRequest() threw IOException: %1", e));

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


	class OOPSelectableCallback : public SelectableCallbackIFC
	{
	public:
		OOPSelectableCallback(
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
		void doSelected(Select_t& selectedObject, EEventType eventType)
		{
			Logger logger(COMPONENT_NAME);
			if (eventType == E_READ_EVENT)
			{
				//OW_LOG_DEBUG(logger, "doSelected() got a read event");
				// read all the data out of the pipe and append it to m_inputBuf
				size_t oldSize = m_inputBuf.size();
				const unsigned int INCREMENT = 1024;
				bool moreDataToRead = true;
				while (moreDataToRead)
				{
					m_inputBuf.resize(oldSize + INCREMENT, 0xFA);
					//OW_LOG_DEBUG(logger, Format("reading some data into offset %1", oldSize));
					ssize_t numRead = m_inputPipe->read(&m_inputBuf[oldSize], INCREMENT, IOIFC::E_RETURN_ON_ERROR);
					int lerrno = errno;
					//OW_LOG_DEBUG(logger, Format("read() returned %1" , numRead));
					if (numRead == -1)
					{
						m_inputBuf.resize(oldSize, 0xFB);
						if (lerrno == ETIMEDOUT)
						{
							 moreDataToRead = false; // end the loop
							 //OW_LOG_DEBUG(logger, "Detected ETIMEDOUT");
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
					OW_LOG_DEBUG(logger, "process() returned FINISHED, telling the select engine to stop");
					m_selectEngine.stop();
					m_finishedSuccessfully = true;
				}
				else
				{
					//OW_LOG_DEBUG(logger, Format("doSelected(): process() returned CONTINUE. m_outputEntries.size() = %1", m_outputEntries.size()));
					if (!m_outputEntries.empty())
					{
						// add it
						m_selectEngine.addSelectableObject(m_outputPipe->getWriteSelectObj(), this, SelectableCallbackIFC::E_WRITE_EVENT);
					}
				}
			}
			else if (eventType == E_WRITE_EVENT)
			{
				//OW_LOG_DEBUG(logger, "doSelected() got a write event");
				if (m_outputEntries.empty())
				{
					//OW_LOG_DEBUG(logger, "m_outputEntries.empty(), removing from select engine");
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
							OW_LOG_DEBUG(logger, Format("doSelected() attempting to pass a input descriptor: %1", entry.pipe->getInputDescriptor()));
							m_outputPipe->passDescriptor(entry.pipe->getInputDescriptor(), m_inputPipe);
						}
						else
						{
							OW_LOG_DEBUG(logger, Format("doSelected() attempting to pass a output descriptor: %1", entry.pipe->getOutputDescriptor()));
							m_outputPipe->passDescriptor(entry.pipe->getOutputDescriptor(), m_inputPipe);
						}
						m_outputEntries.erase(m_outputEntries.begin());
					}
					else // entry.type == E_BUFFER
					{
						//OW_LOG_DEBUG(logger, Format("doSelected() attempting to write %1 bytes", entry.buf.size()));
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
							//OW_LOG_DEBUG(logger, "doSelected() got EAGAIN while attempting to write.");
						}
						//OW_LOG_DEBUG(logger, Format("write returned %1", numWrote));
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

		Array<unsigned char>& m_inputBuf;
		std::deque<OutputEntry>& m_outputEntries;
		UnnamedPipeRef m_inputPipe;
		UnnamedPipeRef m_outputPipe;
		ProviderEnvironmentIFCRef m_env;
		OperationResultHandler& m_result;
		SelectEngine& m_selectEngine;
		bool& m_finishedSuccessfully;
		ThreadPool& m_threadPool;
		OOPProviderBase* m_pprov;
	};

	struct ShutdownThreadPool
	{
		ShutdownThreadPool(ThreadPool& tp) : m_tp(tp) {}
		~ShutdownThreadPool()
		{
			// use an infinite timeout so the threads don't get definitively cancelled, but immediately cooperatively cancel them.
			m_tp.shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, Timeout::relative(0.1), Timeout::infinite);
		}
		ThreadPool& m_tp;
	};

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
		ThreadPool threadPool(ThreadPool::DYNAMIC_SIZE_NO_QUEUE, MAX_CALLBACK_THREADS, MAX_CALLBACK_THREADS, "OOPProtocolCPP1");
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

		Logger logger(COMPONENT_NAME);

		if (readWriteFlag == E_READ_WRITE_UNTIL_FINISHED)
		{
			selectEngine.addSelectableObject(inputPipe->getReadSelectObj(), callback, SelectableCallbackIFC::E_READ_EVENT);
		}

		if (!outputBuf.empty())
		{
			outputEntries.push_back(OutputEntry(outputBuf));
			selectEngine.addSelectableObject(outputPipe->getWriteSelectObj(), callback, SelectableCallbackIFC::E_WRITE_EVENT);
		}

		//OW_LOG_DEBUG(logger, "end() about to run the select engine");
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


	class CIMOMHandleConnectionRunner : public Runnable
	{
	public:
		CIMOMHandleConnectionRunner(const UnnamedPipeRef& conn, UInt8 op, const ProviderEnvironmentIFCRef& provEnv)
			: m_conn(conn)
			, m_binaryRH(new BinaryRequestHandler(BinaryRequestHandler::E_SUPPORT_EXPORT_INDICATION))
			, m_inbuf(m_conn.getPtr())
			, m_instr(&m_inbuf)
			, m_outbuf(m_conn.getPtr())
			, m_outstr(&m_outbuf)
			, m_env(provEnv)
			, m_cancelled(false)
		{
			m_binaryRH->init(ServiceEnvironmentIFCRef(new OOPCallbackServiceEnv(op, provEnv)));
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
					//OW_LOG_DEBUG(logger, "CloneCIMOMHandleConnectionRunner::run() "
					//	"received BIN_END request. shutting down cimom handle");
					return;
				}
				else if (op != BinarySerialization::BIN_OK)
				{
					//OW_LOG_ERROR(logger, "CloneCIMOMHandleConnectionRunner::run() "
					//	"invalid byte received from client");
					return;
				}

				HTTPChunkedIStream operationIstr(m_instr);
				HTTPChunkedOStream operationOstr(m_outstr);
				TempFileStream operationErrstr;
				OW_LOG_DEBUG(logger, "CIMOMHandleConnectionRunner::run() calling m_binaryRH->process");
				m_binaryRH->process(&operationIstr, &operationOstr, &operationErrstr, m_env->getOperationContext());
				HTTPUtils::eatEntity(operationIstr);
				if (m_binaryRH->hasError())
				{
					OW_LOG_DEBUG(logger, "CIMOMHandleConnectionRunner::run() m_binaryRH->hasError()");
					operationOstr.termOutput(HTTPChunkedOStream::E_DISCARD_LAST_CHUNK);
					operationErrstr.rewind();
					m_outstr << operationErrstr.rdbuf();
				}
				else
				{
					OW_LOG_DEBUG(logger, "CIMOMHandleConnectionRunner::run() sending result and BIN_OK");
					operationOstr.termOutput(HTTPChunkedOStream::E_SEND_LAST_CHUNK);
					BinarySerialization::write(m_outbuf, BinarySerialization::BIN_OK);
				}
				if (m_outbuf.pubsync() == -1)
				{
					OW_LOG_ERROR(logger, "CIMOMHandleConnectionRunner::run() failed to flush output");
				}
			}
			OW_LOG_DEBUG(logger, "CIMOMHandleConnectionRunner::run() finished");
		}
		bool cancelled()
		{
			NonRecursiveMutexLock lock(m_guard);
			return m_cancelled;
		}
		virtual void doShutdown()
		{
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
		//OW_LOG_DEBUG(logger, "processOneRequest about to read the code.");
		BinarySerialization::read(inbuf, op);
		//OW_LOG_DEBUG(logger, Format("processOneRequest read code: %1", static_cast<int>(op)));
		switch (op)
		{
			case BinarySerialization::BIN_OK:
			{
				OW_LOG_DEBUG(logger, "processOneRequest got BIN_OK");
				result.handleResult(inbuf, op);
			}
			break;
			case BinarySerialization::BIN_ERROR:
			{
				OW_LOG_DEBUG(logger, "processOneRequest got BIN_ERROR");
				String msg;
				BinarySerialization::read(inbuf, msg);
				OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
			}
			break;

			case BinarySerialization::BIN_EXCEPTION:
			{
				OW_LOG_DEBUG(logger, "processOneRequest got BIN_EXCEPTION");
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
				OW_LOG_DEBUG(logger, "processOneRequest got BINSIG_INST or BINSIG_OP");
				result.handleResult(inbuf, op);
			}
			break;

			case BinarySerialization::BIN_END:
			{
				OW_LOG_DEBUG(logger, "processOneRequest got BIN_END");
				return FINISHED;
			}

			case BinarySerialization::BIN_LOG_MESSAGE:
			{
				//OW_LOG_DEBUG(logger, "processOneRequest got BIN_LOG_MESSAGE");
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
				OW_LOG_DEBUG(logger, "processOneRequest got OPERATION_CONTEXT_GET_DATA");
				String key = BinarySerialization::readString(inbuf);
				//OW_LOG_DEBUG(logger, Format("processOneRequest read key: %1", key));
				OperationContext::DataRef data = env->getOperationContext().getData(key);
				if (data)
				{
					OW_LOG_DEBUG(logger, "processOneRequest found it, writing true");
					BinarySerialization::writeBool(outbuf, true);
					data->writeObject(outbuf);
				}
				else
				{
					OW_LOG_DEBUG(logger, "processOneRequest didn't find it, writing false");
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
				OW_LOG_DEBUG(logger, "processOneRequest got OPERATION_CONTEXT_SET_DATA");
				String key = BinarySerialization::readString(inbuf);
				//OW_LOG_DEBUG(logger, Format("processOneRequest read key: %1", key));
				String type = BinarySerialization::readString(inbuf);
				//OW_LOG_DEBUG(logger, Format("processOneRequest read type: %1", type));
				std::istream instr(&inbuf);
				HTTPChunkedIStreamBuffer chunkedIBuf(instr);
				OperationContext::DataRef data = env->getOperationContext().getData(key);
				if (data)
				{
					// replace the existing object
					data->readObject(chunkedIBuf);

					OW_LOG_DEBUG(logger, "processOneRequest found it, writing true");
					BinarySerialization::writeBool(outbuf, true);
				}
				else if (type == OperationContext::StringData().getType())
				{
					OperationContext::DataRef data(new OperationContext::StringData());
					data->readObject(chunkedIBuf);
					env->getOperationContext().setData(key, data);
					OW_LOG_DEBUG(logger, "processOneRequest didn't find it, but it was a string type, writing true");
					BinarySerialization::writeBool(outbuf, true);
				}
				else
				{
					OW_LOG_DEBUG(logger, "processOneRequest didn't find it, not a string type, writing false");
					BinarySerialization::writeBool(outbuf, false);
				}
				
				// need to consume all the chunking
				HTTPUtils::eatEntity(chunkedIBuf);
			}
			break;

			case BinarySerialization::OPERATION_CONTEXT_REMOVE_DATA:
			{
				OW_LOG_DEBUG(logger, "processOneRequest got OPERATION_CONTEXT_REMOVE_DATA");
				String key = BinarySerialization::readString(inbuf);
				//OW_LOG_DEBUG(logger, Format("processOneRequest read key: %1", key));
				if (env->getOperationContext().keyHasData(key))
				{
					OW_LOG_DEBUG(logger, "processOneRequest found it, writing true");
					BinarySerialization::writeBool(outbuf, true);
					if (outbuf.pubsync() == -1)
					{
						OW_LOG_DEBUG(logger, "processOneRequest flush failed!");
						OW_THROWCIMMSG(CIMException::FAILED, "Writing to process failed");
					}
					env->getOperationContext().removeData(key);
				}
				else
				{
					OW_LOG_DEBUG(logger, "processOneRequest didn't find it, writing false");
					BinarySerialization::writeBool(outbuf, false);
				}
			}
			break;

			case BinarySerialization::OPERATION_CONTEXT_KEY_HAS_DATA:
			{
				OW_LOG_DEBUG(logger, "processOneRequest got OPERATION_CONTEXT_KEY_HAS_DATA");
				String key = BinarySerialization::readString(inbuf);
				//OW_LOG_DEBUG(logger, Format("processOneRequest read key: %1", key));
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
				OW_LOG_DEBUG(logger, "processOneRequest got OPERATION_CONTEXT_GET_OPERATION_ID");
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
				OW_LOG_DEBUG(logger, "processOneRequest got PROVIDER_ENVIRONMENT_REQUEST");
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
			{
				OW_LOG_DEBUG(logger, "processOneRequest got CIMOM_HANDLE_REQUEST or REPOSITORY_CIMOM_HANDLE_REQUEST");
				UnnamedPipeRef connToKeep;
				UnnamedPipeRef connToSend;
				UnnamedPipe::createConnectedPipes(connToKeep, connToSend);
				// Setting the timeouts to be infinite won't cause a problem here.  The OOP interface enforces a timeout for each operation and
				// will kill the provider if we exceeds it. That will cause the other side of the pipe to close and the thread will then exit.
				connToKeep->setTimeouts(Timeout::infinite);
				// Pass a timeout to tryAddWork, because there could be threads in the pool that haven't terminated yet, but
				// just need a chance to run and detect a closed connection. This can happen if the provider creates/destroys
				// a number of CIMOMHandle instances in quick succession.
				if (threadPool.tryAddWork(RunnableRef(new CIMOMHandleConnectionRunner(connToKeep, op, env)), Timeout::relative(10)))
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
				OW_LOG_DEBUG(logger, "processOneRequest got GET_CONFIG_ITEM");
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

	class NoResultHandler : public OperationResultHandler
	{
	public:
		virtual void handleResult(std:: streambuf &instr, UInt8 op)
		{
			OW_THROW(OOPProtocolCPP1Exception, Format("Invalid op, expected nothing, got: %1", static_cast<int>(op)).c_str());
		}
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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::enumInstanceNames about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_ENUMINSTNAMES);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeString(obuf, className);
	BinarySerialization::writeClass(obuf, cimClass);
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::enumInstanceNames finished filling buffer");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::enumInstances about to start filling request buffer");
	
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
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::enumInstances finished filling buffer");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::getInstance about to start filling request buffer");
	
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
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::getInstance finished filling buffer");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::createInstance about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_CREATEINST);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeInstance(obuf, cimInstance);
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::createInstance finished filling buffer");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::modifyInstance about to start filling request buffer");
	
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
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::modifyInstance finished filling buffer");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::deleteInstance about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_DELETEINST);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeObjectPath(obuf, cop);
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::deleteInstance finished filling buffer");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::associators about to start filling request buffer");
	
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

	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::associators finished filling buffer");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::associatorNames about to start filling request buffer");
	
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
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::associatorNames finished filling buffer");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::references about to start filling request buffer");
	
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

	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::references finished filling buffer");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::referenceNames about to start filling request buffer");
	
	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::BIN_REFNAMES);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeObjectPath(obuf, objectName);
	BinarySerialization::writeString(obuf, resultClass);
	BinarySerialization::writeString(obuf, role);
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::referenceNames finished filling buffer");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::invokeMethod about to start writing");

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
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::invokeMethod finished writing.");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::poll about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::POLL);
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::poll finished writing.");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::getInitialPollingInterval about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::GET_INITIAL_POLLING_INTERVAL);
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::getInitialPollingInterval finished writing.");

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
	OW_LOG_DEBUG(logger,
		Format("OOPProtocolCPP1::setPersistent about to start writing: %1",
			persistent));

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::SET_PERSISTENT);
	BinarySerialization::writeBool(obuf, persistent);

	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::setPersistent finished writing.");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::activateFilter about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::ACTIVATE_FILTER);
	BinarySerialization::writeWQLSelectStatement(obuf, filter);
	BinarySerialization::writeString(obuf, eventType);
	BinarySerialization::writeString(obuf, nameSpace);
	BinarySerialization::writeStringArray(obuf, classes);
	Bool(firstActivation).writeObject(obuf);
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::activateFilter finished writing.");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::authorizeFilter about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::AUTHORIZE_FILTER);
	BinarySerialization::writeWQLSelectStatement(obuf, filter);
	BinarySerialization::writeString(obuf, eventType);
	BinarySerialization::writeString(obuf, nameSpace);
	BinarySerialization::writeStringArray(obuf, classes);
	BinarySerialization::writeString(obuf, owner);
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::authorizeFilter finished writing.");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::deActivateFilter about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::DEACTIVATE_FILTER);
	BinarySerialization::writeWQLSelectStatement(obuf, filter);
	BinarySerialization::writeString(obuf, eventType);
	BinarySerialization::writeString(obuf, nameSpace);
	BinarySerialization::writeStringArray(obuf, classes);
	Bool(lastActivation).writeObject(obuf);
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::deActivateFilter finished writing.");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::mustPoll about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::MUST_POLL);
	BinarySerialization::writeWQLSelectStatement(obuf, filter);
	BinarySerialization::writeString(obuf, eventType);
	BinarySerialization::writeString(obuf, nameSpace);
	BinarySerialization::writeStringArray(obuf, classes);
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::mustPoll finished writing.");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::exportIndication about to start writing");

	Array<unsigned char> buf;
	OOPDataOStreamBuf obuf(buf);
	BinarySerialization::write(obuf, BinarySerialization::BinaryProtocolVersion);
	BinarySerialization::write(obuf, BinarySerialization::EXPORT_INDICATION);
	BinarySerialization::writeString(obuf, ns);
	BinarySerialization::writeInstance(obuf, indHandlerInst);
	BinarySerialization::writeInstance(obuf, indicationInst);
	
	//OW_LOG_DEBUG(logger, "OOPProtocolCPP1::exportIndication finished writing.");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::shuttingDown about to start writing");

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
	OW_LOG_DEBUG(logger, "OOPProtocolCPP1::queryInstances about to start filling request buffer");

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







