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
#include "OW_config.h"
#include "OW_HTTPSvrConnection.hpp"
#include "OW_HTTPUtils.hpp"

int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		cerr << "Usage: " << argv[0] << " <host> <port> <name> <password>" 
			<< endl;
		return 1;
	}
	try
	{
		String namePass = argv[3];
		namePass += String(":") + argv[4];
		String encoded = HTTPUtils::base64Encode(namePass);
		String info = "Basic " + encoded;
		SocketAddress addr = SocketAddress::getByName(argv[1], atoi(argv[2]));
		HTTPServer htIn(9877);
		Socket sock(addr);
		HTTPSvrConnection svr(sock, &htIn);
		int sc;
		for (int i = 0; i < 10; i++)
		{
			sc = svr.performBasicAuthentication(info);
			if ( sc < 300 )
			{
				cout << "Good! " << i << endl;
			}
			else
			{
				cout << "Bad! " << i << endl;
			}
		}
	}
	catch (Exception& e)
	{
		cerr << e << endl;
	}
	
	return 0;
}

