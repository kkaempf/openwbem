/*******************************************************************************
* Copyright (C) 2003-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_CIMProtocolIFC.hpp"
#include "OW_HTTPException.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "blocxx/SocketBaseImpl.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMParamValue.hpp"

#include <iostream>
#include <algorithm> // for std::find

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::find;
using namespace OpenWBEM;
using namespace WBEMFlags;
using namespace blocxx;

// create our own TEST_ASSERT, because assert and ASSERT will be turned off
// in non-debug mode.
#define TEST_ASSERT(CON) if (!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)

static const String ns("root/testsuite");

//////////////////////////////////////////////////////////////////////////////
void
usage(const char* name)
{
	cerr << "Usage: " << name << " <url> [dump file extension]" << endl;
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

		if (argc == 3)
		{
			String sockDumpOut = argv[2];
			String sockDumpIn = argv[2];
			sockDumpOut += "SockDumpOut";
			sockDumpIn += "SockDumpIn";
			SocketBaseImpl::setDumpFiles(sockDumpIn.c_str(),
				sockDumpOut.c_str());
		}
		else
		{
			SocketBaseImpl::setDumpFiles("","");
		}

		String url(argv[1]);

		ClientCIMOMHandleRef hdl = ClientCIMOMHandle::createFromURL(url);
		hdl->getWBEMProtocolHandler()->setReceiveTimeout(Timeout::relative(1.0)); // make it really short

		CIMParamValueArray inParams;
		CIMParamValueArray outParams;

		inParams.push_back(CIMParamValue("duration", CIMValue(UInt32(10))));
		CIMObjectPath cop("EXP_BartComputerSystem");

		try
		{
			hdl->invokeMethod("root/testsuite", cop, "Sleep", inParams, outParams);
		}
		catch (HTTPException&)
		{
			// this is what we're expecting.  Anything else is an error!
		}

		return 0;

	}
	catch (Exception& e)
	{
		cerr << e << endl;
	}
	return 1;
}

