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

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */


#include "OW_config.h"
#include "OW_HTTPClient.hpp"
#include "blocxx/TempFileStream.hpp"
#include "OW_Assertion.hpp"
#include "OW_HTTPException.hpp"
#include "OW_SPNEGOHandler.hpp"
#include "blocxx/Process.hpp"
#include "blocxx/Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "blocxx/Exec.hpp"
#include "blocxx/FileSystem.hpp"
#include "blocxx/IOException.hpp"

#include <fstream>
#include <iostream>

using std::cerr;
using std::endl;
using std::cout;
using std::cin;
using std::ifstream;
using std::istream;
using namespace OpenWBEM;

namespace
{

OW_DECLARE_EXCEPTION(GSSAPIHelperSPNEGOHandler);
OW_DEFINE_EXCEPTION(GSSAPIHelperSPNEGOHandler);

String getStderr(const ProcessRef& proc)
{
	// something went wrong, now we'll check stderr
	proc->err()->setReadTimeout(Timeout::relative(1.0));
	String output;
	try
	{
		output = proc->err()->readAll();
	}
	catch (IOException& e)
	{
		// ignore it
	}
	return output;
}

class GSSAPIHelperSPNEGOHandler : public SPNEGOHandler
{
public:
	GSSAPIHelperSPNEGOHandler(const String& serverName)
	{
		startProcess(serverName);
	}

	virtual ~GSSAPIHelperSPNEGOHandler()
	{
		try
		{
			if (m_gssapiHelper)
			{
				m_gssapiHelper->waitCloseTerm(Timeout::relative(0),
				                              Timeout::relative(0.01),
				                              Timeout::relative(0.02));
			}
		}
		catch (...)
		{
			// eat it.
		}

	}

	virtual EResult handshake(const String& inData, String& outData, String& errMessage)
	{
		IOIFCStreamBuffer inbuf(m_gssapiHelper->out().getPtr());
		IOIFCStreamBuffer outbuf(m_gssapiHelper->in().getPtr());
		std::istream istr(&inbuf);
		std::ostream ostr(&outbuf);
		istr.tie(&ostr);
		cerr << "indata: " << inData << endl;
		ostr << inData << '\n';
		ostr << "1" << endl; // only have one connection to worry about.
		String result = String::getLine(istr);
		cerr << result << endl;
		
		if (result == "S")
		{
			String::getLine(istr); // read and discard the username
			// success.
			outData = String::getLine(istr);
			return E_SUCCESS;
		}
		else if (result == "F")
		{
			errMessage = String::getLine(istr);
			return E_FAILURE;
		}
		else if (result == "C")
		{
			cerr << "We are in continue" << endl;
			// continue, so save off the token for the next round.
			outData = String::getLine(istr);
			return E_CONTINUE;
		}
		else
		{
			// something has gone horribly wrong. This shouldn't ever happen
			// unless there is a bug.
			errMessage = Format("SPNEGOAuthentication received unknown response"
			" (%1) from spnego helper process. Terminating.", 
				result);
			m_gssapiHelper->waitCloseTerm(Timeout::relative(0.01),
			                              Timeout::relative(0),
			                              Timeout::relative(0.02));
			errMessage += Format(" stderr = %1. status = %2",
			                     getStderr(m_gssapiHelper),
			                     m_gssapiHelper->processStatus().toString());
			return E_FAILURE;
		}
	}

private:
	ProcessRef m_gssapiHelper;

	void startProcess(const String& serverName)
	{
		String helperPath(ConfigOpts::installed_owlibexec_dir + 
		                  "/owspnegogssapihelper");
		StringArray helperArgv(1, helperPath);
		helperArgv.push_back("client");
		helperArgv.push_back(serverName);
		helperArgv.push_back("HTTP/"); // we'll use the host's id.
		m_gssapiHelper = Exec::spawn(helperArgv, Exec::currentEnvironment);
	
		if (!m_gssapiHelper->processStatus().running())
		{
			String msg = Format("SPNEGOAuthentication failed to start %1.",
			                    helperPath);
			OW_THROW(GSSAPIHelperSPNEGOHandlerException, msg.c_str());
		}
		#if 0
		Timeout to(Timeout::relative(5));
		m_gssapiHelper->in()->setTimeouts(to);
		m_gssapiHelper->out()->setTimeouts(to);
		m_gssapiHelper->err()->setTimeouts(to);
		#endif
	}

//	static String vasHelperPath()
//	{
//		if (FileSystem::exists("/opt/quest/lib/libvas.so.4"))
//		{
//			return ConfigOpts::installed_owlibexec_dir + "/owspnegovas4helper";
//		}
//		else if (FileSystem::exists("/opt/vas/lib/libvas.so.3"))
//		{
//			return ConfigOpts::installed_owlibexec_dir + "/owspnegovas3helper";
//		}
//		return "";
//	}

};

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	if (argc < 3 || argc > 3)
	{
		cerr << "Usage: " << argv[0] << " <url> <libexec dir>" << endl;
		return 2;
	}

	String url(argv[1]);
	URL parsedUrl(url);

	ConfigOpts::installed_owlibexec_dir = argv[2];

	try
	{
		HTTPClient hc(url);
		hc.setSPNEGOHandler(SPNEGOHandlerRef(new GSSAPIHelperSPNEGOHandler(
		                                     parsedUrl.host
		                                                                  )
		                                    )
		                   );
		CIMFeatures cf = hc.getFeatures();
		cout << "CIMProtocolVersion = " << cf.protocolVersion << endl;
		cout << "CIMProduct = " << ((cf.cimProduct == CIMFeatures::SERVER)?
			"CIMServer": "CIMListener") << endl;
		cout << "CIMSupportedGroups = ";
		for (size_t i = 0; i < cf.supportedGroups.size(); i++)
		{
			cout << cf.supportedGroups[i];
			if (i < cf.supportedGroups.size() - 1)
			{
				cout << ", ";
			}
		}
		cout << endl;

		cout << "Supports Batch = " << cf.supportsBatch << endl;
		cout << "CIMValidation = " << cf.validation << endl;
		cout << "CIMSupportedQueryLanguages = ";
		for (size_t i = 0; i < cf.supportedQueryLanguages.size(); i++)
		{
			cout << cf.supportedQueryLanguages[i];
			if (i < cf.supportedQueryLanguages.size() - 1)
			{
				cout << ", ";
			}
		}
		cout << endl;

		cout << "CIMOM path = " << cf.cimom << endl;
		cout << "HTTP Ext URL = " << cf.extURL << endl;
	
	}
	catch(HTTPException& he)
	{
		cerr << he << endl;
		String message = he.getMessage();
		size_t idx = message.indexOf("Unauthorized");
		if (idx != String::npos)
		{
			return 1;
		}
		else
		{
			return 2; // probably "No login/password to send"
		}
	}
	catch(AssertionException& a)
	{
		cerr << "Caught assertion in main(): " << a << endl;
		return 3;
	}
	catch(Exception& e)
	{
		cerr << e << endl;
		return 4;
	}
	catch(...)
	{
		cerr << "Caught Unknown exception in main()" << endl;
		return 5;
	}

	return 0;
}






