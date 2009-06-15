#include "OW_PrivilegeConfig.hpp"
#include "blocxx/FileSystem.hpp"
#include <algorithm>
#include <iterator>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

//
// Usage: read_privconfig <config_file> <path_file> <exec_file> <ident1> ... <identN>
//   config_file: the privilege file containing rules for opening, unlinking,
//      renaming, execution (monitor, user, and both with checked arguments),
//      and similar rules.
//   path_file:  a file containing a newline separated list of paths that is
//      applied to all file-based rules in the config file and any matching
//      rules are printed to stdout.
//   exec_file:  a file containing a newline separated list of command lines
//      that are applied to the exec rules in the config file and any matching
//      rules are printed to stdout.
//   ident1..N: user identifiers to use in comparing the exec tests.
//
// When printing out the matches, any matching rule will be displayed as '1',
// and non-matching rules as '.'


using std::cout;
using std::endl;
using namespace OpenWBEM;
using namespace OpenWBEM::PrivilegeConfig;

template <typename Matcher, typename Container>
void print_matches(
	char const * name, Matcher const & matcher, Container const & c
)
{
	cout << name << ":\n";
	typename Container::const_iterator it, itend = c.end();
	for (it = c.begin(); it != itend; ++it)
	{
		if (matcher.match(*it))
		{
			cout << "  " << *it  << ";\n";
		}
	}
}

template <typename ContainerI, typename ContainerP>
void print_exec_matches(
	char const * name, ExecPatterns const & ep, ContainerI const & idents, const StringArray& envVars,
	ContainerP const & paths
)
{
	cout << name << ":\n";
	typename ContainerP::const_iterator i, iend = paths.end();
	for (i = paths.begin(); i != iend; ++i)
	{
		cout << "  " << *i << " ";
		typename ContainerI::const_iterator j, jend = idents.end();
		for (j = idents.begin(); j != jend; ++j)
		{
			cout << (ep.match(*i, envVars, *j) ? '1' : '.');
		}
		cout << '\n';
	}
}

template <typename ContainerI, typename ContainerP>
void print_mon_exec_matches(
	char const * name, MonitoredUserExecPatterns const & ep, 
	ContainerI const & idents, const StringArray& envVars, ContainerP const & paths
)
{
	cout << name << ":\n";
	typename ContainerI::const_iterator k, kend = idents.end(); 
	for (k = idents.begin(); k != kend; ++k)
	{
		cout << " user: " << *k << '\n';
		typename ContainerP::const_iterator i, iend = paths.end();
		for (i = paths.begin(); i != iend; ++i)
		{
			cout << "  " << *i << " ";
			typename ContainerI::const_iterator j, jend = idents.end();
			for (j = idents.begin(); j != jend; ++j)
			{
				cout << (ep.match(*i, envVars, *j, *k) ? '1' : '.');
			}
			cout << '\n';
		}
	}
}

template <typename ContainerI, typename ContainerP>
void print_exec_args_matches(
	char const * name, ExecArgsPatterns const & ep, const StringArray& envVars, ContainerI const & idents,
	ContainerP const & paths)
{
	cout << name << ":\n";
	for (size_t i = 0; i < paths.size(); ++i)
	{
		cout << "  " << paths[i] << " ";
		typename ContainerI::const_iterator j, jend = idents.end();
		for (j = idents.begin(); j != jend; ++j)
		{
			StringArray tok(paths[i].tokenize(" "));
			StringArray args(tok.begin(), tok.end());
			cout << (ep.match(tok[0], args, envVars, *j) ? '1' : '.');
		}
		cout << '\n';
	}
}

