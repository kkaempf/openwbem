/*******************************************************************************
* Copyright (C) 2005, Vintela, Inc. All rights reserved.
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
*     * Neither the name of 
*       Vintela, Inc., 
*       nor Novell, Inc., 
*       nor the names of its contributors or employees may be used to 
*       endorse or promote products derived from this software without 
*       specific prior written permission.
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
 * @author <see corresponding BloCxx header>
 */




#ifndef OW_ExceptionIds_HPP_INCLUDE_GUARD_
#define OW_ExceptionIds_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include <blocxx/ExceptionIds.hpp>
#include "OW_Exception.hpp"

// This is using BLOCXX_NAMESPACE instead of OW_NAMESPACE because the 
// macros in blocxx/Exception.hpp expect the ExceptionIds to be in 
// BLOCXX_NAMESPACE
namespace BLOCXX_NAMESPACE
{
	namespace ExceptionIds
	{
		// The purpose of this file/namespace is to centralize all 
		// the exception subclass id numbers,
		// in order to to prevent id conflicts.  Numbering starts from 1.
		// All numbers must be positive (BloCxx uses negative numbers). 

		// common library
		static const int CIMExceptionId = 1;
		static const int CIMErrorExceptionId = 2;
		static const int CIMProtocolExceptionId = 8;
		static const int BadCIMSignatureExceptionId = 23;
		static const int CIMDateTimeExceptionId = 49;

		// http
		static const int Base64FormatExceptionId = 1001;
		static const int UnescapeCharForURLExceptionId = 1002;

		// xml
		static const int DOMExceptionId = 2001;

		// client
		static const int CIMOMLocatorExceptionId = 3001;

		// http server
		static const int HTTPServerExceptionId = 4001;

		// wql
		static const int TypeMismatchExceptionId = 5001;

		// cim/xml request handler
		static const int BadStreamExceptionId = 6001;

		// repository
		static const int HDBExceptionId = 7001;
		static const int IndexExceptionId = 7002;

		// server
		static const int AuthManagerExceptionId = 8001;
		static const int DaemonExceptionId = 8002;
		static const int CIMOMEnvironmentExceptionId = 8003;
		static const int IndicationServerExceptionId = 8004;
		static const int EmbeddedCIMOMEnvironmentExceptionId = 8005; 

		// mof compiler lib
		static const int MOFCompilerExceptionId = 9001;
		static const int ParseFatalErrorExceptionId = 9002;

		// cpp provider interface
		static const int CppProviderIFCExceptionId = 10001;
		static const int ProviderEnvironmentExceptionId = 10002;

	}
}

#endif

