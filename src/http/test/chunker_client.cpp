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
#ifdef HAVE_CONFIG_H
#include "OW_config.h"
#include <config.h>
#endif
#include "OW_Socket.hpp"
#include "blocxx/SocketAddress.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "blocxx/String.hpp"
#include <string>
#include <fstream>
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
	if (argc < 4)
	{
		cout << "Usage: " << argv[0] << " <hostname> <port> <file1> [file2]"
			<< endl;
		exit(1);
	}
	try
	{
		SocketAddress addr;
		addr = SocketAddress::getByName(argv[1], atoi(argv[2]));
		cout << "\n\nTrying to connect..." << endl;
		Socket sock(addr);
		sock.waitForOutput();
		char buf[1024];
		istream& tmpIstrm = sock.getInputStream();
		HTTPChunkedIStream istrm(tmpIstrm);
		string recievedChunk;
		ofstream outfile(argv[3], ios::out);
		try
		{
			while (istrm)
			{
				istrm.read(buf, sizeof(buf));
				outfile.write(buf, istrm.gcount());
			}
			if (argc == 5)
			{
				istrm.resetInput();
				ofstream outfile2(argv[4], ios::out);
				while (istrm)
				{
					istrm.read(buf, sizeof(buf));
					outfile2.write(buf, istrm.gcount());
				}
			}
		}
		catch (...)
		{
			cout << "Chunk Error!" << endl;
			exit(1);
		}
		cout << endl;
	}
	catch(SocketException &e)
	{
		cerr << "An exception occurred: " << e << endl;
		return 1;
	}
	catch (...)
	{
		cout << "caught unknown exception" << endl;
	}
	return 0;
}

