/*******************************************************************************
* Copyright (C) 2004 Quest Software, Inc. All rights reserved.
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

#ifndef OW_RESULT_HANDLERS_HPP_INCLUDE_GUARD_
#define OW_RESULT_HANDLERS_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_ResultHandlerIFC.hpp"
#include "blocxx/StringEnumeration.hpp"

namespace OW_NAMESPACE
{

/////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API CIMClassEnumBuilder : public CIMClassResultHandlerIFC
{
public:
	CIMClassEnumBuilder(CIMClassEnumeration& e) : m_e(e) {}
	virtual ~CIMClassEnumBuilder();
protected:
	virtual void doHandle(const CIMClass &c);
private:
	CIMClassEnumeration& m_e;
};

/////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API CIMClassArrayBuilder : public CIMClassResultHandlerIFC
{
public:
	CIMClassArrayBuilder(CIMClassArray& a) : m_a(a) {}
	virtual ~CIMClassArrayBuilder();
protected:
	virtual void doHandle(const CIMClass &c);
private:
	CIMClassArray& m_a;
};

/////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API StringArrayBuilder : public StringResultHandlerIFC
{
public:
	StringArrayBuilder(blocxx::StringArray& a) : m_a(a) {}
	virtual ~StringArrayBuilder();
protected:
	virtual void doHandle(const blocxx::String &s);
private:
	blocxx::StringArray& m_a;
};

/////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API StringEnumBuilder : public StringResultHandlerIFC
{
public:
	StringEnumBuilder(blocxx::StringEnumeration& e) : m_e(e) {}
	virtual ~StringEnumBuilder();
protected:
	virtual void doHandle(const blocxx::String &s);
private:
	blocxx::StringEnumeration& m_e;
};

/////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API CIMObjectPathEnumBuilder : public CIMObjectPathResultHandlerIFC
{
public:
	CIMObjectPathEnumBuilder(CIMObjectPathEnumeration& e) : m_e(e) {}
	virtual ~CIMObjectPathEnumBuilder();
protected:
	virtual void doHandle(const CIMObjectPath &cop);
private:
	CIMObjectPathEnumeration& m_e;
};

/////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API CIMObjectPathArrayBuilder : public CIMObjectPathResultHandlerIFC
{
public:
	CIMObjectPathArrayBuilder(CIMObjectPathArray& a) : m_a(a) {}
	virtual ~CIMObjectPathArrayBuilder();
protected:
	virtual void doHandle(const CIMObjectPath &cop);
private:
	CIMObjectPathArray& m_a;
};

/////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API CIMInstanceEnumBuilder : public CIMInstanceResultHandlerIFC
{
public:
	CIMInstanceEnumBuilder(CIMInstanceEnumeration& e) : m_e(e) {}
	virtual ~CIMInstanceEnumBuilder();
protected:
	virtual void doHandle(const CIMInstance &i);
private:
	CIMInstanceEnumeration& m_e;
};

/////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API CIMInstanceArrayBuilder : public CIMInstanceResultHandlerIFC
{
public:
	CIMInstanceArrayBuilder(CIMInstanceArray& a) : m_a(a) {}
	virtual ~CIMInstanceArrayBuilder();
protected:
	virtual void doHandle(const CIMInstance &i);
private:
	CIMInstanceArray& m_a;
};

/////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API CIMQualifierTypeEnumBuilder : public CIMQualifierTypeResultHandlerIFC
{
public:
	CIMQualifierTypeEnumBuilder(CIMQualifierTypeEnumeration& e) : m_e(e) {}
	virtual ~CIMQualifierTypeEnumBuilder();
protected:
	virtual void doHandle(const CIMQualifierType &qt);
private:
	CIMQualifierTypeEnumeration& m_e;
};

/////////////////////////////////////////////////////////////////////////////
class OW_COMMON_API CIMQualifierTypeArrayBuilder : public CIMQualifierTypeResultHandlerIFC
{
public:
	CIMQualifierTypeArrayBuilder(CIMQualifierTypeArray& a) : m_a(a) {}
	virtual ~CIMQualifierTypeArrayBuilder();
protected:
	virtual void doHandle(const CIMQualifierType &qt);
private:
	CIMQualifierTypeArray& m_a;
};

} // end namespace OW_NAMESPACE

#endif

