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
#include "OW_HTTPClient.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_Assertion.hpp"
#include "OW_GetPass.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMUrl.hpp"

#include <iostream>

using std::cout;
using std::cin;
using std::endl;
using std::cerr;

class GetLoginInfo : public OW_ClientAuthCBIFC
{
	public:
		OW_Bool getCredentials(const OW_String& realm, OW_String& name,
				OW_String& passwd, const OW_String& details)
		{
			(void)details;
			cout << "Authentication required for " << realm << endl;
			cout << "Enter the user name: ";
			name = OW_String::getLine(cin);
			passwd = OW_GetPass::getPass("Enter the password for " +
				name + ": ");
			return OW_Bool(true);
		}
};


//////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout << "Usage: <URL> <namespace>" << endl;
		return 1;
	}

	try
	{
		OW_String url = argv[1];
		OW_String ns = argv[2];

		OW_URL owurl(url);

		OW_CIMProtocolIFCRef client;
		client = new OW_HTTPClient(url);


		/**********************************************************************
		 * Create an instance of our authentication callback class.
		 **********************************************************************/
		
		OW_ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);

		/**********************************************************************
		 * Assign our callback to the HTTP Client.
		 **********************************************************************/

		client->setLoginCallBack(getLoginInfo);

		/**********************************************************************
		 * Here we create a OW_CIMXMLCIMOMHandle and have it use the
		 * OW_HTTPClient we've created.  OW_CIMXMLCIMOMHandle takes
		 * a OW_Reference<OW_CIMProtocol> it it's constructor, so
		 * we have to make a OW_Reference out of our HTTP Client first.
		 * By doing this, we don't have to worry about deleting our
		 * OW_HTTPClient.  OW_Reference will delete it for us when the
		 * last copy goes out of scope (reference count goes to zero).
		 **********************************************************************/

		OW_CIMOMHandleIFCRef rch;
		if (owurl.path.equalsIgnoreCase("/owbinary"))
		{
			rch = new OW_BinaryCIMOMHandle(client);
		}
		else
		{
			rch = new OW_CIMXMLCIMOMHandle(client);
		}

		cout << "Deleting namespace (" << ns << ")" << endl;
		rch->deleteNameSpace(ns);

		return 0;
	}
	catch(const OW_Exception& e)
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
