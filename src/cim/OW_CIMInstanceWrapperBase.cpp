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

#include "OW_config.h"
#include "OW_CIMInstanceWrapperBase.hpp"
#include "OW_Format.hpp"
#include "OW_String.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION(CIMInstanceWrapper);

String
CIMInstanceWrapperBase::toString()
{
	return m_inst.toString();
}

const CIMInstance& 
CIMInstanceWrapperBase::instance()
{
	return m_inst;
}

CIMInstanceWrapperBase::CIMInstanceWrapperBase(const CIMInstance& x, const char* name)
	: m_inst(x)
{
	if (x.getClassName() != name)
	{
		OW_THROW(CIMInstanceWrapperException, 
			Format("CIMInstanceWrapperBase::CIMInstanceWrapperBase expected an instance of %1, but got %2", name, x.getClassName()).c_str());
	}
}

bool 
CIMInstanceWrapperBase::propertyIsNULL(const char* name) const
{
	return !m_inst.propertyHasValue(name);
}

String
CIMInstanceWrapperBase::getStringProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toString();
}
bool
CIMInstanceWrapperBase::getBoolProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toBool();
}
Char16 
CIMInstanceWrapperBase::getChar16Property(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toChar16();
}
UInt8 
CIMInstanceWrapperBase::getUInt8Property(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toUInt8();
}
Int8 
CIMInstanceWrapperBase::getInt8Property(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toInt8();
}
UInt16 
CIMInstanceWrapperBase::getUInt16Property(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toUInt16();
}
Int16 
CIMInstanceWrapperBase::getInt16Property(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toInt16();
}
UInt32 
CIMInstanceWrapperBase::getUInt32Property(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toUInt32();
}
Int32 
CIMInstanceWrapperBase::getInt32Property(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toInt32();
}
UInt64 
CIMInstanceWrapperBase::getUInt64Property(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toUInt64();
}
Int64 
CIMInstanceWrapperBase::getInt64Property(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toInt64();
}
Real32 
CIMInstanceWrapperBase::getReal32Property(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toReal32();
}
Real64 
CIMInstanceWrapperBase::getReal64Property(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toReal64();
}
CIMDateTime 
CIMInstanceWrapperBase::getCIMDateTimeProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toCIMDateTime();
}
CIMObjectPath 
CIMInstanceWrapperBase::getCIMObjectPathProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toCIMObjectPath();
}
CIMClass 
CIMInstanceWrapperBase::getCIMClassProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toCIMClass();
}
CIMInstance 
CIMInstanceWrapperBase::getCIMInstanceProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toCIMInstance();
}
CIMObjectPathArray 
CIMInstanceWrapperBase::getCIMObjectPathArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toCIMObjectPathArray();
}
Char16Array 
CIMInstanceWrapperBase::getChar16ArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toChar16Array();
}
UInt8Array 
CIMInstanceWrapperBase::getUInt8ArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toUInt8Array();
}
Int8Array 
CIMInstanceWrapperBase::getInt8ArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toInt8Array();
}
UInt16Array 
CIMInstanceWrapperBase::getUInt16ArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toUInt16Array();
}
Int16Array 
CIMInstanceWrapperBase::getInt16ArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toInt16Array();
}
UInt32Array 
CIMInstanceWrapperBase::getUInt32ArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toUInt32Array();
}
Int32Array 
CIMInstanceWrapperBase::getInt32ArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toInt32Array();
}
UInt64Array 
CIMInstanceWrapperBase::getUInt64ArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toUInt64Array();
}
Int64Array 
CIMInstanceWrapperBase::getInt64ArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toInt64Array();
}
Real64Array 
CIMInstanceWrapperBase::getReal64ArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toReal64Array();
}
Real32Array 
CIMInstanceWrapperBase::getReal32ArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toReal32Array();
}
StringArray 
CIMInstanceWrapperBase::getStringArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toStringArray();
}
BoolArray 
CIMInstanceWrapperBase::getBoolArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toBoolArray();
}
CIMDateTimeArray 
CIMInstanceWrapperBase::getCIMDateTimeArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toCIMDateTimeArray();
}
CIMClassArray 
CIMInstanceWrapperBase::getCIMClassArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toCIMClassArray();
}
CIMInstanceArray 
CIMInstanceWrapperBase::getCIMInstanceArrayProperty(const char* name) const
{
	return m_inst.getPropertyT(name).getValueT().toCIMInstanceArray();
}

void
CIMInstanceWrapperBase::setStringProperty(const char* name, const String& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setBoolProperty(const char* name, bool val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setChar16Property(const char* name, Char16 val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setUInt8Property(const char* name, UInt8 val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setInt8Property(const char* name, Int8 val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setUInt16Property(const char* name, UInt16 val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setInt16Property(const char* name, Int16 val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setUInt32Property(const char* name, UInt32 val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setInt32Property(const char* name, Int32 val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setUInt64Property(const char* name, UInt64 val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setInt64Property(const char* name, Int64 val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setReal32Property(const char* name, Real32 val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setReal64Property(const char* name, Real64 val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setCIMDateTimeProperty(const char* name, const CIMDateTime& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setCIMObjectPathProperty(const char* name, const CIMObjectPath& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setCIMClassProperty(const char* name, const CIMClass& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setCIMInstanceProperty(const char* name, const CIMInstance& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setCIMObjectPathArrayProperty(const char* name, const CIMObjectPathArray& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setChar16ArrayProperty(const char* name, const Char16Array& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setUInt8ArrayProperty(const char* name, const UInt8Array& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setInt8ArrayProperty(const char* name, const Int8Array& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setUInt16ArrayProperty(const char* name, const UInt16Array& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setInt16ArrayProperty(const char* name, const Int16Array& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setUInt32ArrayProperty(const char* name, const UInt32Array& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setInt32ArrayProperty(const char* name, const Int32Array& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setUInt64ArrayProperty(const char* name, const UInt64Array& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setInt64ArrayProperty(const char* name, const Int64Array& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setReal64ArrayProperty(const char* name, const Real64Array& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setReal32ArrayProperty(const char* name, const Real32Array& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setStringArrayProperty(const char* name, const StringArray& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setBoolArrayProperty(const char* name, const BoolArray& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setCIMDateTimeArrayProperty(const char* name, const CIMDateTimeArray& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setCIMClassArrayProperty(const char* name, const CIMClassArray& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}

void
CIMInstanceWrapperBase::setCIMInstanceArrayProperty(const char* name, const CIMInstanceArray& val)
{
	m_inst.updatePropertyValue(name, CIMValue(val));
}



} // end namespace OW_NAMESPACE



