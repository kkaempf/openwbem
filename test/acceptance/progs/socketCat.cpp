/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

/**
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_SocketAddress.hpp"
#include "OW_Socket.hpp"
#include "OW_String.hpp"
#include "OW_Exception.hpp"
#include "OW_Select.hpp"

#include <vector>

#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <iostream>

#include <sys/socket.h> // for shutdown()

using namespace std;
using namespace OpenWBEM;
using namespace OpenWBEM::Select;

//////////////////////////////////////////////////////////////////////////////
void
usage(const char* name)
{
	cerr << "Usage: " << name << " <addr> [port]" << endl;
}


//////////////////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			usage(argv[0]);
			return 1;
		}


		SocketAddress addr;
		if (argc == 2)
		{
			// uds
			addr = SocketAddress::getUDS(argv[1]);
		}
		else
		{
			// tcp
			addr = SocketAddress::getByName(argv[1], String(argv[2]).toUInt16());
		}

		Socket sock(addr);

		int rc;
		vector<char> buff;
		bool stdinOpen = true;
		do
		{
			SelectObjectArray soa;
			SelectObject soin(0); // TODO: make this portable to windows... Maybe have a way to get an UnnamedPipe to stdin
			if (stdinOpen)
			{
				soin.waitForRead = true;
			}
			soa.push_back(soin);

			SelectObject sosock(sock.getSelectObj());
			sosock.waitForRead = true;
			if (buff.size() > 0)
			{
				sosock.waitForWrite = true;
			}
			soa.push_back(sosock);
			rc = Select::selectRW(soa);

			if (rc > 0)
			{
				// read from stdin into the buffer
				if (soa[0].readAvailable)
				{
					char tmpbuf[1024];
					ssize_t bytesRead = ::read(0, tmpbuf, sizeof(tmpbuf));
					if (bytesRead == -1)
					{
						perror("read from stdin failed");
						return 1;
					}
					else if (bytesRead == 0)
					{
						stdinOpen = false;
					}
					else
					{
						buff.insert(buff.end(), tmpbuf, tmpbuf + bytesRead);
					}
				}

				// write on the socket
				if (soa[1].writeAvailable)
				{
					int bytesWritten = sock.write(&buff[0], buff.size(), true);
					if (bytesWritten > 0)
					{
						buff.erase(buff.begin(), buff.begin() + bytesWritten);

						// check for nothing left to write
						if (buff.size() == 0 && !stdinOpen)
						{
							// by shutting down half the socket, the server can know to disconnect, but we could still read the response.
							::shutdown(sock.getfd(), SHUT_WR); 
						}
					}
					else
					{
						perror("write to socket failed");
						return 1;
					}
				}

				// read from the socket
				if (soa[1].readAvailable)
				{
					char tmpbuf[1024];
					int bytesRead = sock.read(tmpbuf, sizeof(tmpbuf), true);
					if (bytesRead > 0)
					{
						cout.write(tmpbuf, bytesRead);
						if (!cout.good())
						{
							perror("write to cout failed");
							return 1;
						}
					}
					else if (bytesRead == 0)
					{
						break; // once the server closes the socket, we'll just quit
					}
				}
			}

		} while (rc > 0);

		return 0;

	}
	catch (Exception& e)
	{
		cerr << e << endl;
	}
	return 1;
}

