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
#include "Grammar.h"
#include "OW_HTTPClient.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_IPCClient.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "CIMOMVisitor.hpp"
#include "OW_Assertion.hpp"
#include "OW_MofParserErrorHandlerIFC.hpp"
#include "MofCompiler.hpp"
#include "OW_GetPass.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include <iostream>

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

class GetLoginInfo : public OW_ClientAuthCBIFC
{
	public:
		OW_Bool getCredentials(const OW_String& realm, OW_String& name,
				OW_String& passwd)
		{
			cout << "Authentication required for " << realm << endl;
			cout << "Enter the user name: ";
			name = OW_String::getLine(cin);
			passwd = OW_GetPass::getPass("Enter the password for " +
				name + ": ");
			return OW_Bool(true);
		}
};

//////////////////////////////////////////////////////////////////////////////
class TheErrorHandler: public OW_MofParserErrorHandlerIFC
{
public:
	TheErrorHandler() : OW_MofParserErrorHandlerIFC() {}
	virtual ~TheErrorHandler(){}

protected:
	virtual void doFatalError(const char *error, const lineInfo& li )
	{
		cerr << "Fatal error in file: " << li.filename << " on line: " << li.lineNum << ": " << error << endl;
	}

	virtual ParserAction doRecoverableError(const char *error, const lineInfo& li )
	{
		cerr << "Recoverable error in file: " << li.filename << " on line: " << li.lineNum << ": " << error << endl;
		return Ignore;
	}

	virtual void doProgressMessage( const char* message, const lineInfo& li )
	{
		cout << "File: " << li.filename << " Line: " << li.lineNum << ": " << message << endl;
	}

};

int main(int argc, char** argv)
{
	long errors = 1; // default to 1 in case something throws
	try
	{
		if (argc != 4)
		{
			cout << "Usage: URL namespace moffile" << endl;
			return 1;
		}

		OW_URL url(argv[1]);
		OW_Reference<OW_CIMOMHandleIFC> handle;
		OW_Reference<OW_MofParserErrorHandlerIFC> theErrorHandler(new TheErrorHandler);
		OW_CIMProtocolIFCRef client;
		
		if(url.protocol.equalsIgnoreCase("IPC"))
		{
			client = new OW_IPCClient(argv[1]);
			handle = OW_CIMOMHandleIFCRef(new OW_BinaryCIMOMHandle(client));
		}
		else
		{
			client = new OW_HTTPClient(argv[1]);
			if (url.path.equalsIgnoreCase("/owbinary"))
			{
				handle = OW_CIMOMHandleIFCRef(new OW_BinaryCIMOMHandle(client));
			}
			else
			{
				handle = OW_CIMOMHandleIFCRef(new OW_CIMXMLCIMOMHandle(
					client));
			}
		}
		client->setLoginCallBack(OW_ClientAuthCBIFCRef(new GetLoginInfo));

		MofCompiler theCompiler(handle, argv[2], theErrorHandler);
		errors = theCompiler.compile(argv[3]);

		cout
			<< "Compilation finished.  "
			<< errors
			<< " errors occurred." << endl;
	}
	catch(OW_Exception& e)
	{
		cerr << "Caught OW_Exception: " << e.type() << endl;
		cerr << "File: " << e.getFile() << endl;
		cerr << "Line: " << e.getLine() << endl;
		cerr << "Msg: " << e.getMessage() << endl;
	}
	catch(...)
	{
		cerr << "Caught unknown exception" << endl;
	}

	return errors;
}

