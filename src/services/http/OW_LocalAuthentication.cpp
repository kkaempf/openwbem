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
#include "OW_LocalAuthentication.hpp"
#include "OW_HTTPSvrConnection.hpp"
#include "OW_CryptographicRandomNumber.hpp"
#include "OW_UUID.hpp"
#include "OW_FileSystem.hpp"
#include "OW_Format.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_UserUtils.hpp"
#include "OW_AuthenticationException.hpp"
#include "OW_Exec.hpp"
#include "OW_LocalAuthenticationCommon.hpp"

#ifdef OW_HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef OW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef OW_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef OW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <cerrno>

namespace OW_NAMESPACE
{

using namespace LocalAuthenticationCommon;

namespace
{
	bool useHelper()
	{
		// only root can change file ownership, so unless we can, we'll have to rely on owlocalhelper to do it.
		return ::geteuid() != 0;
	}

	String runHelper(const String& inputCmd, const String& extraInput = String())
	{
		StringArray cmd;
		cmd.push_back(OWLOCALHELPER_BINARY);
		String output;
		int processStatus = -1;
		try
		{
			PopenStreams helper = Exec::safePopen(cmd); //, inputCmd + "\n" + extraInput);
			String input = inputCmd + "\n" + extraInput;
			
			// purposely ignore this return code
			helper.in()->write(input.c_str(), input.length());
			helper.in()->close();

			const int TIMEOUT = 10;
			const int OUTPUT_LIMIT = 1024;
			Exec::gatherOutput(output, helper, processStatus, TIMEOUT, OUTPUT_LIMIT);
			if (processStatus == -1)
			{
				processStatus = helper.getExitStatus();
			}
		}
		catch (Exception& e)
		{
			OW_THROW(LocalAuthenticationException, Format("Failed running %1: %2. command = %3, output = \"%4\"", OWLOCALHELPER_BINARY, e, inputCmd, output).c_str());
		}
		if (!WIFEXITED(processStatus) || WEXITSTATUS(processStatus) != 0)
		{
			OW_THROW(LocalAuthenticationException, Format("%1 failed with exit status %2. command = %3, output = \"%4\"", 
				OWLOCALHELPER_BINARY, processStatus, inputCmd, output).c_str());
		}
		return output;
	}

	void initializeHelper()
	{
		runHelper(INITIALIZE_CMD);
	}

	void cleanupEntryHelper(const String& pathToFile, const String& cookie)
	{
		size_t begin = pathToFile.lastIndexOf(OW_FILENAME_SEPARATOR);
		if (begin == String::npos)
		{
			begin = 0;
		}
		String fileName = pathToFile.substring(begin + 1);
		runHelper(REMOVE_CMD, fileName + "\n" + cookie + "\n");
	}

	String createFileHelper(const String& uid, const String& cookie)
	{
 		String filename = runHelper(CREATE_CMD, uid + "\n" + cookie + "\n");
 		// remove the trailing \n
 		if (filename.length() > 0 && filename[filename.length()-1] == '\n')
 		{
 			filename.erase(filename.length()-1);
 		}
 		return filename;

	}

}


//////////////////////////////////////////////////////////////////////////////
LocalAuthentication::LocalAuthentication(const LoggerRef& logger)
	: m_logger(logger)
{
	if (useHelper())
	{
		initializeHelper();
	}
	else
	{
		LocalAuthenticationCommon::initializeDir();
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
				OW_LOG_ERROR(m_logger, Format("LocalAuthentication::~LocalAuthentication() caught exception from cleanupEntry(): %1", e));
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

//////////////////////////////////////////////////////////////////////////////
void
generateNewCookieFile(const String& uid, String& cookieFileName, String& cookie)
{
	// Generate random number to put in file for client to read
	CryptographicRandomNumber rng;
	UInt32 rn1 = rng.getNextNumber();
	UInt32 rn2 = rng.getNextNumber();
	UInt32 rn3 = rng.getNextNumber();
	UInt32 rn4 = rng.getNextNumber();
	UInt32 rn5 = rng.getNextNumber();
	cookie = Format("%1%2%3%4%5", rn1, rn2, rn3, rn4, rn5);

	if (useHelper())
	{
		cookieFileName = createFileHelper(uid, cookie);
	}
	else
	{
		cookieFileName = LocalAuthenticationCommon::createFile(uid, cookie);
	}
}

} // end unnamed namespace

//////////////////////////////////////////////////////////////////////////////
bool
LocalAuthentication::authenticate(String& userName,
		const String& info, HTTPSvrConnection* htcon)
{
	try
	{

		cleanupStaleEntries();
	
		if (info.empty())
		{
			htcon->setErrorDetails("You must authenticate to access this resource");
			return false;
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
				return false;
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
				return false;
			}
	
			// give them back the challenge
			htcon->addHeader("WWW-Authenticate", createNewChallenge(uidStr, userName));
			htcon->setErrorDetails("You must authenticate to access this resource");
			return false;
		}
	
		// it's not an initial connection, so it's phase 2, look for the nonce and cookie
		iter = infoMap.find("nonce");
		if (iter == infoMap.end() || iter->second.empty())
		{
			htcon->setErrorDetails("No nonce was provided");
			return false;
		}
	
		String sNonce = iter->second;
	
		bool nonceFound = false;
		size_t i;
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
			return false;
		}
	
		userName = m_authEntries[i].userName;
	
		iter = infoMap.find("cookie");
		if (iter == infoMap.end() || iter->second.empty())
		{
			htcon->setErrorDetails("No cookie was provided");
			return false;
		}
		String cookie = iter->second;
		if ( cookie == m_authEntries[i].cookie )
		{
			// Match! Authenticated. Clean up.
			cleanupEntry(m_authEntries[i]);
			m_authEntries.erase(m_authEntries.begin() + i);
			return true;
		}
	
		htcon->setErrorDetails("invalid cookie");
		return false;
	}
	catch(LocalAuthenticationException& e)
	{
		OW_LOG_ERROR(m_logger, Format("LocalAuthentication::authenticate(): %1", e));
		htcon->setErrorDetails(Format("%1", e));
		return false;
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
	if (useHelper())
	{
		cleanupEntryHelper(entry.fileName, entry.cookie);
	}
	else
	{
		if (!FileSystem::removeFile(entry.fileName))
		{
			OW_LOG_ERROR(m_logger, Format("LocalAuthentication::cleanupEntry(): Failed to remove %1: %2", entry.fileName, errno));
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
		cleanupEntry(m_authEntries[0]);
		m_authEntries.erase(m_authEntries.begin());
	}
}

} // end namespace OW_NAMESPACE

