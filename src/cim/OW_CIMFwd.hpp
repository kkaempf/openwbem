/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_CIM_FWD_HPP_
#define OW_CIM_FWD_HPP_
#include "OW_config.h"
#include "OW_ArrayFwd.hpp"

namespace OpenWBEM
{

class Bool;
class CIMDateTime;
class CIMBase;
class CIMDataType;
class CIMScope;
class CIMUrl;
class CIMNameSpace;
class CIMElement;
class CIMFlavor;
class CIMProperty;
class CIMObjectPath;
class CIMQualifierType;
class CIMQualifier;
class CIMMethod;
class CIMClass;
class CIMParameter;
class CIMParamValue;
class CIMInstance;
class CIMValue;
class String;
class Char16;
struct CIMFeatures;
template <class T>
class Enumeration;
typedef Enumeration<CIMObjectPath> CIMObjectPathEnumeration;
typedef Enumeration<CIMQualifierType> CIMQualifierTypeEnumeration;
typedef Enumeration<CIMClass> CIMClassEnumeration;
typedef Enumeration<CIMInstance> CIMInstanceEnumeration;
typedef Array<CIMDateTime> 		CIMDateTimeArray;
typedef Array<CIMBase> 				CIMBaseArray;
typedef Array<CIMDataType> 		CIMDataTypeArray;
typedef Array<CIMScope> 			CIMScopeArray;
typedef Array<CIMUrl> 				CIMUrlArray;
typedef Array<CIMNameSpace>		CIMNameSpaceArray;
typedef Array<CIMElement> 			CIMElementArray;
typedef Array<CIMFlavor> 			CIMFlavorArray;
typedef Array<CIMProperty> 		CIMPropertyArray;
typedef Array<CIMObjectPath> 		CIMObjectPathArray;
typedef Array<CIMQualifierType> 	CIMQualifierTypeArray;
typedef Array<CIMQualifier> 		CIMQualifierArray;
typedef Array<CIMMethod> 			CIMMethodArray;
typedef Array<CIMClass> 			CIMClassArray;
typedef Array<CIMParameter> 		CIMParameterArray;
typedef Array<CIMParamValue> 		CIMParamValueArray;
typedef Array<CIMInstance> 		CIMInstanceArray;
typedef Array<CIMValue> 			CIMValueArray;
typedef Array<String>				StringArray;
typedef Array<Char16>				Char16Array;
typedef Array<Bool>       			BoolArray;

} // end namespace OpenWBEM

typedef OpenWBEM::Bool OW_Bool OW_DEPRECATED;
typedef OpenWBEM::CIMDateTime OW_CIMDateTime OW_DEPRECATED;
typedef OpenWBEM::CIMBase OW_CIMBase OW_DEPRECATED;
typedef OpenWBEM::CIMDataType OW_CIMDataType OW_DEPRECATED;
typedef OpenWBEM::CIMScope OW_CIMScope OW_DEPRECATED;
typedef OpenWBEM::CIMUrl OW_CIMUrl OW_DEPRECATED;
typedef OpenWBEM::CIMNameSpace OW_CIMNameSpace OW_DEPRECATED;
typedef OpenWBEM::CIMElement OW_CIMElement OW_DEPRECATED;
typedef OpenWBEM::CIMFlavor OW_CIMFlavor OW_DEPRECATED;
typedef OpenWBEM::CIMProperty OW_CIMProperty OW_DEPRECATED;
typedef OpenWBEM::CIMObjectPath OW_CIMObjectPath OW_DEPRECATED;
typedef OpenWBEM::CIMQualifierType OW_CIMQualifierType OW_DEPRECATED;
typedef OpenWBEM::CIMQualifier OW_CIMQualifier OW_DEPRECATED;
typedef OpenWBEM::CIMMethod OW_CIMMethod OW_DEPRECATED;
typedef OpenWBEM::CIMClass OW_CIMClass OW_DEPRECATED;
typedef OpenWBEM::CIMParameter OW_CIMParameter OW_DEPRECATED;
typedef OpenWBEM::CIMParamValue OW_CIMParamValue OW_DEPRECATED;
typedef OpenWBEM::CIMInstance OW_CIMInstance OW_DEPRECATED;
typedef OpenWBEM::CIMValue OW_CIMValue OW_DEPRECATED;
typedef OpenWBEM::String OW_String OW_DEPRECATED;
typedef OpenWBEM::Char16 OW_Char16 OW_DEPRECATED;
typedef OpenWBEM::CIMFeatures OW_CIMFeatures OW_DEPRECATED;
typedef OpenWBEM::Enumeration<OpenWBEM::CIMObjectPath> OW_CIMObjectPathEnumeration OW_DEPRECATED;
typedef OpenWBEM::Enumeration<OpenWBEM::CIMQualifierType> OW_CIMQualifierTypeEnumeration OW_DEPRECATED;
typedef OpenWBEM::Enumeration<OpenWBEM::CIMClass> OW_CIMClassEnumeration OW_DEPRECATED;
typedef OpenWBEM::Enumeration<OpenWBEM::CIMInstance> OW_CIMInstanceEnumeration OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMDateTime> 		OW_CIMDateTimeArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMBase> 				OW_CIMBaseArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMDataType> 		OW_CIMDataTypeArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMScope> 			OW_CIMScopeArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMUrl> 				OW_CIMUrlArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMNameSpace>		OW_CIMNameSpaceArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMElement> 			OW_CIMElementArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMFlavor> 			OW_CIMFlavorArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMProperty> 		OW_CIMPropertyArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMObjectPath> 		OW_CIMObjectPathArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMQualifierType> 	OW_CIMQualifierTypeArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMQualifier> 		OW_CIMQualifierArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMMethod> 			OW_CIMMethodArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMClass> 			OW_CIMClassArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMParameter> 		OW_CIMParameterArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMParamValue> 		OW_CIMParamValueArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMInstance> 		OW_CIMInstanceArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::CIMValue> 			OW_CIMValueArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::String>				OW_StringArray OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::Char16>				OW_Char16Array OW_DEPRECATED;
typedef OpenWBEM::Array<OpenWBEM::Bool>       			OW_BoolArray OW_DEPRECATED;

#endif
