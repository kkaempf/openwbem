/*******************************************************************************
* Copyright (C) 2004 Quest Software, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#ifndef OW_CIMNAME_HPP_INCLUDE_GUARD_
#define OW_CIMNAME_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_SerializableIFC.hpp"
#include "blocxx/String.hpp"
#include "blocxx/SafeBool.hpp"

#include <iosfwd>

namespace OW_NAMESPACE
{

/**
 * CIMName encapsulates a CIM Name, which is simply a string, with the
 * special semantic of being case-insensitive and preserving.
 */
class OW_COMMON_API CIMName : public SerializableIFC
{
public:

	/**
	 * A default constructed CIMName is the empty string "".
	 */
	CIMName();

	/**
	 * Create an CIMName object with the given name.
	 * This constructor is not explicit in order to facilitate conversions and comparisons
	 * @param name The name for this CIMName specified as an String.
	 */
	CIMName(const blocxx::String& name);

	/**
	 * Create an CIMName object with the given name.
	 * This constructor is not explicit in order to facilitate conversions and comparisons
	 * @param name The name for this CIMName specified as a NULL terminated
	 *		string.
	 */
	CIMName(const char* name);

	~CIMName();

	CIMName& operator=(const blocxx::String& name);
	CIMName& operator=(const char* name);

	/**
	 * Get the String representation of the CIM Name
	 */
	blocxx::String toString() const;

	/**
	 * Read the object from an input stream.
	 * This function is equivalent to String::readObject()
	 * @param istrm The input stream to read from.
	 */
	virtual void readObject(std::streambuf & istrm);

	/**
	 * Write the object to an output stream
	 * This function is equivalent to String::writeObject()
	 * @param ostrm The output stream to write to.
	 */
	virtual void writeObject(std::streambuf & ostrm) const;

	/**
	 * Test if this is a valid CIMName. A valid CIMName is defined as != ""
	 */
	bool isValid() const;

	BLOCXX_SAFE_BOOL_IMPL(CIMName, blocxx::String, CIMName::m_name, isValid())

	friend OW_COMMON_API bool operator<(const CIMName& x, const CIMName& y);
	friend OW_COMMON_API bool operator==(const CIMName& x, const CIMName& y);

private:
	blocxx::String m_name;

};

OW_COMMON_API bool operator<=(const CIMName& x, const CIMName& y);
OW_COMMON_API bool operator>(const CIMName& x, const CIMName& y);
OW_COMMON_API bool operator>=(const CIMName& x, const CIMName& y);
OW_COMMON_API bool operator!=(const CIMName& x, const CIMName& y);

OW_COMMON_API std::ostream& operator<<(std::ostream& ostr, const CIMName& name);

} // end namespace OW_NAMESPACE

#endif

