/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifdef HAVE_CONFIG_H
#include "OW_config.h"
#include <config.h>
#endif
#include "OW_Socket.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_SSLCtxMgr.hpp"
#include "OW_String.hpp"
#include <string>
#include <iostream>
extern "C"
{
#include <stdlib.h>
}
using std::string;
using std::ios;
int
main(int argc, char* argv[])
{
	try
	{
		SSLCtxMgr Ctx;
		Ctx.initClient();
		
		string ipOrHost;
//		cout << "Enter an IP or hostname: ";
//		cin >> ipOrHost;
		SocketAddress addr;
		addr = SocketAddress::getByName(argv[1], 443);
		cout << "\n\nTrying to connect on port 443...\n";
		Socket sock(addr, true);
		cout << "Getting /\n";
		ostream& ostrm = sock.getOutputStream();
		sock.waitForOutput();
		ostrm << "GET / \r\n\r\n";
		ostrm.flush();
		istream& istrm = sock.getInputStream();
		string recievedChunk;
		cout << "Receiving...\n";
		while(istrm >> recievedChunk)
		{
			cout << recievedChunk;
		}
		cout << endl;
	}
	catch(SocketException &e)
	{
		cerr << "An exception occurred: " << e.type() << " " << e.getMessage()
			<< endl;
//		cerr << "Description: " << e.what() << endl;
//		cerr << "Error num: " << e.errornum() << endl;
		return EXIT_SUCCESS;
	}
	return EXIT_SUCCESS;
}

