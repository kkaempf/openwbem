/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ConfigOpts.hpp"
#include "OW_LocalAuthentication.hpp"
#include "OW_HTTPSvrConnection.hpp"
#include "blocxx/Logger.hpp"
#include "blocxx/SecureRand.hpp"
#include "blocxx/UUID.hpp"
#include "blocxx/FileSystem.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/AutoPtr.hpp"
#include "blocxx/UserUtils.hpp"
#include "OW_AuthenticationException.hpp"
#include "blocxx/Process.hpp"
#include "blocxx/Exec.hpp"
#include "OW_LocalAuthenticationCommon.hpp"
#include "blocxx/ThreadOnce.hpp"
#include "OW_PrivilegeManager.hpp"
#include "blocxx/Assertion.hpp"
#include "blocxx/Secure.hpp"

#include <cerrno>

namespace OW_NAMESPACE
{

using namespace LocalAuthenticationCommon;
using namespace blocxx;

/// @todo  For efficiency's sake get rid of owlocalhelper and use the monitor do perform all the necessary work.

namespace
{
	String COMPONENT_NAME("ow.LocalAuthentication");

	bool g_useHelper = false;
	OnceFlag g_useHelperOnceFlag = BLOCXX_ONCE_INIT;
	void initUseHelper()
	{
		// only root can change file ownership, so unless we can, we'll have to rely on the privilege manager/owlocalhelper to do it.
		g_useHelper = ::geteuid() != 0 || !PrivilegeManager::getPrivilegeManager().isNull();
	}

