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

#ifndef OW_CIMOMINFO_HPP_INCLUDE_GUARD_
#define OW_CIMOMINFO_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Map.hpp"

/**
 * A OW_CIMOMInfo object represents information about a CIMOM
 * that is known via a discovery mechanism (slp). 
 */

class OW_CIMOMInfo
{
public:
	/**
	 * Get a URL that can be used to access the CIMOM.  This url
	 * is suitable for passing to a OW_HTTPClient ctor.
	 * @return An OW_String containing a URL to the CIMOM.
	 */
	OW_String getURL() const { return m_url; }

	void setURL(const OW_String& url) { m_url = url; }

	/**
	 * The subscript operator can be used to access the
	 * various attributes associated with a CIMOM.
	 */
	const OW_String operator[](const OW_String& key) const;

	OW_String& operator[](const OW_String& key);

private:
	OW_String m_url;
	OW_Map<OW_String, OW_String> m_attributes;

};

inline bool operator<(const OW_CIMOMInfo& x, const OW_CIMOMInfo& y)
{
    return x.getURL() < y.getURL();
}

inline bool operator==(const OW_CIMOMInfo& x, const OW_CIMOMInfo& y)
{
    return x.getURL() == y.getURL();
}

inline bool operator!=(const OW_CIMOMInfo& x, const OW_CIMOMInfo& y)
{
    return !(x == y);
}

typedef OW_Array<OW_CIMOMInfo> OW_CIMOMInfoArray;

#endif

