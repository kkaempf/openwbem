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
 * @author Dan Nuffer
 */

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_ExecTestCases.hpp"
#include "OW_Exec.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Array.hpp"
#include "OW_Format.hpp"
#include "OW_FileSystem.hpp"

#include <utility> // for pair
#include <cassert>
#include <csignal>

#if defined(OW_HAVE_SYS_WAIT_H) && defined(OW_WIFEXITED_NEEDS_WAIT_H)
#include <sys/wait.h>
#endif

using namespace OpenWBEM;
using namespace std;

void OW_ExecTestCases::setUp()
{
}

void OW_ExecTestCases::tearDown()
{
}

bool OW_ExecTestCases::executeTestScript(const String& option1, const String& option2, const String& desiredOutput)
{
	String output;
	{
		try
		{
			StringArray cmd;
			cmd.push_back("./exec_test_script.sh");
			cmd.push_back(option1);
			cmd.push_back(option2);
			Process::Status status = Exec::executeProcessAndGatherOutput(cmd, output, Timeout::relative(5));
			unitAssert(0);
		}
		catch (const ExecTimeoutException& e)
		{
		}
		String filename = output.trim();
		unitAssert(FileSystem::exists(filename));
		if( FileSystem::exists(filename) )
		{
			StringArray foo = FileSystem::getFileLines(filename);
			FileSystem::removeFile(filename);
			for( StringArray::const_iterator i = foo.begin(); i != foo.end(); ++i )
			{
				if( i->indexOf(desiredOutput) != String::npos )
				{
					return true;
				}
			}
		}
	}
	return false;
}

void OW_ExecTestCases::testExecuteProcessAndGatherOutput()
{
	{
		String output;
		Process::Status status = Exec::executeProcessAndGatherOutput(String(OW_TRUE_PATHNAME).tokenize(), output);
		unitAssert(output.empty());
		unitAssert(status.terminatedSuccessfully());
	}

	{
		String output;
		Process::Status status = Exec::executeProcessAndGatherOutput(String(OW_FALSE_PATHNAME).tokenize(), output);
		unitAssert(output.empty());
		unitAssert(status.exitTerminated());
		// the return value of false is not always 1.
		unitAssert(status.exitStatus() != 0);
	}

	{
		String output;
		Process::Status status = Exec::executeProcessAndGatherOutput(String("/bin/echo false").tokenize(), output);
		StringArray out_array = output.tokenize();
		unitAssertEquals(size_t(1), out_array.size());
		unitAssertEquals("false", *out_array.begin());
		unitAssert(status.terminatedSuccessfully());
	}

	{
		String output;
		Process::Status status = Exec::executeProcessAndGatherOutput(String("/bin/cat").tokenize(), output, Timeout::relative(10.0), -1, "hello to world\n");
		unitAssert(output == "hello to world\n");
		unitAssert(status.terminatedSuccessfully());
	}

	// only do timeout tests if we're doing the long test, since it's slowwww
	if (getenv("OWLONGTEST"))
	{
		// Test our child process which requires a signal, and NOT ignoring the term signal.
		unitAssert(executeTestScript("wait_child", "noignore", "Parent received signal"));
		// Test our child process which requires a signal, and ignoring the term signal.
		unitAssert(executeTestScript("wait_child", "ignore", "Child received signal"));
		// Test our child process which requires a kill signal, yet the child quits when the process group is signaled.
		unitAssert(executeTestScript("sleep", "ignore", "Child sleep terminated"));
	}

	{
		// test output limit
		int processstatus = 0;
		String output;
		try
		{
			Process::Status status = Exec::executeProcessAndGatherOutput(String("/bin/echo 12345").tokenize(), output, Timeout::infinite, 4);
			unitAssert(0);
		}
		catch (const ExecBufferFullException& e)
		{
		}
		unitAssert(output == "1234");
	}

	{
		// test a process that dies from a signal. SIGTERM == 15
		String output;
		Process::Status status = Exec::executeProcessAndGatherOutput(String(FileSystem::Path::getCurrentWorkingDirectory() + "/exitWithSignal 15").tokenize(), output);
		unitAssert(!status.exitTerminated());
		unitAssert(status.signalTerminated());
		unitAssert(status.termSignal() == 15);
	}
}

class TestOutputGatherer : public Exec::OutputCallback
{
public:
	TestOutputGatherer(Array<pair<ProcessRef, String> >& outputs)
		: m_outputs(outputs)
	{
	}
private:
	virtual void doHandleData(const char* data, size_t dataLen, Exec::EOutputSource outputSource, const ProcessRef& theProc, size_t streamIndex, Array<char>& inputBuffer)
	{
		assert(m_outputs[streamIndex].first == theProc); // too bad we can't do unitAssert...
		assert(outputSource == Exec::E_STDOUT);
		m_outputs[streamIndex].second += String(data, dataLen);
	}

