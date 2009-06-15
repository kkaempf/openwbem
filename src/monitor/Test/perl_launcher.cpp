/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Network Associates, nor Quest Software, Inc., nor the
*       names of its contributors or employees may be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"

#include "OW_Assertion.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Exception.hpp"
#include "blocxx/FileSystem.hpp"
#include "OW_MonitoredPerl.hpp"
#include "OW_PrivilegeManager.hpp"
#include "blocxx/Process.hpp"
#include "blocxx/Secure.hpp"
#include "blocxx/StringBuffer.hpp"
#include "blocxx/Thread.hpp"
#include "blocxx/UnnamedPipe.hpp"

#include <iostream>

#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace OpenWBEM;
using namespace std;

OW_DECLARE_EXCEPTION(MonPerlTest);
OW_DEFINE_EXCEPTION(MonPerlTest);

void check(bool b, String const & msg)
{
	if (!b)
	{
		OW_THROW(MonPerlTestException, msg.c_str());
	}
}

class PassOutput : public Thread
{
public:
	PassOutput(UnnamedPipe & out)
	: m_out(out)
	{
	}

private:
	virtual Int32 run();
	UnnamedPipe & m_out;
};

Int32 PassOutput::run()
{
	std::size_t const BUFSZ = 64;
	char buf[BUFSZ];
	int n;
	while ((n = m_out.read(buf, BUFSZ-1, IOIFC::E_THROW_ON_ERROR)) > 0)
	{
		buf[n] = '\0';
		cout << buf;
	}
	cout.flush();
	check(n == 0, "read error");
	return 0;
}

class CollectErr : public Thread
{
public:
	CollectErr(UnnamedPipe & out)
	: m_err(out)
	{
	}

	String get() const
	{
		return m_sbuf.toString();
	}

private:
	virtual Int32 run();
	UnnamedPipe & m_err;
	StringBuffer m_sbuf;
};

Int32 CollectErr::run()
{
	std::size_t const BUFSZ = 64;
	char buf[BUFSZ];
	int n;
	while ((n = m_err.read(buf, BUFSZ-1, IOIFC::E_THROW_ON_ERROR)) > 0)
	{
		buf[n] = '\0';
		m_sbuf += buf;
	}
	check(n == 0, "read error");
	return 0;
}

void prepend(String const & prefix, String & s)
{
	s = prefix + s;
}

int main_aux(int argc, char * * argv)
{
	OW_ASSERT(argc >= 5);
	char const * config_dir = argv[1];
	char const * launcher_privconfig = "perl_launcher.cfg";
	char const * user_name = argv[2];
	String bin_dir = argv[3];
//	String script_path = bin_dir + "/monperltest.pl";
	String script_path = bin_dir + "/monperltest_basic.pl";
	char const * script_privconfig = "perl_script.cfg";
	char const * const * script_args = argv + 4;
	String cwd = FileSystem::Path::getCurrentWorkingDirectory();

	ConfigOpts::installed_owlibexec_dir = "/montest-689acb0e1ec89f45-7085a2a1780f5f42/libexec/openwbem";
	ConfigOpts::installed_owlib_dir = "/montest-689acb0e1ec89f45-7085a2a1780f5f42/lib/openwbem";
	PrivilegeManager::use_lib_path = true;
	PrivilegeManager mgr = PrivilegeManager::createMonitor(
		config_dir, launcher_privconfig, user_name, 0
	);

	ProcessRef proc = MonitoredPerl::monitoredPerl(
		script_path, script_privconfig, script_args, (char const * const *)0
	);
	proc->in()->close();

	PassOutput pass_output(*(proc->out()));
	pass_output.start();

	CollectErr collect_err(*(proc->err()));
	collect_err.start();

	pass_output.join();
	collect_err.join();
	cout << collect_err.get();

	proc->waitCloseTerm(Timeout::relative(3.0), Timeout::relative(0.0), Timeout::relative(6.0));
	Process::Status st = proc->processStatus();
	if (!st.terminatedSuccessfully())
	{
		cout << "Something went wrong with the Perl process..." << endl;
		cout << "running: " << st.running() << endl;
		if (!st.running())
		{
			cout << "exitTerminated: " << st.exitTerminated() << endl;
			if (st.exitTerminated())
			{
				cout << "exitStatus: " << st.exitStatus() << endl;
			}
			cout << "signalTerminated: " << st.signalTerminated() << endl;
			if (st.signalTerminated())
			{
				cout << "termSignal: " << st.termSignal() << endl;
			}
			cout << "stopped: " << st.stopped() << endl;
			if (st.stopped())
			{
				cout << "stopSignal: " << st.stopSignal() << endl;
			}
		}
	}
	return 0;
}

int main(int argc, char * * argv)
{
	try
	{
		return main_aux(argc, argv);
	}
	catch (Exception & e)
	{
		cout << "Caught OpenWBEM::Exception:" << endl;
		cout << "  type: " << e.type() << endl;
		cout << "  msg:  " << e.getMessage() << endl;
		// cout << "  FILE: " << e.getFile() << endl;
		// cout << "  LINE: " << e.getLine() << endl;
		return 1;
	}
	catch (std::exception & e)
	{
		cout << "Caught standard exception:\n";
		cout << "  msg:  " << e.what() << endl;
		return 2;
	}
	catch (...)
	{
		cout << "Caught unknown exception" << endl;
		return 3;
	}
}
