/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_Select.hpp"
#include "OW_Assertion.hpp"

#if defined(OW_WIN32)
#include "OW_AutoPtr.hpp"
#include <cassert>
#endif

extern "C"
{
#ifdef OW_NETWARE
/* The NetWare standard libc library is striving to be posix.  According to the documentation
 * for select, pselect, etc. we must include <sys/select.h> to be posix, whereas earlier
 * standards required the three includes <sys/time.h>, <sys/types.h> and <unistd.h>
 */
#include <sys/select.h>
#endif

#ifdef OW_HAVE_SYS_TIME_H
 #include <sys/time.h>
#endif

#include <sys/types.h>

#ifdef OW_HAVE_UNISTD_H
 #include <unistd.h>
#endif

#include <errno.h>
}

namespace OpenWBEM
{

namespace Select
{
#if defined(OW_WIN32)

int
select(const SelectTypeArray& selarray, UInt32 ms)
{
	int rc;
	size_t hcount = static_cast<DWORD>(selarray.size());
	AutoPtrVec<HANDLE> hdls(new HANDLE[hcount]);

	for(size_t i = 0; i < hcount; i++)
	{
		hdls[i] = selarray[i];
	}

	DWORD timeout = (ms != ~0U) ? ms : INFINITE;
	DWORD cc = ::WaitForMultipleObjects(hcount, hdls.get(), FALSE, timeout);

	assert(cc != WAIT_ABANDONED);

	switch(cc)
	{
		case WAIT_FAILED:
			rc = Select::SELECT_ERROR;
			break;
		case WAIT_TIMEOUT:
			rc = Select::SELECT_TIMEOUT;
			break;
		default:
			rc = cc - WAIT_OBJECT_0;
			break;
	}

	return cc;
}

#else
//////////////////////////////////////////////////////////////////////////////
int
select(const SelectTypeArray& selarray, UInt32 ms)
{
   fd_set rfds;
   struct timeval tv;
   int rc;
   int maxfd = 0;
   FD_ZERO(&rfds);
   for(size_t i = 0; i < selarray.size(); i++)
   {
	  if(maxfd < selarray[i])
	  {
		 maxfd = selarray[i];
	  }
	  FD_SET(selarray[i], &rfds);
   }
   struct timeval* ptv = NULL;
   if (ms != ~0U)
   {
	   ptv = &tv;
	   tv.tv_sec = ms / 1000;
	   tv.tv_usec = (ms % 1000) * 1000;
   }
   rc = ::select(maxfd+1, &rfds, NULL, NULL, ptv);
   if(rc < 0)
   {
	   if (errno == EINTR)
	   {
		   return Select::SELECT_INTERRUPTED;
	   }
	   else
	   {
		   return Select::SELECT_ERROR;
	   }
   }
   if(rc == 0)
   {
	   return Select::SELECT_TIMEOUT;
   }
   for(size_t i = 0; i < selarray.size(); i++)
   {
	  if(FD_ISSET(selarray[i], &rfds))
	  {
		 return int(i);
	  }
   }
   OW_THROW(Assertion, "Logic error in Select. Didn't find file handle");
   return Select::SELECT_ERROR;
}
#endif	// #else OW_WIN32

} // end namespace Select

} // end namespace OpenWBEM

