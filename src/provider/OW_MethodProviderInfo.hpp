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

#ifndef OW_METHOD_PROVIDER_INFO_HPP_INCLUDE_GUARD_
#define OW_METHOD_PROVIDER_INFO_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Array.hpp"

namespace OpenWBEM
{

class OW_PROVIDER_API MethodProviderInfo
{
public:
	struct ClassInfo
	{
		explicit ClassInfo(String const& className_);
		ClassInfo(String const& className_, StringArray const& namespaces_, StringArray const& methods_);
		String className;
		StringArray namespaces;
		StringArray methods;
	};
	typedef Array<ClassInfo> ClassInfoArray;
	/**
	 * Add a class name to the list of instrumented classes for the provider.
	 * This will not have a specific namespace associated with it, it will be
	 * associated to all namespaces.
	 * It will not have one method associated with it, it will be assumed that
	 * the provider handles all the methods in the class
	 * @param className The class name.
	 */
	void addInstrumentedClass(String const& className);
	void addInstrumentedClass(ClassInfo const& classInfo);
	
	const ClassInfoArray& getClassInfo() const;
	void setProviderName(String const& name);
	String getProviderName() const;
private:
	ClassInfoArray m_instrumentedClasses;
	String m_name;
};
typedef Array<MethodProviderInfo> MethodProviderInfoArray;

} // end namespace OpenWBEM

#endif
