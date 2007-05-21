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
#include "OW_AutoDescriptor.hpp"
#include "OW_IPCIO.hpp"
#include "OW_PrivilegeCommon.hpp"
#include "OW_Secure.hpp"
#include "OW_String.hpp"
#include "OW_Timeout.hpp"
#include "OW_Format.hpp"
#include <cstdlib>
#include <cstring>
#include <cerrno>
#ifndef OW_WIN32
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace OW_NAMESPACE
{
namespace PrivilegeCommon
{

#define CONN_DESC "OW_PRIVILEGE_MONITOR_DESCRIPTOR"
const char* const CONN_DESC_ENV_VAR = CONN_DESC;
const char* const CONN_DESC_ENV_ENTRY_PFX = CONN_DESC "=";

DescriptorInfo monitor_descriptor()
{
	DescriptorInfo rv;
	rv.descriptor = -1;
	char const * s_desc = std::getenv(CONN_DESC_ENV_VAR);
	if (!s_desc)
	{
		rv.errnum =  EXIT_NO_DESCRIPTOR_STRING;
		return rv;
	}
	char * end;
	errno = 0;
	long tmp = std::strtol(s_desc, &end, 10);
	int desc = static_cast<int>(tmp);
	if (errno != 0 || *end != '\0' || desc < 0 || desc != tmp)
	{
		rv.errnum = EXIT_BAD_DESCRIPTOR_STRING;
		return rv;
	}
	rv.errnum = 0;
	rv.descriptor = desc;
	return rv;
}

OW_DEFINE_EXCEPTION(SpawnMonitor);

#define CHECK(tst, msg) CHECK_E((tst), SpawnMonitorException, (msg))
#define CHECK_ERRNO(tst, msg) CHECK_ERRNO_E((tst), SpawnMonitorException, (msg))

SpawnMonitorPolicy::~SpawnMonitorPolicy()
{
}

void spawn_monitor(
	char const * config_dir, char const * app_name,	SpawnMonitorPolicy & policy
)
{
	config_dir = policy.check_config_dir(config_dir);
	CHECK(app_name, "app_name must be non-null");
	CHECK(!std::strchr(app_name, '/'), "app_name must not contain '/'");

#ifdef OW_WIN32
#pragma message(Reminder "TODO: implement it for Win!")
	HANDLE sockfds[2];
	AutoDescriptor parent_desc(sockfds[0]);
	AutoDescriptor child_desc(sockfds[1]);
#else
	int sockfds[2];
	CHECK_ERRNO(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockfds) == 0,
		"socketpair");

	AutoDescriptor parent_desc(sockfds[0]);
	AutoDescriptor child_desc(sockfds[1]);
	OW_ASSERT(parent_desc.get() >= 3);
	OW_ASSERT(child_desc.get() >= 3);
#endif
	// This call may throw
	policy.spawn(child_desc.get(), parent_desc.get(), config_dir, app_name);
	child_desc.reset();

	IPCIO conn(parent_desc, Timeout::relative(60.0));
	// We use unbuffered reads, because there is one item (the ProcId)
	// initially sent from the monitor, that we don't read here, and we
	// don't want it to get sucked into conn's internal buffer.
	// The spawn method of policy generally dup's its own copy of
	// parent_desc.get() for later communication with the monitor.

	EStatus status;
	ipcio_get(conn, status, IPCIO::E_THROW_ON_EOF, IPCIO::E_UNBUFFERED);
	if (status == PrivilegeCommon::E_ERROR)
	{
		String errmsg;
		int errcode;
		ipcio_get(conn, errcode);
		ipcio_get(conn, errmsg);
		CHECK(false, Format("creation of monitor failed (%1): %2", errcode, errmsg));
	}

	String unpriv_user;
	ipcio_get(conn, unpriv_user, std::size_t(-1),
		IPCIO::E_THROW_ON_EOF, IPCIO::E_UNBUFFERED);
	// no get_sync(), as we haven't read the child pid yet
	Secure::runAs(unpriv_user.c_str());
}

} // namespace PrivilegeCommon
} // namespace OW_NAMESPACE
