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
#include "OW_MD5.hpp"
#include "OW_String.hpp"
#include "OW_GetPass.hpp"
#include "OW_SocketAddress.hpp"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <stdlib.h>

using std::ofstream;
using std::cout;
using std::cerr;
using std::endl;

void usage(const char* cmd)
{
	cerr << "Usage: " << cmd << " -l <login_name> [-h <hostname>] "
		"-f <password_file> [-p <password>]" << endl;
}

int main(int argc, char* argv[])
{
	const char* const short_options = "l:h:f:p:";
	OW_String name;
	OW_String hostname;
	OW_String filename;
	OW_String passwd;

	int c = getopt(argc, argv, short_options);
	while (c != -1)
	{
		switch (c)
		{
			case 'l':
				name = optarg;
				break;
			case 'h':
				hostname = optarg;
				break;
			case 'f':
				filename = optarg;
				break;
			case 'p':
				passwd = optarg;
				break;
			default:
				usage(argv[0]);
				exit(1);
		}
		c = getopt(argc, argv, short_options);
	}

	if (filename.empty() || name.empty() )
	{
		usage(argv[0]);
		exit(1);
	}

	if (hostname.empty())
	{
		OW_SocketAddress iaddr = OW_SocketAddress::getAnyLocalHost();
		hostname = iaddr.getName();
	}

	ofstream outfile(filename.c_str(), std::ios::app);

	if(!outfile)
	{
		cerr << "Unable to open password file " << filename << endl;
		exit(1);
	}

	if (passwd.empty())
	{
		for(;;)
		{
			passwd = OW_GetPass::getPass("Please enter the password for " +
				name + ": ");
			OW_String rePasswd = OW_GetPass::getPass("Please retype the password for " +
				name + ": ");
			if(passwd.equals(rePasswd))
			{
				break;
			}
			else
			{
				cout << "Passwords do not match.  Please try again" << endl;
				continue;
			}
		}
	}

	OW_MD5 md5;

	md5.update(name);
	md5.update(":");
	md5.update(hostname);
	md5.update(":");
	md5.update(passwd);

	outfile << name << ":" << hostname << ":" << md5.toString() << endl;

	return 0;
}




