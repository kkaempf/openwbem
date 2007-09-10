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

#ifndef OW_OOP_PROTOCOL_CPP1_IMPL_HPP_INCLUDE_GUARD_
#define OW_OOP_PROTOCOL_CPP1_IMPL_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_Array.hpp"
#include "OW_Assertion.hpp"
#include "OW_ProviderFwd.hpp"
#include "OW_OOPFwd.hpp"
#include "OW_SelectableCallbackIFC.hpp"
#include <deque>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

namespace OOPProtocolCPP1Impl
{

OW_DECLARE_EXCEPTION(OOPProtocolCPP1);

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

class OperationResultHandler
{
public:
	virtual ~OperationResultHandler()
	{
	}

	virtual void handleResult(std::streambuf & instr, UInt8 op) = 0;
};

class NoResultHandler : public OperationResultHandler
{
public:
	virtual void handleResult(std:: streambuf &instr, UInt8 op);
};

enum 
{ 
	CONTINUE = 0, 
	FINISHED = 1
};

int processOneRequest(std::streambuf & in,
	std::streambuf & out,
	std::deque<OutputEntry>& outputEntries,
	const ProviderEnvironmentIFCRef& env,
	OperationResultHandler& result,
	ThreadPool& threadPool,
	OOPProviderBase* pprov);

struct ShutdownThreadPool
{
	ShutdownThreadPool(ThreadPool& tp);
	~ShutdownThreadPool();
	ThreadPool& m_tp;
};

int process(Array<unsigned char>& in,
	std::deque<OutputEntry>& outputEntries,
	const ProviderEnvironmentIFCRef& env,
	const Logger& logger, 
	OperationResultHandler& result,
	ThreadPool& threadPool,
	OOPProviderBase* pprov);

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
		);
	void doSelected(Select_t& selectedObject, EEventType eventType);
private:
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

} // end namespace OOPProtocolCPP1Impl


} // end namespace OW_NAMESPACE

#endif



