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
#include "OW_CIMUrl.hpp"
#include "OW_String.hpp"
#include "OW_MutexLock.hpp"
#include <iostream>


using std::istream;
using std::ostream;


struct OW_CIMUrl::URLData
{
	URLData() :
		m_spec(), m_protocol(), m_host(), m_port(0), m_file(), m_ref(),
		m_localHost(true) {}

	URLData(const URLData& x) :
		m_spec(x.m_spec), m_protocol(x.m_protocol), m_host(x.m_host),
		m_port(x.m_port), m_file(x.m_file), m_ref(x.m_ref),
		m_localHost(x.m_localHost) {}

	URLData& operator= (const URLData& x)
	{
		m_spec = x.m_spec;
		m_protocol = x.m_protocol;
		m_host = x.m_host;
		m_port = x.m_port;
		m_file = x.m_file;
		m_ref = x.m_ref;
		m_localHost = x.m_localHost;
		return *this;
	}

	OW_String m_spec;
	OW_String m_protocol;
	OW_String m_host;
	int m_port;
	OW_String m_file;
	OW_String m_ref;
	OW_Bool m_localHost;
};

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
OW_CIMUrl::OW_CIMUrl(OW_Bool notNull) :
	OW_CIMBase(), m_pdata((notNull) ? new URLData : NULL)
{
	if(!m_pdata.isNull())
		setDefaultValues();
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
	const OW_String& file, int port) :
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
	if(m_pdata->m_protocol.length() == 0)
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
int
OW_CIMUrl::getPort() const {  return m_pdata->m_port; }

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMUrl::getFile() const {  return m_pdata->m_file; }

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMUrl::getRef() const {  return m_pdata->m_ref; }

//////////////////////////////////////////////////////////////////////////////
OW_Bool
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
	if(m_pdata->m_host.length() == 0
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
	if(m_pdata->m_protocol.length() == 0)
	{
		m_pdata->m_protocol = "http";
	}

	setLocalHost();
	if(m_pdata->m_port <= 0)
	{
		m_pdata->m_port = 5988;
	}

	m_pdata->m_file.trim();
	if(m_pdata->m_file.length() == 0)
	{
		m_pdata->m_file = "cimom";
		m_pdata->m_ref = "";
	}

	buildSpec();
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
void
OW_CIMUrl::setHost(const OW_String& host)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_host = host;
	setLocalHost();
	buildSpec();
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
OW_Bool
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
OW_Bool
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
	if(m_pdata->m_spec.length() == 0)
		return;

	OW_String spec(m_pdata->m_spec);
	m_pdata->m_protocol = "";
	m_pdata->m_host = "";
	m_pdata->m_port = 0;
	m_pdata->m_file = "";
	m_pdata->m_ref = "";
	m_pdata->m_localHost = true;

	int ndx = spec.indexOf("://");
	if(ndx != -1)
	{
		m_pdata->m_protocol = spec.substring(0, ndx);
		spec = spec.substring(ndx+3);
	}

	ndx = spec.indexOf('/');
	if(ndx != -1)
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
	if(ndx != -1)
	{
		OW_String sport = m_pdata->m_host.substring(ndx+1);
		m_pdata->m_port = (int)sport.toInt32();
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
	if(m_pdata->m_file.length() > 0)
	{
		int ndx = m_pdata->m_file.indexOf('#');
		if(ndx != -1)
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

	if(m_pdata->m_file.length() > 0)
	{
		m_pdata->m_spec += "/" + m_pdata->m_file;
	}

	if(m_pdata->m_ref.length() > 0)
	{
		m_pdata->m_spec += "#" + m_pdata->m_ref;
	}
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC
ostream&
operator<< (ostream& ostrm, const OW_CIMUrl& arg)
{
	ostrm << arg.toString();
	return ostrm;
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

	OW_MutexLock l = m_pdata.getWriteLock();
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
/*
void
OW_CIMUrl::toXML(std::ostream& ostr) const
	{  ostr << "UNIMPLEMENTED"; }
*/

