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

#ifndef __OW_CIMELEMENT_HPP__
#define __OW_CIMELEMENT_HPP__
#include "OW_config.h"
#include "OW_CIMBase.hpp"
#include "OW_Bool.hpp"

class OW_String;

/**
 * The OW_CIMElement class is used to enforce behavior from class that make
 * up CIM classes and CIM Instances. This would be OW_CIMClass, OW_CIMInstance,
 * OW_CIMMethod, OW_CIMParameter, OW_CIMProperty, OW_CIMQualifier and
 * OW_CIMQualifierType.
 */
class OW_CIMElement : public OW_CIMBase
{
public:

	/**
	 * @return The name of this OW_CIMElement object as an OW_String.
	 */
	virtual OW_String getName() const = 0;

	/**
	 * Set the name of this OW_CIMElement object.
	 * @param name The new name of this OW_CIMElement object.
	 */
	virtual void setName(const OW_String& name) = 0;

	/**
	 * Compare this OW_CIMElement object with another.
	 * @param arg	The OW_CIMElement object to compare this one to.
	 * @return 0 if this OW_CIMElenent is equal to arg. Less than 0 if this
	 * OW_CIMElement is less than arg. Greater than 0 if this OW_CIMElement is
	 * greater than arg.
	 */
	int compare(const OW_CIMElement& arg);

	/**
	 * Compare this OW_CIMElement object with another.
	 * @param arg	The OW_CIMElement object to compare this one to.
	 * @return true if this OW_CIMElement is equal to the given OW_CIMElement.
	 * Otherwise false.
	 */
	OW_Bool equals(const OW_CIMElement& arg) 
		{  return (compare(arg) == 0); }

	/**
	 * Equality operator
	 * @param arg	The OW_CIMElement object to compare this one to.
	 * @return true if this OW_CIMElement is equal to the given OW_CIMElement.
	 * Otherwise false.
	 */
	OW_Bool operator== (const OW_CIMElement& arg) {  return equals(arg); }

	/**
	 * Inequality operator
	 * @param arg	The OW_CIMElement object to compare this one to.
	 * @return true if this OW_CIMElement is not equal to the given 
	 * OW_CIMElement. Otherwise false.
	 */
	OW_Bool operator!= (const OW_CIMElement& arg) {  return (compare(arg) != 0); }

	/**
	 * Less than operator
	 * @param arg	The OW_CIMElement object to compare this one to.
	 * @return true if this OW_CIMElement is less than the given OW_CIMElement.
	 * Otherwise false.
	 */
	OW_Bool operator< (const OW_CIMElement& arg) {  return (compare(arg) < 0); }

	/**
	 * Less than or equal operator
	 * @param arg	The OW_CIMElement object to compare this one to.
	 * @return true if this OW_CIMElement is less than or equal to the given 
	 * OW_CIMElement. Otherwise false.
	 */
	OW_Bool operator<= (const OW_CIMElement& arg) {  return (compare(arg) <= 0); }

	/**
	 * Greater than operator
	 * @param arg	The OW_CIMElement object to compare this one to.
	 * @return true if this OW_CIMElement is greater than the given 
	 * OW_CIMElement. Otherwise false.
	 */
	OW_Bool operator> (const OW_CIMElement& arg) {  return (compare(arg) > 0); }

	/**
	 * Greate than or equal operator
	 * @param arg	The OW_CIMElement object to compare this one to.
	 * @return true if this OW_CIMElement is greater than or equal to the given 
	 * OW_CIMElement. Otherwise false.
	 */
	OW_Bool operator>= (const OW_CIMElement& arg) {  return (compare(arg) >= 0); }

	/**
	 * @return An OW_String representing this OW_CIMElement.
	 */
	virtual OW_String toString() const;

};

#endif	// __OW_CIMELEMENT_HPP__
