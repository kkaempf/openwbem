/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_CIMQUALIFIERTYPE_HPP_INCLUDE_GUARD_
#define OW_CIMQUALIFIERTYPE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_CIMElement.hpp"
#include "OW_COWReference.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMNULL.hpp"

/**
 * The OW_CIMQualifierType class encapsulates the data and functionality
 * associated with a CIM qualifier type.
 */
class OW_CIMQualifierType : public OW_CIMElement
{
public:
	struct QUALTData;

	/**
	 * Create a new OW_CIMQualifierType object.
	 */
	OW_CIMQualifierType();

	/**
	 * Create a NULL OW_CIMQualifierType object.
	 */
	explicit OW_CIMQualifierType(OW_CIMNULL_t);

	/**
	 * Create an OW_CIMQualifierType with a given name.
	 * @param name	The name for this OW_CIMQualifierType object.
	 */
	OW_CIMQualifierType(const OW_String& name);

	/**
	 * Create an OW_CIMQualifierType with a given name.
	 * @param name	The name for this OW_CIMQualifierType object as a NULL
	 *		terminated string.
	 */
	OW_CIMQualifierType(const char* name);

	/**
	 * Copy ctor
	 * @param arg The OW_CIMQualifierType to make this object a copy of.
	 */
	OW_CIMQualifierType(const OW_CIMQualifierType& arg);

	/**
	 * Destroy this OW_CIMQualifierType object.
	 */
	~OW_CIMQualifierType();

	/**
	 * Set this to a null object.
	 */
	virtual void setNull();

	/**
	 * Assignment operator
	 * @param arg The OW_CIMQualifierType to assign to this object.
	 * @return A reference to this object after the assignment has been made.
	 */
	OW_CIMQualifierType& operator= (const OW_CIMQualifierType& arg);

	/**
	 * @return the Scopes for this OW_CIMQualifierType object
	 */
	const OW_CIMScopeArray& getScope() const;

	/**
	 * @return The data type for this OW_CIMQualifierType object.
	 */
	OW_CIMDataType getDataType() const;

	/**
	 * @return The size of the data for OW_CIMQualifierType object.
	 */
	OW_Int32 getDataSize() const;

	/**
	 * @return The default value for this OW_CIMQualifierType object.
	 */
	OW_CIMValue getDefaultValue() const;

	/**
	 * Set the data type for this OW_CIMQualifierType object.
	 * @param dataType	The OW_CIMDataType for this OW_CIMQualifierType object.
	 * @return a reference to *this
	 */
	OW_CIMQualifierType& setDataType(const OW_CIMDataType& dataType);

	/**
	 * Set the data type for this OW_CIMQualifierType object.
	 * @param dataType	The OW_CIMDataType::Type for this OW_CIMQualifierType object.
	 * @return a reference to *this
	 */
	OW_CIMQualifierType& setDataType(const OW_CIMDataType::Type& dataType);

	/**
	 * Set the default value for this OW_CIMQualifierType object.
	 * @param defValue	The default value for this OW_CIMQualifierType.
	 * @return a reference to *this
	 */
	OW_CIMQualifierType& setDefaultValue(const OW_CIMValue& defValue);

	/**
	 * Add a scope to this OW_CIMQualifierType object.
	 * @param newScope	The scope to add to this OW_CIMQualifierType.
	 * @return a reference to *this
	 */
	OW_CIMQualifierType& addScope(const OW_CIMScope& newScope);

	/**
	 * Determine if this OW_CIMQualifierType has a specified scope.
	 * @param scopeArg	The scope to check for.
	 * @return true if this OW_CIMQualifierType has the given scope. Otherwise
	 * false.
	 */
	OW_Bool hasScope(const OW_CIMScope& scopeArg) const;

	/**
	 * Determine if this OW_CIMQualifierType has a specified flavor.
	 * @param flavorArg	The flavor to check for.
	 * @return true if this OW_CIMQualifierType has the given flavor. Otherwise
	 * false.
	 */
	OW_Bool hasFlavor(const OW_CIMFlavor& flavorArg) const;

	/**
	 * Add a flavor to this OW_CIMQualifierType object.
	 * @param newFlavor	The flavor to add to this OW_CIMQualifierType object.
	 * @return a reference to *this
	 */
	OW_CIMQualifierType& addFlavor(const OW_CIMFlavor& newFlavor);

	/**
	 * Remove a flavor from this OW_CIMQualifierType.
	 * @param flavor The integral value of the flavor to remove.
	 * @return a reference to *this
	 */
	OW_CIMQualifierType& removeFlavor(const OW_Int32 flavor);

	/**
	 * @return The flavors for this OW_CIMQualifierType object.
	 */
	OW_CIMFlavorArray getFlavors() const;

	/**
	 * @return true this OW_CIMQualifierType has a default value. Otherwise
	 * false.
	 */
	OW_Bool hasDefaultValue() const;

	/**
	 * @return true if the default value is an array type. Otherwise false.
	 */
	OW_Bool isDefaultValueArray() const;

	/**
	 * Write this OW_CIMQualifierType object to the given output stream.
	 * @param ostrm	The output stream to write this OW_CIMQualifierType to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

	/**
	 * Read this OW_CIMQualifierType object from the given input stream.
	 * @param istrm	The input stream to read this OW_CIMQualifierType from.
	 */
	virtual void readObject(std::istream &istrm);
	
	/**
	 * @return An OW_String representing this OW_CIMQualifierType.
	 */
	virtual OW_String toString() const;

	/**
	 * @return An OW_String that contains the MOF representation of this
	 * OW_CIMQualifierType object.
	 */
	virtual OW_String toMOF() const;
	
	/**
	 * @return The name of this qualifier type.
	 */
	virtual OW_String getName() const;

	/**
	 * Set the name of this qualifier type.
	 * @param name The new name for this qualifier type.
	 */
	virtual void setName(const OW_String& name);

private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (!m_pdata.isNull()) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (!m_pdata.isNull()) ? 0: &dummy::nonnull; }

	friend bool operator<(const OW_CIMQualifierType& x, const OW_CIMQualifierType& y);

private:

	OW_COWReference<QUALTData> m_pdata;
};

#endif

