/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/


#ifndef OW_CIMPROTOCOLISTREAMIFC_HPP_INCLUDE_GUARD_
#define OW_CIMPROTOCOLISTREAMIFC_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Reference.hpp"

#if defined(OW_HAVE_ISTREAM)
#include <istream>
#elif defined(OW_HAVE_ISTREAM_H)
#include <istream.h>
#else
#include <iostream>
#endif

class OW_CIMProtocolIStreamIFC;

typedef OW_Reference<OW_CIMProtocolIStreamIFC> OW_CIMProtocolIStreamIFCRef;


class OW_CIMProtocolIStreamIFC : public std::istream
{
public:
	OW_CIMProtocolIStreamIFC(std::streambuf* strbuf) : std::istream(strbuf) {}

	/**
	 * This function should be called after the end of the stream has 
	 * been reached.  It will check the trailers for an error, and through
	 * an appropriate CIMException (constructed from the info from the 
	 * trailers).
	 */
	virtual void checkForError() const { }
	virtual ~OW_CIMProtocolIStreamIFC() {}
};

#endif



