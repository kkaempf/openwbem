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

#ifndef __OW_CIM_FWD_HPP__
#define __OW_CIM_FWD_HPP__

#include "OW_config.h"
#include "OW_ArrayFwd.hpp"

class OW_Bool;
class OW_CIMDateTime;
class OW_CIMBase;
class OW_CIMDataType;
class OW_CIMScope;
class OW_CIMUrl;
class OW_CIMNameSpace;
class OW_CIMElement;
class OW_CIMFlavor;
class OW_CIMProperty;
class OW_CIMObjectPath;
class OW_CIMQualifierType;
class OW_CIMQualifier;
class OW_CIMMethod;
class OW_CIMClass;
class OW_CIMParameter;
class OW_CIMInstance;
class OW_CIMValue;
class OW_String;
class OW_Char16;
struct OW_CIMFeatures;

template <class T>
class OW_Enumeration;

typedef OW_Enumeration<OW_CIMObjectPath> OW_CIMObjectPathEnumeration;
typedef OW_Enumeration<OW_CIMQualifierType> OW_CIMQualifierTypeEnumeration;
typedef OW_Enumeration<OW_CIMClass> OW_CIMClassEnumeration;
typedef OW_Enumeration<OW_CIMInstance> OW_CIMInstanceEnumeration;

typedef OW_Array<OW_CIMDateTime> 		OW_CIMDateTimeArray;
typedef OW_Array<OW_CIMBase> 				OW_CIMBaseArray;
typedef OW_Array<OW_CIMDataType> 		OW_CIMDataTypeArray;
typedef OW_Array<OW_CIMScope> 			OW_CIMScopeArray;
typedef OW_Array<OW_CIMUrl> 				OW_CIMUrlArray;
typedef OW_Array<OW_CIMNameSpace>		OW_CIMNameSpaceArray;
typedef OW_Array<OW_CIMElement> 			OW_CIMElementArray;
typedef OW_Array<OW_CIMFlavor> 			OW_CIMFlavorArray;
typedef OW_Array<OW_CIMProperty> 		OW_CIMPropertyArray;
typedef OW_Array<OW_CIMObjectPath> 		OW_CIMObjectPathArray;
typedef OW_Array<OW_CIMQualifierType> 	OW_CIMQualifierTypeArray;
typedef OW_Array<OW_CIMQualifier> 		OW_CIMQualifierArray;
typedef OW_Array<OW_CIMMethod> 			OW_CIMMethodArray;
typedef OW_Array<OW_CIMClass> 			OW_CIMClassArray;
typedef OW_Array<OW_CIMParameter> 		OW_CIMParameterArray;
typedef OW_Array<OW_CIMInstance> 		OW_CIMInstanceArray;
typedef OW_Array<OW_CIMValue> 			OW_CIMValueArray;
typedef OW_Array<OW_String>				OW_StringArray;
typedef OW_Array<OW_Char16>				OW_Char16Array;
typedef OW_Array<OW_Bool>       			OW_BoolArray;



#endif	// __OW_CIM_FWD_HPP__

