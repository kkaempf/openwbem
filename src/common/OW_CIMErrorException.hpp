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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#ifndef OW_CIMERROREXCEPTION_H_INCLUDE_GUARD_
#define OW_CIMERROREXCEPTION_H_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"

namespace OpenWBEM
{

class CIMErrorException: public Exception
{
public:
	CIMErrorException(const char* file, int line, const char* msg)
		: Exception(file, line, msg) {}
	virtual const char* type() const { return "CIMErrorException"; }
	static const char* const unsupported_protocol_version; 
	static const char* const multiple_requests_unsupported; 
	static const char* const unsupported_cim_version; 
	static const char* const unsupported_dtd_version; 
	static const char* const request_not_valid; 
	static const char* const request_not_well_formed;
	static const char* const request_not_loosely_valid;
	static const char* const header_mismatch;
	static const char* const unsupported_operation;
	
};

} // end namespace OpenWBEM

typedef OpenWBEM::CIMErrorException OW_CIMErrorException OW_DEPRECATED;

#endif
