/*******************************************************************************
* Copyright (C) 2003 Vintela, Inc. All rights reserved.
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
#include "PyOW_Array.hpp"

namespace OpenWBEM
{

void registerArrayImpl_String(const char*);
void registerArrayImpl_CIMClass(const char*);
void registerArrayImpl_CIMDataType(const char*);
void registerArrayImpl_CIMDateTime(const char*);
void registerArrayImpl_CIMFlavor(const char*);
void registerArrayImpl_CIMInstance(const char*);
void registerArrayImpl_CIMMethod(const char*);
void registerArrayImpl_CIMObjectPath(const char*);
void registerArrayImpl_CIMParamValue(const char*);
void registerArrayImpl_CIMParameter(const char*);
void registerArrayImpl_CIMProperty(const char*);
void registerArrayImpl_CIMQualifier(const char*);
void registerArrayImpl_CIMQualifierType(const char*);
void registerArrayImpl_CIMScope(const char*);
void registerArrayImpl_CIMValue(const char*);
void registerArrayImpl_Bool(const char*);
void registerArrayImpl_Char16(const char*);
void registerArrayImpl_UInt8(const char*);
void registerArrayImpl_Int8(const char*);
void registerArrayImpl_UInt16(const char*);
void registerArrayImpl_Int16(const char*);
void registerArrayImpl_UInt32(const char*);
void registerArrayImpl_Int32(const char*);
void registerArrayImpl_UInt64(const char*);
void registerArrayImpl_Int64(const char*);
void registerArrayImpl_Real32(const char*);
void registerArrayImpl_Real64(const char*);
void registerArrayImpl_CIMOMInfo(const char*);
void registerArray()
{
	registerArrayImpl_String("StringArray");
	registerArrayImpl_CIMClass("CIMClassArray");
	registerArrayImpl_CIMDataType("CIMDataTypeArray");
	registerArrayImpl_CIMDateTime("CIMDateTimeArray");
	registerArrayImpl_CIMFlavor("CIMFlavorArray");
	registerArrayImpl_CIMInstance("CIMInstanceArray");
	registerArrayImpl_CIMMethod("CIMMethodArray");
	registerArrayImpl_CIMObjectPath("CIMObjectPathArray");
	registerArrayImpl_CIMParamValue("CIMParamValueArray");
	registerArrayImpl_CIMParameter("CIMParameterArray");
	registerArrayImpl_CIMProperty("CIMPropertyArray");
	registerArrayImpl_CIMQualifier("CIMQualifierArray");
	registerArrayImpl_CIMQualifierType("CIMQualifierTypeArray");
	registerArrayImpl_CIMScope("CIMScopeArray");
	registerArrayImpl_CIMValue("CIMValueArray");
	registerArrayImpl_Bool("BoolArray");
	registerArrayImpl_Char16("Char16Array");
	registerArrayImpl_UInt8("UInt8Array");
	registerArrayImpl_Int8("Int8Array");
	registerArrayImpl_UInt16("UInt16Array");
	registerArrayImpl_Int16("Int16Array");
	registerArrayImpl_UInt32("UInt32Array");
	registerArrayImpl_Int32("Int32Array");
	registerArrayImpl_UInt64("UInt64Array");
	registerArrayImpl_Int64("Int64Array");
	registerArrayImpl_Real32("Real32Array");
	registerArrayImpl_Real64("Real64Array");
	registerArrayImpl_CIMOMInfo("CIMOMInfoArray");
}

} // end namespace OpenWBEM

