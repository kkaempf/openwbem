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
#ifdef OW_HAVE_OPENSSL
#include "OW_SSLCtxMgr.hpp"
#include "OW_GetPass.hpp"
#include "OW_Format.hpp"
#include "OW_FileSystem.hpp"
#include "OW_RandomNumber.hpp"
#include "OW_Mutex.hpp"
#include "OW_ThreadImpl.hpp"
#include <openssl/rand.h>
#include <openssl/err.h>
#include <string.h>

static OpenWBEM::Mutex* mutex_buf = 0;
extern "C"
{
struct CRYPTO_dynlock_value
{
	OpenWBEM::Mutex mutex;
};
static struct CRYPTO_dynlock_value * dyn_create_function(const char *,int)
{
	return new CRYPTO_dynlock_value;
}
static void dyn_lock_function(int mode, struct CRYPTO_dynlock_value *l,
				  const char *, int)
{
	if (mode & CRYPTO_LOCK)
	{
		l->mutex.acquire();
	}
	else
	{
		l->mutex.release();
	}
}
static void dyn_destroy_function(struct CRYPTO_dynlock_value *l,
				 const char *, int)
{
	delete l;
}
static unsigned long id_function()
{
	return (unsigned long)OpenWBEM::ThreadImpl::currentThread();
}
static void locking_function(int mode, int n, const char*, int)
{
	if (mode & CRYPTO_LOCK)
	{
		mutex_buf[n].acquire();
	}
	else
	{
		mutex_buf[n].release();
	}
}
}

