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

#ifndef OW_CIMMETHOD_HPP_
#define OW_CIMMETHOD_HPP_


#include "OW_config.h"

#include "OW_Reference.hpp"
#include "OW_CIMElement.hpp"
#include "OW_CIMFwd.hpp"

/**
 * The OW_CIMMethod class encapsulates all data and behaviour pertinent to an
 * CIM method. OW_CIMMethods are ref counted, copy on write objects.
 * It is possible to have an OW_CIMMethod object that is NULL. The method
 * to check for this condition is as follows:
 *
 *		OW_CIMMethod ch = cc.getMethod(...);
 *		if(!ch)
 *		{
 *			// Null method
 *		}
 *		else
 *		{
 *			// Valid method
 *		}
 */
class OW_CIMMethod : public OW_CIMElement
{
public:

	struct METHData;

	/**
	 * Create a new OW_CIMMethod object.
	 * @param notNull If false, This object will not contain any data/
	 * 	implementation, which will cause all subsequent operations to fail.
	 *		If true, this object will be a valid OW_CIMMethod with default values.
	 */
	explicit OW_CIMMethod(OW_Bool notNull=false);

	/**
	 * Create an OW_CIMMethod with a name
	 * @param name	The name of this OW_CIMMethod.
	 */
	explicit OW_CIMMethod(const OW_String& name);

	/**
	 * Create an OW_CIMMethod with a name
	 * @param name	The name of this OW_CIMMethod as a NULL terminated string.
	 */
	explicit OW_CIMMethod(const char* name);

	/**
	 * Create an OW_CIMMethod from an XML definition.
	 * @param node	The OW_XMLNode that contains the definition of this method.
	 */
	//OW_CIMMethod(const OW_XMLNode& node);

	/**
	 * Copy constructor
	 * @param arg The OW_CIMMethod this object will be a copy of.
	 */
	OW_CIMMethod(const OW_CIMMethod& arg);

	/**
	 * Destroy this OW_CIMMethod object.
	 */
	~OW_CIMMethod();

	/**
	 * Set this to a null object. All subsequent operation will fail after this
	 * method is called.
	 */
	virtual void setNull();

	/**
	 * Assignment operator
	 * @param arg The OW_CIMMethod to assign to this object.
	 * @return A reference to this OW_CIMMethod after the assignment has taken
	 * place.
	 */
	OW_CIMMethod& operator= (const OW_CIMMethod& arg);

	/**
	 * Set the qualifiers for this method.
	 * @param quals An OW_CIMQualifierArray that contains the qualifiers for
	 *		this OW_CIMInstance.
	 */
	void setQualifiers(const OW_CIMQualifierArray& quals);

	/**
	 * Add a qualifier to this OW_CIMMethod.
	 * @param qual The OW_CIMQualifier to add to this method.
	 */
	void addQualifier(const OW_CIMQualifier& qual);

	/**
	 * Get the qualifiers for this method.
	 * @return An OW_CIMQualifierArray that contains the qualifiers for this
	 * method.
	 */
	OW_CIMQualifierArray getQualifiers() const;

	/**
	 * Get the qualifier associated with a specified name.
	 * @param name	The name of the qualifier to retrieve.
	 * @return The OW_CIMQualifier associated with the given name if found.
	 * Otherwise a NULL OW_CIMQualifier.
	 */
	OW_CIMQualifier getQualifier(const OW_String& name) const;

	/**
	 * Get the origin class for this method.
	 * @return The name of the origin class.
	 */
	OW_String getOriginClass() const;

	/**
	 * Set thr origin class for this method.
	 * @param originCls	The name of the origin class for this method.
	 */
	void setOriginClass(const OW_String& originCls);
	
	/**
	 * Add a parameter to this OW_CIMMethod.
	 * @param param The OW_CIMParameter to add to this method.
	 */
	void addParameter(const OW_CIMParameter& param);

	/**
	 * Set the parameters for this method
	 * @param inParms	An OW_CIMParameterArray that contains the parameters for
	 * 	this method.
	 */
	void setParameters(const OW_CIMParameterArray& inParms);

	/**
	 * Get the parameters for this method.
	 * @return An OW_CIMParameterArray that contains the parameters for this
	 * method.
	 */
	OW_CIMParameterArray getParameters() const;

	/**
	 * Get the IN parameters for this method.
	 * @return An OW_CIMParameterArray that contains IN the parameters for this
	 * method.
	 */
	OW_CIMParameterArray getINParameters() const;

	/**
	 * Get the OUT parameters for this method.
	 * @return An OW_CIMParameterArray that contains OUT the parameters for this
	 * method.
	 */
	OW_CIMParameterArray getOUTParameters() const;

	/**
	 * Set the return data type for this parameter
	 * @param type	An OW_CIMDataType object that represents the type.
	 */
	void setReturnType(const OW_CIMDataType& type);

	/**
	 * @return The return data type for this method.
	 */
	OW_CIMDataType getReturnType() const;

	/**
	 * @return The size of the return data type.
	 */
	OW_Int32 getReturnDataSize() const;

	/**
	 * Set name of overriding method.
	 * @param omname The name of the overriding method.
	 */
	void setOverridingMethod(const OW_String& omname);

	/**
	 * @return The name of the overriding method.
	 */
	OW_String getOverridingMethod() const;

	/**
	 * Set the propagated flag for this method.
	 * @param propagated If true method is propagated. Otherwise false.
	 */
	void setPropagated(OW_Bool propagated=true);

	/**
	 * Get the propagated flag.
	 * @return true if this method is propagated. Otherwise false.
	 */
	OW_Bool getPropagated() const;

	/**
	 * Create an OW_CIMMethod object based on this one, using the criteria
	 * specified in the parameters.
	 *
	 * @param includeQualifiers If false, no qualifiers will be included in the
	 *		OW_CIMMethod returned. Otherwise all qualifiers are included.
	 * @param includeClassOrigin I false, the class origin will not be included
	 *		in the OW_CIMMethod returned. Otherwise the class origin is included
	 *
	 * @return A new OW_CIMMethod that is a copy of this one with the
	 * 		qualifiers and class origin optionally exclued.
	 */
	OW_CIMMethod clone(OW_Bool includeQualifiers=true,
		OW_Bool includeClassOrigin=true) const;

	/**
	 * @return The name of this method as an OW_String.
	 */
	virtual OW_String getName() const;

	/**
	 * Set the name of this method.
	 * @param name The new name for this method.
	 */
	virtual void setName(const OW_String& name);

	/**
	 * Read this object from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream &istrm);

	/**
	 * Write this object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

	/**
	 * @return The MOF representation of this object as an OW_String.
	 */
	virtual OW_String toMOF() const;

	/**
	 * @return The string representation of this method as an OW_String.
	 */
	virtual OW_String toString() const;

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

protected:

	OW_Reference<METHData> m_pdata;

	friend bool operator<(const OW_CIMMethod& x, const OW_CIMMethod& y);
};

#endif	// __OW_CIMMETHOD_HPP__


