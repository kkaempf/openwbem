/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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

#ifndef OW_PROVIDER_INFO_BASE_HPP_INCLUDE_GUARD_
#define OW_PROVIDER_INFO_BASE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Array.hpp"

///////////////////////////////////////////////////////////////////////////////
struct OW_InstClassInfo
{
	explicit OW_InstClassInfo(OW_String const& className_);
	OW_InstClassInfo(OW_String const& className_, OW_StringArray const& namespaces_);
	~OW_InstClassInfo();
	OW_String className;
	OW_StringArray namespaces;
};

///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
class OW_ProviderInfoBase
{
public:

	typedef ClassInfoT ClassInfo;
	typedef OW_Array<ClassInfoT> ClassInfoArray;

	virtual ~OW_ProviderInfoBase();

	/**
	 * Add a class name to the list of instrumented classes for the provider.
	 * This will not have a specific namespace associated with it, it will be
	 * associated to all namespaces.
	 * @param className The class name.
	 */
	void addInstrumentedClass(OW_String const& className);
	void addInstrumentedClass(ClassInfoT const& classInfo);
	const ClassInfoArray& getClassInfo() const;
	void setProviderName(OW_String const& name);
	OW_String getProviderName() const;

private:
	ClassInfoArray m_instrumentedClasses;
	OW_String m_name;

};

///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
OW_ProviderInfoBase<ClassInfoT>::~OW_ProviderInfoBase() 
{
}

///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
void 
OW_ProviderInfoBase<ClassInfoT>::addInstrumentedClass(OW_String const& className)
{
	m_instrumentedClasses.push_back(ClassInfoT(className));
}

///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
void 
OW_ProviderInfoBase<ClassInfoT>::addInstrumentedClass(ClassInfoT const& classInfo)
{
	m_instrumentedClasses.push_back(classInfo);
}

///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
const typename OW_ProviderInfoBase<ClassInfoT>::ClassInfoArray& 
OW_ProviderInfoBase<ClassInfoT>::getClassInfo() const
{
	return m_instrumentedClasses;
}

///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
void 
OW_ProviderInfoBase<ClassInfoT>::setProviderName(OW_String const& name)
{
	m_name = name;
}

///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
OW_String 
OW_ProviderInfoBase<ClassInfoT>::getProviderName() const
{
	return m_name;
}


#endif


