/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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

#include "OWBI1_config.h"
#include "OWBI1_CIMPropertyList.hpp"
#include "OWBI1_Array.hpp"
#include "OWBI1_COWIntrusiveCountableBase.hpp"

namespace OWBI1
{

struct CIMPropertyList::Rep : public COWIntrusiveCountableBase
{
	Rep()
		: allProperties(true)
	{
	}

	Rep(const CIMNameArray& n)
		: allProperties(false)
		, names(n.begin(), n.end())
	{
	}

	Rep(const StringArray& n)
		: allProperties(false)
		, names(n.begin(), n.end())
	{
	}

	bool allProperties;
	SortedVectorSet<CIMName> names;
};

/////////////////////////////////////////////////////////////////////////////
CIMPropertyList::CIMPropertyList()
	: m_pdata(new Rep)
{

}

/////////////////////////////////////////////////////////////////////////////
CIMPropertyList::CIMPropertyList(const StringArray& properties)
	: m_pdata(new Rep(properties))
{

}

/////////////////////////////////////////////////////////////////////////////
CIMPropertyList::CIMPropertyList(const CIMNameArray& properties)
	: m_pdata(new Rep(properties))
{

}

/////////////////////////////////////////////////////////////////////////////
CIMPropertyList::~CIMPropertyList()
{
}

/////////////////////////////////////////////////////////////////////////////
CIMPropertyList::CIMPropertyList(const CIMPropertyList& other)
	: SerializableIFC(other)
	, m_pdata(other.m_pdata)
{
}

/////////////////////////////////////////////////////////////////////////////
CIMPropertyList& 
CIMPropertyList::operator=(const CIMPropertyList& other)
{
	m_pdata = other.m_pdata;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
CIMPropertyList::const_iterator 
CIMPropertyList::begin() const
{
	return m_pdata->names.begin();
}

/////////////////////////////////////////////////////////////////////////////
CIMPropertyList::const_iterator 
CIMPropertyList::end() const
{
	return m_pdata->names.end();
}

/////////////////////////////////////////////////////////////////////////////
bool 
CIMPropertyList::isAllProperties() const
{
	return m_pdata->allProperties;
}

/////////////////////////////////////////////////////////////////////////////
bool 
CIMPropertyList::hasProperty(const CIMName& propertyName) const
{
	if (m_pdata->allProperties)
	{
		return true;
	}
	return m_pdata->names.count(propertyName) == 1;
}

/////////////////////////////////////////////////////////////////////////////
void
CIMPropertyList::readObject(std::istream &istrm)
{

}

/////////////////////////////////////////////////////////////////////////////
void 
CIMPropertyList::writeObject(std::ostream &ostrm) const
{

}



} // end namespace OWBI1



