/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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
#ifndef OW_RESULT_HANDLERS_HPP_INCLUDE_GUARD_
#define OW_RESULT_HANDLERS_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_ResultHandlerIFC.hpp"

namespace OpenWBEM
{


class CIMClassEnumBuilder : public CIMClassResultHandlerIFC
{
public:
	CIMClassEnumBuilder(CIMClassEnumeration& e) : m_e(e) {}
protected:
	virtual void doHandle(const CIMClass &c)
	{
		m_e.addElement(c);
	}
private:
	CIMClassEnumeration& m_e;
};

class CIMClassArrayBuilder : public CIMClassResultHandlerIFC
{
public:
	CIMClassArrayBuilder(CIMClassArray& a) : m_a(a) {}
protected:
	virtual void doHandle(const CIMClass &c)
	{
		m_a.push_back(c);
	}
private:
	CIMClassArray& m_a;
};

class StringArrayBuilder : public StringResultHandlerIFC
{
public:
	StringArrayBuilder(StringArray& a) : m_a(a) {}
protected:
	virtual void doHandle(const String &s)
	{
		m_a.push_back(s);
	}
private:
	StringArray& m_a;
};

class StringEnumBuilder : public StringResultHandlerIFC
{
public:
	StringEnumBuilder(StringEnumeration& e) : m_e(e) {}
protected:
	virtual void doHandle(const String &s)
	{
		m_e.addElement(s);
	}
private:
	StringEnumeration& m_e;
};

class CIMObjectPathEnumBuilder : public CIMObjectPathResultHandlerIFC
{
public:
	CIMObjectPathEnumBuilder(CIMObjectPathEnumeration& e) : m_e(e) {}
protected:
	virtual void doHandle(const CIMObjectPath &cop)
	{
		m_e.addElement(cop);
	}
private:
	CIMObjectPathEnumeration& m_e;
};
class CIMObjectPathArrayBuilder : public CIMObjectPathResultHandlerIFC
{
public:
	CIMObjectPathArrayBuilder(CIMObjectPathArray& a) : m_a(a) {}
protected:
	virtual void doHandle(const CIMObjectPath &cop)
	{
		m_a.push_back(cop);
	}
private:
	CIMObjectPathArray& m_a;
};
class CIMInstanceEnumBuilder : public CIMInstanceResultHandlerIFC
{
public:
	CIMInstanceEnumBuilder(CIMInstanceEnumeration& e) : m_e(e) {}
protected:
	virtual void doHandle(const CIMInstance &i)
	{
		m_e.addElement(i);
	}
private:
	CIMInstanceEnumeration& m_e;
};
class CIMInstanceArrayBuilder : public CIMInstanceResultHandlerIFC
{
public:
	CIMInstanceArrayBuilder(CIMInstanceArray& a) : m_a(a) {}
protected:
	virtual void doHandle(const CIMInstance &i)
	{
		m_a.push_back(i);
	}
private:
	CIMInstanceArray& m_a;
};

class CIMQualifierTypeEnumBuilder : public CIMQualifierTypeResultHandlerIFC
{
public:
	CIMQualifierTypeEnumBuilder(CIMQualifierTypeEnumeration& e) : m_e(e) {}
protected:
	virtual void doHandle(const CIMQualifierType &qt)
	{
		m_e.addElement(qt);
	}
private:
	CIMQualifierTypeEnumeration& m_e;
};
class CIMQualifierTypeArrayBuilder : public CIMQualifierTypeResultHandlerIFC
{
public:
	CIMQualifierTypeArrayBuilder(CIMQualifierTypeArray& a) : m_a(a) {}
protected:
	virtual void doHandle(const CIMQualifierType &qt)
	{
		m_a.push_back(qt);
	}
private:
	CIMQualifierTypeArray& m_a;
};

} // end namespace OpenWBEM

#endif

