/*******************************************************************************
* Copyright (C) 2001-3 Center 7, Inc All rights reserved.
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

#ifndef OW_SSLCtxMgr_HPP_INCLUDE_GUARD_
#define OW_SSLCtxMgr_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_SSLException.hpp"

#ifdef OW_HAVE_OPENSSL

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>


#define OW_SSLCTX_MAX_CN_LEN 256

/**
 * Verify a X509 certificate.
 * @param cert a pointer to the certificate to verify.
 * @return 1 if the certificate is good, 0 if the certificate is bad.
 * 	If 0 is returned, the SSL handshake will abort.
 */
typedef int (*certVerifyFuncPtr_t)(X509* cert, const OW_String& hostName);

class OW_SSLCtxMgr
{
public:

	/**
	 * The callback for getting a passphrase on a certificate.
	 */
	static int pem_passwd_cb(char* buf, int size, int rwflag, void *userData);


	/**
	 * Check a certificate based on the callback function for client cert
	 * verification. 
	 * @param ssl A pointer to the SSL context
	 * @param hostname the hostname of the client machine
	 * @return True if the certificate is good, false otherwise
	 */
	static OW_Bool checkClientCert(SSL* ssl, const OW_String& hostName);

	/**
	 * Check a certificate based on the callback function for server cert
	 * verification. 
	 * @param ssl A pointer to the SSL context
	 * @param hostname the hostname of the server machine
	 * @return True if the certificate is good, false otherwise
	 */
	static OW_Bool checkServerCert(SSL* ssl, const OW_String& hostName);

	/**
	 * Initialize for a client
	 * @param keyFile the path to the file containing the key
	 * @exception OW_SSLException
	 */
	static void initClient(const OW_String& keyFile = OW_String());

	/**
	 * Initialize for a server
	 * @param keyFile the path to the file containing the key
	 * @exception OW_SSLException
	 */
	static void initServer(const OW_String& keyFile);	

	/** 
	 * get the Server SSL Context
	 * @return the server SSL_CTX
	 */
	static SSL_CTX* getSSLCtxServer()
	{
		return m_ctxServer;
	}

	/**
	 * get the Client SSL Context
	 * @return the client SSL_CTX
	 */
	static SSL_CTX* getSSLCtxClient()
	{
		return m_ctxClient;
	}

	/**
	 * Read from a SSL connection
	 * @param ssl a pointer to the SSL Context for the connection
	 * @param buf a pointer to a buffer where data should be copied.
	 * @param len the number of bytes to read.
	 * @return the number of bytes read.
	 * @exception OW_SSLException
	 */
	static int sslRead(SSL* ssl, char* buf, int len); 

	/**
	 * Write to a SSL connection
	 * @param ssl a pointer to a SSL Context for the connection
	 * @param buf the buffer containing the data to write.
	 * @param len the number of bytes to write.
	 * @return the number of bytes written.
	 * @exception OW_SSLException
	 */
	static int sslWrite(SSL* ssl, const char* buf, int len);

	/**
	 * Have we been initialized as a client?
	 * @return true if initialized as a client
	 */
	static OW_Bool isClient() { return OW_Bool(m_ctxClient != NULL); }

	/**
	 * Have we been initialized as a server?
	 * @return true if initialized as a server
	 */
	static OW_Bool isServer() { return OW_Bool(m_ctxServer != NULL); }

	/**
	 * Assign a callback function to be used to verify SSL certificates.
	 * @param cbfunc the callback function.  Signature:
	 *		typedef void (*certVerifyFuncPtr_t)(X509* cert);
	 */
	static void setClientCertVerifyCallback(certVerifyFuncPtr_t cbfunc)
		{ m_clientCertVerifyCB = cbfunc; }

	/**
	 * Assign a callback function to be used to verify SSL certificates.
	 * @param cbfunc the callback function.  Signature:
	 *		typedef void (*certVerifyFuncPtr_t)(X509* cert);
	 */
	static void setServerCertVerifyCallback(certVerifyFuncPtr_t cbfunc)
		{ m_serverCertVerifyCB = cbfunc; }

private:
	static SSL_CTX* m_ctxClient;
	static SSL_CTX* m_ctxServer;
	static BIO* m_bio_err;
	static certVerifyFuncPtr_t m_clientCertVerifyCB;
	static certVerifyFuncPtr_t m_serverCertVerifyCB;

	static SSL_CTX* initCtx(const OW_String& keyfile) /*throw (OW_SSLException)*/;
	static void loadDHParams(SSL_CTX* ctx, const OW_String& file);
	/*throw (OW_SSLException)*/
	static void generateEphRSAKey(SSL_CTX* ctx); /*throw (OW_SSLException)*/
	static void uninit(); // set type to NOT_INIT and free memory.
	static void uninitServer();
	static void uninitClient();

	// don't allow instantiation
	OW_SSLCtxMgr();
	/** 
	 * This probably needs to say something useful.
	 */
	static OW_Bool checkCert(SSL* ssl, const OW_String& hostName, certVerifyFuncPtr_t cbFunc);
};

#endif // ifdef OW_HAVE_OPENSSL

#endif
