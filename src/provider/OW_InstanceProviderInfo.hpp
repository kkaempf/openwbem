/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_INSTANCE_PROVIDER_INFO_HPP_INCLUDE_GUARD_
#define OW_INSTANCE_PROVIDER_INFO_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Array.hpp"

class OW_InstanceProviderInfo
{
public:
	struct ClassInfo
	{
		explicit ClassInfo(OW_String const& className_)
			: className(className_)
		{}
		ClassInfo(OW_String const& className_, OW_StringArray const& namespaces_)
			: className(className_)
			, namespaces(namespaces_)
		{}
		OW_String className;
		OW_StringArray namespaces;
	};

	typedef OW_Array<ClassInfo> ClassInfoArray;

	/**
	 * Add a class name to the list of instrumented classes for the provider.
	 * This will not have a specific namespace associated with it, it will be
	 * associated to all namespaces.
	 * @param className The class name.
	 */
	void addInstrumentedClass(OW_String const& className)
	{
		m_instrumentedClasses.push_back(ClassInfo(className));
	}
	void addInstrumentedClass(ClassInfo const& classInfo)
	{
		m_instrumentedClasses.push_back(classInfo);
	}
	
	const ClassInfoArray& getClassInfo() const
	{
		return m_instrumentedClasses;
	}

	void setProviderName(OW_String const& name)
	{
		m_name = name;
	}

	OW_String getProviderName() const
	{
		return m_name;
	}

private:
	ClassInfoArray m_instrumentedClasses;
	OW_String m_name;

};

typedef OW_Array<OW_InstanceProviderInfo> OW_InstanceProviderInfoArray;

// may need to make this a separate class
typedef OW_InstanceProviderInfo OW_AssociatorProviderInfo;
typedef OW_Array<OW_AssociatorProviderInfo> OW_AssociatorProviderInfoArray;

#endif


