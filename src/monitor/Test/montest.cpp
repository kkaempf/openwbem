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

#include "OW_Array.hpp"
#include "OW_Assertion.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Exception.hpp"
#include "OW_FileSystem.hpp"
#include "OW_PrivilegeManager.hpp"
#include "OW_String.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_Format.hpp"

#include <algorithm>
#include <iostream>
#include <string>

#include <pwd.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fcntl.h>

using namespace OpenWBEM;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

typedef OpenWBEM::PrivilegeManager::OpenFlags OpenFlags;

OW_DECLARE_EXCEPTION(MonTest);
OW_DEFINE_EXCEPTION(MonTest);

void check(bool b, String const & msg)
{
	if (!b)
	{
		OW_THROW(MonTestException, msg.c_str());
	}
}

template<typename T>
void println(T const & x)
{
	cout << "  " << x << endl;
}

String get_string()
{
	std::string tmp;
	cin >> tmp;
	OW_ASSERT(cin);
	return String(tmp.c_str());
}

OpenFlags get_open_flags()
{
	String text = get_string();
	StringArray toks = text.tokenize("|");
	int flags = 0;
	for (std::size_t i = 0; i < toks.size(); ++i)
	{
		if (toks[i] == "in")
		{
			flags |= PrivilegeManager::in;
		}
		else if (toks[i] == "out")
		{
			flags |= PrivilegeManager::out;
		}
		else if (toks[i] == "trunc")
		{
			flags |= PrivilegeManager::trunc;
		}
		else if (toks[i] == "app")
		{
			flags |= PrivilegeManager::app;
		}
		else if (toks[i] == "ate")
		{
			flags |= PrivilegeManager::ate;
		}
		else if (toks[i] == "binary")
		{
			flags |= PrivilegeManager::binary;
		}
		else if (toks[i] == "nonblock")
		{
			flags |= PrivilegeManager::posix_nonblock;
		}
		else
		{
			OW_ASSERT(false);
		}
	}
	return static_cast<OpenFlags>(flags);
}

PrivilegeManager::OpenPerms get_perms()
{
	unsigned n = get_string().toUnsignedInt(8);
	return static_cast<PrivilegeManager::OpenPerms>(n);
}

PrivilegeManager::ReadDirOptions get_read_dir_opt()
{
	String tmp = get_string();
	if (tmp == "keep_special")
	{
		return PrivilegeManager::E_KEEP_SPECIAL;
	}
	else if (tmp == "omit_special")
	{
		return PrivilegeManager::E_OMIT_SPECIAL;
	}
	else
	{
		OW_ASSERT(false);
		// Not normally reached.
		return PrivilegeManager::E_KEEP_SPECIAL;
	}
}

void output_args(int argc, char * * argv)
{
	cout << "argv:" << endl;
	for (int i = 0; i < argc; ++i)
	{
		cout << "  " << argv[i] << ';' << endl;
	}
}

extern char **environ;

void output_env()
{
	cout << "environ:" << endl;
	// output them in sorted order
	StringArray envvars;
	for (char const * const * penv = environ; *penv; ++penv)
	{
		envvars.push_back(String("  ") + *penv + ';');
	}
	std::sort(envvars.begin(), envvars.end());
	for (size_t i = 0; i < envvars.size(); ++i)
	{
		cout << envvars[i] << endl;
	}
}

void output_username()
{
	::uid_t uid = ::getuid();
	::uid_t euid = ::geteuid();
	check(uid == euid, "uid and euid differ");
	struct passwd * pw = getpwuid(uid);
	check(pw, "getpwuid failed");
	cout << "User: " << pw->pw_name << endl;
}

