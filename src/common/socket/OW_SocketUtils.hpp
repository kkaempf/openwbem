/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef _OW_SOCKETUTILS_HPP__
#define _OW_SOCKETUTILS_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_NetworkTypes.hpp"

class OW_String;
class OW_Bool;

class OW_SocketUtils
{
public:

	/**
	 * Wait for input or output on a socket.
	 * @param fd the handle of the socket to wait on.
	 * @param timeOutSecs the number of seconds to wait.
	 * @param forInput true if we are waiting for input.
	 * return zero if we got input before the timeout expired, non-zero
	 * 	otherwise.
	 */
	static int waitForIO(OW_SocketHandle_t fd, int timeOutSecs, OW_Bool forInput);
	static OW_String inetAddrToString(OW_UInt64 addr); // TODO OS abstract this?
private:
	OW_SocketUtils();	// don't allow instantiation.
};


#endif	// _OW_SOCKETUTILS_HPP__

