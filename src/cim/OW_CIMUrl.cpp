/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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

#include "OW_config.h"
#include "OW_CIMUrl.hpp"
#include "OW_String.hpp"
#include "OW_StrictWeakOrdering.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif



using std::istream;
using std::ostream;


struct OW_CIMUrl::URLData
{
	URLData() :
		m_port(0),
		m_localHost(true) {}

	OW_String m_spec;
	OW_String m_protocol;
	OW_String m_host;
	OW_Int32 m_port;
	OW_String m_file;
	OW_String m_ref;
	OW_Bool m_localHost;

    URLData* clone() { return new URLData(*this); }
};

//////////////////////////////////////////////////////////////////////////////
bool operator<(const OW_CIMUrl::URLData& x, const OW_CIMUrl::URLData& y)
{
	return x.m_spec < y.m_spec;
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
OW_CIMUrl::OW_CIMUrl() :
	OW_CIMBase(), m_pdata(new URLData)
{
	setDefaultValues();
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
OW_CIMUrl::OW_CIMUrl(OW_CIMNULL_t) :
	OW_CIMBase(), m_pdata(0)
{
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
OW_CIMUrl::OW_CIMUrl(const OW_String& spec) :
	OW_CIMBase(), m_pdata(new URLData)
{
	m_pdata->m_spec = spec;
	setComponents();
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
OW_CIMUrl::OW_CIMUrl(const OW_String& protocol, const OW_String& host,
	const OW_String& file, OW_Int32 port) :
	OW_CIMBase(), m_pdata(new URLData)
{
	m_pdata->m_protocol = protocol;
	m_pdata->m_host = host;
	m_pdata->m_port = port;
	m_pdata->m_file = file;
	setDefaultValues();
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
OW_CIMUrl::OW_CIMUrl(const OW_CIMUrl& context, const OW_String& spec) :
	OW_CIMBase(), m_pdata(new URLData)
{
	m_pdata->m_spec = spec;
	setComponents();
	if(m_pdata->m_protocol.empty())
	{
		m_pdata->m_protocol = context.getProtocol();
	}
	else
	{
		if(m_pdata->m_protocol == context.m_pdata->m_protocol)
		{
			m_pdata->m_host = context.m_pdata->m_host;
			m_pdata->m_port = context.m_pdata->m_port;
			m_pdata->m_file = context.m_pdata->m_file;
		}
	}

	setDefaultValues();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMUrl::OW_CIMUrl(const OW_CIMUrl& x)
	: OW_CIMBase() , m_pdata(x.m_pdata)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMUrl::~OW_CIMUrl()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMUrl&
OW_CIMUrl::operator= (const OW_CIMUrl& x)
{
	m_pdata = x.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMUrl::setNull()
{
	m_pdata = NULL;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMUrl::getSpec() const {  return m_pdata->m_spec; }

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMUrl::getProtocol() const {  return m_pdata->m_protocol; }

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMUrl::getHost() const {  return m_pdata->m_host; }

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_CIMUrl::getPort() const {  return m_pdata->m_port; }

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMUrl::getFile() const {  return m_pdata->m_file; }

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMUrl::getRef() const {  return m_pdata->m_ref; }

//////////////////////////////////////////////////////////////////////////////
bool
OW_CIMUrl::isLocal() const {  return m_pdata->m_localHost; }

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMUrl::toString() const {  return OW_String(m_pdata->m_spec); }
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_CIMUrl::setLocalHost()
{
	m_pdata->m_localHost = false;
	m_pdata->m_host.trim();
	if(m_pdata->m_host.empty()
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
OW_CIMUrl::setDefaultValues()
{
	m_pdata->m_protocol.trim();
	if(m_pdata->m_protocol.empty())
	{
		m_pdata->m_protocol = "http";
	}

	setLocalHost();
	if(m_pdata->m_port <= 0)
	{
		m_pdata->m_port = 5988;
	}

	m_pdata->m_file.trim();
	if(m_pdata->m_file.empty())
	{
		m_pdata->m_file = "cimom";
		m_pdata->m_ref = OW_String();
	}

	buildSpec();
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
OW_CIMUrl&
OW_CIMUrl::setHost(const OW_String& host)
{
	m_pdata->m_host = host;
	setLocalHost();
	buildSpec();
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMUrl&
OW_CIMUrl::setProtocol(const OW_String& protocol)
{
	m_pdata->m_protocol = protocol;
	buildSpec();
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
bool
OW_CIMUrl::equals(const OW_CIMUrl& arg) const
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
OW_CIMUrl::sameFile(const OW_CIMUrl& arg) const
{
	return (m_pdata->m_protocol == arg.m_pdata->m_protocol
		&& m_pdata->m_host == arg.m_pdata->m_host
		&& m_pdata->m_port == arg.m_pdata->m_port
		&& m_pdata->m_file == arg.m_pdata->m_file);
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_CIMUrl::setComponents()
{
	if(m_pdata->m_spec.empty())
		return;

	OW_String spec(m_pdata->m_spec);
	m_pdata->m_protocol = OW_String();
	m_pdata->m_host = OW_String();
	m_pdata->m_port = 0;
	m_pdata->m_file = OW_String();
	m_pdata->m_ref = OW_String();
	m_pdata->m_localHost = true;

	size_t ndx = spec.indexOf("://");
	if(ndx != OW_String::npos)
	{
		m_pdata->m_protocol = spec.substring(0, ndx);
		spec = spec.substring(ndx+3);
	}

	// parse and remove name and password
	ndx = spec.indexOf('@');
	if (ndx != OW_String::npos)
	{
		spec = spec.substring(ndx + 1);
	}

	ndx = spec.indexOf('/');
	if(ndx != OW_String::npos)
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
	if(ndx != OW_String::npos)
	{
		OW_String sport = m_pdata->m_host.substring(ndx+1);
		try
		{
			m_pdata->m_port = sport.toInt32();
		}
		catch (OW_StringConversionException&)
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
OW_CIMUrl::checkRef()
{
	if(!m_pdata->m_file.empty())
	{
		size_t ndx = m_pdata->m_file.indexOf('#');
		if(ndx != OW_String::npos)
		{
			m_pdata->m_ref = m_pdata->m_file.substring(ndx+1);
			m_pdata->m_file = m_pdata->m_file.substring(0, ndx);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_CIMUrl::buildSpec()
{
	m_pdata->m_spec = m_pdata->m_protocol + "://" + m_pdata->m_host;
	if(m_pdata->m_port > 0)
	{
		m_pdata->m_spec += ":" + OW_String(m_pdata->m_port);
	}

	if(!m_pdata->m_file.empty())
	{
		m_pdata->m_spec += "/" + m_pdata->m_file;
	}

	if(!m_pdata->m_ref.empty())
	{
		m_pdata->m_spec += "#" + m_pdata->m_ref;
	}
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
void
OW_CIMUrl::readObject(istream &istrm)
{
	OW_CIMBase::readSig( istrm, OW_CIMURLSIG );
	OW_String spec;
	spec.readObject(istrm);
	
	if(m_pdata.isNull())
	{
		m_pdata = new URLData;
	}

	m_pdata->m_spec = spec;
	setComponents();
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
void
OW_CIMUrl::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMURLSIG );
	m_pdata->m_spec.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMUrl::toMOF() const {  return "UNIMPLEMENTED"; }

//////////////////////////////////////////////////////////////////////////////
bool operator<(const OW_CIMUrl& lhs, const OW_CIMUrl& rhs)
{
	return *lhs.m_pdata < *rhs.m_pdata;
}

