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

void OW_ExecTestCases::testSafePopen()
{
	Array<String> command;
	command.push_back("/bin/cat");
	PopenStreams rval = Exec::safePopen( command );
	rval.in()->write("hello world\n", 12);
	rval.in()->close();
	String out = rval.out()->readAll();
	rval.getExitStatus();
	unitAssert(out == "hello world\n");

	unitAssertThrows(Exec::safePopen(String("/a/non-existent/binary").tokenize()));
}

void OW_ExecTestCases::testExecuteProcessAndGatherOutput()
{
	{
		String output;
		int processstatus(0);
		Exec::executeProcessAndGatherOutput(String(OW_TRUE_PATHNAME).tokenize(), output, processstatus);
		unitAssert(output.empty());
		unitAssert(WIFEXITED(processstatus));
		unitAssert(WEXITSTATUS(processstatus) == 0);
	}

	{
		String output;
		int processstatus(0);
		Exec::executeProcessAndGatherOutput(String(OW_FALSE_PATHNAME).tokenize(), output, processstatus);
		unitAssert(output.empty());
		unitAssert(WIFEXITED(processstatus));
		unitAssert(WEXITSTATUS(processstatus) == 1);
	}

	{
		String output;
		int processstatus(0);
		Exec::executeProcessAndGatherOutput(String("/bin/echo false").tokenize(), output, processstatus);
		StringArray out_array = output.tokenize();
		unitAssert(out_array.size() == 1)
		unitAssert(*out_array.begin() == "false");
		unitAssert(WIFEXITED(processstatus));
		unitAssert(WEXITSTATUS(processstatus) == 0);
	}

	{
		String output;
		int processstatus(0);
		Exec::executeProcessAndGatherOutput(String("/bin/cat").tokenize(), output, processstatus, Exec::INFINITE_TIMEOUT, -1, "hello to world\n");
		unitAssert(output == "hello to world\n");
		unitAssert(WIFEXITED(processstatus));
		unitAssert(WEXITSTATUS(processstatus) == 0);
	}

	// only do timeout tests if we're doing the long test, since it's slowwww
	if (getenv("OWLONGTEST"))
	{
		// do a timeout
		int processstatus = 0;
		String output;
		try
		{
			StringArray cmd;
			cmd.push_back("/bin/sh");
			cmd.push_back("-c");

			// We want a delay in this test before any output occurs.  This is
			// important because on some platforms (like Linux), the output could be
			// received even if the timeout was set as 0.  With a sleep at the
			// beginning of the test, this should hopefully avoid any scheduling
			// issues with a child process going too fast on an unloaded machine.

			// The sequence should go like this:
			// 1. sleep 1 - test blocks
			// 2. echo before - test gets it then resets timeout to 2 seconds.
			// 3. sleep 4 starts
			// 4. test times out after 2 seconds and throws.
			cmd.push_back("sleep 1; echo before; sleep 4; echo after");
			Exec::executeProcessAndGatherOutput(cmd, output, processstatus, 2);
			unitAssert(0);
		}
		catch (const ExecTimeoutException& e)
		{
		}
		unitAssert(output == "before\n");
	}

	{
		// test output limit
		int processstatus = 0;
		String output;
		try
		{
			Exec::executeProcessAndGatherOutput(String("/bin/echo 12345").tokenize(), output, processstatus, -1, 4);
			unitAssert(0);
		}
		catch (const ExecBufferFullException& e)
		{
		}
		unitAssert(output == "1234");
	}

	{
		// test a process that dies from a signal. SIGTERM == 15
		int processstatus = 0;
		String output;
		Exec::executeProcessAndGatherOutput(String(FileSystem::Path::getCurrentWorkingDirectory() + "/exitWithSignal 15").tokenize(), output, processstatus);
		unitAssert(!WIFEXITED(processstatus));
		unitAssert(WIFSIGNALED(processstatus));
		unitAssert(WTERMSIG(processstatus) == 15);
	}
}

class TestOutputGatherer : public Exec::OutputCallback
{
public:
	TestOutputGatherer(Array<pair<PopenStreams, String> >& outputs)
		: m_outputs(outputs)
	{
	}
private:
	virtual void doHandleData(const char* data, size_t dataLen, Exec::EOutputSource outputSource, PopenStreams& theStream, size_t streamIndex, Array<char>& inputBuffer)
	{
		assert(m_outputs[streamIndex].first == theStream); // too bad we can't do unitAssert...
		assert(outputSource == Exec::E_STDOUT);
		m_outputs[streamIndex].second += String(data, dataLen);
	}

	Array<pair<PopenStreams, String> >& m_outputs;
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
	virtual void doGetData(Array<char>& inputBuffer, PopenStreams& theStream, size_t streamIndex)
	{
		if (streamIndex < m_inputs.size() && m_inputs[streamIndex].size() > 0)
		{
			inputBuffer.insert(inputBuffer.end(), m_inputs[streamIndex].begin(), m_inputs[streamIndex].end());
			m_inputs[streamIndex].clear();
		}
		else if (theStream.in()->isOpen())
		{
			theStream.in()->close();
		}
	}
	Array<Array<char> > m_inputs;
};

