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

#ifndef OW_SESSIONLANGUAGE_HPP_INCLUDE_GUARD
#define OW_SESSIONLANGUAGE_HPP_INCLUDE_GUARD

#include "OW_config.h"
#include "blocxx/String.hpp"
#include "blocxx/Array.hpp"
#include "OW_OperationContext.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_SerializableIFC.hpp"

namespace OW_NAMESPACE
{

//////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API LanguageTag : public SerializableIFC
{
public:
	LanguageTag();
	LanguageTag(const char* languageTag);
	LanguageTag(const LanguageTag& arg);
	virtual ~LanguageTag();

	bool invalid() const { return (m_subtag1[0] == 0); }
	Int32 getWeight() const { return m_weight; }

	LanguageTag& operator= (const LanguageTag& arg);
	LanguageTag& operator= (const char* arg);

	int compareWeight(const LanguageTag& arg) const;
	bool operator < (const LanguageTag& arg) const { return (compareWeight(arg) < 0); }
	bool operator > (const LanguageTag& arg) const { return (compareWeight(arg) > 0); }
	bool operator <= (const LanguageTag& arg) const { return (compareWeight(arg) <= 0); }
	bool operator >= (const LanguageTag& arg) const { return (compareWeight(arg) >= 0); }
	bool operator == (const LanguageTag& arg) const { return (compareWeight(arg) == 0); }
	bool operator != (const LanguageTag& arg) const { return (compareWeight(arg) != 0); }
	const char* assign(const char* arg);
	LanguageTag& copy(const LanguageTag& arg);
	String toString() const;
	String getSubtag1() const { return String(m_subtag1); }
	String getSubtag2() const { return String(m_subtag2); }
	String getSubtag3() const { return String(m_subtag3); }

	virtual void readObject(std::streambuf & istrm);
	virtual void writeObject(std::streambuf & ostrm) const;

private:
	const char* setWeight(const char* arg);
	const char* setSubTags(const char* languageTag);
	const char* parseSubTag(const char* arg, char* tagField);


	// Rules for subtag 1:
	//	- Required
	//	- May only contain chars a-z,A-Z
	//	- Maximum length of 8 chars.
	//	- 2 chars = Language code from ISO 639/639-1
	//	- 3 chars = Language code from ISO 639-2
	//	- 'i' = Language tag explicity IANA-registered
	//	- 'x' = Language tag is private, nonstandard extension
	char m_subtag1[9];

	// Rules for subtag 2:
	//	- Optional
	//	- May only contain chars 0-9,a-z,A-Z
	//	- Maximum length of 8 chars
	//  - 2 chars = Country/Region code from ISO 3166
	//	- 3-8 chars = May be registered with IANA
	//	- 1 char = Invalid subtag
	char m_subtag2[9];

	// Rules for subtag3:
	//	- Optional
	//	- Maximum length of 8 chars
	char m_subtag3[9];

	Int32 m_weight;

	bool m_explicitQualityValue;

	friend class SessionLanguage;
};

typedef Array<LanguageTag> LanguageTagArray;

//////////////////////////////////////////////////////////////////////////////
/// When getting a SessionLanguage instance from the OperationContext, use
/// OperationContext::SESSION_LANGUAGE_KEY as the key.
class OW_COMMON_API SessionLanguage : public OperationContext::Data
{
public:
	SessionLanguage();
	SessionLanguage(const char* acceptLangHdrValue);
	SessionLanguage(const SessionLanguage& arg);
	SessionLanguage& operator=(const SessionLanguage& arg);
	virtual ~SessionLanguage();
	
	int langCount() const { return m_langTags.size(); }
	String getAcceptLanguageString() const { return m_acceptLanguageString; }
	SessionLanguage& assign(const char* acceptLangHdrValue);

	/**
	 * Evaluate the best language to use, based upon what the http client specified, 
	 * as represented by this SessionLanguage object, and what the caller supports.
	 * @param languages The set of languages the caller supports.
	 * @param defaultLanguage The default language returned if no match is made.
	 */
	String getBestLanguage(const StringArray& languages, const String& defaultLanguage = String()) const;
	void addContentLanguage(const String& contentLanguage);
	String getContentLanguage() const;

	virtual void readObject(std::streambuf & istrm);
	virtual void writeObject(std::streambuf & ostrm) const;
	virtual String getType() const;

private:
	void buildLangTags(const char* acceptLangHdrValue);
	static bool langsMatch(const LanguageTag& t1, const LanguageTag& t2,
		int level);

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	LanguageTagArray m_langTags;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	String m_contentLanguage;
	String m_acceptLanguageString;
	static const String s_type;
};
OW_EXPORT_TEMPLATE(OW_COMMON_API, IntrusiveReference, SessionLanguage);

}	// End of namespace OW_NAMESPACE

#endif

