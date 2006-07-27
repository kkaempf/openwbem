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

#ifndef OW_WQL_INSTANCE_PROPERTY_SOURCE_HPP_INCLUDE_GUARD
#define OW_WQL_INSTANCE_PROPERTY_SOURCE_HPP_INCLUDE_GUARD
#include "OW_config.h"
#include "OW_WQLPropertySource.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_String.hpp"

namespace OW_NAMESPACE
{

class OW_COMMON_API WQLInstancePropertySource : public WQLPropertySource
{
public:
	WQLInstancePropertySource(const CIMInstance& ci_,
		const CIMOMHandleIFCRef& hdl,
		const String& ns)
		: ci(ci_)
		, m_hdl(hdl)
		, m_ns(ns)
	{
	}
	~WQLInstancePropertySource();
	virtual bool evaluateISA(const String &propertyName, const String &className) const;
	virtual bool getValue(const String &propertyName, WQLOperand &value) const;
private:
	// This is for recursion on embedded instances
	bool evaluateISAAux(const CIMInstance& ci, StringArray propNames, const String &className) const;
	bool classIsDerivedFrom(const String& cls, const String& className) const;
	// This is for recursion on embedded instances
	static bool getValueAux(const CIMInstance& ci, const StringArray& propNames, WQLOperand& value);
private:
	CIMInstance ci;

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	CIMOMHandleIFCRef m_hdl;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	String m_ns;
};

} // end namespace OW_NAMESPACE

#endif
