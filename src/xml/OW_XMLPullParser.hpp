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
 * @author Dan Nuffer
 */

#ifndef OW_XMLPULLPARSER_HPP_INCLUDE_GUARD_
#define OW_XMLPULLPARSER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/AutoPtr.hpp"
#include "blocxx/TempFileStream.hpp"
#include "blocxx/String.hpp"
#include "OW_XMLParserCore.hpp"

#ifdef OW_HAVE_ISTREAM
#include <istream>
#else
#include <iostream>
#endif

namespace OW_NAMESPACE
{

class OW_XML_API XMLPullParser
{
public:
	XMLPullParser(const blocxx::String& str);
	XMLPullParser(std::istream& sb);
	XMLPullParser();
	virtual ~XMLPullParser();

	blocxx::String mustGetAttribute(const char* const attrName) const
	{
		return getAttribute(attrName, true);
	}
	/// @todo  Get rid of all these bool parameters!
	blocxx::String getAttribute(const char* const attrName, bool throwIfError = false) const;
	void getChild();
	void mustGetChild();
	void getNextTag(bool throwIfError = false);
	void getNext(bool throwIfError = false);
	void mustGetNext()
	{
		getNext(true);
	}
	void mustGetNextTag()
	{
		getNextTag(true);
	}
	void mustGetEndTag();
	bool tokenIs(const char* const arg) const
	{
		return m_curTok.text.equals(arg);
	}
	blocxx::String getName() const;
	blocxx::String getData() const;
	bool isData() const;
protected:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	blocxx::AutoPtr<blocxx::TempFileStream> m_ptfs;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	XMLParserCore m_parser;
	XMLToken m_curTok;
	bool m_good;

	void prime();
	void nextToken();
	void skipData();

private:
	// unimplemented
	XMLPullParser(const XMLPullParser& x);
	XMLPullParser& operator=(const XMLPullParser& x);

	friend OW_XML_API std::ostream& operator<<(std::ostream& ostr, const XMLPullParser& p);
};

} // end namespace OW_NAMESPACE

#endif
