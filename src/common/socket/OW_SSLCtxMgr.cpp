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

#include "OW_config.h"
#include "OW_SSLCtxMgr.hpp"
#include "OW_GetPass.hpp"
#include "OW_Format.hpp"
#include "OW_FileSystem.hpp"
#include "OW_RandomNumber.hpp"

#ifdef OW_HAVE_OPENSSL

#include <openssl/rand.h>
#include <string.h>

BIO* OW_SSLCtxMgr::m_bio_err = 0;
SSL_CTX* OW_SSLCtxMgr::m_ctxClient = 0;
SSL_CTX* OW_SSLCtxMgr::m_ctxServer = 0;
certVerifyFuncPtr_t OW_SSLCtxMgr::m_certVerifyCB = 0;


//////////////////////////////////////////////////////////////////////////////
SSL_CTX*
OW_SSLCtxMgr::initCtx(const OW_String& keyfile)
	/*throw (OW_SSLException)*/
{
	if (!m_bio_err)
	{
		SSL_library_init();
		SSL_load_error_strings();
		m_bio_err = BIO_new_fp(stderr, BIO_NOCLOSE); // TODO fix this
	}

	SSL_CTX* ctx = SSL_CTX_new(SSLv23_method());


	SSL_CTX_set_default_passwd_cb(ctx, pem_passwd_cb);
	if (keyfile.length() > 0)
	{
		if (!(SSL_CTX_use_certificate_chain_file(ctx, keyfile.c_str())))
		{
			OW_THROW(OW_SSLException, format("Couldn't read certificate file: %1",
				keyfile).c_str());
		}
		if (!(SSL_CTX_use_PrivateKey_file(ctx, keyfile.c_str(), SSL_FILETYPE_PEM)))
		{
			OW_THROW(OW_SSLException, "Couldn't read key file");
		}
	}

	/*OW_String CAFile = OW_Environment::getConfigItem("SSL_CAFile");
	OW_String CAPath;
	char* CAFileStr = NULL;
	char* CAPathStr = NULL;
	if (CAFile.length() > 0)
		CAFileStr = strdup(CAFile.c_str());
	if (CAPath.length() > 0)
		CAPathStr = const_cast<char*>(CAPath.c_str());

	if (!(SSL_CTX_load_verify_locations(ctx, CAFileStr, CAPathStr)))
		OW_THROW(OW_SSLException, "Couldn't read CA list");
	free(CAFileStr);*/ // TODO do we need this?

	// TODO get random file from GlobalConfig??? or at least OW_OS

	char randFile[MAXPATHLEN];
	const char* rval = RAND_file_name(randFile, MAXPATHLEN);
	if (!rval)
	{
		OW_THROW(OW_SSLException, "Couldn't obtain random filename");
	}
	OW_UInt32 fileSize = 0;
	if (!OW_FileSystem::getFileSize(OW_String(randFile), fileSize)
			|| fileSize < 1024)
	{
		OW_FileSystem::initRandomFile(OW_String(randFile));
	}

	if (!(RAND_load_file(randFile, 1024))) // TODO is this sufficient?
	{
		OW_THROW(OW_SSLException, "Couldn't load randomness");
	}

	return ctx;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class OW_SSLCtxDestroyer
{
public:
	~OW_SSLCtxDestroyer()
	{
		if (OW_SSLCtxMgr::isClient() || OW_SSLCtxMgr::isServer())
		{
			char randFile[MAXPATHLEN];
			const char* rval;
			rval = RAND_file_name(randFile, MAXPATHLEN);
			if (rval)
			{
				RAND_write_file(randFile);
			}
		}
	}
};
// This is kind of a hack to get the random file to be written at
// the time of shutdown.
static OW_SSLCtxDestroyer g_sslCtxDestroyer;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void
OW_SSLCtxMgr::loadDHParams(SSL_CTX* ctx, const OW_String& file)
	/*throw (OW_SSLException)*/
{
	DH* ret = 0;
	BIO* bio;

	if ((bio = BIO_new_file(file.c_str(),"r")) == NULL)
	{
		OW_THROW(OW_SSLException, "Couldn't open DH file");
	}

	ret = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
	BIO_free(bio);
	{
		if (SSL_CTX_set_tmp_dh(ctx, ret) < 0)
			OW_THROW(OW_SSLException, "Couldn't set DH parameters");
	}
}


//////////////////////////////////////////////////////////////////////////////
void
OW_SSLCtxMgr::generateEphRSAKey(SSL_CTX* ctx)
	/*throw (OW_SSLException)*/
{
	RSA* rsa;
	rsa = RSA_generate_key(512, RSA_F4, NULL, NULL);
	if (!SSL_CTX_set_tmp_rsa(ctx, rsa))
	{
		RSA_free(rsa);
		OW_THROW(OW_SSLException,"Couldn't set RSA key");
	}
	RSA_free(rsa);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SSLCtxMgr::initClient(const OW_String& keyfile)
	/*throw (OW_SSLException)*/
{
	if (m_ctxClient)
	{
		uninitClient();
	}

	//OW_String keyfile = OW_Environment::getConfigItem(
	//		OW_Environment::SSL_CERT_opt);

	m_ctxClient = initCtx(keyfile);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_SSLCtxMgr::initServer(const OW_String& keyfile)
	/*throw (OW_SSLException)*/
{
	if (m_ctxServer)
	{
		uninitServer();
	}
	//OW_String keyfile = OW_Environment::getConfigItem(
	//		OW_Environment::SSL_CERT_opt);

	m_ctxServer = initCtx(keyfile);
	//OW_String dhfile = "certs/dh1024.pem"; // TODO = GlobalConfig.getSSLDHFile();
	//loadDHParams(m_ctx, dhfile);
	generateEphRSAKey(m_ctxServer);
	OW_String sessID("OW_SSL_SESSION_");
	OW_RandomNumber rn(0, 10000);
	sessID += OW_String((OW_UInt32)rn.getNextNumber());
	int sessIDLen =
		(SSL_MAX_SSL_SESSION_ID_LENGTH < (sessID.length() * sizeof(char))) ?
		SSL_MAX_SSL_SESSION_ID_LENGTH : (sessID.length() * sizeof(char));
	SSL_CTX_set_session_id_context(m_ctxServer,
			reinterpret_cast<const unsigned char*>(sessID.c_str()), sessIDLen);
}


//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_SSLCtxMgr::pem_passwd_cb(char* buf, int size, int /*rwflag*/,
	void* /*userData*/)
{
	OW_String passwd;
	// TODO OW_String = GlobalConfig.getSSLCertificatePassword();
	
	if (passwd.length() < 1)
	{
		passwd = OW_GetPass::getPass(
			"Enter the password for the SSL certificate: ");
	}

	strncpy(buf, passwd.c_str(), passwd.length());
	buf[size - 1] = '\0';
	return(passwd.length());
}


//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_Bool
OW_SSLCtxMgr::checkCertChain(SSL* ssl, const OW_String& hostName)
{
	(void)hostName; // need to use this?
	X509 *peer;

	/* TODO this isn't working.
	if(SSL_get_verify_result(ssl)!=X509_V_OK)
	{
		cout << "SSL_get_verify_results failed." << endl;
		return false;
	}
	*/

	/*Check the cert chain. The chain length
	  is automatically checked by OpenSSL when we
	  set the verify depth in the ctx */

	/*Check the common name*/
	peer=SSL_get_peer_certificate(ssl);

	if (m_certVerifyCB)
	{
		if (m_certVerifyCB(peer) == 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_SSLCtxMgr::sslRead(SSL* ssl, char* buf, int len) /*throw (OW_SSLException)*/
{
	int r;
	r = SSL_read(ssl, buf, len);
	switch (SSL_get_error(ssl, r))
	{
		case SSL_ERROR_NONE:
			return r;
		case SSL_ERROR_ZERO_RETURN:
			// TODO should this be an exception???
			return -1;
		default:
			//OW_THROW(OW_SSLException, "SSL read problem");
			return -1;
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_SSLCtxMgr::sslWrite(SSL* ssl, const char* buf, int len)
	/*throw (OW_SSLException)*/
{
	int r;
	int myLen = len;
	int offset = 0;
	while(myLen > 0)
	{
		r = SSL_write(ssl, buf + offset, myLen);
		switch(SSL_get_error(ssl, r))
		{
			case SSL_ERROR_NONE:
				myLen -= r;
				offset += r;
				break;
			default:
				//OW_THROW(OW_SSLException, "SSL write problem");
				return -1;
		}
	}
	return len;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SSLCtxMgr::uninit()
{
	uninitClient();
	uninitServer();
	if (m_bio_err)
	{
		free(m_bio_err);
		m_bio_err = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SSLCtxMgr::uninitClient()
{
	if (m_ctxClient)
	{
		free(m_ctxClient);
		m_ctxClient = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SSLCtxMgr::uninitServer()
{
	if (m_ctxServer)
	{
		free(m_ctxServer);
		m_ctxServer = NULL;
	}
}

#endif // #ifdef OW_HAVE_OPENSSL