	Array<pair<ProcessRef, String> >& m_outputs;
};

class TestInputCallback : public Exec::InputCallback
{
public:
	TestInputCallback(const Array<Array<char> >& inputs)
		: m_inputs(inputs)
	{
	}
	TestInputCallback()
	{
	}
private:
	virtual void doGetData(Array<char>& inputBuffer, const ProcessRef& theProc, size_t streamIndex)
	{
		if (streamIndex < m_inputs.size() && m_inputs[streamIndex].size() > 0)
		{
			inputBuffer.insert(inputBuffer.end(), m_inputs[streamIndex].begin(), m_inputs[streamIndex].end());
			m_inputs[streamIndex].clear();
		}
		else if (theProc->in()->isOpen())
		{
			theProc->in()->close();
		}
	}
	Array<Array<char> > m_inputs;
};

void OW_ExecTestCases::testgatherOutput()
{
	{
		Array<ProcessRef> procs;
		Array<pair<ProcessRef, String> > outputs;
		TestInputCallback inputs;
		const int TEST_PROC_COUNT = 5;
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			ProcessRef curStream(Exec::spawn(String(String("/bin/echo ") + String(i)).tokenize()));
			procs.push_back(curStream);
			outputs.push_back(make_pair(curStream, String()));
		}

		TestOutputGatherer testOutputGatherer(outputs);
		processInputOutput(testOutputGatherer, procs, inputs, Timeout::relative(10.0));
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			procs[i]->waitCloseTerm();
			Process::Status status = procs[i]->processStatus();
			unitAssert(status.terminatedSuccessfully());
			unitAssert(outputs[i].second == String(i) + "\n");
		}
	}

	{
		Array<ProcessRef> procs;
		Array<pair<ProcessRef, String> > outputs;
		Array<Array<char> > inputData;
		const int TEST_PROC_COUNT = 5;
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			ProcessRef curStream(Exec::spawn(String("/bin/cat").tokenize()));
			procs.push_back(curStream);
			outputs.push_back(make_pair(curStream, String()));
			String num(i);
			num += '\n';
			inputData.push_back(Array<char>(num.c_str(), num.c_str() + num.length()));
		}
		TestInputCallback inputs(inputData);

		TestOutputGatherer testOutputGatherer(outputs);
		processInputOutput(testOutputGatherer, procs, inputs, Timeout::relative(10.0));
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			procs[i]->waitCloseTerm();
			Process::Status status = procs[i]->processStatus();
			unitAssert(status.terminatedSuccessfully());
			unitAssert(outputs[i].second == String(i) + "\n");
		}
	}

	// only do timeout tests if we're doing the long test, since it's slowwww
	if (getenv("OWLONGTEST"))
	{
		Array<ProcessRef> streams;
		Array<pair<ProcessRef, String> > outputs;
		const int TEST_PROC_COUNT = 4;
		const Timeout TEST_TIMEOUT = Timeout::relativeWithReset(2.0);
		TestInputCallback inputs;
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			StringArray cmd;
			cmd.push_back("/bin/sh");
			cmd.push_back("-c");
			cmd.push_back(Format("sleep %1; echo before; sleep %2; echo after", i, i * i));
			ProcessRef curStream(Exec::spawn(cmd));
			streams.push_back(curStream);
			outputs.push_back(make_pair(curStream, String()));
		}

		TestOutputGatherer testOutputGatherer(outputs);
		try
		{
			processInputOutput(testOutputGatherer, streams, inputs, TEST_TIMEOUT);
			unitAssert(0);
		}
		catch (ExecTimeoutException& e)
		{
		}
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			streams[i]->waitCloseTerm(Timeout::relative(0.0), Timeout::relative(0.0), Timeout::relative(0.001));
		}
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			Process::Status status = streams[i]->processStatus();
			if (i * i + i < 2 + TEST_PROC_COUNT) // all the ones that finished before the timout
			{
				unitAssert(status.terminatedSuccessfully());
			}
			else // these ones got killed
			{
				unitAssert(status.signalTerminated());
				unitAssert(status.termSignal() == SIGTERM || status.termSignal() == SIGPIPE);
			}
			if (i * i + i < 2 + TEST_PROC_COUNT)
			{
				unitAssert(outputs[i].second == "before\nafter\n");
			}
			else
			{
				// these ones got killed during the middle sleep
				unitAssert(outputs[i].second == "before\n");
			}
		}
	}


}

Test* OW_ExecTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_Exec");

	ADD_TEST_TO_SUITE(OW_ExecTestCases, testExecuteProcessAndGatherOutput);
	ADD_TEST_TO_SUITE(OW_ExecTestCases, testgatherOutput);

	return testSuite;
}

