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

#include "OW_config.h"
#include "OW_File.hpp"

#include <fcntl.h>

namespace {
/////////////////////////////////////////////////////////////////////////////
// implementation of lock functions
int
doLock(int hdl, int cmd, short int type)
{
	struct flock lck;
	::memset ((char *) &lck, '\0', sizeof (lck));
	lck.l_type = type;       /* write (exclusive) lock */
	lck.l_whence = 0;           /* 0 offset for l_start */
	lck.l_start = 0L;           /* lock starts at BOF */
	lck.l_len = 0L;             /* extent is entire file */
	return ::fcntl(hdl, cmd, &lck);
}
} // end unnamed namespace

/////////////////////////////////////////////////////////////////////////////
int 
OW_File::getLock()
{
	return doLock(m_hdl, F_SETLKW, F_WRLCK);
}

/////////////////////////////////////////////////////////////////////////////
int
OW_File::tryLock()
{
	return doLock(m_hdl, F_SETLK, F_WRLCK);
}

/////////////////////////////////////////////////////////////////////////////
int
OW_File::unlock()
{
	return doLock(m_hdl, F_SETLK, F_UNLCK);
}




