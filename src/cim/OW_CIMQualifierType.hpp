/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_CIMQUALIFIERTYPE_HPP_INCLUDE_GUARD_
#define OW_CIMQUALIFIERTYPE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMElement.hpp"
#include "blocxx/COWIntrusiveReference.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMNULL.hpp"
#include "OW_CIMName.hpp" // necessary for implicit conversion (const char* -> CIMName) to work
#include "OW_SafeBool.hpp"

namespace OW_NAMESPACE
{

/**
 * The CIMQualifierType class encapsulates the data and functionality
 * associated with a CIM qualifier type.
 */
class OW_COMMON_API CIMQualifierType : public CIMElement
{
public:
	struct QUALTData;
	/**
	 * Create a new CIMQualifierType object.
	 */
	CIMQualifierType();
	/**
	 * Create a NULL CIMQualifierType object.
	 */
	explicit CIMQualifierType(CIMNULL_t);
	/**
	 * Create an CIMQualifierType with a given name.
	 * @param name	The name for this CIMQualifierType object.
	 */
	CIMQualifierType(const CIMName& name);
	/**
	 * Create an CIMQualifierType with a given name.
	 * @param name	The name for this CIMQualifierType object as a NULL
	 *		terminated string.
	 */
	CIMQualifierType(const char* name);
	/**
	 * Copy ctor
	 * @param arg The CIMQualifierType to make this object a copy of.
	 */
	CIMQualifierType(const CIMQualifierType& arg);
	/**
	 * Destroy this CIMQualifierType object.
	 */
	~CIMQualifierType();
	/**
	 * Set this to a null object.
	 */
	virtual void setNull();
	/**
	 * Assignment operator
	 * @param arg The CIMQualifierType to assign to this object.
	 * @return A reference to this object after the assignment has been made.
	 */
	CIMQualifierType& operator= (const CIMQualifierType& arg);
	/**
	 * @return the Scopes for this CIMQualifierType object
	 */
	const CIMScopeArray& getScope() const;
	/**
	 * @return The data type for this CIMQualifierType object.
	 */
	CIMDataType getDataType() const;
	/**
	 * @return The size of the data for CIMQualifierType object.
	 */
	Int32 getDataSize() const;
	/**
	 * @return The default value for this CIMQualifierType object.
	 */
	CIMValue getDefaultValue() const;
	/**
	 * Set the data type for this CIMQualifierType object.
	 * @param dataType	The CIMDataType for this CIMQualifierType object.
	 * @return a reference to *this
	 */
	CIMQualifierType& setDataType(const CIMDataType& dataType);
	/**
	 * Set the data type for this CIMQualifierType object.
	 * @param dataType	The CIMDataType::Type for this CIMQualifierType object.
	 * @return a reference to *this
	 */
	CIMQualifierType& setDataType(const CIMDataType::Type& dataType);
	/**
	 * Set the default value for this CIMQualifierType object.
	 * @param defValue	The default value for this CIMQualifierType.
	 * @return a reference to *this
	 */
	CIMQualifierType& setDefaultValue(const CIMValue& defValue);
	/**
	 * Add a scope to this CIMQualifierType object.
	 * @param newScope	The scope to add to this CIMQualifierType.
	 * @return a reference to *this
	 */
	CIMQualifierType& addScope(const CIMScope& newScope);
	/**
	 * Determine if this CIMQualifierType has a specified scope.
	 * @param scopeArg	The scope to check for.
	 * @return true if this CIMQualifierType has the given scope. Otherwise
	 * false.
	 */
	bool hasScope(const CIMScope& scopeArg) const;
	/**
	 * Determine if this CIMQualifierType has a specified flavor.
	 * @param flavorArg	The flavor to check for.
	 * @return true if this CIMQualifierType has the given flavor. Otherwise
	 * false.
	 */
	bool hasFlavor(const CIMFlavor& flavorArg) const;
	/**
	 * Add a flavor to this CIMQualifierType object.
	 * @param newFlavor	The flavor to add to this CIMQualifierType object.
	 * @return a reference to *this
	 */
	CIMQualifierType& addFlavor(const CIMFlavor& newFlavor);
	/**
	 * Remove a flavor from this CIMQualifierType.
	 * @param flavor The integral value of the flavor to remove.
	 * @return a reference to *this
	 */
	CIMQualifierType& removeFlavor(const Int32 flavor);
	/**
	 * @return The flavors for this CIMQualifierType object.
	 */
	CIMFlavorArray getFlavors() const;
	/**
	 * @return true this CIMQualifierType has a default value. Otherwise
	 * false.
	 */
	bool hasDefaultValue() const;
	/**
	 * @return true if the default value is an array type. Otherwise false.
	 */
	bool isDefaultValueArray() const;
	/**
	 * Write this CIMQualifierType object to the given output stream.
	 * @param ostrm	The output stream to write this CIMQualifierType to.
	 */
	virtual void writeObject(std::streambuf & ostrm) const;
	/**
	 * Read this CIMQualifierType object from the given input stream.
	 * @param istrm	The input stream to read this CIMQualifierType from.
	 */
	virtual void readObject(std::streambuf & istrm);
	
	/**
	 * @return An String representing this CIMQualifierType.
	 */
	virtual String toString() const;
	/**
	 * @return An String that contains the MOF representation of this
	 * CIMQualifierType object.
	 */
	virtual String toMOF() const;
	
	/**
	 * @return The name of this qualifier type.
	 */
	virtual String getName() const;
	/**
	 * Set the name of this qualifier type.
	 * @param name The new name for this qualifier type.
	 */
	virtual void setName(const CIMName& name);

	OW_SAFE_BOOL_IMPL(CIMQualifierType, COWIntrusiveReference<QUALTData>, CIMQualifierType::m_pdata, m_pdata)

	friend OW_COMMON_API bool operator<(const CIMQualifierType& x, const CIMQualifierType& y);
private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	COWIntrusiveReference<QUALTData> m_pdata;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

};

} // end namespace OW_NAMESPACE

#endif
