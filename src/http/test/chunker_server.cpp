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
/*
#include "OW_config.h"
#include "easyaddress.h"
#include "easyneterror.h"
#include "easysocket.h"
#include "easyserver.h"
*/
#include "OW_ServerSocket.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include <iostream>
#include <fstream.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
using namespace std;
using namespace OpenWBEM;

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		cout << "Usage: " << argv[0] << " <port> <file1> [file2]" << endl;
		exit(1);
	}
	try
	{
		unsigned short port;
		port = atoi(argv[1]);
		ServerSocket svr(port);
		SocketAddress localAddr = svr.getLocalAddress();
		cout << "Listening on address " << localAddr.getName() << 
			"(" << localAddr.getAddress() << ")" << ":" 
			<< localAddr.getPort() << endl;
		cout.flush();
		Socket sock = svr.accept();
		SocketAddress peerAddr = sock.getPeerAddress();
		cout << "Connection is made. Client is " << peerAddr.getName() << 
			"(" << peerAddr.getAddress() << ")" << ":" 
			<< peerAddr.getPort() << endl;
		ostream& ostr = sock.getOutputStream();
		HTTPChunkedOStream ostrm(ostr);
		ifstream filestr(argv[2], ios::in);
		char bfr[1024];
		int count = 0;
		while(filestr)
		{
			filestr.read(bfr, sizeof(bfr));
			ostrm.write(bfr, filestr.gcount());
			count += filestr.gcount();
		}
		cout << "Wrote " << count << " bytes" << endl;
		ostrm.flush();
		ostrm.termOutput();
		if (argc == 4)
		{
			ifstream filestr2(argv[3], ios::in);
			count = 0;
			while(filestr2)
			{
				filestr2.read(bfr, sizeof(bfr));
				ostrm.write(bfr, filestr2.gcount());
				count += filestr2.gcount();
			}
			cout << "Wrote " << count << " bytes" << endl;
			ostrm.flush();
			ostrm.termOutput();
		}
		svr.close();
		sock.disconnect();
	}
	catch(SocketException &e)
	{
		cerr << "An exception occurred in " << e << endl;
		return 1;
	}
	return 0;
}