template <typename ContainerI, typename ContainerP>
void print_mon_exec_args_matches(
	char const * name, MonitoredUserExecArgsPatterns const & ep, const StringArray& envVars,
	ContainerI const & idents, ContainerP const & paths)
{
	cout << name << ":\n";
	typename ContainerI::const_iterator k, kend = idents.end();
	for (k = idents.begin(); k != kend; ++k)
	{
		cout << " user: " << *k << '\n';
		for (size_t i = 0; i < paths.size(); ++i)
		{
			cout << "  " << paths[i] << " ";
			typename ContainerI::const_iterator j, jend = idents.end();
			for (j = idents.begin(); j != jend; ++j)
			{
				StringArray tok(paths[i].tokenize(" "));
				StringArray args(tok.begin(), tok.end());
				cout << (ep.match(tok[0], args, envVars, *j, *k) ? '1' : '.');
			}
			cout << '\n';
		}
	}
}

namespace OW_NAMESPACE
{
std::istream & operator>>(std::istream & is, String & s)
{
	std::string ss;
	is >> ss;
	String(ss.c_str()).swap(s);
	return is;
}
} // end namespace OW_NAMESPACE

class TestIncludeHandler : public OpenWBEM::PrivilegeConfig::IncludeHandler
{
public:
	TestIncludeHandler()
	{
	}

	virtual std::istream* getInclude(const String& includeParam)
	{
		// this shouldn't ever get called.
		assert(0);
		return 0;
	}
	virtual void endInclude()
	{
		// this shouldn't ever get called.
		assert(0);
	}
};



int main_(int argc, char * * argv)
{
	assert(argc >= 3);
	std::ifstream is_cfg(argv[1]);

	StringArray path_vec = FileSystem::getFileLines(argv[2]);
	StringArray exec_vec = FileSystem::getFileLines(argv[3]);

	std::vector<String> idents;
	for (size_t i = 4; argv[i]; ++i)
	{
		idents.push_back(argv[i]);
	}

	StringArray envVars; // TODO: fill this in somehow

	TestIncludeHandler tih;
	openwbem_privconfig_Lexer lex(is_cfg, tih, argv[1]);
	OpenWBEM::PrivilegeConfig::Privileges priv;
	OpenWBEM::PrivilegeConfig::ParseError err;
	int code = openwbem_privconfig_parse(&priv, &err, &lex);
	if (code != 0)
	{
		cout << "Parse failed." << endl;
		cout << "Column " << err.column << " of line " << err.line << endl;
		cout << err.message << endl;
	}
	else
	{
		cout << "unpriv_user: " << priv.unpriv_user << ";\n";
		print_matches("read_dir", priv.read_dir, path_vec);
		print_exec_matches("monitored_exec", priv.monitored_exec, idents, envVars, path_vec);
		print_exec_matches("user_exec", priv.user_exec, idents, envVars, path_vec);
		print_exec_args_matches("monitored_exec_check_args", priv.monitored_exec_check_args, envVars, idents, exec_vec);
		print_exec_args_matches("user_exec_check_args", priv.user_exec_check_args, envVars, idents, exec_vec);
		print_matches("open_read", priv.open_read, path_vec);
		print_matches("open_write", priv.open_write, path_vec);
		print_matches("open_append", priv.open_append, path_vec);
		print_matches("check_path", priv.check_path, path_vec);
		print_matches("rename_from", priv.rename_from, path_vec);
		print_matches("rename_to", priv.rename_to, path_vec);
		print_matches("remove_file", priv.remove_file, path_vec);
		print_mon_exec_matches("monitored_user_exec", priv.monitored_user_exec, idents, envVars, path_vec);
		print_mon_exec_args_matches("monitored_user_exec_check_args", priv.monitored_user_exec_check_args, envVars, idents, exec_vec);
	}
	return 0;
}

int main(int argc, char * * argv)
{
	try
	{
		return main_(argc, argv);
	}
	catch (Exception & e)
	{
		cout << "Exception thrown: " << e << '\n';
		return 1;
	}
	catch (std::exception & e)
	{
		cout << "std::exception thrown: " << e.what() << '\n';
		return 1;
	}
	catch (...)
	{
		cout << "Unknown exception thrown\n";
		return 2;
	}
}
