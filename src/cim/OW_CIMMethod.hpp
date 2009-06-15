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

#ifndef OW_CIMMETHOD_HPP_INCLUDE_GUARD_
#define OW_CIMMETHOD_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/COWIntrusiveReference.hpp"
#include "OW_CIMElement.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMNULL.hpp"
#include "OW_WBEMFlags.hpp"
#include "OW_CIMName.hpp" // necessary for implicit conversion (const char* -> CIMName) to work
#include "OW_SafeBool.hpp"

namespace OW_NAMESPACE
{

/**
 * The CIMMethod class encapsulates all data and behaviour pertinent to an
 * CIM method. CIMMethods are ref counted, copy on write objects.
 * It is possible to have an CIMMethod object that is NULL. The method
 * to check for this condition is as follows:
 *
 *		CIMMethod ch = cc.getMethod(...);
 *		if (!ch)
 *		{
 *			// Null method
 *		}
 *		else
 *		{
 *			// Valid method
 *		}
 */
class OW_COMMON_API CIMMethod : public CIMElement
{
public:
	struct METHData;
	/**
	 * Create a new CIMMethod object.
	 */
	CIMMethod();
	/**
	 * Create a NULL CIMMethod object.
	 */
	explicit CIMMethod(CIMNULL_t);
	/**
	 * Create an CIMMethod with a name
	 * @param name	The name of this CIMMethod.
	 */
	explicit CIMMethod(const CIMName& name);
	/**
	 * Create an CIMMethod with a name
	 * @param name	The name of this CIMMethod as a NULL terminated string.
	 */
	explicit CIMMethod(const char* name);
	/**
	 * Copy constructor
	 * @param arg The CIMMethod this object will be a copy of.
	 */
	CIMMethod(const CIMMethod& arg);
	/**
	 * Destroy this CIMMethod object.
	 */
	~CIMMethod();
	/**
	 * Set this to a null object. All subsequent operation will fail after this
	 * method is called.
	 */
	virtual void setNull();
	/**
	 * Assignment operator
	 * @param arg The CIMMethod to assign to this object.
	 * @return A reference to this CIMMethod after the assignment has taken
	 * place.
	 */
	CIMMethod& operator= (const CIMMethod& arg);
	/**
	 * Set the qualifiers for this method.
	 * @param quals An CIMQualifierArray that contains the qualifiers for
	 *		this CIMInstance.
	 * @return a reference to *this
	 */
	CIMMethod& setQualifiers(const CIMQualifierArray& quals);
	/**
	 * Add a qualifier to this CIMMethod.
	 * @param qual The CIMQualifier to add to this method.
	 * @return a reference to *this
	 */
	CIMMethod& addQualifier(const CIMQualifier& qual);
	/**
	 * Get the qualifiers for this method.
	 * @return An CIMQualifierArray that contains the qualifiers for this
	 * method.
	 */
	CIMQualifierArray getQualifiers() const;
	/**
	 * Get the qualifier associated with a specified name.
	 * @param name	The name of the qualifier to retrieve.
	 * @return The CIMQualifier associated with the given name if found.
	 * Otherwise a NULL CIMQualifier.
	 */
	CIMQualifier getQualifier(const CIMName& name) const;
	/**
	 * Get the origin class for this method.
	 * @return The name of the origin class.
	 */
	String getOriginClass() const;
	/**
	 * Set thr origin class for this method.
	 * @param originCls	The name of the origin class for this method.
	 * @return a reference to *this
	 */
	CIMMethod& setOriginClass(const CIMName& originCls);
	
	/**
	 * Add a parameter to this CIMMethod.
	 * @param param The CIMParameter to add to this method.
	 * @return a reference to *this
	 */
	CIMMethod& addParameter(const CIMParameter& param);
	/**
	 * Set the parameters for this method
	 * @param inParms	An CIMParameterArray that contains the parameters for
	 * 	this method.
	 * @return a reference to *this
	 */
	CIMMethod& setParameters(const CIMParameterArray& inParms);
	/**
	 * Get the parameters for this method.
	 * @return An CIMParameterArray that contains the parameters for this
	 * method.
	 */
	CIMParameterArray getParameters() const;
	/**
	 * Get the IN parameters for this method.
	 * @return An CIMParameterArray that contains IN the parameters for this
	 * method.
	 */
	CIMParameterArray getINParameters() const;
	/**
	 * Get the OUT parameters for this method.
	 * @return An CIMParameterArray that contains OUT the parameters for this
	 * method.
	 */
	CIMParameterArray getOUTParameters() const;
	/**
	 * Set the return data type for this parameter
	 * @param type	An CIMDataType object that represents the type.
	 * @return a reference to *this
	 */
	CIMMethod& setReturnType(const CIMDataType& type);
	/**
	 * @return The return data type for this method.
	 */
	CIMDataType getReturnType() const;
	/**
	 * @return The size of the return data type.
	 */
	Int32 getReturnDataSize() const;
	/**
	 * Set name of overriding method.
	 * @param omname The name of the overriding method.
	 * @return a reference to *this
	 */
	CIMMethod& setOverridingMethod(const CIMName& omname);
	/**
	 * @return The name of the overriding method.
	 */
	String getOverridingMethod() const;
	/**
	 * Set the propagated flag for this method.
	 * @param propagated If true method is propagated. Otherwise false.
	 * @return a reference to *this
	 */
	CIMMethod& setPropagated(bool propagated=true);
	/**
	 * Get the propagated flag.
	 * @return true if this method is propagated. Otherwise false.
	 */
	bool getPropagated() const;
	/**
	 * Create an CIMMethod object based on this one, using the criteria
	 * specified in the parameters.
	 *
	 * @param includeQualifiers If false, no qualifiers will be included in the
	 *		CIMMethod returned. Otherwise all qualifiers are included.
	 * @param includeClassOrigin I false, the class origin will not be included
	 *		in the CIMMethod returned. Otherwise the class origin is included
	 *
	 * @return A new CIMMethod that is a copy of this one with the
	 * 		qualifiers and class origin optionally exclued.
	 */
	CIMMethod clone(
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_INCLUDE_CLASS_ORIGIN) const;
	/**
	 * @return The name of this method as an String.
	 */
	virtual String getName() const;
	/**
	 * Set the name of this method.
	 * @param name The new name for this method.
	 */
	virtual void setName(const CIMName& name);
	/**
	 * Read this object from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::streambuf & istrm);
	/**
	 * Write this object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::streambuf & ostrm) const;
	/**
	 * @return The MOF representation of this object as an String.
	 */
	virtual String toMOF() const;
	/**
	 * @return The string representation of this method as an String.
	 */
	virtual String toString() const;

	OW_SAFE_BOOL_IMPL(CIMMethod, COWIntrusiveReference<METHData>, CIMMethod::m_pdata, m_pdata)

protected:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	COWIntrusiveReference<METHData> m_pdata;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	friend OW_COMMON_API bool operator<(const CIMMethod& x, const CIMMethod& y);
};

} // end namespace OW_NAMESPACE

#endif
