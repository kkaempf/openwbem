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

#ifndef OW_EXEC_HPP_
#define OW_EXEC_HPP_

#include "OW_config.h"
#include "OW_Reference.hpp"
#include "OW_String.hpp"
#include "OW_ArrayFwd.hpp"

class OW_UnnamedPipe;
typedef OW_Reference<OW_UnnamedPipe> OW_UnnamedPipeRef;

class OW_PopenStreamsImpl;
class OW_PopenStreams
{
	public:
		OW_PopenStreams();
		~OW_PopenStreams();
		OW_PopenStreams(const OW_PopenStreams& src);
		OW_PopenStreams& operator=(const OW_PopenStreams& src);

		OW_UnnamedPipeRef in() const;
		void in(OW_UnnamedPipeRef pipe);
		OW_UnnamedPipeRef out() const;
		void out(OW_UnnamedPipeRef pipe);
		OW_UnnamedPipeRef err() const;
		void err(OW_UnnamedPipeRef pipe);
		pid_t pid() const;
		void pid(pid_t newPid);
		int getExitStatus();

	private:
		OW_Reference<OW_PopenStreamsImpl> m_impl;
};

class OW_Exec
{
	public:

		static int safeSystem(const OW_Array<OW_String>& command);
		static OW_PopenStreams safePopen(const OW_Array<OW_String>& command,
				const OW_String& initialInput = OW_String());

};

#endif
