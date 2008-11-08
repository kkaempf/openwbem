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
 * @author Kevin Harris
 */

#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_ExecTestCases.hpp"

AUTO_UNIT_TEST_SUITE_NAMED(OW_ExecTestCases,"OW_Exec");

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

namespace
{
	bool doesOutputContain(const StringArray& output, const String& text)
	{
		bool has_text = false;
		for( StringArray::const_iterator iter = output.begin();
			iter != output.end();
			++iter)
		{
			if( iter->indexOf(text) != String::npos )
			{
				has_text = true;
				break;
			}
		}
		return has_text;
	}

	void dumpOutput(const StringArray& output)
	{
		std::cerr << "Output contains:" << std::endl;
		std::cerr << "--->" << std::endl;
		for( StringArray::const_iterator iter = output.begin();
			  iter != output.end();
			  ++iter)
		{
			std::cerr << Format("(%1): %2", std::distance(output.begin(), iter), *iter) << std::endl;
		}
		std::cerr << "<---" << std::endl;
	}

	bool outputContains(const StringArray& output, const String& text)
	{
		if( !doesOutputContain(output, text) )
		{
			std::cerr << Format("\nOutput does not contain desired text: %1", text) << std::endl;
			dumpOutput(output);
			return false;
		}
		return true;
	}

	bool outputDoesNotContain(const StringArray& output, const String& text)
	{
		if( doesOutputContain(output, text) )
		{
			std::cerr << Format("\nOutput contains forbidden text: %1", text) << std::endl;
			dumpOutput(output);
			return false;
		}
		return true;
	}
}


bool OW_ExecTestCases::executeTestProgram(const String& parentArgs, const String& childArgs, StringArray& outputLines, int sleeptime)
{
	bool retval = true;
	{
		String output;
		try
		{
			String parentCommand = "./run_process.sh ./ExecTestProgram --program-name ParentProcess " + parentArgs;
			String childCommand = "./ExecTestProgram --program-name ChildProcess " + childArgs;
			StringArray cmd = parentCommand.tokenize();
			cmd.push_back("--spawn-child");
			cmd.push_back(childCommand);

			Process::Status status = Exec::executeProcessAndGatherOutput(cmd, output, Timeout::relative(5));
			retval = false;
		}
		catch (const ExecTimeoutException& e)
		{
		}
		String filename = output.trim();
		if( FileSystem::exists(filename) )
		{
			if( sleeptime != 0 )
			{
				// Wait to allow more output to be written to the file.
				sleep(sleeptime);
			}

			outputLines = FileSystem::getFileLines(filename);
			FileSystem::removeFile(filename);
		}
		else
		{
			retval = false;
			cerr << Format("Executing process failed: output=\"%1\"", output) << endl;

			// Yes, we just tested this, but we want a full test failure and want
			// to know what the output was.  An exception would probably be
			// better.
			unitAssert(FileSystem::exists(filename));
		}
	}
	return retval;
}

void OW_ExecTestCases::testExecuteProcessAndGatherOutput()
{
	{
		String output;
		Process::Status status = Exec::executeProcessAndGatherOutput(String(OW_TRUE_PATHNAME).tokenize(), output);
		unitAssert(status.terminatedSuccessfully());
		unitAssert(output.empty());
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
// Unfortunately these tests are not yet 100% reliable, so they are disabled until they are fixed
#if 0
	// only do timeout tests if we're doing the long test, since it's slowwww
	if (getenv("OWLONGTEST"))
	{
		{
			// Both the parent and child should quit normally.
			StringArray output;
			// This returns false because it didn't timeout.
			unitAssert(!executeTestProgram("--sleep-time 3", "--sleep-time 2", output));
			unitAssert(outputContains(output, "ChildProcess: Quitting"));
			unitAssert(outputContains(output, "ParentProcess: Quitting"));
		}

		{
			// The parent should timeout and get a sigterm (which it ignores).
			// The child should quit normally.  The parent should quit before any
			// forceful termination is attempted.
			StringArray output;
			unitAssert(executeTestProgram("--sleep-time 7 --block-signals term,chld", "--sleep-time 2", output, 10));
			unitAssert(outputContains(output, "ChildProcess: Quitting"));
			unitAssert(outputContains(output, "ParentProcess: Ignoring signal TERM"));
			unitAssert(outputContains(output, "ParentProcess: Done sleeping"));
			unitAssert(outputContains(output, "ParentProcess: Quitting"));
		}

		{
			// The parent should timeout and get a sigterm (which it ignores).  It
			// should then be forcefully terminated.
			StringArray output;
			unitAssert(executeTestProgram("--sleep-time 12 --block-signals term,chld", "--sleep-time 2", output, 14));
			unitAssert(outputContains(output, "ChildProcess: Quitting"));
			unitAssert(outputContains(output, "ParentProcess: Ignoring signal TERM"));
			// The parent process should be terminated, so it shouldn't contain
			// "Quitting" or "Done Sleeping".
			unitAssert(outputDoesNotContain(output, "ParentProcess: Done sleeping"));
			unitAssert(outputDoesNotContain(output, "ParentProcess: Quitting"));
		}

		{
			// The parent ignores the term, but the child is terminated.
			StringArray output;
			unitAssert(executeTestProgram("--sleep-time 6 --block-signals term", "--sleep-time 12", output, 14));
			unitAssert(outputContains(output, "ParentProcess: Done sleeping"));
			unitAssert(outputContains(output, "ParentProcess: Ignoring signal TERM"));
			unitAssert(outputDoesNotContain(output, "ChildProcess: Quitting")); // Because it was terminated...
			unitAssert(outputContains(output, "ParentProcess: Rough child exit: TERM"));
			unitAssert(outputContains(output, "ParentProcess: Quitting")); // Must exit cleanly.
		}

		{
			// Parent and child ignore the term.  Both will be killed forcefully.
			StringArray output;
			unitAssert(executeTestProgram("--sleep-time 6 --block-signals term", "--sleep-time 12 --block-signals term", output, 14));
			unitAssert(outputContains(output, "ParentProcess: Done sleeping"));
			unitAssert(outputContains(output, "ParentProcess: Ignoring signal TERM"));
			unitAssert(outputContains(output, "ChildProcess: Ignoring signal TERM"));
			unitAssert(outputDoesNotContain(output, "Process: Quitting"));
			unitAssert(outputDoesNotContain(output, "ChildProcess: Quitting"));
		}
	}
#endif

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
			streams[i]->waitCloseTerm(Timeout::relative(0.0), Timeout::relative(0.0), Timeout::relative(0.1));
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
#ifdef BLOCXX_SOLARIS
				// The solaris shell eats SIGTERM and returns 143 because the sleep gets killed
				unitAssert(status.exitTerminated());
				unitAssertEquals(status.exitStatus(), 143);

#else
				unitAssert(status.signalTerminated());
#ifdef BLOCXX_WIN32 //in Windows SIGPIPE doesn't exist
				unitAssert(status.termSignal() == SIGTERM );
#else
				unitAssert(status.termSignal() == SIGTERM || status.termSignal() == SIGPIPE);
#endif
#endif
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

#ifndef OW_WIN32
	ADD_TEST_TO_SUITE(OW_ExecTestCases, testExecuteProcessAndGatherOutput);
	ADD_TEST_TO_SUITE(OW_ExecTestCases, testgatherOutput);
#endif

	return testSuite;
}

