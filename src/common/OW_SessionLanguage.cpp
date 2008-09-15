/*******************************************************************************
* Copyright (C) 2001-2004 Novell, Inc. All rights reserved.
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
*  - Neither the name of Novell, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Jon Carey
 */
#include "OW_config.h"
#include "OW_SessionLanguage.hpp"
#include "OW_BinarySerialization.hpp"

#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cerrno>
#include <algorithm>	// for sort
#include <functional>

namespace OW_NAMESPACE
{

namespace
{

inline const char* 
skipWhite(const char* arg)
{
	while (*arg && isspace(*arg)) { arg++; }
	return arg;
}

bool
subtagsMatch(const char* arg1, const char* arg2)
{
	// If either arg is a wildcard, return match
	if (*arg1 == '*' || *arg2 == '*')
	{
		return true;
	}

	// Assume lower case in both args
	return (::strcmp(arg1, arg2) == 0);
}

}	// End of unnamed namespace

//////////////////////////////////////////////////////////////////////////////
LanguageTag::LanguageTag()
{
	m_subtag1[0] = 0;
	m_subtag2[0] = 0;
	m_subtag3[0] = 0;
	m_weight = 0;
	m_explicitQualityValue = false;
}

//////////////////////////////////////////////////////////////////////////////
LanguageTag::LanguageTag(const char* languageTag)
{
	assign(languageTag);
}

//////////////////////////////////////////////////////////////////////////////
LanguageTag::LanguageTag(const LanguageTag& arg)
	: SerializableIFC(arg)
{
	copy(arg);
}

//////////////////////////////////////////////////////////////////////////////
LanguageTag::~LanguageTag()
{
}

//////////////////////////////////////////////////////////////////////////////
LanguageTag& 
LanguageTag::operator= (const LanguageTag& arg)
{
	return copy(arg);
}

//////////////////////////////////////////////////////////////////////////////
LanguageTag&
LanguageTag::operator= (const char* arg)
{
	assign(arg);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
const char*
LanguageTag::assign(const char* arg)
{
	m_weight = 0;
	m_explicitQualityValue = false;
    return setSubTags(arg);
}

//////////////////////////////////////////////////////////////////////////////
LanguageTag& 
LanguageTag::copy(const LanguageTag& arg)
{
	::memcpy(m_subtag1, arg.m_subtag1, sizeof(m_subtag1));
	::memcpy(m_subtag2, arg.m_subtag2, sizeof(m_subtag2));
	::memcpy(m_subtag3, arg.m_subtag3, sizeof(m_subtag3));
	m_weight = arg.m_weight;
	m_explicitQualityValue = arg.m_explicitQualityValue;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
int 
LanguageTag::compareWeight(const LanguageTag& arg) const
{
	int v = m_weight - arg.m_weight;
	if (v == 0)
	{
		if (m_explicitQualityValue != arg.m_explicitQualityValue)
		{
			v = (arg.m_explicitQualityValue) ? -1 : 1;
		}
	}

	return v;
}

//////////////////////////////////////////////////////////////////////////////
String 
LanguageTag::toString() const
{
	char tmpBuf[sizeof(m_subtag1)+sizeof(m_subtag2)+sizeof(m_subtag3)+10];
	tmpBuf[0] = 0;
	if (!invalid())
	{
		::strcpy(tmpBuf, m_subtag1);
		if (m_subtag2[0])
		{
			::strcat(tmpBuf, "-");
			::strcat(tmpBuf, m_subtag2);
			if (m_subtag3[0])
			{
				::strcat(tmpBuf, "-");
				::strcat(tmpBuf, m_subtag3);
			}
		}
	}

	return String(tmpBuf);
}

//////////////////////////////////////////////////////////////////////////////
const char*
LanguageTag::setSubTags(const char* languageTag)
{
	// Parse section of string of the format: "en-GB ; q = 0.8 "

	m_subtag1[0] = 0;
	m_subtag2[0] = 0;
	m_subtag3[0] = 0;
	m_weight = 100;
	m_explicitQualityValue = false;

	const char* p = skipWhite(languageTag); // point to start of language tag

	if (!(p = parseSubTag(p, m_subtag1)))	// Get subtag1
		return 0;	// Invalid

	p = skipWhite(p);
	if (*p == ';') // Start of quality value for language tag?
		return setWeight(p); // End of this language tag

	if (*p != '-') // Start of subtag2?
		return p;	// Must be the end of the entire language tag

	// Must be starting subtag2
	++p;	// Skip the '-' character
	if (!(p = parseSubTag(p, m_subtag2)))	// Get subtag2
		return p;	// Invalid

	p = skipWhite(p);
	if (*p == ';') // Start of quality value for language tag?
		return setWeight(p); // End of this language tag

	if (*p != '-') // Start of subtag3?
		return p;	// Must be the end of the entire language tag

	++p;	// Skip the '-' character
	return parseSubTag(p, m_subtag3);	// Get subtag3
}

//////////////////////////////////////////////////////////////////////////////
const char*
LanguageTag::parseSubTag(const char* arg, char* tagField)
{
	int i = 0;
	while (true)
	{
		if (!isalpha(*arg) && *arg != '*')
		{
			if (*arg != '-'			// Start of next subtag?
			   && *arg != ';'		// Start of quality value?
			   && *arg != ','		// End of language tag
			   && !isspace(*arg)	// End of tag?
			   && *arg != 0)		// End of string?
			{
				// Invalid char in subtag
				m_subtag1[0] = 0;
				arg = 0;
			}
			break;
		}

		if (i == 8)
		{
			// subtag to long
			m_subtag1[0] = 0;
			arg = 0;
			break;
		}

		tagField[i++] = tolower(*arg);
		tagField[i] = 0;
		arg++;
	}

	return arg;
}

//////////////////////////////////////////////////////////////////////////////
const char*
LanguageTag::setWeight(const char* arg)
{
	// Parse section of string of the format: " ; q = 0.8 "

	m_weight = 0;
	// Skip white space and the ';' character
	while (*arg && (*arg == ';' || isspace(*arg)))
	{
		++arg;
	}

	// If we didn't encounter a 'q' or 'Q' then its invalid
	if (*arg != 'q' && *arg != 'Q')
	{
		m_subtag1[0] = 0;
		return 0;
	}
	arg++;		// Skip the 'q'/'Q'
	arg = skipWhite(arg);	// Eat the white space up to the '='
	if (*arg != '=')
	{
		m_subtag1[0] = 0;
		return 0;
	}
	arg++;					// Skip the '='
	arg = skipWhite(arg);	// Eat any white space after '='
	const char* p = arg;	// Save start of numeric value

	if (!isdigit(*arg) && *arg != '.')
	{
		m_subtag1[0] = 0;
		return 0;
	}

	while (isdigit(*arg))	// Look for decimal point or end of number
		++arg;

	if (*arg == '.')		// If we hit the decimal then keep going
	{
		do
		{
			++arg;
		} while (isdigit(*arg));
	}

	errno = 0;
	double fv = strtod(p, 0);
	if (errno == ERANGE)
	{
		m_subtag1[0] = 0;
		arg = 0;
	}
	else
	{
		fv *= 100.0;
		m_weight = static_cast<Int32>(fv);
	}
	m_explicitQualityValue = true;
	return arg;
}

//////////////////////////////////////////////////////////////////////////////
void 
LanguageTag::readObject(std::streambuf & istrm)
{
	*this = LanguageTag(BinarySerialization::readString(istrm).c_str());
}

//////////////////////////////////////////////////////////////////////////////
void
LanguageTag::writeObject(std::streambuf & ostrm) const
{
	BinarySerialization::writeString(ostrm, toString());
}

//////////////////////////////////////////////////////////////////////////////
SessionLanguage::SessionLanguage()
	: OperationContext::Data()
	, m_langTags()
	, m_contentLanguage()
	, m_acceptLanguageString()
{
}

//////////////////////////////////////////////////////////////////////////////
SessionLanguage::SessionLanguage(const char* acceptLangHdrValue)
	: OperationContext::Data()
	, m_langTags()
	, m_contentLanguage()
	, m_acceptLanguageString()
{
    assign(acceptLangHdrValue);
}

//////////////////////////////////////////////////////////////////////////////
SessionLanguage::SessionLanguage(const SessionLanguage& arg)
	: OperationContext::Data()
	, m_langTags(arg.m_langTags)
	, m_contentLanguage(arg.m_contentLanguage)
	, m_acceptLanguageString(arg.m_acceptLanguageString)
{
}

//////////////////////////////////////////////////////////////////////////////
SessionLanguage::~SessionLanguage()
{
}

//////////////////////////////////////////////////////////////////////////////
SessionLanguage&
SessionLanguage::operator=(const SessionLanguage& arg)
{
	m_langTags = arg.m_langTags;
	m_contentLanguage = arg.m_contentLanguage;
	m_acceptLanguageString = arg.m_acceptLanguageString;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
SessionLanguage&
SessionLanguage::assign(const char* acceptLangHdrValue)
{
	buildLangTags(acceptLangHdrValue);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void 
SessionLanguage::buildLangTags(const char* acceptLangHdrValue)
{
	m_acceptLanguageString = acceptLangHdrValue;
	m_langTags.clear();
	const char* p = skipWhite(acceptLangHdrValue);
	if (!(*p))
	{
		return;
	}

	LanguageTag ltag;
	while (*p)
	{
		p = ltag.assign(p);
		if (!p)
		{
			break;
		}
		m_langTags.append(ltag);
		if (!*p)
		{
			break;
		}
		++p;
	}

	if (!p)
	{
		m_langTags.clear();
	}
	else
	{
		std::sort(m_langTags.begin(), m_langTags.end(), 
			std::greater<LanguageTag>());
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
bool 
SessionLanguage::langsMatch(const LanguageTag& t1, const LanguageTag& t2,
	int level)
{
	bool matches = subtagsMatch(t1.m_subtag1, t2.m_subtag1);
	if (level > 1 && matches)
	{
		matches = subtagsMatch(t1.m_subtag2, t2.m_subtag2);
	}
	if (level > 2 && matches)
	{
		matches = subtagsMatch(t1.m_subtag3, t2.m_subtag3);
	}
	return matches;
}

//////////////////////////////////////////////////////////////////////////////
String 
SessionLanguage::getBestLanguage(const StringArray& languages, const String& defaultLanguage) const
{
	if (languages.size() == 0)
	{
		return defaultLanguage;
	}

	if (m_langTags.size() == 0)
	{
		return languages[0];	// Return default content language?
	}

	int bestIndex = 0;
	int bestWeight = -1;

	for (int level = 3; level > 0; level--)
	{
		// Try to find fully qualified match first (all subtags match)
		for (StringArray::size_type i = 0; i < languages.size(); i++)
		{
			LanguageTag lt(languages[i].c_str());
			for (LanguageTagArray::size_type j = 0; j < m_langTags.size(); j++)
			{
				if (langsMatch(m_langTags[j], lt, level))
				{
					if (m_langTags[j].getWeight() > 0)
					{
						// If the quality value for this language is better
						// than the one found previously, then save it for the
						// return value
						if (m_langTags[j].getWeight() > bestWeight)
						{
							bestWeight = m_langTags[j].getWeight();
							bestIndex = i;	// Index of languages parm
						}
						break;
					}
				}
			}
		}
	}  

	return (bestWeight > -1) ? languages[bestIndex] : defaultLanguage;
}

//////////////////////////////////////////////////////////////////////////////
void
SessionLanguage::addContentLanguage(const String& contentLanguage)
{
	if (!m_contentLanguage.empty())
	{
		m_contentLanguage += ", ";
	}

	StringArray currentLangs = m_contentLanguage.tokenize(", ");
	if (std::find(currentLangs.begin(), currentLangs.end(), contentLanguage) == currentLangs.end())
	{
		m_contentLanguage += contentLanguage;
	}
}

//////////////////////////////////////////////////////////////////////////////
String 
SessionLanguage::getContentLanguage() const
{
	return m_contentLanguage;
}

//////////////////////////////////////////////////////////////////////////////
void 
SessionLanguage::readObject(std::streambuf & istrm)
{
	BinarySerialization::readArray(istrm, m_langTags);
	m_contentLanguage = BinarySerialization::readString(istrm);
	m_acceptLanguageString = BinarySerialization::readString(istrm);
}

//////////////////////////////////////////////////////////////////////////////
void
SessionLanguage::writeObject(std::streambuf & ostrm) const
{
	BinarySerialization::writeArray(ostrm, m_langTags);
	BinarySerialization::writeString(ostrm, m_contentLanguage);
	BinarySerialization::writeString(ostrm, m_acceptLanguageString);
}

//////////////////////////////////////////////////////////////////////////////
const String SessionLanguage::s_type("SessionLanguage");

//////////////////////////////////////////////////////////////////////////////
String
SessionLanguage::getType() const
{
	return s_type;
}

}	// end namespace OW_NAMESPACE

