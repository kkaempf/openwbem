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
#include "OW_config.h"
#include "OW_HTTPClient.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_SocketBaseImpl.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMScope.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_Format.hpp"
#include "OW_GetPass.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMClass.hpp"

#include <iostream>


using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using namespace OpenWBEM;

#define TEST_ASSERT(CON) if(!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)
//////////////////////////////////////////////////////////////////////////////

void
usage(const char* name)
{
	cerr << "Usage: " << name << " <url> [dump file extension]" << endl;
}

//////////////////////////////////////////////////////////////////////////////

/****************************************************************************
 * This is the class that will be used to obtain authentication credentials
 * if none are provided in the URL used by the HTTP Client.
 ****************************************************************************/
class GetLoginInfo : public ClientAuthCBIFC
{
	public:
		bool getCredentials(const String& realm, String& name,
				String& passwd, const String& details)
		{
			(void)details;
			cout << "Authentication required for " << realm << endl;
			cout << "Enter the user name: ";
			name = String::getLine(cin);
			passwd = GetPass::getPass("Enter the password for " +
				name + ": ");
			return true;
		}
};

/****************************************************************************
 * This is the prototype for our SSL certificate verification function.
 * This function is called during the client connect of the SSL handshake.
 * It should return 1 if the certificate is to be accepted, and 0 if it
 * is to rejected (and the connection should not be established).
 * This function could check the certificate against a list of accepted
 * Certificate Authorities or something.  Ours will simply display
 * the certificate and ask the user if he/she wishes to accept it.
 ****************************************************************************/
#ifdef OW_HAVE_OPENSSL
int ssl_verifycert_callback(X509* cert);
#endif

//////////////////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			usage(argv[0]);
			exit(1);
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
		URL owurl(url);

		/**********************************************************************
		 * We assign our SSL certificate callback into the SSLCtxMgr.
		 * If we don't do this, we'll accept any server certificate without
		 * any verification.  We leave this commented out here, so our
		 * acceptance test will run without user interaction.
		 **********************************************************************/

#ifdef OW_HAVE_OPENSSL
		//SSLCtxMgr::setCertVerifyCallback(ssl_verifycert_callback);
#endif
		
		
		/**********************************************************************
		 * Here we create the concrete CIMProtocol that we want
		 * our CIMXMLCIMOMHandle to use.  We'll use the HTTPClient
		 * (capable of handling HTTP/1.1 and HTTPS -- HTTP over SSL).
		 * The HTTPClient takes a URL in it's constructor, representing
		 * the CIM Server that it will connect to.  A URL has the form
		 *   http[s]://[USER:PASSWORD@]HOSTNAME[:PORT][/PATH].
		 *
		 * Example:  https://bill:secret@managedhost.example.com/cimom
		 *
		 * If no port is given, the defaults are used: 5988 for HTTP,
		 * and 5989 for HTTPS.  If no username and password are given,
		 * and the CIM Server requires authentication, a callback may
		 * be provided to retrieve authentication credentials.
		 **********************************************************************/

		CIMProtocolIFCRef client;
		client = new HTTPClient(url);


		/**********************************************************************
		 * Create an instance of our authentication callback class.
		 **********************************************************************/
		
		ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);

		/**********************************************************************
		 * Assign our callback to the HTTP Client.
		 **********************************************************************/

		client->setLoginCallBack(getLoginInfo);

		/**********************************************************************
		 * Here we create a CIMXMLCIMOMHandle and have it use the
		 * HTTPClient we've created.  CIMXMLCIMOMHandle takes
		 * a Reference<CIMProtocol> it it's constructor, so
		 * we have to make a Reference out of our HTTP Client first.
		 * By doing this, we don't have to worry about deleting our
		 * HTTPClient.  Reference will delete it for us when the
		 * last copy goes out of scope (reference count goes to zero).
		 **********************************************************************/

		CIMOMHandleIFCRef chRef;
		if (owurl.path.equalsIgnoreCase("owbinary"))
		{
			chRef = new BinaryCIMOMHandle(client);
		}
		else
		{
			chRef = new CIMXMLCIMOMHandle(client);
		}

		CIMOMHandleIFC& rch = *chRef;

		/**********************************************************************
		 * Now we have essentially established a "connection" to the CIM
		 * Server.  We can access the methods on the remote CIMOM handle,
		 * and these methods will call into the CIM Server.  The HTTPClient
		 * will take care of the particulars of the HTTP protocol, including
		 * authentication, compression, SSL, chunking, etc.
		 **********************************************************************/


		cout << "** Enumerating instances (0 instances)" << endl;
		CIMObjectPathEnumeration copEnu = rch.enumInstanceNamesE("root", "TestInstance");
		TEST_ASSERT(copEnu.numberOfElements() == 0);

		cout << "** Getting class" << endl;
		CIMClass cc = rch.getClass("root", "TestInstance");

		cout << "** Creating instance one" << endl;
		CIMInstance inst = cc.newInstance();
		inst.setProperty("Name", CIMValue(String("One")));
		StringArray sa;
		sa.push_back(String("One"));
		sa.push_back(String("Two"));
#if 1
		inst.setProperty("Params", CIMValue(sa));
#else
		inst.setProperty(CIMProperty(String("Params"), CIMValue(sa)));
#endif
		CIMObjectPath ccop("TestInstance", "root");
		CIMObjectPath icop = ccop;
		icop.setKeyValue("Name", CIMValue(String("One")));
		rch.createInstance("root", inst);

		cout << "** Enumerating instances (1 instance)" << endl;
		copEnu = rch.enumInstanceNamesE("root", "TestInstance");
		TEST_ASSERT(copEnu.numberOfElements() == 1);

		cout << "** Getting Instance" << endl;
		inst = rch.getInstance(icop.getNameSpace(), icop);
		sa.clear();
		cout << "** Checking array property on instance" << endl;
		inst.getProperty("Params").getValue().get(sa);
		TEST_ASSERT(sa.size() == 2);
		TEST_ASSERT(sa[0] == "One");
		TEST_ASSERT(sa[1] == "Two");

		cout << "** Modifying array property on instance" << endl;
		sa.push_back(String("Three"));

#if 1
		inst.setProperty("Params", CIMValue(sa));
#else
		inst.setProperty(CIMProperty(String("Params"), CIMValue(sa)));
#endif

		rch.modifyInstance("root", inst);

		cout << "** Getting new instance" << endl;
		inst = rch.getInstance(icop.getNameSpace(), icop);

		cout << "** Checking array property on new instance" << endl;
		sa.clear();
		inst.getProperty("Params").getValue().get(sa);
		TEST_ASSERT(sa.size() == 3);
		TEST_ASSERT(sa[0] == "One");
		TEST_ASSERT(sa[1] == "Two");
		TEST_ASSERT(sa[2] == "Three");

		return 0;
	}
	catch (AssertionException& a)
	{
		cerr << "Caught Assertion: " << a << endl;
	}
	catch (Exception& e)
	{
		cerr << e << endl;
	}
	return 1;
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
	for(i=0;i<16;i++)
	{
		printf("%02X",digest[i]);
		if(i != 15)
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

	if(!bPrompted)
	{
		X509   *pX509Cert = cert;

		bPrompted = true;

		cout << "The SSL connection received the following certificate:" << endl;

		display_cert(pX509Cert);

		cout << "\nDo you want to accept this+Certificate (Y/N)? ";
		String response = String::getLine(cin);
		if(response.compareToIgnoreCase("Y") != 0)
			return 0;
	}
	return 1;
}

#endif
