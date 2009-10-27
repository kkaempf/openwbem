/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_CIMELEMENT_HPP_INCLUDE_GUARD_
#define OW_CIMELEMENT_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMBase.hpp"
#include "OW_CIMFwd.hpp"

/// @todo  Remove this
#include "blocxx/String.hpp"

namespace OW_NAMESPACE
{

/**
 * The CIMElement class is used to enforce behavior from class that make
 * up CIM classes and CIM Instances. This would be CIMClass, CIMInstance,
 * CIMMethod, CIMParameter, CIMProperty, CIMQualifier and
 * CIMQualifierType.
 */
class OW_COMMON_API CIMElement : public CIMBase
{
public:
	virtual ~CIMElement();
	/**
	 * In the future, this function will be changed to return a CIMName
	 * @return The name of this CIMElement object as an String.
	 */
	virtual blocxx::String getName() const = 0;
	/**
	 * Set the name of this CIMElement object.
	 * @param name The new name of this CIMElement object.
	 */
	virtual void setName(const CIMName& name) = 0;
	/**
	 * @return An String representing this CIMElement.
	 */
	virtual blocxx::String toString() const;

	/// @todo  Deprecate these
	int compare(const CIMElement& arg)
	{
		return getName().compareToIgnoreCase(arg.getName());
	}
	bool equals(const CIMElement& arg)
	{
		return (compare(arg) == 0);
	}
	bool operator== (const CIMElement& arg)
	{
		return equals(arg);
	}
	bool operator!= (const CIMElement& arg)
	{
		return compare(arg) != 0;
	}

};

} // end namespace OW_NAMESPACE

#endif
