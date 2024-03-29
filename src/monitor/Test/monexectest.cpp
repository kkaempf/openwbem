#include "OW_config.h"
#include "blocxx/Array.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Exception.hpp"
#include "blocxx/FileSystem.hpp"
#include "OW_PrivilegeManager.hpp"
#include "blocxx/String.hpp"
#include "blocxx/StringBuffer.hpp"
#include "blocxx/Thread.hpp"
#include "blocxx/UnnamedPipe.hpp"
#include "blocxx/Format.hpp"

#include <iostream>
#include <signal.h>

using namespace OpenWBEM;
using std::cin;
using std::cout;
using std::endl;
using namespace blocxx;

OW_DECLARE_EXCEPTION(MonTest);
OW_DEFINE_EXCEPTION(MonTest);

void check(bool b, String const & msg)
{
	if (!b)
	{
		OW_THROW(MonTestException, msg.c_str());
	}
}

class PassInput : public Thread
{
public:
	PassInput(UnnamedPipe & in)
	: m_in(in)
	{
	}

private:
	virtual Int32 run();
	UnnamedPipe & m_in;
};

Int32 PassInput::run()
{
	std::size_t const BUFSZ = 64;
	char buf[BUFSZ];
	while (cin)
	{
		cin.read(buf, BUFSZ);
		int nr = cin.gcount();
		int nw = 0;
		while (nw < nr)
		{
			int n = m_in.write(buf + nw, nr - nw);
			check(n > 0, "write failure");
			nw += n;
		}
	}
	m_in.close();
	return 0;
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

void copy_env_var(char const * varname, StringArray & env)
{
	char const * lib_path= ::getenv(varname);
	if (lib_path)
	{
		Format fmt("%1=%2", varname, lib_path);
		env.push_back(fmt.toString());
	}
}

int main_aux(int argc, char * * argv)
{
	check(argc == 8, "wrong number of program arguments");
	char const * config_dir = argv[1];
	char const * app_name = argv[2];
	char const * exec_path = argv[3];
	char const * exec_app_name = argv[4];
	StringArray exec_argv = String(argv[5]).tokenize("+");
	StringArray exec_envp = String(argv[6]).tokenize("+");
	Int8 monitored_user_exec = String(argv[7]).toInt8();
	copy_env_var("LD_LIBRARY_PATH", exec_envp);
	copy_env_var("LIBPATH", exec_envp); // AIX
	copy_env_var("SHLIB_PATH", exec_envp); // HPUX
	copy_env_var("DYLD_LIBRARY_PATH", exec_envp); // DARWIN (OS X)

	ConfigOpts::installed_owlibexec_dir =
		"/monexectest-689acb0e1ec89f45-7085a2a1780f5f42/libexec/openwbem";
	PrivilegeManager::use_lib_path = true;
	PrivilegeManager mgr = PrivilegeManager::createMonitor(config_dir, app_name);
	ProcessRef p_proc;
	if (monitored_user_exec)
	{
		p_proc =
			mgr.monitoredUserSpawn(exec_path, exec_app_name, exec_argv,
			exec_envp, "owcimomd");
	}
	else
	{
		p_proc =
			mgr.monitoredSpawn(exec_path, exec_app_name, exec_argv, exec_envp);
	}

	Process::Status ps = p_proc->processStatus();
	check(ps.running(), "child process not running");

	PassInput pass_input(*(p_proc->in()));
	pass_input.start();

	PassOutput pass_output(*(p_proc->out()));
	pass_output.start();

	CollectErr collect_err(*(p_proc->err()));
	collect_err.start();

	pass_input.join();
	pass_output.join();
	collect_err.join();
	cout << "ERROR OUTPUT: ";
	cout << collect_err.get();

	p_proc->waitCloseTerm(Timeout::relative(5.0), Timeout::relative(0.0), Timeout::relative(10.0));
	ps = p_proc->processStatus();
	check(ps.exitTerminated(), "problem with child termination");
	check(ps.exitStatus() == 0, "nonzero exit status for child");

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

		if( dynamic_cast<PrivilegeManagerException*>(&e) )
		{
			cout << "Expected this exception type..." << endl;
			return 0;
		}
		cout << "Exception type was not expected." << endl;
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