namespace OpenWBEM
{

BIO* SSLCtxMgr::m_bio_err = 0;
SSL_CTX* SSLCtxMgr::m_ctxClient = 0;
SSL_CTX* SSLCtxMgr::m_ctxServer = 0;
certVerifyFuncPtr_t SSLCtxMgr::m_clientCertVerifyCB = 0;
certVerifyFuncPtr_t SSLCtxMgr::m_serverCertVerifyCB = 0;
//////////////////////////////////////////////////////////////////////////////
SSL_CTX*
SSLCtxMgr::initCtx(const String& keyfile)
{
	if (!m_bio_err)
	{
		SSL_library_init();
		SSL_load_error_strings();
		m_bio_err = BIO_new_fp(stderr, BIO_NOCLOSE); // TODO fix this
	}
	if (!mutex_buf)
	{
		mutex_buf = new Mutex[CRYPTO_num_locks()];
	}
	CRYPTO_set_id_callback(id_function);
	CRYPTO_set_locking_callback(locking_function);
	  /* The following three CRYPTO_... functions are the OpenSSL functions
		 for registering the callbacks we implemented above */
	CRYPTO_set_dynlock_create_callback(dyn_create_function);
	CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
	CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);
	SSL_CTX* ctx = SSL_CTX_new(SSLv23_method());
	SSL_CTX_set_default_passwd_cb(ctx, pem_passwd_cb);
	if (!keyfile.empty())
	{
		if (!(SSL_CTX_use_certificate_chain_file(ctx, keyfile.c_str())))
		{
			OW_THROW(SSLException, Format("Couldn't read certificate file: %1",
				keyfile).c_str());
		}
		if (!(SSL_CTX_use_PrivateKey_file(ctx, keyfile.c_str(), SSL_FILETYPE_PEM)))
		{
			OW_THROW(SSLException, "Couldn't read key file");
		}
	}
	/*String CAFile = Environment::getConfigItem("SSL_CAFile");
	String CAPath;
	char* CAFileStr = NULL;
	char* CAPathStr = NULL;
	if (CAFile.length() > 0)
		CAFileStr = strdup(CAFile.c_str());
	if (CAPath.length() > 0)
		CAPathStr = const_cast<char*>(CAPath.c_str());
	if (!(SSL_CTX_load_verify_locations(ctx, CAFileStr, CAPathStr)))
		OW_THROW(SSLException, "Couldn't read CA list");
	free(CAFileStr);*/ // TODO do we need this?
	// TODO get random file from GlobalConfig??? or at least OS
	char randFile[MAXPATHLEN];
	const char* rval = RAND_file_name(randFile, MAXPATHLEN);
	if (!rval)
	{
		OW_THROW(SSLException, "Couldn't obtain random filename");
	}
	off_t fileSize = 0;
	if (!FileSystem::getFileSize(String(randFile), fileSize)
			|| fileSize < 1024)
	{
		FileSystem::initRandomFile(String(randFile));
	}
	if (!(RAND_load_file(randFile, 1024))) // TODO is this sufficient?
	{
		OW_THROW(SSLException, Format("Couldn't load randomness from %1, errno=%2(%3)", randFile, errno, strerror(errno)).c_str());
	}
	return ctx;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class SSLCtxDestroyer
{
public:
	~SSLCtxDestroyer()
	{
		if (SSLCtxMgr::isClient() || SSLCtxMgr::isServer())
		{
			char randFile[MAXPATHLEN];
			const char* rval;
			rval = RAND_file_name(randFile, MAXPATHLEN);
			if (rval)
			{
				RAND_write_file(randFile);
			}
		}
		SSLCtxMgr::uninit();
	}
};
// This is kind of a hack to get the random file to be written at
// the time of shutdown.
static SSLCtxDestroyer g_sslCtxDestroyer;
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::loadDHParams(SSL_CTX* ctx, const String& file)
{
	DH* ret = 0;
	BIO* bio;
	if ((bio = BIO_new_file(file.c_str(),"r")) == NULL)
	{
		OW_THROW(SSLException, "Couldn't open DH file");
	}
	ret = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
	BIO_free(bio);
	{
		if (SSL_CTX_set_tmp_dh(ctx, ret) < 0)
		{
			OW_THROW(SSLException, "Couldn't set DH parameters");
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::generateEphRSAKey(SSL_CTX* ctx)
{
	RSA* rsa;
	rsa = RSA_generate_key(512, RSA_F4, NULL, NULL);
	if (!SSL_CTX_set_tmp_rsa(ctx, rsa))
	{
		RSA_free(rsa);
		OW_THROW(SSLException,"Couldn't set RSA key");
	}
	RSA_free(rsa);
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::initClient(const String& keyfile)
{
	if (m_ctxClient)
	{
		uninitClient();
	}
	//String keyfile = Environment::getConfigItem(
	//		Environment::SSL_CERT_opt);
	m_ctxClient = initCtx(keyfile);
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::initServer(const String& keyfile)
{
	if (m_ctxServer)
	{
		uninitServer();
	}
	//String keyfile = Environment::getConfigItem(
	//		Environment::SSL_CERT_opt);
	m_ctxServer = initCtx(keyfile);
	//String dhfile = "certs/dh1024.pem"; // TODO = GlobalConfig.getSSLDHFile();
	//loadDHParams(m_ctx, dhfile);
	generateEphRSAKey(m_ctxServer);
	String sessID("SSL_SESSION_");
	RandomNumber rn(0, 10000);
	sessID += String(static_cast<UInt32>(rn.getNextNumber()));
	int sessIDLen =
		(SSL_MAX_SSL_SESSION_ID_LENGTH < (sessID.length() * sizeof(char))) ?
		SSL_MAX_SSL_SESSION_ID_LENGTH : (sessID.length() * sizeof(char));
	SSL_CTX_set_session_id_context(m_ctxServer,
			reinterpret_cast<const unsigned char*>(sessID.c_str()), sessIDLen);
	SSL_CTX_set_verify(m_ctxServer,
		SSL_VERIFY_PEER /*| SSL_VERIFY_FAIL_IF_NO_PEER_CERT*/, NULL);
	/*
	// grabbed this from volutionlwc.  may need to use it (Filename issue on Win32)
	if (!SSL_CTX_load_verify_locations(ctx,VOL_CONFIG_DIR"/cacerts/volution-authority.cacert",NULL))
	{
		fprintf(stderr, "Couldn't set load_verify_location\n");
		exit(1);
	}
	SSL_CTX_set_verify_depth(ctx,1);
	*/
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
SSLCtxMgr::pem_passwd_cb(char* buf, int size, int /*rwflag*/,
	void* /*userData*/)
{
	String passwd;
	// TODO String = GlobalConfig.getSSLCertificatePassword();
	
	if (passwd.empty())
	{
		passwd = GetPass::getPass(
			"Enter the password for the SSL certificate: ");
	}
	strncpy(buf, passwd.c_str(), passwd.length());
	buf[size - 1] = '\0';
	return(passwd.length());
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
SSLCtxMgr::checkClientCert(SSL* ssl, const String& hostName)
{
	return checkCert(ssl, hostName, m_clientCertVerifyCB);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
SSLCtxMgr::checkServerCert(SSL* ssl, const String& hostName)
{
	return checkCert(ssl, hostName, m_serverCertVerifyCB);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
SSLCtxMgr::checkCert(SSL* ssl, const String& hostName, 
	certVerifyFuncPtr_t certVerifyCB)
{
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
	if (certVerifyCB)
	{
		if (certVerifyCB(peer, hostName) == 0)
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
SSLCtxMgr::sslRead(SSL* ssl, char* buf, int len)
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
			//OW_THROW(SSLException, "SSL read problem");
			return -1;
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
int
SSLCtxMgr::sslWrite(SSL* ssl, const char* buf, int len)
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
				//OW_THROW(SSLException, "SSL write problem");
				return -1;
		}
	}
	return len;
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::uninit()
{
	uninitClient();
	uninitServer();
	if (m_bio_err)
	{
		free(m_bio_err);
		m_bio_err = NULL;
		
		// free up memory allocated in SSL_library_init()
		EVP_cleanup();
		// free up memory allocated in SSL_load_error_strings()
		ERR_free_strings();
	}
	delete[] mutex_buf;
	mutex_buf = 0;
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::uninitClient()
{
	if (m_ctxClient)
	{
		free(m_ctxClient);
		m_ctxClient = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::uninitServer()
{
	if (m_ctxServer)
	{
		free(m_ctxServer);
		m_ctxServer = NULL;
	}
}

} // end namespace OpenWBEM

#endif // #ifdef OW_HAVE_OPENSSL

