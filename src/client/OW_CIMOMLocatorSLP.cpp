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
#include "OW_CIMOMLocatorSLP.hpp"
#include "OW_Format.hpp"

#ifdef OW_HAVE_SLP_H

struct CBData
{
	OW_Array<OW_String> urls;
	OW_Array<OW_UInt16> lifetimes;
	SLPError errcode;
};

extern "C"
{
SLPBoolean MySLPSrvURLCallback( SLPHandle /*hslp*/, 
	const char* srvurl, 
	unsigned short lifetime, 
	SLPError errcode, 
	void* cookie ) 
{
	switch (errcode)
	{
		case SLP_OK:
			((CBData*)cookie)->urls.push_back(srvurl);
			((CBData*)cookie)->lifetimes.push_back(lifetime);
			break;
		case SLP_LAST_CALL:
			((CBData*)cookie)->errcode = SLP_OK; 
			break;
		default:
			((CBData*)cookie)->errcode = errcode;
			break;
	}



	/* return SLP_TRUE because we want to be called again */ 
	/* if more services were found                        */ 

	return SLP_TRUE; 
} 
//////////////////////////////////////////////////////////////////////////////

SLPBoolean MySLPAttrCallback(SLPHandle /*hslp*/, 
                            const char* attrlist, 
                            SLPError errcode, 
                            void* cookie ) 
{ 
	if(errcode == SLP_OK) 
	{ 
		(*(OW_String*)cookie) = attrlist;
	} 
	
	return SLP_FALSE; 
}

} // extern "C"

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMLocatorSLP::OW_CIMOMLocatorSLP()	
{
	SLPError err = SLPOpen("en", SLP_FALSE, &m_hslp);
	if (err != SLP_OK)
	{
		SLPClose(m_hslp);
		OW_THROW(OW_CIMOMLocatorException, 
			format("Error opening SLP handle: %1", err).c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_CIMOMLocatorSLP::processAttributes(const OW_String& attrs, 
		OW_CIMOMInfo& info)
{
	OW_String sattrs(attrs);
	int idx = sattrs.indexOf('(');
	while (idx >= 0)
	{
		sattrs = sattrs.substring(idx + 1);
		int endIdx = sattrs.indexOf('=');
		OW_String key = sattrs.substring(0, endIdx);
		sattrs = sattrs.substring(endIdx + 1);
		endIdx = sattrs.indexOf(')');
		OW_String val = sattrs.substring(0, endIdx);
		info[key] = val;
		idx = sattrs.indexOf('(');
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMInfoArray 
OW_CIMOMLocatorSLP::findCIMOMs()
{
	SLPError err;
	CBData data;

	OW_CIMOMInfoArray rval;

	err = SLPFindSrvs(m_hslp, OW_CIMOM_SLP_SERVICE_TYPE, 0, 0, MySLPSrvURLCallback, &data);
	if((err != SLP_OK) || (data.errcode != SLP_OK)) 
	{ 
		OW_THROW(OW_CIMOMLocatorException, 
			format("Error finding service: %1.  SLP Error code: %2",
				OW_CIMOM_SLP_SERVICE_TYPE, err).c_str());
	} 

	for (size_t i = 0; i < data.urls.size(); ++i)
	{
		OW_CIMOMInfo info;
		OW_String SLPUrl = data.urls[i];
		int idx = SLPUrl.indexOf("http");
		info.setURL(SLPUrl.substring(idx));
		OW_String attrList;

		err = SLPFindAttrs(m_hslp, data.urls[i].c_str(), "", "", 
			MySLPAttrCallback, &attrList);
		if (err != SLP_OK)
		{
			OW_THROW(OW_CIMOMLocatorException, 
				format("Error retrieving attributes for %1",
					data.urls[i]).c_str());
		}

		processAttributes(attrList, info);
		
		OW_String url = info.getURL();
		char* cpUrl = 0;
		err = SLPUnescape(url.c_str(), &cpUrl, SLP_FALSE);
		if (err != SLP_OK)
		{
			if (cpUrl)
			{
				SLPFree(cpUrl);
			}
			OW_THROW(OW_CIMOMLocatorException, 
				format("Error unescaping URL: %1", err).c_str());
		}
		url = cpUrl;
		SLPFree(cpUrl);


		info.setURL(url);

		rval.push_back(info);
	}
	
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMOMLocatorSLP::~OW_CIMOMLocatorSLP()	
{
	SLPClose(m_hslp);
}

#endif // HAVE_SLP_H

