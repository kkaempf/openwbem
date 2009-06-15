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
#include "blocxx/SocketAddress.hpp"
#include "OW_Socket.hpp"
#include "blocxx/String.hpp"
#include "OW_Exception.hpp"
#include "blocxx/Select.hpp"
#include "blocxx/Thread.hpp"

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
// TODO: make this portable to windows... Maybe have a way to get an UnnamedPipe to stdin
class DataTransfer : public Thread
{
public:
	DataTransfer(int inputfd, int outputfd) 
		: m_inputfd(inputfd)
		, m_outputfd(outputfd)
	{
	}

	Int32 run()
	{
		int rc = 0;
		vector<char> buff;
		do
		{
			SelectObjectArray soa;
			if (m_inputfd != -1)
			{
				SelectObject soin(m_inputfd); 
				soin.waitForRead = true;
				soa.push_back(soin);
			}

			if (buff.size() > 0 && m_outputfd != -1)
			{
				SelectObject soout(m_outputfd);
				soout.waitForWrite = true;
				soa.push_back(soout);
			}

			if (soa.size() == 0)
			{
				return 0;
			}

			rc = Select::selectRW(soa);

			for (int i = 0; i < rc; ++i)
			{
				const SelectObject& so(soa[i]);
				if (so.readAvailable)
				{
					char tmpbuf[1024];
					ssize_t bytesRead = ::read(so.s, tmpbuf, sizeof(tmpbuf));
					if (bytesRead == -1)
					{
						perror("read failed");
						return 1;
					}
					else if (bytesRead == 0)
					{
						::shutdown(so.s, SHUT_RD);
						m_inputfd = -1;
					}
					else
					{
						buff.insert(buff.end(), tmpbuf, tmpbuf + bytesRead);
					}
				}

				if (so.writeAvailable)
				{
					int bytesWritten = ::write(so.s, &buff[0], buff.size());
					if (bytesWritten > 0)
					{
						buff.erase(buff.begin(), buff.begin() + bytesWritten);

						// check for nothing left to write
						if (buff.size() == 0 && m_inputfd == -1)
						{
							// by shutting down half the socket, the server can know to disconnect, but we could still read the response.
							// TODO: make this portable to win32
							::shutdown(so.s, SHUT_WR);
							m_outputfd = -1;
							return 0;
						}
					}
					else if (bytesWritten < 0)
					{
						perror("write to socket failed");
						return 1;
					}
				}

			}

		} while (rc > 0);

		return 0;
	}

private:
	int m_inputfd;
	int m_outputfd;
};

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

		const int stdinfd = 0;
		const int stdoutfd = 1;
		DataTransfer input(stdinfd, sock.getfd());
		DataTransfer output(sock.getfd(), stdoutfd);

		input.start();
		output.start();

		Int32 inputrv = input.join();
		Int32 outputrv = output.join(); 

		return inputrv + outputrv;

	}
	catch (Exception& e)
	{
		cerr << e << endl;
	}
	return 1;
}

