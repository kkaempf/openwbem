/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "PyOW_Array.hpp"

void registerOW_ArrayImpl_OW_String(const char*);
void registerOW_ArrayImpl_OW_CIMClass(const char*);
void registerOW_ArrayImpl_OW_CIMDataType(const char*);
void registerOW_ArrayImpl_OW_CIMDateTime(const char*);
void registerOW_ArrayImpl_OW_CIMFlavor(const char*);
void registerOW_ArrayImpl_OW_CIMInstance(const char*);
void registerOW_ArrayImpl_OW_CIMMethod(const char*);
void registerOW_ArrayImpl_OW_CIMObjectPath(const char*);
void registerOW_ArrayImpl_OW_CIMParamValue(const char*);
void registerOW_ArrayImpl_OW_CIMParameter(const char*);
void registerOW_ArrayImpl_OW_CIMProperty(const char*);
void registerOW_ArrayImpl_OW_CIMQualifier(const char*);
void registerOW_ArrayImpl_OW_CIMQualifierType(const char*);
void registerOW_ArrayImpl_OW_CIMScope(const char*);
void registerOW_ArrayImpl_OW_CIMValue(const char*);
void registerOW_ArrayImpl_OW_Bool(const char*);
void registerOW_ArrayImpl_OW_Char16(const char*);
void registerOW_ArrayImpl_OW_UInt8(const char*);
void registerOW_ArrayImpl_OW_Int8(const char*);
void registerOW_ArrayImpl_OW_UInt16(const char*);
void registerOW_ArrayImpl_OW_Int16(const char*);
void registerOW_ArrayImpl_OW_UInt32(const char*);
void registerOW_ArrayImpl_OW_Int32(const char*);
void registerOW_ArrayImpl_OW_UInt64(const char*);
void registerOW_ArrayImpl_OW_Int64(const char*);
void registerOW_ArrayImpl_OW_Real32(const char*);
void registerOW_ArrayImpl_OW_Real64(const char*);
void registerOW_ArrayImpl_OW_CIMOMInfo(const char*);

void registerOW_Array()
{
    registerOW_ArrayImpl_OW_String("OW_StringArray");
    registerOW_ArrayImpl_OW_CIMClass("OW_CIMClassArray");
    registerOW_ArrayImpl_OW_CIMDataType("OW_CIMDataTypeArray");
    registerOW_ArrayImpl_OW_CIMDateTime("OW_CIMDateTimeArray");
    registerOW_ArrayImpl_OW_CIMFlavor("OW_CIMFlavorArray");
    registerOW_ArrayImpl_OW_CIMInstance("OW_CIMInstanceArray");
    registerOW_ArrayImpl_OW_CIMMethod("OW_CIMMethodArray");
    registerOW_ArrayImpl_OW_CIMObjectPath("OW_CIMObjectPathArray");
    registerOW_ArrayImpl_OW_CIMParamValue("OW_CIMParamValueArray");
    registerOW_ArrayImpl_OW_CIMParameter("OW_CIMParameterArray");
    registerOW_ArrayImpl_OW_CIMProperty("OW_CIMPropertyArray");
    registerOW_ArrayImpl_OW_CIMQualifier("OW_CIMQualifierArray");
    registerOW_ArrayImpl_OW_CIMQualifierType("OW_CIMQualifierTypeArray");
    registerOW_ArrayImpl_OW_CIMScope("OW_CIMScopeArray");
    registerOW_ArrayImpl_OW_CIMValue("OW_CIMValueArray");
    registerOW_ArrayImpl_OW_Bool("OW_BoolArray");
    registerOW_ArrayImpl_OW_Char16("OW_Char16Array");
    registerOW_ArrayImpl_OW_UInt8("OW_UInt8Array");
    registerOW_ArrayImpl_OW_Int8("OW_Int8Array");
    registerOW_ArrayImpl_OW_UInt16("OW_UInt16Array");
    registerOW_ArrayImpl_OW_Int16("OW_Int16Array");
    registerOW_ArrayImpl_OW_UInt32("OW_UInt32Array");
    registerOW_ArrayImpl_OW_Int32("OW_Int32Array");
    registerOW_ArrayImpl_OW_UInt64("OW_UInt64Array");
    registerOW_ArrayImpl_OW_Int64("OW_Int64Array");
    registerOW_ArrayImpl_OW_Real32("OW_Real32Array");
    registerOW_ArrayImpl_OW_Real64("OW_Real64Array");
    registerOW_ArrayImpl_OW_CIMOMInfo("OW_CIMOMInfoArray");
}

