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

#ifdef HAVE_CONFIG_H
#include "OW_config.h"
#include <config.h>
#endif

#include "OW_Socket.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_HTTPChunker.hpp"
#include "OW_String.hpp"
#include <iostream>
#include <string>

extern "C"
{
#include <stdlib.h>
}

using std::string;
using std::ios;

int
main(int argc, char* argv[])
{
	OW_HTTPChunker* chunker;
	if (argc < 3)
	{
		cout << "Usage: " << argv[0] << " <hostname> <path> [chunked]" << endl;
		exit(1);
	}
	istream* istrm;
	try
	{
		string ipOrHost;

//		cout << "Enter an IP or hostname: ";
//		cin >> ipOrHost;

		OW_SocketAddress addr;
		addr = OW_SocketAddress::getByName(argv[1], 80);

		cout << "\n\nTrying to connect on port 80...\n";
		OW_Socket sock(addr);

		cout << "Getting /\n";

		ostream& ostrm = sock.getOutputStream();
		sock.waitForOutput();
		ostrm << "GET " << argv[2] << " HTTP/1.1\r\n\r\n";
		ostrm.flush();
		char buf[1024];
		istrm = &sock.getInputStream();

		cout << "Receiving...\n";
		while (istrm->getline(buf, 1023))
		{
			cout << buf << endl;
			if (strcmp(buf,"\r") == 0)
				break;
		}
		cout << "^^HEADER^^" << endl;
		cout << "stream states: good bad fail eof rdstate" << endl;
		cout << istrm->good() << istrm->bad() << istrm->fail() << istrm->eof()
			<< istrm->rdstate() << endl;
		

		if (argc >= 4 && strcmp(argv[3], "chunked") == 0)
		{
			chunker = new OW_HTTPChunker(istrm);
			istrm = &chunker->getInputStream();
			cout << "Switched istream pointer to chunker" << endl;
			cout << "stream states: good bad fail eof rdstate" << endl;
			cout << istrm->good() << istrm->bad() << istrm->fail() << istrm->eof()
				<< istrm->rdstate() << endl;
		}

		string recievedChunk;
		cout << "~~BODY~~" << endl;
		try
		{
			while((*istrm) >> recievedChunk)
			{
				cout << recievedChunk;
			}
		}
		catch (OW_HTTPChunkException& e)
		{
			cout << "Chunk Error!" << endl;
			exit(1);
		}


		cout << endl;
	}
	catch(OW_SocketException &e)
	{
		cerr << "An exception occurred: " << e.type() << " " << e.getMessage()
			<< endl;
//		cerr << "Description: " << e.what() << endl;
//		cerr << "Error num: " << e.errornum() << endl;
		return EXIT_SUCCESS;
	}
	catch (...)
	{
		cout << "caught unknown exception" << endl;
	}

	cout << "^^END OF BODY^^" << endl;
	cout << "stream states: good bad fail eof rdstate" << endl;
	cout << istrm->good() << istrm->bad() << istrm->fail() << istrm->eof()
		<< istrm->rdstate() << endl;

	return EXIT_SUCCESS;
}