	bool useHelper()
	{
		callOnce(g_useHelperOnceFlag, initUseHelper);
		return g_useHelper;
	}

} // end anonymous namespace


//////////////////////////////////////////////////////////////////////////////
LocalAuthentication::LocalAuthentication()
	: m_logger(COMPONENT_NAME)
	, m_localAuthDir(ConfigOpts::installed_owdata_dir + "/OWLocal")
	, m_localHelperBinPath(ConfigOpts::installed_owlibexec_dir + "/owlocalhelper" + String(OW_OPENWBEM_LIBRARY_VERSION))
{
	if (useHelper())
	{
		initializeHelper();
		m_localAuthDir = FileSystem::Path::realPath(m_localAuthDir);
	}
	else
	{
		m_localAuthDir = LocalAuthenticationCommon::initializeDir(m_localAuthDir);
	}
}

//////////////////////////////////////////////////////////////////////////////
LocalAuthentication::~LocalAuthentication()
{
	for (size_t i = 0; i < m_authEntries.size(); ++i)
	{
		try
		{
			cleanupEntry(m_authEntries[i]);
		}
		catch (Exception& e)
		{
			try
			{
				BLOCXX_LOG_ERROR(m_logger, Format("LocalAuthentication::~LocalAuthentication() caught exception from cleanupEntry(): %1", e));
			}
			catch (...)
			{
			}
		}
		catch (...)
		{
			// eat all exceptions
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
namespace {

//////////////////////////////////////////////////////////////////////////////
String
generateNewNonce()
{
	UUID u;
	return u.toString();
}

void
parseInfo(const String& pinfo, SortedVectorMap<String, String>& infoMap)
{
	size_t idx = pinfo.indexOf("OWLocal");
	String info;
	if (idx != String::npos)
	{
		info = pinfo.substring(8);
	}
	else
	{
		OW_THROW(AuthenticationException, "Error parsing OWLocal Response");
	}
	Array<String> infoAr = info.tokenize(",");
	for (size_t i = 0; i < infoAr.size(); ++i)
	{
		String lhs, rhs;
		idx = infoAr[i].indexOf('=');
		if (idx != String::npos)
		{
			lhs = infoAr[i].substring(0, idx);
			lhs.trim();
			if (idx + 1 < infoAr[i].length())
			{
				rhs = infoAr[i].substring(idx + 1);
				rhs.trim();
				if (rhs[0] == '\"')
				{
					rhs = rhs.substring(1);
					rhs = rhs.substring(0, rhs.length() - 1);
				}
				infoMap.insert(std::make_pair(lhs, rhs));
				continue;
			}
		}
		OW_THROW(AuthenticationException, "Error parsing OWLocal Response");
	}
}


} // end unnamed namespace

//////////////////////////////////////////////////////////////////////////////
EAuthenticateResult
LocalAuthentication::authenticate(String& userName,
		const String& info, HTTPSvrConnection* htcon)
{
	try
	{

		cleanupStaleEntries();

		BLOCXX_ASSERT(!info.empty());

		BLOCXX_LOG_DEBUG3(m_logger, Format("LocalAuthentication::authenticate() called for user \"%1\"", userName));

		if (info.empty())
		{
			// shouldn't ever happen, but just in case.
			htcon->setErrorDetails("You must authenticate to access this resource");
			return E_AUTHENTICATE_CONTINUE;
		}

		typedef SortedVectorMap<String, String> map_t;
		map_t infoMap;

		parseInfo(info, infoMap);

		// look for an initial connection where the client specifies their uid
		map_t::const_iterator iter = infoMap.find("uid");
		if (iter != infoMap.end() && !iter->second.empty())
		{
			String uidStr = iter->second;

			// Lookup the username given the uid
			uid_t uid;
			try
			{
				uid = uidStr.toUInt32();
			}
			catch (StringConversionException& e)
			{
				htcon->setErrorDetails("Invalid uid");
				return E_AUTHENTICATE_FAIL;
			}

			bool ok;
			String uname(UserUtils::getUserName(uid, ok));
			if (ok)
			{
				userName = uname;
			}
			else
			{
				htcon->setErrorDetails("Invalid uid");
				return E_AUTHENTICATE_FAIL;
			}

			// give them back the challenge
			htcon->addHeader("WWW-Authenticate", createNewChallenge(uidStr, userName));
			htcon->setErrorDetails("OWLocal:2");

			BLOCXX_LOG_DEBUG3(m_logger, Format("Have UID %1 (%2), asking to continue", uid, userName));

			return E_AUTHENTICATE_CONTINUE;
		}

		// it's not an initial connection, so it's phase 2, look for the nonce and cookie
		iter = infoMap.find("nonce");
		if (iter == infoMap.end() || iter->second.empty())
		{
			htcon->setErrorDetails("No nonce was provided");
			return E_AUTHENTICATE_FAIL;
		}

		String sNonce = iter->second;

		bool nonceFound = false;
		size_t i = 0;
		if (!sNonce.empty())
		{
			for (i = 0; i < m_authEntries.size(); ++i)
			{
				if (sNonce == m_authEntries[i].nonce)
				{
					nonceFound = true;
					break;
				}
			}
		}
		if (!nonceFound)
		{
			htcon->setErrorDetails("invalid nonce");
			return E_AUTHENTICATE_FAIL;
		}

		userName = m_authEntries[i].userName;

		iter = infoMap.find("cookie");
		if (iter == infoMap.end() || iter->second.empty())
		{
			htcon->setErrorDetails("No cookie was provided");
			return E_AUTHENTICATE_FAIL;
		}
		String cookie = iter->second;

		if ( cookie == m_authEntries[i].cookie )
		{
			BLOCXX_LOG_DEBUG3(m_logger, "Cookie matched.  Authentication successful.  Cleaning up.");
			// Match! Authenticated. Clean up.
			try
			{
				cleanupEntry(m_authEntries[i]);
			}
			catch (LocalAuthenticationException& e)
			{
				BLOCXX_LOG_ERROR(m_logger, Format("LocalAuthentication::authenticate() failed to clean up entry: %1", e));
			}
			m_authEntries.erase(m_authEntries.begin() + i);

			BLOCXX_LOG_DEBUG3(m_logger, "Successful authentication");
			return E_AUTHENTICATE_SUCCESS;
		}
		BLOCXX_LOG_DEBUG3(m_logger, "Invalid cookie supplied");


		htcon->setErrorDetails("invalid cookie");
		return E_AUTHENTICATE_FAIL;
	}
	catch(LocalAuthenticationException& e)
	{
		BLOCXX_LOG_ERROR(m_logger, Format("LocalAuthentication::authenticate(): %1", e));
		htcon->setErrorDetails(Format("%1", e));
		return E_AUTHENTICATE_FAIL;
	}
}
//////////////////////////////////////////////////////////////////////////////
String
LocalAuthentication::createNewChallenge(const String& uid, const String& userName)
{
	String nonce = generateNewNonce();
	String cookieFileName;
	String cookie;
	generateNewCookieFile(uid, cookieFileName, cookie);

	AuthEntry newEntry;
	newEntry.fileName = cookieFileName;
	newEntry.cookie = cookie;
	newEntry.nonce = nonce;
	newEntry.creationTime.setToCurrent();
	newEntry.userName = userName;
	m_authEntries.push_back(newEntry);

	return String("OWLocal nonce=\"" + nonce + "\", cookiefile=\"" + cookieFileName + "\"");
}

//////////////////////////////////////////////////////////////////////////////
void
LocalAuthentication::cleanupEntry(const AuthEntry& entry)
{
	BLOCXX_LOG_DEBUG3(m_logger, Format("LocalAuthentication::cleanupEntry(): cleaning up %1 for %2", entry.fileName, entry.userName));
	if (useHelper())
	{
		cleanupEntryHelper(entry.fileName, entry.cookie);
	}
	else
	{
		if (!FileSystem::removeFile(entry.fileName))
		{
			BLOCXX_LOG_ERROR(m_logger, Format("LocalAuthentication::cleanupEntry(): Failed to remove %1: %2", entry.fileName, errno));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
LocalAuthentication::cleanupStaleEntries()
{
	DateTime oneMinuteAgo;
	oneMinuteAgo.setToCurrent();
	oneMinuteAgo.addMinutes(-1);

	// beginning of m_authEntries are the oldest, so we'll just keep erasing the
	// entries from the front as long as they're older than 1 minute.
	while (m_authEntries.size() > 0 && m_authEntries[0].creationTime < oneMinuteAgo)
	{
		try
		{
			cleanupEntry(m_authEntries[0]);
		}
		catch (LocalAuthenticationException& e)
		{
			BLOCXX_LOG_ERROR(m_logger, Format("LocalAuthentication::cleanupStaleEntries() failed to remove entry: %1 for %2: %3",
				m_authEntries[0].fileName, m_authEntries[0].userName, e));
		}
		m_authEntries.erase(m_authEntries.begin());
	}
}

//////////////////////////////////////////////////////////////////////////////
void
LocalAuthentication::checkProcess()
{
	if (m_owlocalhelper && m_owlocalhelper->processStatus().running())
	{
		return;
	}

	if (m_owlocalhelper)
	{
		// must have died
		BLOCXX_LOG_ERROR(m_logger, Format("LocalAuthentication detected that \"%1\" is not running. Status: %2",
			m_localHelperBinPath, m_owlocalhelper->processStatus().toString()));
		m_owlocalhelper->waitCloseTerm(0.00, 0.01, 0.02);
		m_owlocalhelper = 0;
	}

	PrivilegeManager privMan(PrivilegeManager::getPrivilegeManager());
	BLOCXX_ASSERT(!privMan.isNull());

	#ifdef OW_WIN32
	static const char* const SUPERUSER = "SYSTEM";
	#else
	static const char* const SUPERUSER = "root";
	#endif

	StringArray cmd;
	cmd.push_back(m_localHelperBinPath);
	cmd.push_back(m_localAuthDir);
	m_owlocalhelper = privMan.userSpawn(m_localHelperBinPath, cmd, Secure::minimalEnvironment(), SUPERUSER);

	if (!m_owlocalhelper->processStatus().running())
	{
		String msg = Format("LocalAuthentication failed to start %1. status = %2, stderr = %3",
			m_localHelperBinPath, m_owlocalhelper->processStatus().toString(), m_owlocalhelper->err()->readAll());
		BLOCXX_LOG_ERROR(m_logger, msg);
		OW_THROW(LocalAuthenticationException, msg.c_str());
	}

	Timeout to(Timeout::infinite);
	m_owlocalhelper->in()->setTimeouts(to);
	m_owlocalhelper->out()->setTimeouts(to);
	m_owlocalhelper->err()->setTimeouts(to);
}

//////////////////////////////////////////////////////////////////////////////
String
LocalAuthentication::processHelperCommand(const String& inputCmd, const String& extraInput)
{
	checkProcess();
	String output;
	try
	{
		IOIFCStreamBuffer inbuf(m_owlocalhelper->out().getPtr());
		IOIFCStreamBuffer outbuf(m_owlocalhelper->in().getPtr());
		std::istream istr(&inbuf);
		std::ostream ostr(&outbuf);
		istr.tie(&ostr);
		ostr << inputCmd << '\n';
		ostr << extraInput << std::flush;
		BLOCXX_LOG_DEBUG3(m_logger, Format("LocalAuthentication::processHelperCommand() got request, sending to helper: \"%1\"", inputCmd));
		String result = String::getLine(istr);
		BLOCXX_LOG_DEBUG3(m_logger, Format("LocalAuthentication::processHelperCommand() got response: \"%1\"", result));
		if (result == "S")
		{
			output = String::getLine(istr);
			BLOCXX_LOG_DEBUG3(m_logger, Format("LocalAuthentication::processHelperCommand() got success. output: \"%1\"", output));
		}
		else if (result == "F")
		{
			String details = String::getLine(istr);
			details.trim();
			BLOCXX_LOG_ERROR(m_logger, Format("LocalAuthentication::processHelperCommand() got failure. details: \"%1\"", details));
			size_t idx = details.indexOf(' ');
			if (idx != String::npos)
			{
				Int32 errCode = -1;
				try
				{
					errCode = details.substring(0, idx).toInt32();
				}
				catch (StringConversionException&)
				{
				}
				OW_THROW_ERR(LocalAuthenticationException, details.substring(idx + 1).c_str(), errCode);
			}
			else
			{
				OW_THROW(LocalAuthenticationException, details.c_str());
			}
		}
		else
		{
			// something is messed up, just kill the process!
			BLOCXX_LOG_ERROR(m_logger, Format("LocalAuthentication::processHelperCommand() got unexpected output: \"%1\"", result));
			m_owlocalhelper->waitCloseTerm(0.00, 0.01, 0.02);
			m_owlocalhelper = 0;
			OW_THROW(LocalAuthenticationException, Format("LocalAuthentication::processHelperCommand() got unexpected output: \"%1\"", result).c_str());
		}
	}
	catch (LocalAuthenticationException&)
	{
		throw;
	}
	catch (Exception& e)
	{
		m_owlocalhelper->waitCloseTerm(0.00, 0.01, 0.02);
		m_owlocalhelper = 0;
		OW_THROW_SUBEX(LocalAuthenticationException, Format("Failed running %1. command = %2, output = \"%3\"",
			m_localHelperBinPath, inputCmd, output).c_str(), e);
	}
	return output;
}

//////////////////////////////////////////////////////////////////////////////
void
LocalAuthentication::initializeHelper()
{
	processHelperCommand(INITIALIZE_CMD);
}

//////////////////////////////////////////////////////////////////////////////
void
LocalAuthentication::cleanupEntryHelper(const String& pathToFile, const String& cookie)
{
	size_t begin = pathToFile.lastIndexOf(OW_FILENAME_SEPARATOR);
	if (begin == String::npos)
	{
		begin = 0;
	}
	String fileName = pathToFile.substring(begin + 1);
	Logger logger(COMPONENT_NAME);
	BLOCXX_LOG_DEBUG3(logger, Format("cleanupEntryHelper: pathToFile = %1, fileName = %2", pathToFile, fileName));
	processHelperCommand(REMOVE_CMD, fileName + "\n" + cookie + "\n");
}

//////////////////////////////////////////////////////////////////////////////
String
LocalAuthentication::createFileHelper(const String& uid, const String& cookie)
{
	String extra_input = uid + "\n" + cookie + "\n";
	String filename = processHelperCommand(CREATE_CMD, extra_input);
	// remove the trailing \n
	if (filename.length() > 0 && filename[filename.length()-1] == '\n')
	{
		filename.erase(filename.length()-1);
	}
	if (filename.empty())
	{
		OW_THROW(LocalAuthenticationException,
			"createFileHelper: got back empty filename from owlocalhelper!");
	}
	return filename;
}

//////////////////////////////////////////////////////////////////////////////
void
LocalAuthentication::generateNewCookieFile(const String& uid, String& cookieFileName, String& cookie)
{
	// Generate random number to put in file for client to read
	UInt32 rn1 = Secure::rand_uint<UInt32>();
	UInt32 rn2 = Secure::rand_uint<UInt32>();
	UInt32 rn3 = Secure::rand_uint<UInt32>();
	UInt32 rn4 = Secure::rand_uint<UInt32>();
	UInt32 rn5 = Secure::rand_uint<UInt32>();
	cookie = Format("%1-%2-%3-%4-%5", rn1, rn2, rn3, rn4, rn5);

	if (useHelper())
	{
		cookieFileName = createFileHelper(uid, cookie);
	}
	else
	{
		cookieFileName = LocalAuthenticationCommon::createFile(m_localAuthDir, uid, cookie);
	}
}

} // end namespace OW_NAMESPACE

