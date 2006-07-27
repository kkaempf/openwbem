#include "OW_PrivilegeConfig.hpp"
#include "OW_FileSystem.hpp"
#include <algorithm>
#include <iterator>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

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
	char const * name, ExecPatterns const & ep, ContainerI const & idents,
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
			cout << (ep.match(*i, *j) ? '1' : '.');
		}
		cout << '\n';
	}
}

template <typename ContainerI, typename ContainerP>
void print_exec_args_matches(
	char const * name, ExecArgsPatterns const & ep, ContainerI const & idents,
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
			StringArray args(tok.begin() + 1, tok.end());
			cout << (ep.match(tok[0], args, *j) ? '1' : '.');
		}
		cout << '\n';
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
}

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

	openwbem_privconfig_Lexer lex(is_cfg);
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
		print_exec_matches("monitored_exec", priv.monitored_exec, idents, path_vec);
		print_exec_matches("user_exec", priv.user_exec, idents, path_vec);
		print_exec_args_matches("monitored_exec_check_args", priv.monitored_exec_check_args, idents, exec_vec);
		print_exec_args_matches("user_exec_check_args", priv.user_exec_check_args, idents, exec_vec);
		print_matches("open_read", priv.open_read, path_vec);
		print_matches("open_write", priv.open_write, path_vec);
		print_matches("open_append", priv.open_append, path_vec);
		print_matches("check_path", priv.check_path, path_vec);
		print_matches("rename_from", priv.rename_from, path_vec);
		print_matches("rename_to", priv.rename_to, path_vec);
		print_matches("unlink", priv.unlink, path_vec);
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
