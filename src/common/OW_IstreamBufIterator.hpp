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
#ifndef OW_ISTREAMBUFITERATOR_HPP_INCLUDE_GUARD_
#define OW_ISTREAMBUFITERATOR_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#if defined(OW_HAVE_ISTREAM)
#include <istream>
#else
#include <iostream>
#endif

// This class is just a replacement for std::istreambuf_iterator<>, since it
// doesn't exist in the gcc 2.95.x standard lib.

namespace OpenWBEM
{

class IstreamBufIterator
{
public:
	class proxy
	{
		friend class IstreamBufIterator;
	public:
		char operator*() {  return m_keep; }
	private:
		proxy(char c, std::streambuf* sb) : m_keep(c), m_sbuf(sb) {}
		char m_keep;
		std::streambuf* m_sbuf;
	};
	IstreamBufIterator() : m_sbuf(0) {}
	IstreamBufIterator(std::istream& s) : m_sbuf(s.rdbuf()) {}
	IstreamBufIterator(std::streambuf* b) : m_sbuf(b) {}
	IstreamBufIterator(const proxy& p) : m_sbuf(p.m_sbuf) {}
	char operator*() const
	{
		if (m_sbuf)
		{
			return (m_sbuf->sgetc());
		}
		else
		{
			return 0;
		}
	}
	IstreamBufIterator& operator++()
	{
		if (m_sbuf)
		{
			m_sbuf->sbumpc();
			if (m_sbuf->sgetc() == EOF)
			{
				m_sbuf = 0;
			}
		}
		return *this;
	}
	proxy operator++(int)
	{
		if (m_sbuf)
		{
			proxy temp(m_sbuf->sgetc(), m_sbuf);
			m_sbuf->sbumpc();
			if (m_sbuf->sgetc() == EOF)
			{
				m_sbuf = 0;
			}
			return temp;
		}
		else
		{
			return proxy(0,0);
		}
	}
	
	bool equal(const IstreamBufIterator& b) const
	{
		if (((m_sbuf == 0) && (b.m_sbuf == 0)) || ((m_sbuf != 0) && (b.m_sbuf != 0)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
private:
	std::streambuf* m_sbuf;
};
inline bool operator==(const IstreamBufIterator& lhs, const IstreamBufIterator& rhs)
{
	return lhs.equal(rhs);
}
inline bool operator!=(const IstreamBufIterator& lhs, const IstreamBufIterator& rhs)
{
	return !lhs.equal(rhs);
}

} // end namespace OpenWBEM

typedef OpenWBEM::IstreamBufIterator OW_IstreamBufIterator OW_DEPRECATED;

#endif
