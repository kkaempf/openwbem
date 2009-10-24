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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CIMUrl.hpp"
#include "blocxx/String.hpp"
#include "blocxx/StrictWeakOrdering.hpp"
#include "blocxx/Bool.hpp"
#include "blocxx/COWIntrusiveCountableBase.hpp"
#include "blocxx/StringBuffer.hpp"

namespace OW_NAMESPACE
{

using std::streambuf;

struct CIMUrl::URLData : public COWIntrusiveCountableBase
{
	URLData() :
		m_port(0),
		m_localHost(true) {}
	virtual ~URLData();
	String m_spec;
	String m_protocol;
	String m_host;
	Int32 m_port;
	String m_file;
	String m_ref;
	Bool m_localHost;
	URLData* clone() { return new URLData(*this); }
};
//////////////////////////////////////////////////////////////////////////////
CIMUrl::URLData::~URLData()
{
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMUrl::URLData& x, const CIMUrl::URLData& y)
{
	return x.m_spec < y.m_spec;
}
//////////////////////////////////////////////////////////////////////////////
// PUBLIC
CIMUrl::CIMUrl() :
	CIMBase(), m_pdata(new URLData)
{
	setDefaultValues();
}
//////////////////////////////////////////////////////////////////////////////
// PUBLIC
CIMUrl::CIMUrl(CIMNULL_t) :
	CIMBase(), m_pdata(0)
{
}
//////////////////////////////////////////////////////////////////////////////
// PUBLIC
CIMUrl::CIMUrl(const String& spec) :
	CIMBase(), m_pdata(new URLData)
{
	m_pdata->m_spec = spec;
	setComponents();
}
//////////////////////////////////////////////////////////////////////////////
// PUBLIC
CIMUrl::CIMUrl(const String& protocol, const String& host,
	const String& file, Int32 port) :
	CIMBase(), m_pdata(new URLData)
{
	m_pdata->m_protocol = protocol;
	m_pdata->m_host = host;
	m_pdata->m_port = port;
	m_pdata->m_file = file;
	setDefaultValues();
}
//////////////////////////////////////////////////////////////////////////////
// PUBLIC
CIMUrl::CIMUrl(const CIMUrl& context, const String& spec) :
	CIMBase(), m_pdata(new URLData)
{
	m_pdata->m_spec = spec;
	setComponents();
	if (m_pdata->m_protocol.empty())
	{
		m_pdata->m_protocol = context.getProtocol();
	}
	else
	{
		if (m_pdata->m_protocol == context.m_pdata->m_protocol)
		{
			m_pdata->m_host = context.m_pdata->m_host;
			m_pdata->m_port = context.m_pdata->m_port;
			m_pdata->m_file = context.m_pdata->m_file;
		}
	}
	setDefaultValues();
}
//////////////////////////////////////////////////////////////////////////////
CIMUrl::CIMUrl(const CIMUrl& x)
	: CIMBase() , m_pdata(x.m_pdata)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMUrl::~CIMUrl()
{
}
//////////////////////////////////////////////////////////////////////////////
CIMUrl&
CIMUrl::operator= (const CIMUrl& x)
{
	m_pdata = x.m_pdata;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMUrl::setNull()
{
	m_pdata = NULL;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMUrl::getSpec() const {  return m_pdata->m_spec; }
//////////////////////////////////////////////////////////////////////////////
String
CIMUrl::getProtocol() const {  return m_pdata->m_protocol; }
//////////////////////////////////////////////////////////////////////////////
String
CIMUrl::getHost() const {  return m_pdata->m_host; }
//////////////////////////////////////////////////////////////////////////////
Int32
CIMUrl::getPort() const {  return m_pdata->m_port; }
//////////////////////////////////////////////////////////////////////////////
String
CIMUrl::getFile() const {  return m_pdata->m_file; }
//////////////////////////////////////////////////////////////////////////////
String
CIMUrl::getRef() const {  return m_pdata->m_ref; }
//////////////////////////////////////////////////////////////////////////////
bool
CIMUrl::isLocal() const {  return m_pdata->m_localHost; }
//////////////////////////////////////////////////////////////////////////////
String
CIMUrl::toString() const {  return String(m_pdata->m_spec); }
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
CIMUrl::setLocalHost()
{
	m_pdata->m_localHost = false;
	m_pdata->m_host.trim();
	if (m_pdata->m_host.empty()
		|| m_pdata->m_host.equals("127.0.0.1")
		|| m_pdata->m_host.equalsIgnoreCase("localhost"))
	{
		m_pdata->m_localHost = true;
		m_pdata->m_host = "127.0.0.1";
	}
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
CIMUrl::setDefaultValues()
{
	m_pdata->m_protocol.trim();
	if (m_pdata->m_protocol.empty())
	{
		m_pdata->m_protocol = "http";
	}
	setLocalHost();
	if (m_pdata->m_port <= 0)
	{
		m_pdata->m_port = 5988;
	}
	m_pdata->m_file.trim();
	if (m_pdata->m_file.empty())
	{
		m_pdata->m_file = "cimom";
		m_pdata->m_ref = String();
	}
	buildSpec();
}
//////////////////////////////////////////////////////////////////////////////
// PUBLIC
CIMUrl&
CIMUrl::setHost(const String& host)
{
	m_pdata->m_host = host;
	setLocalHost();
	buildSpec();
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMUrl&
CIMUrl::setProtocol(const String& protocol)
{
	m_pdata->m_protocol = protocol;
	buildSpec();
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
// PUBLIC
bool
CIMUrl::equals(const CIMUrl& arg) const
{
	return (m_pdata->m_protocol == arg.m_pdata->m_protocol
		&& m_pdata->m_host == arg.m_pdata->m_host
		&& m_pdata->m_port == arg.m_pdata->m_port
		&& m_pdata->m_file == arg.m_pdata->m_file
		&& m_pdata->m_ref == arg.m_pdata->m_ref);
}
//////////////////////////////////////////////////////////////////////////////
// PUBLIC
bool
CIMUrl::sameFile(const CIMUrl& arg) const
{
	return (m_pdata->m_protocol == arg.m_pdata->m_protocol
		&& m_pdata->m_host == arg.m_pdata->m_host
		&& m_pdata->m_port == arg.m_pdata->m_port
		&& m_pdata->m_file == arg.m_pdata->m_file);
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
CIMUrl::setComponents()
{
	if (m_pdata->m_spec.empty())
	{
		return;
	}
	String spec(m_pdata->m_spec);
	m_pdata->m_protocol = String();
	m_pdata->m_host = String();
	m_pdata->m_port = 0;
	m_pdata->m_file = String();
	m_pdata->m_ref = String();
	m_pdata->m_localHost = true;
	size_t ndx = spec.indexOf("://");
	if (ndx != String::npos)
	{
		m_pdata->m_protocol = spec.substring(0, ndx);
		spec = spec.substring(ndx+3);
	}
	// parse and remove name and password
	ndx = spec.indexOf('@');
	if (ndx != String::npos)
	{
		spec = spec.substring(ndx + 1);
	}
	ndx = spec.indexOf('/');
	if (ndx != String::npos)
	{
		m_pdata->m_host = spec.substring(0, ndx);
		m_pdata->m_file = spec.substring(ndx+1);
		checkRef();
	}
	else
	{
		m_pdata->m_host = spec.substring(0);
	}
	ndx = m_pdata->m_host.indexOf(':');
	if (ndx != String::npos)
	{
		String sport = m_pdata->m_host.substring(ndx+1);
		try
		{
			m_pdata->m_port = sport.toInt32();
		}
		catch (StringConversionException&)
		{
			m_pdata->m_port = 5988;
		}
		m_pdata->m_host = m_pdata->m_host.substring(0, ndx);
	}
	checkRef();
	setDefaultValues();
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
CIMUrl::checkRef()
{
	if (!m_pdata->m_file.empty())
	{
		size_t ndx = m_pdata->m_file.indexOf('#');
		if (ndx != String::npos)
		{
			m_pdata->m_ref = m_pdata->m_file.substring(ndx+1);
			m_pdata->m_file = m_pdata->m_file.substring(0, ndx);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
CIMUrl::buildSpec()
{
	StringBuffer tmp(m_pdata->m_protocol);
	tmp += "://";
	tmp += m_pdata->m_host;
	if (m_pdata->m_port > 0)
	{
		tmp += ":";
		tmp += String(m_pdata->m_port);
	}
	if (!m_pdata->m_file.empty())
	{
		tmp += '/';
		tmp += m_pdata->m_file;
	}
	if (!m_pdata->m_ref.empty())
	{
		tmp += '#';
		tmp += m_pdata->m_ref;
	}

	m_pdata->m_spec = tmp.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
// PUBLIC
void
CIMUrl::readObject(streambuf & istrm)
{
	CIMBase::readSig( istrm, OW_CIMURLSIG );
	String spec;
	spec.readObject(istrm);

	if (!m_pdata)
	{
		m_pdata = new URLData;
	}
	m_pdata->m_spec = spec;
	setComponents();
}
//////////////////////////////////////////////////////////////////////////////
// PUBLIC
void
CIMUrl::writeObject(streambuf & ostrm) const
{
	CIMBase::writeSig( ostrm, OW_CIMURLSIG );
	m_pdata->m_spec.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMUrl::toMOF() const {  return "UNIMPLEMENTED"; }
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMUrl& lhs, const CIMUrl& rhs)
{
	return *lhs.m_pdata < *rhs.m_pdata;
}

} // end namespace OW_NAMESPACE

