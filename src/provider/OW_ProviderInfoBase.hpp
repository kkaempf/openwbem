/*******************************************************************************
* Copyright (C) 2003-2004 Quest Software, Inc. All rights reserved.
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

#ifndef OW_PROVIDER_INFO_BASE_HPP_INCLUDE_GUARD_
#define OW_PROVIDER_INFO_BASE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/String.hpp"
#include "blocxx/Array.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

///////////////////////////////////////////////////////////////////////////////
struct OW_PROVIDER_API InstClassInfo
{
	explicit InstClassInfo(blocxx::String const& className_);
	InstClassInfo(blocxx::String const& className_, blocxx::StringArray const& namespaces_);
	~InstClassInfo();
	blocxx::String className;
	blocxx::StringArray namespaces;
};
///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
class ProviderInfoBase
{
public:
	typedef ClassInfoT ClassInfo;
	typedef blocxx::Array<ClassInfoT> ClassInfoArray;
	virtual ~ProviderInfoBase();
	/**
	 * Add a class name to the list of instrumented classes for the provider.
	 * This will not have a specific namespace associated with it, it will be
	 * associated to all namespaces.
	 * @param className The class name.
	 */
	void addInstrumentedClass(blocxx::String const& className);
	void addInstrumentedClass(ClassInfoT const& classInfo);
	const ClassInfoArray& getClassInfo() const;
	void setProviderName(blocxx::String const& name);
	blocxx::String getProviderName() const;
private:
	ClassInfoArray m_instrumentedClasses;
	blocxx::String m_name;
};
///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
ProviderInfoBase<ClassInfoT>::~ProviderInfoBase()
{
}
///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
void
ProviderInfoBase<ClassInfoT>::addInstrumentedClass(blocxx::String const& className)
{
	m_instrumentedClasses.push_back(ClassInfoT(className));
}
///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
void
ProviderInfoBase<ClassInfoT>::addInstrumentedClass(ClassInfoT const& classInfo)
{
	m_instrumentedClasses.push_back(classInfo);
}
///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
const typename ProviderInfoBase<ClassInfoT>::ClassInfoArray&
ProviderInfoBase<ClassInfoT>::getClassInfo() const
{
	return m_instrumentedClasses;
}
///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
void
ProviderInfoBase<ClassInfoT>::setProviderName(blocxx::String const& name)
{
	m_name = name;
}
///////////////////////////////////////////////////////////////////////////////
template <class ClassInfoT>
blocxx::String
ProviderInfoBase<ClassInfoT>::getProviderName() const
{
	return m_name;
}

#if defined(__GNUC__)
extern template class ProviderInfoBase<InstClassInfo>;
#endif

} // end namespace OW_NAMESPACE

#endif
