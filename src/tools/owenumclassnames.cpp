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
#include "OW_config.h"
#include "OW_HTTPClient.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_Assertion.hpp"
#include "OW_GetPass.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>

using namespace OpenWBEM;

using std::cout;
using std::cin;
using std::endl;
using std::cerr;
class GetLoginInfo : public ClientAuthCBIFC
{
	public:
		bool getCredentials(const String& realm, String& name,
				String& passwd, const String& details)
		{
			(void)details;
			cout << "Authentication required for " << realm << endl;
			cout << "Enter the user name: ";
			name = String::getLine(cin);
			passwd = GetPass::getPass("Enter the password for " +
					name + ": ");
			return true;
		}
};
class classNamePrinter : public StringResultHandlerIFC
{
	public:
		void doHandle(const String& t)
		{
			cout << t << '\n';
		}
};	
//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		cout << "Usage: <URL> <namespace> <classname>" << endl;
		return 1;
	}
	try
	{
		String url = argv[1];
		String ns = argv[2];
		String classname = argv[3];
		URL owurl(url);
		CIMProtocolIFCRef client;
		client = new HTTPClient(url);
		/**********************************************************************
		 * Create an instance of our authentication callback class.
		 **********************************************************************/
		ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);
		/**********************************************************************
		 * Assign our callback to the HTTP Client.
		 **********************************************************************/
		client->setLoginCallBack(getLoginInfo);
		/**********************************************************************
		 * Here we create a CIMXMLCIMOMHandle and have it use the
		 * HTTPClient we've created.  CIMXMLCIMOMHandle takes
		 * a Reference<CIMProtocol> it it's constructor, so
		 * we have to make a Reference out of our HTTP Client first.
		 * By doing this, we don't have to worry about deleting our
		 * HTTPClient.  Reference will delete it for us when the
		 * last copy goes out of scope (reference count goes to zero).
		 **********************************************************************/
		CIMOMHandleIFCRef rch;
		if (owurl.path.equalsIgnoreCase("/owbinary"))
		{
			rch = new BinaryCIMOMHandle(client);
		}
		else
		{
			rch = new CIMXMLCIMOMHandle(client);
		}
		classNamePrinter handler;
		rch->enumClassNames(ns, classname, handler);
		return 0;
	}
	catch(const Exception& e)
	{
		cerr << e << endl;
	}
	catch(const std::exception& e)
	{
		cerr << e.what() << endl;
	}
	catch(...)
	{
		cerr << "Caught unknown exception in main" << endl;
	}
	return 1;
}


