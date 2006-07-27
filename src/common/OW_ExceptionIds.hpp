/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#ifndef OW_EXCEPTION_IDS_HPP_INCLUDE_GUARD
#define OW_EXCEPTION_IDS_HPP_INCLUDE_GUARD
#include "OW_config.h"

namespace OW_NAMESPACE
{
	namespace ExceptionIds
	{
		// The purpose of this file/namespace is to centralize all the exception subclass id numbers,
		// in order to to prevent id conflicts.  Numbering starts from 1.

		// common library
		static const int CIMExceptionId = 1;
		static const int CIMErrorExceptionId = 2;
		static const int HTTPExceptionId = 3;
		static const int XMLParseExceptionId = 4;
		static const int AssertionExceptionId = 5;
		static const int ValueCastExceptionId = 6;
		static const int AuthenticationExceptionId = 7;
		static const int CIMProtocolExceptionId = 8;
		static const int UnknownHostExceptionId = 9;
		static const int SocketAddressExceptionId = 10;
		static const int SocketExceptionId = 11;
		static const int SocketTimeoutExceptionId = 12;
		static const int ConditionLockExceptionId = 13;
		static const int ConditionResourceExceptionId = 14;
		static const int ConfigExceptionId = 15;
		static const int NULLCOWReferenceExceptionId = 16;
		static const int DateTimeExceptionId = 17;
		static const int EnumerationExceptionId = 18;
		static const int ExecTimeoutExceptionId = 19;
		static const int ExecBufferFullExceptionId = 20;
		static const int ExecErrorExceptionId = 21;
		static const int IOExceptionId = 22;
		static const int BadCIMSignatureExceptionId = 23;
		static const int LoggerExceptionId = 24;
		static const int DeadlockExceptionId = 25;
		static const int NoSuchPropertyExceptionId = 26;
		static const int NoSuchProviderExceptionId = 27;
		static const int NoSuchQualifierExceptionId = 28;
		static const int NULLValueExceptionId = 29;
		static const int ContextDataNotFoundExceptionId = 30;
		static const int OutOfBoundsExceptionId = 31;
		static const int FileSystemExceptionId = 32;
		static const int RWLockerExceptionId = 33;
		static const int NULLReferenceExceptionId = 34;
		static const int SelectExceptionId = 35;
		static const int SharedLibraryExceptionId = 36;
		static const int StringConversionExceptionId = 37;
		static const int TimeoutExceptionId = 38;
		static const int ThreadExceptionId = 39;
		static const int CancellationDeniedExceptionId = 40;
		static const int ThreadBarrierExceptionId = 41;
		static const int MalformedURLExceptionId = 42;
		static const int UUIDExceptionId = 43;
		static const int UnnamedPipeExceptionId = 44;
		static const int InvalidUTF8ExceptionId = 45;
		static const int CmdLineParserExceptionId = 46;
		static const int NULLCOWIntrusiveReferenceExceptionId = 47;
		static const int LogMessagePatternFormatterExceptionId = 48;
		static const int CIMDateTimeExceptionId = 49;
		static const int CryptographicRandomNumberExceptionId = 50;
		static const int SSLExceptionId = 51;

		// http
		static const int Base64FormatExceptionId = 1001;
		static const int UnescapeCharForURLExceptionId = 1002;
		static const int MD5ExceptionId = 1003;

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

