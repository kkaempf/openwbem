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

#ifndef OW_ISTREAMBUFITERATOR_HPP_INCLUDE_GUARD_
#define OW_ISTREAMBUFITERATOR_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Bool.hpp"
#if defined(OW_HAVE_ISTREAM)
#include <istream>
#else
#include <iostream>
#endif


class OW_IstreamBufIterator
{
public:
	class proxy
	{
		friend class OW_IstreamBufIterator;

	public:
		char operator*() {  return m_keep; }

	private:
		proxy(char c, std::streambuf* sb) : m_keep(c), m_sbuf(sb) {}
		char m_keep;
		std::streambuf* m_sbuf;
	};

	OW_IstreamBufIterator() : m_sbuf(0) {}
	OW_IstreamBufIterator(std::istream& s) : m_sbuf(s.rdbuf()) {}
	OW_IstreamBufIterator(std::streambuf* b) : m_sbuf(b) {}
	OW_IstreamBufIterator(const proxy& p) : m_sbuf(p.m_sbuf) {}

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

	OW_IstreamBufIterator& operator++()
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
	
	OW_Bool equal(const OW_IstreamBufIterator& b) const
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


inline OW_Bool operator==(const OW_IstreamBufIterator& lhs, const OW_IstreamBufIterator& rhs)
{
	return lhs.equal(rhs);
}

inline OW_Bool operator!=(const OW_IstreamBufIterator& lhs, const OW_IstreamBufIterator& rhs)
{
	return !lhs.equal(rhs);
}


#endif


