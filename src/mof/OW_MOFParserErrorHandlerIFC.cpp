/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#include "OW_config.h"
#include "OW_MOFParserErrorHandlerIFC.hpp"

/////////////////////////////////////////////////////////////////////////////// 
OW_MofParserErrorHandlerIFC::OW_MofParserErrorHandlerIFC() 
	: m_errorCount(0) 
{
}

/////////////////////////////////////////////////////////////////////////////// 
OW_MofParserErrorHandlerIFC::~OW_MofParserErrorHandlerIFC()
{
}

/////////////////////////////////////////////////////////////////////////////// 
long
OW_MofParserErrorHandlerIFC::errorCount() 
{ 
	return m_errorCount; 
}

/////////////////////////////////////////////////////////////////////////////// 
void OW_MofParserErrorHandlerIFC::fatalError( const char* error, const lineInfo& li )
{
	++m_errorCount;
	doFatalError(error,li);
	OW_THROW(OW_MofParseFatalErrorException, "");
}

/////////////////////////////////////////////////////////////////////////////// 
void OW_MofParserErrorHandlerIFC::recoverableError( const char* error, const lineInfo& li )
{
	++m_errorCount;
	if ( doRecoverableError(error,li) == Abort )
		OW_THROW(OW_MofParseFatalErrorException, "");
}

/////////////////////////////////////////////////////////////////////////////// 
void OW_MofParserErrorHandlerIFC::progressMessage( const char* message, const lineInfo& li )
{
	doProgressMessage( message, li );
}


