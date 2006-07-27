/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

/**
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_OperationContext.hpp"
#include "OW_String.hpp"
#include "OW_UserInfo.hpp"
#include "OW_Array.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_BinarySerialization.hpp"

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(ContextDataNotFound);

/////////////////////////////////////////////////////////////////////////////
OperationContext::OperationContext()
{
}

/////////////////////////////////////////////////////////////////////////////
UserInfo
OperationContext::getUserInfo() const
{
	try
	{
		return UserInfo(getStringData(USER_NAME));
	}
	catch (ContextDataNotFoundException& e)
	{
		return UserInfo();
	}
}


/////////////////////////////////////////////////////////////////////////////
OperationContext::Data::~Data()
{
}

/////////////////////////////////////////////////////////////////////////////
void 
OperationContext::setData(const String& key, const DataRef& data)
{
	doSetData(key, data);
}

/////////////////////////////////////////////////////////////////////////////
void
OperationContext::removeData(const String& key)
{
	doRemoveData(key);
}

/////////////////////////////////////////////////////////////////////////////
bool
OperationContext::keyHasData(const String& key) const
{
	return doKeyHasData(key);
}

/////////////////////////////////////////////////////////////////////////////
OperationContext::StringData::StringData() 
{
}

/////////////////////////////////////////////////////////////////////////////
OperationContext::StringData::StringData(const String& str) 
	: m_str(str) 
{
}

/////////////////////////////////////////////////////////////////////////////
void
OperationContext::StringData::writeObject(std::streambuf& ostr) const
{
	BinarySerialization::writeString(ostr, m_str);
}

/////////////////////////////////////////////////////////////////////////////
void
OperationContext::StringData::readObject(std::streambuf& istr)
{
	m_str = BinarySerialization::readString(istr);
}

/////////////////////////////////////////////////////////////////////////////
String
OperationContext::StringData::getType() const
{
	return s_type;
}

/////////////////////////////////////////////////////////////////////////////
const String OperationContext::StringData::s_type("StringData");

/////////////////////////////////////////////////////////////////////////////
void  
OperationContext::setStringData(const String& key, const String& str)
{
	setData(key, DataRef(new StringData(str)));
}

/////////////////////////////////////////////////////////////////////////////
String  
OperationContext::getStringData(const String& key) const
{
	IntrusiveReference<StringData> strData = getDataAs<StringData>(key);
	if (!strData)
	{
		OW_THROW(ContextDataNotFoundException, key.c_str());
	}
	return strData->getString();
}

/////////////////////////////////////////////////////////////////////////////
String  
OperationContext::getStringDataWithDefault(const String& key, const String& def) const
{
	IntrusiveReference<StringData> strData = getDataAs<StringData>(key);
	if (!strData)
	{
		return def;
	}
	return strData->getString();
}

/////////////////////////////////////////////////////////////////////////////
OperationContext::DataRef
OperationContext::getData(const String& key) const
{
	DataRef rval;
	if (doGetData(key, rval))
	{
		return rval;
	}
	else
	{
		return DataRef();
	}
}

/////////////////////////////////////////////////////////////////////////////
const char* const OperationContext::USER_NAME = "USER_NAME";
const char* const OperationContext::USER_PASSWD = "USER_PASSWD";
const char* const OperationContext::HTTP_PATH = "HTTP_PATH";
const char* const OperationContext::CURUSER_UIDKEY = "CURUSER_UIDKEY";
const char* const OperationContext::SESSION_LANGUAGE_KEY = "SESSION_LANGUAGE_KEY";
const char* const OperationContext::HTTP_ACCEPT_LANGUAGE_KEY = "HTTP_ACCEPT_LANGUAGE_KEY";
const char* const OperationContext::CLIENT_IPADDR = "CLIENT_IPADDR"; 


} // end namespace OW_NAMESPACE




