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
#include "OW_IPCClient.hpp"
#include "OW_Format.hpp"
#include "OW_BinIfcIO.hpp"
#include "OW_CIMException.hpp"
#include "OW_URL.hpp"
#include "OW_IOException.hpp"
#include "OW_Assertion.hpp"

//////////////////////////////////////////////////////////////////////////////
OW_IPCClient::OW_IPCClient(const OW_String &url)
	: OW_CIMProtocolIFC()
	, m_conn()
	, m_url(url)
	, m_authenticated(false)
{
	m_conn.connect();
}

//////////////////////////////////////////////////////////////////////////////
OW_IPCClient::~OW_IPCClient()
{
	try
	{
		OW_BinIfcIO::write(m_conn.getOutputStream(),
			(OW_Int32) OW_IPC_CLOSECONN);
		m_conn.disconnect();
	}
	catch(...)
	{
		// Ignore, or else we could seg-fault.
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Reference<std::iostream>
OW_IPCClient::beginRequest(const OW_String& methodName,
	const OW_String& nameSpace)
{
	(void)methodName; (void)nameSpace;
	authenticateIfNeeded();
	// create no-delete reference, since we can't copy the stream
	OW_Reference<std::iostream> rval(&m_conn.getIOStream(), true);
	OW_BinIfcIO::write(*rval, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_ASSERT(m_contentType.length() > 0);
	OW_BinIfcIO::write(*rval, m_contentType, OW_Bool(true));
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
istream&
OW_IPCClient::endRequest(OW_Reference<std::iostream> request, const OW_String& methodName,
			const OW_String& nameSpace)
{
	(void)methodName;
	(void)nameSpace;
	(void)request;
	return m_conn.getInputStream();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMFeatures
OW_IPCClient::getFeatures()
{
	authenticateIfNeeded();
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_FUNCTIONCALL, OW_Bool(true));
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_GETSVRFEATURES, OW_Bool(true));

	checkError();

	OW_CIMFeatures f;
	OW_Int32 val;

	std::istream& istrm = m_conn.getInputStream();
	OW_BinIfcIO::read(istrm, val, OW_Bool(true));
	f.cimProduct = (OW_CIMFeatures::OW_CIMProduct_t) val;
	f.extURL = OW_BinIfcIO::readString(istrm);
	f.supportedGroups = OW_BinIfcIO::readStringArray(istrm);
	f.supportsBatch = OW_BinIfcIO::readBool(istrm);
	f.supportedQueryLanguages = OW_BinIfcIO::readStringArray(istrm);
	f.validation = OW_BinIfcIO::readString(istrm);
	f.cimom = OW_BinIfcIO::readString(istrm);
	f.protocolVersion = OW_BinIfcIO::readString(istrm);

	return f;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_IPCClient::authenticate(const OW_String& userName,
	const OW_String& info, OW_String& details, OW_Bool throwIfFailure)
{
	std::ostream& ostrm = m_conn.getOutputStream();
	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_IPC_AUTHENTICATE, OW_Bool(true));
	OW_BinIfcIO::writeString(ostrm, userName);
	OW_BinIfcIO::writeString(ostrm, info);

	checkError();
	std::istream& istrm = m_conn.getInputStream();
	OW_Bool authenticated = OW_BinIfcIO::readBool(istrm);
	details = OW_BinIfcIO::readString(istrm);
	if(!authenticated && throwIfFailure)
	{
		OW_THROWCIMMSG(OW_CIMException::ACCESS_DENIED,
			format("Authentication failed: %1", details).c_str());
	}

	m_authenticated = true;
	return authenticated;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCClient::authenticateIfNeeded()
{
	if (!m_authenticated)
	{
		int trycount = 0;
		OW_URL url(m_url);
		while (trycount < 3)
		{
			OW_String details;
			if(!authenticate(url.username, url.password, details, false))
			{
				if (!m_loginCB)
				{
					OW_THROWCIMMSG(OW_CIMException::ACCESS_DENIED,
						format("Authentication failed: %1", details).c_str());
				}
				else
				{
					if (!m_loginCB->getCredentials(m_url, url.username, url.password))
					{
						OW_THROWCIMMSG(OW_CIMException::ACCESS_DENIED,
							"Authentication failed: Unable to get valid credentials");
					}
				}
			}
			else
			{
				// sucessfully authenticated
				m_authenticated = true;
				return;
			}
		}
		// couldn't authenticate after 3 tries
		OW_THROWCIMMSG(OW_CIMException::ACCESS_DENIED,
			"Authentication failed: Unable to get valid credentials");
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCClient::checkError()
{
	std::istream& istrm = m_conn.getInputStream();
	OW_Int32 rc;
	OW_BinIfcIO::read(istrm, rc, OW_Bool(true));
	if(rc != OW_BIN_OK)
	{
		switch(rc)
		{
			case OW_BIN_ERROR:
			{
				OW_String msg;
				OW_BinIfcIO::read(istrm, msg, OW_Bool(true));
				OW_THROW(OW_IOException, msg.c_str());
			}
			case OW_BIN_EXCEPTION:
			{
				OW_Int32 cimerrno;
				OW_String cimMsg;
				OW_BinIfcIO::read(istrm, cimerrno, OW_Bool(true));
				OW_BinIfcIO::read(istrm, cimMsg, OW_Bool(true));
				OW_THROWCIMMSG(cimerrno, cimMsg.c_str());
			}
			default:
				m_conn.disconnect();
				OW_THROW(OW_IOException,
					"Unexpected value received from server. disconnecting");
		}
	}
}




