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

/*
#include "OW_config.h"
#include "easyaddress.h"
#include "easyneterror.h"
#include "easysocket.h"
#include "easyserver.h"
*/

#include "OW_InetServerSocket.hpp"
#include "OW_InetAddress.hpp"

#include <iostream>
#include <fstream.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

int main(void)
{
	try
	{
		cout << "What port do you want this server to squat on? ";
		unsigned short port;
		cin >> port;

		OW_InetServerSocket svr(port);
		OW_InetAddress localAddr = svr.getLocalAddress();
		cout << "Listening on address " << localAddr.getName() << 
			"(" << localAddr.getAddress() << ")" << ":" 
			<< localAddr.getPort() << endl;
		cout.flush();

		OW_InetSocket sock = svr.accept();
		OW_InetAddress peerAddr = sock.getPeerAddress();
		cout << "Connection is made. Client is " << peerAddr.getName() << 
			"(" << peerAddr.getAddress() << ")" << ":" 
			<< peerAddr.getPort() << endl;
		ostream& ostrm = sock.getOutputStream();
		ostrm << "Hello, this is a silly server. Enter a line of text!\n\r";
		ostrm.flush();

		ifstream& istrm = (ifstream&) sock.getInputStream();
		char bfr[512];
		sock.waitForInput();
		istrm.getline(bfr, sizeof(bfr), '\n');

		cout << bfr;
		cout.flush();
		cout << endl;

		svr.close();
		sock.disconnect();
	}
	catch(OW_SocketException &e)
	{
		cerr << "An exception occurred in " << endl;
		return 1;
	}

	return 0;
}
