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
*     * Neither the name of Quest Software, Inc., nor the
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

/**
 * @author Kevin S. Van Horn
 */

#include "OW_config.h"
#include "blocxx/Array.hpp"
#include "OW_Exception.hpp"
#include "OW_PrivilegeManager.hpp"
#include "blocxx/String.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

#include <pwd.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>

using namespace std;
using namespace OpenWBEM;
using namespace blocxx;

extern char * * environ;

char const * const specialvars[] =
{
	"LD_LIBRARY_PATH=", "LIBPATH=", "SHLIB_PATH=", "DYLD_LIBRARY_PATH=",
	"OW_PRIVILEGE_MONITOR_DESCRIPTOR=", 0
};

bool startswith(char const * s, char const * prefix)
{
	return strncmp(s, prefix, strlen(prefix)) == 0;
}

bool special_env_var(char const * s)
{
	for (char const * const * p = specialvars; *p; ++p)
	{
		if (startswith(s, *p))
		{
			return true;
		}
	}
	return false;
}

int main_aux(int argc, char * * argv)
{
	uid_t uid = getuid();
	uid_t euid = geteuid();
	cout << "uid == euid: " << (uid == euid) << endl;

	struct passwd * ppwd = getpwuid(uid);
	cout << "User: " << (ppwd ? ppwd->pw_name : "<NULL>") << endl;

	cout << cin.rdbuf() << endl;

	char const * fname = argc > 1 ? argv[1] : 0;
	cout << "argv:";
	for ( ; *argv; ++argv)
	{
		cout << " " << *argv;
	}
	cout << endl;
	cout << "env:";
	for (char const * const * p = environ; *p; ++p)
	{
		if (!special_env_var(*p))
		{
			cout << " " << *p;
		}
	}
	cout << endl;

	PrivilegeManager mgr = PrivilegeManager::connectToMonitor();
	cout << "read_dir:" << endl;
	if (fname)
	{
		StringArray entries = mgr.readDirectory(fname, PrivilegeManager::E_OMIT_SPECIAL);
		sort(entries.begin(), entries.end());
		for (size_t i = 0; i < entries.size(); ++i)
		{
			cout << "  " << entries[i] << endl;
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
		cout << "Exception: " << e << endl;
		return 1;
	}
	catch (...)
	{
		cout << "Unkown exception" << endl;
		return 2;
	}
}
