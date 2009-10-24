/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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

#ifndef OW_CIM_INSTANCE_WRAPPER_BASE_HPP_INCLUDE_GUARD_
#define OW_CIM_INSTANCE_WRAPPER_BASE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_Types.hpp"
#include "blocxx/String.hpp"
#include "blocxx/Array.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMClass.hpp"
#include "blocxx/Bool.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_APIEXCEPTION(CIMInstanceWrapper, OW_COMMON_API);

class CIMInstanceWrapperBase
{
public:
	String toString();
	const CIMInstance& instance();

protected:
	/**
	 * @throws CIMInstanceWrapperException if x.getClassName() != name
	 */
	CIMInstanceWrapperBase(const CIMInstance& x, const char* name);

	bool propertyIsNULL(const char* name) const;


	// getters
	String getStringProperty(const char* name) const;
	bool getBoolProperty(const char* name) const;
	Char16 getChar16Property(const char* name) const;
	UInt8 getUInt8Property(const char* name) const;
	Int8 getInt8Property(const char* name) const;
	UInt16 getUInt16Property(const char* name) const;
	Int16 getInt16Property(const char* name) const;
	UInt32 getUInt32Property(const char* name) const;
	Int32 getInt32Property(const char* name) const;
	UInt64 getUInt64Property(const char* name) const;
	Int64 getInt64Property(const char* name) const;
	Real32 getReal32Property(const char* name) const;
	Real64 getReal64Property(const char* name) const;
	CIMDateTime getCIMDateTimeProperty(const char* name) const;
	CIMObjectPath getCIMObjectPathProperty(const char* name) const;
	CIMClass getCIMClassProperty(const char* name) const;
	CIMInstance getCIMInstanceProperty(const char* name) const;
	CIMObjectPathArray getCIMObjectPathArrayProperty(const char* name) const;
	Char16Array getChar16ArrayProperty(const char* name) const;
	UInt8Array getUInt8ArrayProperty(const char* name) const;
	Int8Array getInt8ArrayProperty(const char* name) const;
	UInt16Array getUInt16ArrayProperty(const char* name) const;
	Int16Array getInt16ArrayProperty(const char* name) const;
	UInt32Array getUInt32ArrayProperty(const char* name) const;
	Int32Array getInt32ArrayProperty(const char* name) const;
	UInt64Array getUInt64ArrayProperty(const char* name) const;
	Int64Array getInt64ArrayProperty(const char* name) const;
	Real64Array getReal64ArrayProperty(const char* name) const;
	Real32Array getReal32ArrayProperty(const char* name) const;
	StringArray getStringArrayProperty(const char* name) const;
	BoolArray getBoolArrayProperty(const char* name) const;
	CIMDateTimeArray getCIMDateTimeArrayProperty(const char* name) const;
	CIMClassArray getCIMClassArrayProperty(const char* name) const;
	CIMInstanceArray getCIMInstanceArrayProperty(const char* name) const;

	// setters
	void setStringProperty(const char* name, const String& val);
	void setBoolProperty(const char* name, bool val);
	void setChar16Property(const char* name, Char16 val);
	void setUInt8Property(const char* name, UInt8 val);
	void setInt8Property(const char* name, Int8 val);
	void setUInt16Property(const char* name, UInt16 val);
	void setInt16Property(const char* name, Int16 val);
	void setUInt32Property(const char* name, UInt32 val);
	void setInt32Property(const char* name, Int32 val);
	void setUInt64Property(const char* name, UInt64 val);
	void setInt64Property(const char* name, Int64 val);
	void setReal32Property(const char* name, Real32 val);
	void setReal64Property(const char* name, Real64 val);
	void setCIMDateTimeProperty(const char* name, const CIMDateTime& val);
	void setCIMObjectPathProperty(const char* name, const CIMObjectPath& val);
	void setCIMClassProperty(const char* name, const CIMClass& val);
	void setCIMInstanceProperty(const char* name, const CIMInstance& val);
	void setCIMObjectPathArrayProperty(const char* name, const CIMObjectPathArray& val);
	void setChar16ArrayProperty(const char* name, const Char16Array& val);
	void setUInt8ArrayProperty(const char* name, const UInt8Array& val);
	void setInt8ArrayProperty(const char* name, const Int8Array& val);
	void setUInt16ArrayProperty(const char* name, const UInt16Array& val);
	void setInt16ArrayProperty(const char* name, const Int16Array& val);
	void setUInt32ArrayProperty(const char* name, const UInt32Array& val);
	void setInt32ArrayProperty(const char* name, const Int32Array& val);
	void setUInt64ArrayProperty(const char* name, const UInt64Array& val);
	void setInt64ArrayProperty(const char* name, const Int64Array& val);
	void setReal64ArrayProperty(const char* name, const Real64Array& val);
	void setReal32ArrayProperty(const char* name, const Real32Array& val);
	void setStringArrayProperty(const char* name, const StringArray& val);
	void setBoolArrayProperty(const char* name, const BoolArray& val);
	void setCIMDateTimeArrayProperty(const char* name, const CIMDateTimeArray& val);
	void setCIMClassArrayProperty(const char* name, const CIMClassArray& val);
	void setCIMInstanceArrayProperty(const char* name, const CIMInstanceArray& val);


private:
	CIMInstance m_inst;
};

} // end namespace OW_NAMESPACE

#endif


