/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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
#include "OW_ThreadImpl.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_Assertion.hpp"
#include "OW_MD5.hpp"
#include "OW_Array.hpp"
#include <openssl/rand.h>
#include <openssl/err.h>
#include <string.h>


#ifdef OW_DEBUG
#include <iostream>
#endif

#include <fstream>

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
	return (unsigned long)OpenWBEM::ThreadImpl::thread_t_ToUInt64(OpenWBEM::ThreadImpl::currentThread());
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
#if 0
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
#endif
	SSL_CTX* ctx = SSL_CTX_new(SSLv23_method());
	SSL_CTX_set_default_passwd_cb(ctx, pem_passwd_cb);
	if (!keyfile.empty())
	{
		if (SSL_CTX_use_certificate_chain_file(ctx, keyfile.c_str()) != 1)
		{
			OW_THROW(SSLException, Format("Couldn't read certificate file: %1",
				keyfile).c_str());
		}
		if (SSL_CTX_use_PrivateKey_file(ctx, keyfile.c_str(), SSL_FILETYPE_PEM) != 1)
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
class SSLGlobalWork
{
public:
	SSLGlobalWork()
	{
		if (!mutex_buf)
		{
			mutex_buf = new Mutex[CRYPTO_num_locks()];
		}
	//if (!m_bio_err)
	//{
		SSL_library_init();
		SSL_load_error_strings();
		//m_bio_err = BIO_new_fp(stderr, BIO_NOCLOSE); // TODO What the heck is this?
	//}
		CRYPTO_set_id_callback(id_function);
		CRYPTO_set_locking_callback(locking_function);
	  /* The following three CRYPTO_... functions are the OpenSSL functions
		 for registering the callbacks we implemented above */
		CRYPTO_set_dynlock_create_callback(dyn_create_function);
		CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
		CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);
	}

	~SSLGlobalWork()
	{
		if (SSLCtxMgr::isClient() || SSLCtxMgr::isServer())
		{
			char randFile[MAXPATHLEN];
			const char* rval = RAND_file_name(randFile, MAXPATHLEN);
			if (rval)
			{
				RAND_write_file(randFile);
			}
		}
		SSLCtxMgr::uninit();
		delete[] mutex_buf;
		mutex_buf = 0;
	}
private:
};
// This is kind of a hack to get the random file to be written at
// the time of shutdown.
static SSLGlobalWork g_sslGLobalWork;
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::loadDHParams(SSL_CTX* ctx, const String& file)
{
	DH* ret = 0;
	BIO* bio = BIO_new_file(file.c_str(),"r");
	if (bio == NULL)
	{
		OW_THROW(SSLException, "Couldn't open DH file");
	}
	ret = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
	BIO_free(bio);
	if (SSL_CTX_set_tmp_dh(ctx, ret) < 0)
	{
		OW_THROW(SSLException, "Couldn't set DH parameters");
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
	/*
	if (!SSL_CTX_get_certificate(m_ctxClient))
	{
		std::cerr << "******** SSL_CTX has no cert!" << std::endl;
	}
	else
	{
		std::cerr << "******* SSL_CTX cert hash: " << std::hex << X509_subject_name_hash(SSL_CTX_get_certificate(m_ctxClient)) << std:: endl;
	}
	*/

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
	/* TODO this isn't working.
	if (SSL_get_verify_result(ssl)!=X509_V_OK)
	{
		cout << "SSL_get_verify_results failed." << endl;
		return false;
	}
	*/
	/*Check the cert chain. The chain length
	  is automatically checked by OpenSSL when we
	  set the verify depth in the ctx */
	/*Check the common name*/
	X509 *peer = SSL_get_peer_certificate(ssl);
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
	int cc = SSL_ERROR_WANT_READ;
	int r, retries = 0;
	while(cc == SSL_ERROR_WANT_READ && retries < OW_SSL_RETRY_LIMIT)
	{
		r = SSL_read(ssl, buf, len);
		cc = SSL_get_error(ssl, r);
		retries++;
	}
	
	switch (cc)
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
	int r, cc, retries;
	int myLen = len;
	int offset = 0;
	while (myLen > 0)
	{
		cc = SSL_ERROR_WANT_WRITE;
		retries = 0;
		while(cc == SSL_ERROR_WANT_WRITE && retries < OW_SSL_RETRY_LIMIT)
		{
			r = SSL_write(ssl, buf + offset, myLen);
			cc = SSL_get_error(ssl, r);
			retries++;
		}

		if (cc == SSL_ERROR_NONE)
		{
			myLen -= r;
			offset += r;
		}
		else
		{
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
		BIO_free(m_bio_err);
		m_bio_err = NULL;
		
		// free up memory allocated in SSL_library_init()
		EVP_cleanup();
		// free up memory allocated in SSL_load_error_strings()
		ERR_free_strings();
	}
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::uninitClient()
{
	if (m_ctxClient)
	{
		SSL_CTX_free(m_ctxClient);
		m_ctxClient = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
SSLCtxMgr::uninitServer()
{
	if (m_ctxServer)
	{
		SSL_CTX_free(m_ctxServer);
		m_ctxServer = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////
extern "C"
{
int verify_callback(int ok, X509_STORE_CTX *store)
{
    char data[256];
    SSL* ssl =
        (SSL*) X509_STORE_CTX_get_ex_data(store,
                                          SSL_get_ex_data_X509_STORE_CTX_idx());
    OWSSLContext* owctx =
        (OWSSLContext*) SSL_get_ex_data(ssl, SSLServerCtx::SSL_DATA_INDEX);
    OW_ASSERT(owctx);

	/**
	 * This callback is called multiple times while processing a cert.
	 * Supposedly, it is called at each depth of the cert chain.
	 * However, even with a self-signed cert, it is called twice, both times
	 * at depth 0.  Since we will change the status from NOT_OK to OK, the
	 * next time through the preverify status will also be changed.  Therefore
	 * we need to make sure that we set the status on owctx to failed with
	 * any failure, and then that we don't inadvertantly change it to PASS
	 * on subsequent calls.
	 */
	if (!ok)
	{
		owctx->peerCertPassedVerify = OWSSLContext::VERIFY_FAIL;
	}
	else
	{
		// if the cert failed on a previous call, we don't want to change
		// the status.
		if (owctx->peerCertPassedVerify != OWSSLContext::VERIFY_FAIL)
		{
			owctx->peerCertPassedVerify = OWSSLContext::VERIFY_PASS;
		}
	}

#ifdef OW_DEBUG
    if (!ok)
    {
        X509 *cert = X509_STORE_CTX_get_current_cert(store);
        int  depth = X509_STORE_CTX_get_error_depth(store);
        int  err = X509_STORE_CTX_get_error(store);

        fprintf(stderr, "-Error with certificate at depth: %i\n", depth);
        X509_NAME_oneline(X509_get_issuer_name(cert), data, 256);
        fprintf(stderr, "  issuer   = %s\n", data);
        X509_NAME_oneline(X509_get_subject_name(cert), data, 256);
        fprintf(stderr, "  subject  = %s\n", data);
        fprintf(stderr, "  err %i:%s\n", err, X509_verify_cert_error_string(err));
    }
#endif

    return 1;
}
}
//////////////////////////////////////////////////////////////////////////////
SSLCtxBase::SSLCtxBase(const SSLOpts& opts)
	: m_ctx(0)
{
	m_ctx = SSL_CTX_new(SSLv23_method());
	SSL_CTX_set_default_passwd_cb(m_ctx, SSLCtxMgr::pem_passwd_cb); // TODO cb func moved elsewhere?
	if (!opts.keyfile.empty())
	{
		if (!(SSL_CTX_use_certificate_chain_file(m_ctx, opts.keyfile.c_str())))
		{
			OW_THROW(SSLException, Format("Couldn't read certificate file: %1",
				opts.keyfile).c_str());
		}
		if (!(SSL_CTX_use_PrivateKey_file(m_ctx, opts.keyfile.c_str(), SSL_FILETYPE_PEM)))
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
	//String dhfile = "certs/dh1024.pem"; // TODO = GlobalConfig.getSSLDHFile();
	//loadDHParams(m_ctx, dhfile);
	SSLCtxMgr::generateEphRSAKey(m_ctx); // TODO what the heck is this?
	String sessID("SSL_SESSION_");
	RandomNumber rn(0, 10000);
	sessID += String(static_cast<UInt32>(rn.getNextNumber()));
	int sessIDLen =
		(SSL_MAX_SSL_SESSION_ID_LENGTH < (sessID.length() * sizeof(char))) ?
		SSL_MAX_SSL_SESSION_ID_LENGTH : (sessID.length() * sizeof(char));
	SSL_CTX_set_session_id_context(m_ctx,
			reinterpret_cast<const unsigned char*>(sessID.c_str()), sessIDLen);


	if (opts.verifyMode != SSLOpts::MODE_DISABLED && !opts.trustStore.empty())
	{
		if (!OpenWBEM::FileSystem::exists(opts.trustStore) )
		{
			OW_THROW(SSLException, Format("Error loading truststore %1",
										  opts.trustStore).c_str());
		}
		if (SSL_CTX_load_verify_locations(m_ctx,0,opts.trustStore.c_str()) != 1)
		{
			OW_THROW(SSLException, Format("Error loading truststore %1",
										  opts.trustStore).c_str());
		}
	}
	/* TODO remove.
	if (SSL_CTX_set_default_verify_paths(m_ctx) != 1)
	{
		OW_THROW(SSLException, "Error loading default CA store(s)");
	}
	*/
	switch (opts.verifyMode)
	{
	case SSLOpts::MODE_DISABLED:
		SSL_CTX_set_verify(m_ctx,
			SSL_VERIFY_NONE, 0);
		break;
	case SSLOpts::MODE_REQUIRED:
		SSL_CTX_set_verify(m_ctx,
			SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);
		break;
	case SSLOpts::MODE_OPTIONAL:
	case SSLOpts::MODE_AUTOUPDATE:
		SSL_CTX_set_verify(m_ctx,
			SSL_VERIFY_PEER, verify_callback);
		break;
	default:
		OW_ASSERT("Bad option, shouldn't happen" == 0);
		break;
	}

	SSL_CTX_set_verify_depth(m_ctx, 4);

}

//////////////////////////////////////////////////////////////////////////////
SSLCtxBase::~SSLCtxBase()
{
}

//////////////////////////////////////////////////////////////////////////////
SSL_CTX*
SSLCtxBase::getSSLCtx() const
{
	return m_ctx;
}

SSLOpts::SSLOpts()
	: verifyMode(MODE_DISABLED)
{
}





//////////////////////////////////////////////////////////////////////////////
SSLServerCtx::SSLServerCtx(const SSLOpts& opts)
	: SSLCtxBase(opts)
{
}
//////////////////////////////////////////////////////////////////////////////
SSLClientCtx::SSLClientCtx(const SSLOpts& opts)
	: SSLCtxBase(opts)
{
}

static Mutex m_mapGuard;

//////////////////////////////////////////////////////////////////////////////
SSLTrustStore::SSLTrustStore(const String& storeLocation)
	: m_store(storeLocation)
{
	m_mapfile = m_store + "/map";
	if (FileSystem::exists(m_mapfile))
	{
		MutexLock mlock(m_mapGuard);
		readMap();
	}
}

//////////////////////////////////////////////////////////////////////////////
bool
SSLTrustStore::getUser(const String& certhash, String& user, String& uid)
{
	MutexLock mlock(m_mapGuard);
	Map<String, UserInfo>::const_iterator iter = m_map.find(certhash);
	if (iter == m_map.end())
	{
		return false;
	}
	user = iter->second.user;
	uid = iter->second.uid;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
void
SSLTrustStore::addCertificate(X509* cert, const String& user, const String& uid)
{
	static const int numtries = 1000;
	OW_ASSERT(cert);
	OStringStream ss;
	unsigned long hash = X509_subject_name_hash(cert);
	if (hash == 0)
	{
		OW_THROW(SSLException, "Unable to extract hash from certificate");
	}
	ss << std::hex << hash << std::ends;
	String filename = m_store + "/" + ss.toString() + ".";
	int i = 0;
	for (i = 0; i < numtries; ++i)
	{
		String temp = filename + String(i);
		if (FileSystem::exists(temp))
		{
			continue;
		}
		filename = temp;
		break;
	}
	if (i == numtries)
	{
		OW_THROW(SSLException, "Unable to find a valid filename to store cert");
	}
	FILE* fp = fopen(filename.c_str(), "w");
	if (!fp)
	{
		OW_THROW(SSLException, Format("Unable to open new cert file for writing: %1", filename).c_str());
	}
	// Undocumented function in OpenSSL.  We assume it returns 1 on success
	// like most OpenSSL funcs.
	if (PEM_write_X509(fp, cert) != 1)
	{
		OW_THROW(SSLException, Format("SSL error while writing certificate to: %1", filename).c_str());
	}
	fclose(fp);

	String digest = getCertMD5Fingerprint(cert);
	MutexLock mlock(m_mapGuard);
	UserInfo info;
	info.user = user;
	info.uid = uid;
	m_map[digest] = info;
	writeMap();
}

//////////////////////////////////////////////////////////////////////////////
String
SSLTrustStore::getCertMD5Fingerprint(X509* cert)
{
	unsigned char digest[16];
	unsigned int len = 16;
	X509_digest(cert, EVP_md5(), digest, &len);
	return MD5::convertBinToHex(digest);
}

//////////////////////////////////////////////////////////////////////////////
void
SSLTrustStore::writeMap()
{
	std::ofstream f(m_mapfile.c_str(), std::ios::out);
	if (!f)
	{
		OW_THROW(SSLException, Format("SSL error opening map file: %1",
									  m_mapfile).c_str());
	}
	for (Map<String, UserInfo>::const_iterator iter = m_map.begin();
		  iter != m_map.end(); ++iter)
	{
		f << iter->first << " " << iter->second.user
			<< " " << iter->second.uid << "\n";
	}
	f.close();
}

//////////////////////////////////////////////////////////////////////////////
void
SSLTrustStore::readMap()
{
	std::ifstream f(m_mapfile.c_str(), std::ios::in);
	if (!f)
	{
		OW_THROW(SSLException, Format("SSL error opening map file: %1",
									  m_mapfile).c_str());
	}
	int lineno = 0;
	while (f)
	{
		String line = String::getLine(f);
		if (!f)
		{
			break;
		}
		++lineno;
		StringArray toks = line.tokenize();
		if (toks.size() != 3 && toks.size() != 2)
		{
			OW_THROW(SSLException, Format("Error processing user map %1 at line %2",
										  m_mapfile, lineno).c_str());
		}
		UserInfo info;
		info.user = toks[1];
		if (toks.size() == 3)
		{
			info.uid = toks[2];
		}
		m_map.insert(std::make_pair(toks[0], info));
	}
#ifdef OW_DEBUG
	std::cerr << "cert<>user map initizialized with " << m_map.size() << " users" << std::endl;
#endif
	f.close();
}

//////////////////////////////////////////////////////////////////////////////

OWSSLContext::OWSSLContext()
    : peerCertPassedVerify(VERIFY_NONE)
{
}
//////////////////////////////////////////////////////////////////////////////
OWSSLContext::~OWSSLContext()
{
}


} // end namespace OpenWBEM

#endif // #ifdef OW_HAVE_OPENSSL

