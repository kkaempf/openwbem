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
#include "OW_TempFileStream.hpp"
#include "OW_Assertion.hpp"
#include "OW_HTTPException.hpp"

#include <fstream>
#include <iostream>

#ifdef OW_HAVE_OPENSSL
#include <openssl/ssl.h>
#include "OW_SSLCtxMgr.hpp"
#endif

using std::cerr;
using std::endl;
using std::cout;
using std::cin;
using std::ifstream;
using std::istream;
using namespace OpenWBEM;

// certificate verify callback.
#ifdef OW_HAVE_OPENSSL
int ssl_verifycert_callback(X509* cert);
#endif

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	if (argc < 2 || argc > 3)
	{
		cerr << "Usage: " << argv[0] << " <url> [input_file_name]" << endl;
		cerr << "	If no input file is given, OPTIONS is assumed" << endl;
		return 2;
	}

	String url(argv[1]);



	try
	{

		SSLClientCtxRef sslctx;
#ifdef OW_HAVE_OPENSSL
		//SSLCtxMgr::setCertVerifyCallback(ssl_verifycert_callback);
		char* sslcert = getenv("OW_SSL_CLIENT_CERT");
		if (sslcert)
		{
			cerr << "Initializing SSL in client mode with " << sslcert << endl;
			SSLOpts opts;
			opts.certfile = String(sslcert);
			sslctx = SSLClientCtxRef(new SSLClientCtx(opts));
		}
#endif

		
		HTTPClient hc(url, sslctx);
	
		if (argc == 3)
		{
			ifstream infile(argv[2]);
			hc.setContentType("application/xml");
			Reference<std::ostream> tfsOut = hc.beginRequest("CIMBatch", "");
			*tfsOut << infile.rdbuf();
			// this should extract the version out of the xml...
			Reference<std::istream> istr = hc.endRequest(tfsOut, "CIMBatch", "", CIMProtocolIFC::E_CIM_BATCH_OPERATION_REQUEST, "1.0");
			cout << istr->rdbuf() << endl;
			hc.endResponse(*istr);
		}
		else
		{
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

	
	}
	catch(HTTPException& he)
	{
		cerr << he << endl;
		String message = he.getMessage();
		size_t idx = message.indexOf("Authentication failure");
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
		cerr << "Caught assertion in main(): " << a.getMessage() << endl;
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

//////////////////////////////////////////////////////////////////////////////
#ifdef OW_HAVE_OPENSSL
void display_name(const char* prefix, X509_NAME* name)
{
	char buf[256];

	X509_NAME_get_text_by_NID(name,
							  NID_organizationName,
							  buf,
							  256);
	printf("%s%s\n",prefix,buf);
	X509_NAME_get_text_by_NID(name,
							  NID_organizationalUnitName,
							  buf,
							  256);
	printf("%s%s\n",prefix,buf);
	X509_NAME_get_text_by_NID(name,
							  NID_commonName,
							  buf,
							  256);
	printf("%s%s\n",prefix,buf);
	X509_NAME_get_text_by_NID(name,
							  NID_pkcs9_emailAddress,
							  buf,
							  256);
	printf("%s%s\n",prefix,buf);
}

/////////////////////////////////////////////////////////////////////////////
void display_cert(X509* cert)
{
	X509_NAME*      name;
	int             unsigned i = 16;
	unsigned char   digest[16];

	cout << endl;

	/* print the issuer */
	printf("   issuer:\n");
	name = X509_get_issuer_name(cert);
	display_name("      ",name);

	/* print the subject */
	name = X509_get_subject_name(cert);
	printf("   subject:\n");
	display_name("      ",name);

	/* print the fingerprint */
	X509_digest(cert,EVP_md5(),digest,&i);
	printf("   fingerprint:\n");
	printf("      ");
	for (i=0;i<16;i++)
	{
		printf("%02X",digest[i]);
		if (i != 15)
		{
			printf(":");
		}
	}
	printf("\n");
}

//////////////////////////////////////////////////////////////////////////////
int ssl_verifycert_callback(X509* cert)
{
	static bool     bPrompted = false;

	if (!bPrompted)
	{
		X509   *pX509Cert = cert;

		bPrompted = true;

		cout << "The SSL connection received the following certificate:" << endl;

		display_cert(pX509Cert);

		cout << "\nDo you want to accept this+Certificate (Y/N)? ";
		String response = String::getLine(cin);
		if (response.compareToIgnoreCase("Y") != 0)
			return 0;
	}
	return 1;
}
#endif

//////////////////////////////////////////////////////////////////////////////



