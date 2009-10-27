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

#include "OW_config.h"
#include "OW_HDBCommon.hpp"
#include "OW_CIMObjectPath.hpp"
#include "blocxx/StringBuffer.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMUrl.hpp"

#include <algorithm> // for std::sort

namespace OW_NAMESPACE
{

using namespace blocxx;

HDBUtilKeyArray::HDBUtilKeyArray(const CIMPropertyArray& props)
	: m_props(props)
{
	std::sort(m_props.begin(), m_props.end());
}

namespace
{

void asKeyString(StringBuffer& out, const CIMValue& val);

struct KeySorter
{
	bool operator()(const CIMProperty& x, const CIMProperty& y) const
	{
		return x.getName().compareToIgnoreCase(y.getName()) < 0;
	}
};

String
modelPath(const CIMObjectPath& cop)
{
	StringBuffer rv(cop.getClassName().toString().toLowerCase());
	CIMPropertyArray keys(cop.getKeys());
	std::sort(keys.begin(), keys.end(), KeySorter());
	for (size_t i = 0; i < keys.size(); i++)
	{
		const CIMProperty& cp(keys[i]);
		if (i > 0)
		{
			rv += ',';
		}
		else
		{
			rv += '.';
		}
		rv += cp.getName().toLowerCase();
		rv += "=\"";
		asKeyString(rv, cp.getValue());
	}
	return rv.releaseString();
}

String
objectPathAsKeyString(const CIMObjectPath& cop)
{
	StringBuffer rv;
	CIMUrl url = cop.getNameSpaceUrl();
	if (!url.isLocal())
	{
		rv += url.getProtocol().toLowerCase();
		rv += "://";
		rv += url.getHost().toLowerCase();
		if (url.getPort() != 5988)
		{
			rv += ":";
			rv += url.getPort();
		}
	}
	rv += '/';
	rv += cop.getNameSpace();
	rv += ':';
	rv += modelPath(cop);
	return rv.releaseString();
}


void asKeyString(StringBuffer& out, const CIMValue& val)
{
	if (!val)
	{
		OW_THROW(HDBException, "NULL value not allowed in a key property");
	}
	out += '"';
	if (val.getType() == CIMDataType::REFERENCE)
	{
		// arrays of references aren't allowed, so we don't need to handle that.
		out += CIMObjectPath::escape(objectPathAsKeyString(val.toCIMObjectPath()));
	}
	else
	{
		out += CIMObjectPath::escape(val.toMOF());
	}
	out += '"';
}

} // end unnamed namespace

void
HDBUtilKeyArray::toString(StringBuffer& out)
{
	for (size_t i = 0; i < m_props.size(); i++)
	{
		char c = (i == 0) ? '.' : ',';
		out += c;
		out += m_props[i].getName().toLowerCase();
		out += '=';
		asKeyString(out, m_props[i].getValue());
	}
}


} // end namespace OW_NAMESPACE



