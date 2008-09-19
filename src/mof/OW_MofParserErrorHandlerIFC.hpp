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

#ifndef OW_MOF_PARSER_ERROR_HANDLER_IFC_HPP_
#define OW_MOF_PARSER_ERROR_HANDLER_IFC_HPP_
#include "OW_config.h"
#include "LineInfo.hpp"
#include "OW_Exception.hpp"

DEFINE_EXCEPTION(MofParseFatalError);

// this is an abstract base class for create concrete error handlers for the mof parser
class OW_MofParserErrorHandlerIFC
{
public:
	OW_MofParserErrorHandlerIFC() : m_errorCount(0) {}
	virtual ~OW_MofParserErrorHandlerIFC(){}

	void fatalError( const char* error, const lineInfo& li );

	enum ParserAction
	{
		Abort,
		Ignore
	};

	void recoverableError( const char* error, const lineInfo& li );

	void progressMessage( const char* message, const lineInfo& li );

	long errorCount() { return m_errorCount; }

protected:
	virtual void doFatalError( const char* error, const lineInfo& li ) = 0;
	virtual ParserAction doRecoverableError( const char* error, const lineInfo & li ) = 0;
	virtual void doProgressMessage( const char* message, const lineInfo& li ) = 0;


private:
	long m_errorCount;

};

inline void OW_MofParserErrorHandlerIFC::fatalError( const char* error, const lineInfo& li )
{
	++m_errorCount;
	doFatalError(error,li);
	OW_THROW(OW_MofParseFatalErrorException, "");
}

inline void OW_MofParserErrorHandlerIFC::recoverableError( const char* error, const lineInfo& li )
{
	++m_errorCount;
	if ( doRecoverableError(error,li) == Abort )
		OW_THROW(OW_MofParseFatalErrorException, "");
}

inline void OW_MofParserErrorHandlerIFC::progressMessage( const char* message, const lineInfo& li )
{
	doProgressMessage( message, li );
}


#endif //MOF_PARSER_ERROR_HANDLER_HPP