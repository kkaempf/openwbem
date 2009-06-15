#include "OW_config.h"
#include "blocxx/Array.hpp"
#include "blocxx/String.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <algorithm>

using namespace std;
using namespace OpenWBEM;

extern char * * environ;

int const EXIT_NO_ARGV = -1;
int const EXIT_NO_STATUS_CODE = -2;
int const EXIT_STD_EXCEPTION = -3;
int const EXIT_UNKNOWN_EXCEPTION = -4;
int const EXIT_NO_ERRSTR = -5;
int const EXIT_BAD_ID = -6;

StringArray env()
{
	StringArray sarr;
	for (char const * const * envp = environ; *envp; ++envp)
	{
		sarr.push_back(String(*envp));
	}
	return sarr;
}

void outp(unsigned argc, char const * const * argv)
{
	cout << "inp: " << cin.rdbuf();
	cout << ";\nargs:\n";
	std::size_t i;
	for (i = 0; i < argc; ++i)
	{
		cout << "  " << argv[i] << '\n';
	}
	StringArray envarr = env();
	std::sort(envarr.begin(), envarr.end());
	cout << "env:\n";
	for (i = 0; i < envarr.size(); ++i)
	{
		cout << "  " << envarr[i] << '\n';
	}
}

#define CHECK(b, msg) \
if (!(b)) \
{ \
  cout << (msg); \
  return false; \
}

bool check_id()
{
	struct passwd* pw = getpwnam("root");
	CHECK(pw, "getpwnam(\"root\") failed");
	CHECK(pw->pw_uid == ::getuid(), "uid is wrong");
	CHECK(pw->pw_uid == ::geteuid(), "euid is wrong");
	CHECK(pw->pw_gid == ::getgid(), "gid is wrong");
	CHECK(pw->pw_gid == ::getegid(), "egid is wrong");
	return true;
}

int main_aux(int argc, char * * argv)
{
	if (!argv)
	{
		return EXIT_NO_ARGV;
	}
	if (argc < 2)
	{
		return EXIT_NO_STATUS_CODE;
	}
	if (argc < 3)
	{
		return EXIT_NO_ERRSTR;
	}
	int exit_status = std::atoi(argv[1]);
	char const * errstr = argv[2];

	if (!check_id())
	{
		return EXIT_BAD_ID;
	}
	std::cerr << errstr;
	outp(static_cast<unsigned>(argc), argv);

	return exit_status;
}

int main(int argc, char * * argv)
{
	try
	{
		return main_aux(argc, argv);
	}
	catch (std::exception & e)
	{
		cout << "Caught standard exception:\n";
		cout << "  msg: " << e.what() << endl;
		return EXIT_STD_EXCEPTION;
	}
	catch (...)
	{
		cout << "Caught unknown exception" << endl;
		return EXIT_UNKNOWN_EXCEPTION;
	}
}
