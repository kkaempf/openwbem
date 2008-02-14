#ifndef OW_PRIVILEGE_COMMON_HPP_INCUDE_GUARD_
#define OW_PRIVILEGE_COMMON_HPP_INCUDE_GUARD_

/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* Copyright (C) 2006, Novell, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Network Associates, 
*       nor Quest Software, Inc., nor Novell, Inc., nor the
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
#include "OW_Cstr.hpp"
#include "OW_Exception.hpp"

#define WRAP_STMT(stmt) \
	do { stmt; } while (false)


#define THROW_MSG_E(exType, msg) \
	OW_THROW(exType, ::OpenWBEM::Cstr::to_char_ptr(msg))

#define THROW_MSG_ERR(exType, msg, err) \
	OW_THROW_ERR(exType, ::OpenWBEM::Cstr::to_char_ptr(msg), err)

#define THROW_ERRNO_MSG1_E(exType, msg, errnum) \
	OW_THROW_ERRNO_MSG1(exType, ::OpenWBEM::Cstr::to_char_ptr(msg), errnum)

#define THROW_ERRNO_MSG_E(exType, msg) \
	WRAP_STMT( \
		int e_L2PAUaO0QnF587qEKERm = errno; \
		THROW_ERRNO_MSG1_E(exType, msg, e_L2PAUaO0QnF587qEKERm) \
	)
// errno saved in case evaluation of msg alters it

#define CHECK_E(tst, exType, msg) \
	WRAP_STMT(if (!(tst)) THROW_MSG_E(exType, msg))

#define CHECK_E_ERR(tst, exType, msg, err) \
	WRAP_STMT(if (!(tst)) THROW_MSG_ERR(exType, msg, err))

#define CHECK_ERRNO1_E(tst, exType, msg, errnum) \
	WRAP_STMT(if (!(tst)) THROW_ERRNO_MSG1_E(exType, msg, errnum))

#define CHECK_ERRNO_E(tst, exType, msg) \
	WRAP_STMT(if (!(tst)) THROW_ERRNO_MSG_E(exType, msg))

namespace OW_NAMESPACE
{
namespace PrivilegeCommon
{
	enum ECommand
	{
		E_CMD_SET_LOGGER,
		E_CMD_OPEN,
		E_CMD_READ_DIR,
		E_CMD_READ_LINK,
#if 0
		E_CMD_CHECK_PATH,
#endif
		E_CMD_RENAME,
		E_CMD_UNLINK,
		E_CMD_MONITORED_SPAWN,
		E_CMD_USER_SPAWN,
		E_CMD_KILL,
		E_CMD_POLL_STATUS,
		E_CMD_DONE,
		E_CMD_MONITORED_USER_SPAWN,
		E_CMD_STAT,
		E_CMD_LSTAT,
		E_NUM_MONITOR_CMDS // must be last
	};

	enum EStatus
	{
		E_OK,
		E_ERROR
	};



	struct DescriptorInfo
	{
		// Either 0 (no error) or EXIT_NO_DESCRIPTOR_STRING or
		// EXIT_BAD_DESCRIPTOR_STRING
		int errnum;
		int descriptor;
	};

	DescriptorInfo monitor_descriptor();

	OW_DECLARE_EXCEPTION(SpawnMonitor);

	struct SpawnMonitorPolicy
	{
		virtual ~SpawnMonitorPolicy();
		virtual char const * check_config_dir(char const * config_dir) = 0;
		virtual void spawn(
			int child_desc, int parent_desc,
			char const * config_dir, char const * app_name
		) = 0;
	};

	void spawn_monitor(
		char const * config_dir, char const * app_name,
		SpawnMonitorPolicy & policy
	);

	extern const char* const CONN_DESC_ENV_VAR;
	extern const char* const CONN_DESC_ENV_ENTRY_PFX;
	int const EXIT_UNCAUGHT_EXCEPTION = 1;
	int const EXIT_FAILED_INIT = 2;
	int const EXIT_NO_DESCRIPTOR_STRING = 3;
	int const EXIT_BAD_DESCRIPTOR_STRING = 4;
	int const EXIT_IPCIO_ERROR = 5;

} // namespace PrivilegeCommon
} // namespace OW_NAMESPACE

#endif
