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
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_StrictWeakOrdering.hpp"
#include <cctype>

namespace OpenWBEM
{

using std::ostream;
using std::istream;
//////////////////////////////////////////////////////////////////////////////
struct CIMNameSpace::NSData
{
	String m_nameSpace;
	CIMUrl m_url;
    NSData* clone() const { return new NSData(*this); }
};
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMNameSpace::NSData& x, const CIMNameSpace::NSData& y)
{
	return StrictWeakOrdering(
		x.m_nameSpace, y.m_nameSpace,
		x.m_url, y.m_url);
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace::CIMNameSpace() :
	CIMBase(), m_pdata(new NSData)
{
	//m_pdata->m_nameSpace = CIM_OW_DEFAULT_NS;
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace::CIMNameSpace(CIMNULL_t) :
	CIMBase(), m_pdata(0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace::CIMNameSpace(const CIMUrl& hostUrl,
	const String& nameSpace) :
	CIMBase(), m_pdata(new NSData)
{
	m_pdata->m_url = hostUrl;
	if(nameSpace.empty())
	{
		//m_pdata->m_nameSpace = String(CIM_OW_DEFAULT_NS);
	}
	else
	{
		setNameSpace(nameSpace);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace::CIMNameSpace(const String& nameSpace) :
	CIMBase(), m_pdata(new NSData)
{
	if(nameSpace.empty())
	{
		//m_pdata->m_nameSpace = String(CIM_OW_DEFAULT_NS);
	}
	else
	{
		setNameSpace(nameSpace);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace::CIMNameSpace(const char* nameSpace) :
	CIMBase(), m_pdata(new NSData)
{
	if(nameSpace == 0 || nameSpace[0] == '\0')
	{
		//m_pdata->m_nameSpace = String(CIM_OW_DEFAULT_NS);
	}
	else
	{
		setNameSpace(nameSpace);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace::CIMNameSpace(const CIMNameSpace& arg) :
	CIMBase(), m_pdata(arg.m_pdata)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace::~CIMNameSpace()
{
}
//////////////////////////////////////////////////////////////////////////////
void
CIMNameSpace::setNull()
{
	m_pdata = 0;
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace&
CIMNameSpace::operator= (const CIMNameSpace& arg)
{
	m_pdata = arg.m_pdata;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMNameSpace::getNameSpace() const
{
	return m_pdata->m_nameSpace;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMNameSpace::getHost() const
{
	return m_pdata->m_url.getHost();
}
//////////////////////////////////////////////////////////////////////////////
CIMUrl
CIMNameSpace::getHostUrl() const
{
	return m_pdata->m_url;
}
//////////////////////////////////////////////////////////////////////////////
Int32
CIMNameSpace::getPortNumber() const
{
	return m_pdata->m_url.getPort();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMNameSpace::getProtocol() const
{
	return m_pdata->m_url.getProtocol();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMNameSpace::getFileName() const
{
	return m_pdata->m_url.getFile();
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMNameSpace::isLocal() const
{
	return m_pdata->m_url.isLocal();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMNameSpace::toString() const
{
	return String(m_pdata->m_nameSpace);
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace&
CIMNameSpace::setNameSpace(const String& nameSpace)
{
	// Remove any preceeding '/' chars or spaces
	String tns(nameSpace);
	tns.trim();
	const char* p = tns.c_str();
	while(*p && *p == '/')
	{
		p++;
	}
	m_pdata->m_nameSpace = p;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace&
CIMNameSpace::setHostUrl(const CIMUrl& hostUrl)
{
	m_pdata->m_url = hostUrl;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace&
CIMNameSpace::setHost(const String& host)
{
	m_pdata->m_url.setHost(host);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace&
CIMNameSpace::setProtocol(const String& protocol)
{
	m_pdata->m_url.setProtocol(protocol);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMNameSpace::readObject(istream &istrm)
{
	CIMBase::readSig( istrm, OW_CIMNAMESPACESIG );
	String ns;
	ns.readObject(istrm);
	CIMUrl url(CIMNULL);
	url.readObject(istrm);
	// Assign here in case exception gets thrown on preceeding readObjects
	if(m_pdata.isNull())
	{
		m_pdata = new NSData;
	}
	m_pdata->m_nameSpace = ns;
	m_pdata->m_url = url;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMNameSpace::writeObject(ostream &ostrm) const
{
	CIMBase::writeSig( ostrm, OW_CIMNAMESPACESIG );
	m_pdata->m_nameSpace.writeObject(ostrm);
	m_pdata->m_url.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMNameSpace& lhs, const CIMNameSpace& rhs)
{
	return *lhs.m_pdata < *rhs.m_pdata;
}

} // end namespace OpenWBEM