char const * make_cstr(std::string const & s)
{
	return s == "null" ? 0 : s == "empty" ? "" : s.c_str();
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
	std::size_t const BUFSZ = 1024;
	char cwdbuf[BUFSZ + 1];
	std::string config_dir, app_name, user_name, tmp;
	cin >> tmp >> config_dir;
	OW_ASSERT(cin);
	OW_ASSERT(tmp == "config_dir");
	cin >> tmp >> app_name;
	OW_ASSERT(cin);
	OW_ASSERT(tmp == "app_name");
	cin >> tmp >> user_name;
	OW_ASSERT(cin);
	OW_ASSERT(tmp == "user_name");

	ConfigOpts::installed_owlibexec_dir = 
		"/montest-689acb0e1ec89f45-7085a2a1780f5f42/libexec/openwbem";
	PrivilegeManager::use_lib_path = true;
	PrivilegeManager mgr;
	if (argc == 1) // Test program was run directly
	{
		LoggerSpec ls;
		ls.categories.push_back("*");
		ls.message_format = "[%c] [%p] %m";
		ls.file_name = config_dir.c_str();
		ls.file_name += "/monitorlog";
		ls.max_file_size = UInt64(-1);
		ls.max_backup_index = UInt32(-1);
		mgr = PrivilegeManager::createMonitor(
			make_cstr(config_dir), make_cstr(app_name), make_cstr(user_name),
			&ls);
	}
	else // Test program was run via call to PrivilegeManager::monitoredSpawn
	{
		mgr = PrivilegeManager::connectToMonitor();
		output_args(argc, argv);
		output_env();
	}

	check(::getcwd(cwdbuf, BUFSZ), "getcwd failed");
	check(std::strcmp(cwdbuf, "/") == 0, "cwd != '/'");
	// Chdir to /tmp, so coredump can be written if program crashes.
	check(::chdir("/tmp") == 0, "chdir failed");

	output_username();

	while (cin >> tmp)
	{

		try
		{
			if (!tmp.empty() && tmp[0] == '#')
			{
				//				cerr << "skipping comment line: " << tmp;
				std::getline(cin, tmp);
				//				cerr << tmp << endl;
				continue;
			}
			cout << '\n' << tmp << endl;
			if (tmp == "open")
			{
				OpenFlags flags = get_open_flags();
				PrivilegeManager::OpenPerms perms = get_perms();
				String path = get_string();

				//				cerr << std::oct << "Opening file " << path << " with flags " << flags << " and perms " << perms << std::dec << endl;
				String contents;
				if (flags & PrivilegeManager::out)
				{
					// This has to happen before call to mgr.open, in
					// case an exception gets thrown
					contents = get_string();
				}
// 				if( !contents.empty() )
// 				{
// 					cerr << Format("Writing to file:\n%1", contents) << endl;
// 				}
				AutoDescriptor f = mgr.open(path.c_str(), flags, perms);


				if( flags & PrivilegeManager::posix_nonblock )
				{
					// Verify the nonblocking status on the open file descriptor.
					int fd_flags = ::fcntl(f.get(), F_GETFL);
					check(fd_flags & O_NONBLOCK, Format("Nonblocking open did not set nonblocking status: set flags=%1", fd_flags));
					cout << "--->Nonblocking status set properly<---" << endl;
				}

				if (flags & PrivilegeManager::out)
				{
					FileSystem::write(f.get(), contents.c_str(), contents.length());
				}
				if (flags & PrivilegeManager::in)
				{
					if (flags & PrivilegeManager::out)
					{
						FileSystem::rewind(f.get());
					}
					std::size_t const BUFSZ = 1024;
					char buf[BUFSZ];
					StringBuffer sbuf;
					std::size_t r;
					while ((r = FileSystem::read(f.get(), buf, BUFSZ)) > 0)
					{
						sbuf.append(buf, r);
					}
					cout << "  " << sbuf.c_str() << endl;
				}
			}
			else if (tmp == "readDirectory")
			{
				String dirpath = get_string();
				PrivilegeManager::ReadDirOptions opt = get_read_dir_opt();
				StringArray sa = mgr.readDirectory(dirpath.c_str(), opt);
				std::sort(sa.begin(), sa.end());
				std::for_each(sa.begin(), sa.end(), &println<String>);
			}
			else if (tmp == "readLink")
			{
				String s = mgr.readLink(get_string());
				cout << "  " << s << endl;
			}
			else if (tmp == "rename")
			{
				String from = get_string();
				String to = get_string();
				mgr.rename(from.c_str(), to.c_str());
			}
			else if (tmp == "unlink")
			{
				String path = get_string();
				bool retval = mgr.unlink(path.c_str());
				cout << retval << endl;
			}
			else if (tmp == "monitoredSpawn")
			{
				// failure test only
				String exec_path = get_string();
				String app_name = get_string();
				StringArray argv = get_string().tokenize("+");
				StringArray envp = get_string().tokenize("+");
				copy_env_var("LD_LIBRARY_PATH", envp);
				copy_env_var("LIBPATH", envp); // AIX
				copy_env_var("SHLIB_PATH", envp); // HPUX
				copy_env_var("DYLD_LIBRARY_PATH", envp); // DARWIN (OS X)
				ProcessRef pproc(
					mgr.monitoredSpawn(exec_path, app_name, argv, envp));
			}
			else if (tmp == "userSpawn")
			{
				// failure test only
				String exec_path = get_string();
				StringArray argv = get_string().tokenize("+");
				StringArray envp = get_string().tokenize("+");
				copy_env_var("LD_LIBRARY_PATH", envp);
				copy_env_var("LIBPATH", envp); // AIX
				copy_env_var("SHLIB_PATH", envp); // HPUX
				copy_env_var("DYLD_LIBRARY_PATH", envp); // DARWIN (OS X)
				String user = get_string();
				ProcessRef pproc(mgr.userSpawn(exec_path, argv, envp, user, ""));
			}
			else if (tmp == "monitoredUserSpawn")
			{
				// failure test only
				String exec_path = get_string();
				String app_name = get_string();
				StringArray argv = get_string().tokenize("+");
				StringArray envp = get_string().tokenize("+");
				String user_name = get_string();
				copy_env_var("LD_LIBRARY_PATH", envp);
				copy_env_var("LIBPATH", envp); // AIX
				copy_env_var("SHLIB_PATH", envp); // HPUX
				copy_env_var("DYLD_LIBRARY_PATH", envp); // DARWIN (OS X)
				ProcessRef pproc(
					mgr.monitoredUserSpawn(exec_path, app_name, argv, envp, user_name));
			}
		}
		catch (Exception & e)
		{
			cout << "Exception:" << endl;
			cout << "  type: " << e.type() << endl;
			// If the monitor has exited because we tried to carry out an
			// operation we weren't authorized for, subsequent attempts to
			// carry out PrivilegeManager operations will result in IPCIO
			// errors, but the exact error message will be depend on timing.
			// Thus we don't output the error message in this case.
			if (std::strcmp(e.type(), "IPCIOException") != 0)
			{
				cout << "  msg:  " << e.getMessage() << endl;
			}
		}
	}
	if (argc != 1)
	{
		cout.flush();
		cerr << "err output" << endl;
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
		//cout << "  : " << e << endl;
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
