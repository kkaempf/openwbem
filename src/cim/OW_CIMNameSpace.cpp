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
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_StrictWeakOrdering.hpp"
#include <cctype>

using std::ostream;
using std::istream;

//////////////////////////////////////////////////////////////////////////////
struct OW_CIMNameSpace::NSData
{
	OW_String m_nameSpace;
	OW_CIMUrl m_url;

    NSData* clone() const { return new NSData(*this); }
};

//////////////////////////////////////////////////////////////////////////////
bool operator<(const OW_CIMNameSpace::NSData& x, const OW_CIMNameSpace::NSData& y)
{
	return OW_StrictWeakOrdering(
		x.m_nameSpace, y.m_nameSpace,
		x.m_url, y.m_url);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMNameSpace::OW_CIMNameSpace() :
	OW_CIMBase(), m_pdata(new NSData)
{
	//m_pdata->m_nameSpace = CIM_DEFAULT_NS;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMNameSpace::OW_CIMNameSpace(OW_CIMNULL_t) :
	OW_CIMBase(), m_pdata(0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMNameSpace::OW_CIMNameSpace(const OW_CIMUrl& hostUrl,
	const OW_String& nameSpace) :
	OW_CIMBase(), m_pdata(new NSData)
{
	m_pdata->m_url = hostUrl;
	if(nameSpace.empty())
	{
		//m_pdata->m_nameSpace = OW_String(CIM_DEFAULT_NS);
	}
	else
	{
		setNameSpace(nameSpace);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMNameSpace::OW_CIMNameSpace(const OW_String& nameSpace) :
	OW_CIMBase(), m_pdata(new NSData)
{
	if(nameSpace.empty())
	{
		//m_pdata->m_nameSpace = OW_String(CIM_DEFAULT_NS);
	}
	else
	{
		setNameSpace(nameSpace);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMNameSpace::OW_CIMNameSpace(const char* nameSpace) :
	OW_CIMBase(), m_pdata(new NSData)
{
	if(nameSpace == 0 || nameSpace[0] == '\0')
	{
		//m_pdata->m_nameSpace = OW_String(CIM_DEFAULT_NS);
	}
	else
	{
		setNameSpace(nameSpace);
	}
}

//////////////////////////////////////////////////////////////////////////////

OW_CIMNameSpace::OW_CIMNameSpace(const OW_CIMNameSpace& arg) :
	OW_CIMBase(), m_pdata(arg.m_pdata)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMNameSpace::~OW_CIMNameSpace()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMNameSpace::setNull()
{
	m_pdata = 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMNameSpace&
OW_CIMNameSpace::operator= (const OW_CIMNameSpace& arg)
{
	m_pdata = arg.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMNameSpace::getNameSpace() const
{
	return m_pdata->m_nameSpace;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMNameSpace::getHost() const
{
	return m_pdata->m_url.getHost();
}
//////////////////////////////////////////////////////////////////////////////
OW_CIMUrl
OW_CIMNameSpace::getHostUrl() const
{
	return m_pdata->m_url;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_CIMNameSpace::getPortNumber() const
{
	return m_pdata->m_url.getPort();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMNameSpace::getProtocol() const
{
	return m_pdata->m_url.getProtocol();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMNameSpace::getFileName() const
{
	return m_pdata->m_url.getFile();
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMNameSpace::isLocal() const
{
	return m_pdata->m_url.isLocal();
}


//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMNameSpace::toString() const
{
	return OW_String(m_pdata->m_nameSpace);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMNameSpace&
OW_CIMNameSpace::setNameSpace(const OW_String& nameSpace)
{
	// Remove any preceeding '/' chars or spaces
	OW_String tns(nameSpace);
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
OW_CIMNameSpace&
OW_CIMNameSpace::setHostUrl(const OW_CIMUrl& hostUrl)
{
	m_pdata->m_url = hostUrl;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMNameSpace&
OW_CIMNameSpace::setHost(const OW_String& host)
{
	m_pdata->m_url.setHost(host);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMNameSpace&
OW_CIMNameSpace::setProtocol(const OW_String& protocol)
{
	m_pdata->m_url.setProtocol(protocol);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMNameSpace::readObject(istream &istrm)
{
	OW_CIMBase::readSig( istrm, OW_CIMNAMESPACESIG );
	OW_String ns;
	ns.readObject(istrm);

	OW_CIMUrl url(OW_CIMNULL);
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
OW_CIMNameSpace::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMNAMESPACESIG );
	m_pdata->m_nameSpace.writeObject(ostrm);
	m_pdata->m_url.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
bool operator<(const OW_CIMNameSpace& lhs, const OW_CIMNameSpace& rhs)
{
	return *lhs.m_pdata < *rhs.m_pdata;
}
