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

#ifndef OWBI1_CIM_PROPERTY_LIST_HPP_INCLUDE_GUARD_
#define OWBI1_CIM_PROPERTY_LIST_HPP_INCLUDE_GUARD_
#include "OWBI1_config.h"
#include "OWBI1_SerializableIFC.hpp"
#include "OWBI1_COWIntrusiveReference.hpp"
#include "OWBI1_CIMName.hpp" // necessary for implicit conversion (const char* -> CIMName) to work
#include "OWBI1_SortedVectorSet.hpp"

namespace OWBI1
{

class OWBI1_OWBI1PROVIFC_API CIMPropertyList : public SerializableIFC
{
public:
	typedef SortedVectorSet<CIMName>::const_iterator const_iterator;

	/**
	 * Create an "All Properties" PropertyList, referred to as a NULL PropertyList in the WBEM specification.
	 */
	CIMPropertyList();

	/**
	 * Create a property list which for the property names in properties. Duplicate names are okay.
	 */
	CIMPropertyList(const StringArray& properties);
	
	/**
	 * Create a property list which for the property names in properties. Duplicate names are okay.
	 */
	CIMPropertyList(const CIMNameArray& properties);
	
	~CIMPropertyList();
	CIMPropertyList(const CIMPropertyList& other);
	CIMPropertyList& operator=(const CIMPropertyList& other);

	/**
	 * Get the begin iterator to the set of property names. If this is an All Properties PropertyList, the set will be empty.
	 */
	const_iterator begin() const;
	
	/**
	 * Get the end iterator to the set of property names. If this is an All Properties PropertyList, the set will be empty.
	 */
	const_iterator end() const;

	/**
	 * Test if this object represents a request for all properties (aka a NULL PropertyList).
	 */
	bool isAllProperties() const;

	/**
	 * Test if this CIMPropertyList contains property propertyName.
	 */
	bool hasProperty(const CIMName& propertyName) const;

	/**
	 * Read this CIMPropertyList object from an input stream.
	 * @param istrm The input stream.
	 */
	virtual void readObject(std::istream &istrm);
	
	/**
	 * Write this CIMPropertyList object to an output stream
	 * @param ostrm The output stream.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

private:
	struct Rep;

	COWIntrusiveReference<Rep> m_pdata;

};

} // end namespace OWBI1

#endif