void OW_ExecTestCases::testgatherOutput()
{
	{
		Array<PopenStreams> streams;
		Array<pair<PopenStreams, String> > outputs;
		TestInputCallback inputs;
		const int TEST_PROC_COUNT = 5;
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			PopenStreams curStream(Exec::safePopen(String(String("/bin/echo ") + String(i)).tokenize()));
			streams.push_back(curStream);
			outputs.push_back(make_pair(curStream, String()));
		}

		TestOutputGatherer testOutputGatherer(outputs);
		Array<Exec::ProcessStatus> processStatuses;
		processInputOutput(testOutputGatherer, streams, processStatuses, inputs, Exec::INFINITE_TIMEOUT);
		unitAssert(processStatuses.size() == size_t(TEST_PROC_COUNT));
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			int exitStatus = streams[i].getExitStatus();
			if (processStatuses[i].hasExited())
			{
				unitAssert(exitStatus == processStatuses[i].getStatus());
			}
			unitAssert(WIFEXITED(exitStatus));
			unitAssert(WEXITSTATUS(exitStatus) == 0);
			unitAssert(outputs[i].second == String(i) + "\n");
		}
	}

	{
		Array<PopenStreams> streams;
		Array<pair<PopenStreams, String> > outputs;
		Array<Array<char> > inputData;
		const int TEST_PROC_COUNT = 5;
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			PopenStreams curStream(Exec::safePopen(String("/bin/cat").tokenize()));
			streams.push_back(curStream);
			outputs.push_back(make_pair(curStream, String()));
			String num(i);
			num += '\n';
			inputData.push_back(Array<char>(num.c_str(), num.c_str() + num.length()));
		}
		TestInputCallback inputs(inputData);

		TestOutputGatherer testOutputGatherer(outputs);
		Array<Exec::ProcessStatus> processStatuses;
		processInputOutput(testOutputGatherer, streams, processStatuses, inputs, Exec::INFINITE_TIMEOUT);
		unitAssert(processStatuses.size() == size_t(TEST_PROC_COUNT));
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			int exitStatus = streams[i].getExitStatus();
			if (processStatuses[i].hasExited())
			{
				unitAssert(exitStatus == processStatuses[i].getStatus());
			}
			unitAssert(WIFEXITED(exitStatus));
			unitAssert(WEXITSTATUS(exitStatus) == 0);
			unitAssert(outputs[i].second == String(i) + "\n");
		}
	}

	// only do timeout tests if we're doing the long test, since it's slowwww
	if (getenv("OWLONGTEST"))
	{
		Array<PopenStreams> streams;
		Array<pair<PopenStreams, String> > outputs;
		const int TEST_PROC_COUNT = 4;
		const int TEST_TIMEOUT = 2;
		TestInputCallback inputs;
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			StringArray cmd;
			cmd.push_back("/bin/sh");
			cmd.push_back("-c");
			cmd.push_back(Format("sleep %1; echo before; sleep %2; echo after", i, i * i));
			PopenStreams curStream(Exec::safePopen(cmd));
			streams.push_back(curStream);
			outputs.push_back(make_pair(curStream, String()));
		}

		TestOutputGatherer testOutputGatherer(outputs);
		Array<Exec::ProcessStatus> processStatuses;
		try
		{
			processInputOutput(testOutputGatherer, streams, processStatuses, inputs, TEST_TIMEOUT);
			unitAssert(0);
		}
		catch (ExecTimeoutException& e)
		{
		}
		unitAssert(processStatuses.size() == size_t(TEST_PROC_COUNT));
		for (int i = 0; i < TEST_PROC_COUNT; ++i)
		{
			int exitStatus = streams[i].getExitStatus();
			if (i * i + i < TEST_TIMEOUT + TEST_PROC_COUNT) // all the ones that finished before the timout
			{
				unitAssert(processStatuses[i].hasExited());
				unitAssert(processStatuses[i].getStatus() == exitStatus);
				unitAssert(WIFEXITED(exitStatus));
				unitAssert(WEXITSTATUS(exitStatus) == 0);
			}
			else // these ones got killed
			{
				unitAssert(!processStatuses[i].hasExited());
				unitAssert(WIFSIGNALED(exitStatus));
				unitAssert(WTERMSIG(exitStatus) == SIGTERM || WTERMSIG(exitStatus) == SIGPIPE);
			}
			if (i * i + i < TEST_TIMEOUT + TEST_PROC_COUNT)
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

	ADD_TEST_TO_SUITE(OW_ExecTestCases, testSafePopen);
	ADD_TEST_TO_SUITE(OW_ExecTestCases, testExecuteProcessAndGatherOutput);
	ADD_TEST_TO_SUITE(OW_ExecTestCases, testgatherOutput);

	return testSuite;
}

