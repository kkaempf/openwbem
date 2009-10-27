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

#include <algorithm>
#include <iostream>
#include <signal.h>
#include <pwd.h>
#include <unistd.h>

using namespace OpenWBEM;
using std::cin;
using std::cout;
using std::endl;
using namespace blocxx;

OW_DECLARE_EXCEPTION(UserExecTest);
OW_DEFINE_EXCEPTION(UserExecTest);

void check(bool b, String const & msg)
{
	if (!b)
	{
		OW_THROW(UserExecTestException, msg.c_str());
	}
}

class SendInput : public Thread
{
public:
	SendInput(UnnamedPipe & in, char const * s)
	: m_in(in),
	  m_send(s)
	{
	}

private:
	virtual Int32 run();
	UnnamedPipe & m_in;
	String m_send;
};

Int32 SendInput::run()
{
	char const * buf = m_send.c_str();
	int const bufsz = m_send.length();
	int nw = 0;
	while (nw < bufsz)
	{
		int n = m_in.write(buf + nw, bufsz - nw);
		check(n > 0, "write failure");
		nw += n;
	}
	m_in.close();
	return 0;
}

class CollectOutput : public Thread
{
public:
	CollectOutput(UnnamedPipe & out)
	: m_out(out)
	{
	}

	String get() const
	{
		return m_sbuf.toString();
	}

private:
	virtual Int32 run();
	UnnamedPipe & m_out;
	StringBuffer m_sbuf;
};

Int32 CollectOutput::run()
{
	std::size_t const BUFSZ = 64;
	char buf[BUFSZ];
	int n;
	while ((n = m_out.read(buf, BUFSZ-1, IOIFC::E_THROW_ON_ERROR)) > 0)
	{
		buf[n] = '\0';
		m_sbuf += buf;
	}
	check(n == 0, "read error");
	return 0;
}

String expected_out(
	String const & child_inp,
	StringArray const & exec_argv, StringArray const & exec_envp
)
{
	StringBuffer sbuf;
	sbuf += "inp: ";
	sbuf += child_inp;
	sbuf += ";\nargs:\n";
	std::size_t i;
	for (i = 0; i < exec_argv.size(); ++i)
	{
		sbuf += "  ";
		sbuf += exec_argv[i];
		sbuf += '\n';
	}
	StringArray tmp = exec_envp;
	std::sort(tmp.begin(), tmp.end());
	sbuf += "env:\n";
	for (i = 0; i < tmp.size(); ++i)
	{
		sbuf += "  ";
		sbuf += tmp[i];
		sbuf += '\n';
	}
	return sbuf.releaseString();
}

void check_id()
{
	struct passwd * ppwd = ::getpwnam("owprovdr");
	check(ppwd, "getpwnam(\"owprovdr\") failed");
	check(ppwd->pw_uid == ::getuid(), "uid is wrong");
	check(ppwd->pw_uid == ::geteuid(), "euid is wrong");
	check(ppwd->pw_gid == ::getgid(), "gid is wrong");
	check(ppwd->pw_gid == ::getegid(), "egid is wrong");
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
	check(argc == 7, "wrong number of program arguments");
	char const * const * argp = argv + 1;
	char const * config_dir = *argp++;
	char const * app_name = *argp++;
	char const * exec_path = *argp++;
	StringArray exec_argv = String(*argp++).tokenize("+");
	StringArray exec_envp = String(*argp++).tokenize("+");
	copy_env_var("LD_LIBRARY_PATH", exec_envp);
	copy_env_var("LIBPATH", exec_envp); // AIX
	copy_env_var("SHLIB_PATH", exec_envp); // HPUX
	copy_env_var("DYLD_LIBRARY_PATH", exec_envp); // DARWIN (OS X)

	char const * child_inp = *argp++;
	check(exec_argv.size() >= 3, "too few exec args");

	int expected_exit_status = exec_argv[1].toInt();
	String err_str = exec_argv[2];

	ConfigOpts::installed_owlibexec_dir =
		"/userexectest-689acb0e1ec89f45-7085a2a1780f5f42/libexec/openwbem";
	PrivilegeManager::use_lib_path = true;
	PrivilegeManager mgr = PrivilegeManager::createMonitor(config_dir, app_name);
	check_id();
	ProcessRef p_proc =
		mgr.userSpawn(exec_path, exec_argv, exec_envp, "root", "");

	Process::Status ps = p_proc->processStatus();
	check(ps.running(), "child process not running");

	SendInput send_input(*(p_proc->in()), child_inp);
	send_input.start();

	CollectOutput collect_output(*(p_proc->out()));
	collect_output.start();

	CollectOutput collect_err(*(p_proc->err()));
	collect_err.start();

	send_input.join();
	collect_output.join();
	collect_err.join();

	p_proc->waitCloseTerm(Timeout::relative(5.0), Timeout::relative(0.0), Timeout::relative(10.0));
	ps = p_proc->processStatus();
	check(ps.exitTerminated(), "problem with child termination");
	check(ps.exitStatus() == expected_exit_status,
		Format("wrong exit status for child: %1, expected %2, output: %3", ps.exitStatus(), expected_exit_status, collect_output.get()).c_str());
	check(collect_output.get() == expected_out(child_inp, exec_argv, exec_envp),
		"child wrote wrong data to stdout");
	check(collect_err.get() == err_str, "child wrote wrong data to stderr");

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
