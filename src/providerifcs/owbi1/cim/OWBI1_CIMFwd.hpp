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

#ifndef OWBI1_CIM_FWD_HPP_INCLUDE_GUARD
#define OWBI1_CIM_FWD_HPP_INCLUDE_GUARD
#include "OWBI1_config.h"
#include "OWBI1_ArrayFwd.hpp"

namespace OWBI1
{

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
class CIMName;
class CIMPropertyList;

template <class T> class Enumeration;
typedef Enumeration<CIMObjectPath> CIMObjectPathEnumeration;
typedef Enumeration<CIMQualifierType> CIMQualifierTypeEnumeration;
typedef Enumeration<CIMClass> CIMClassEnumeration;
typedef Enumeration<CIMInstance> CIMInstanceEnumeration;
typedef Enumeration<CIMName> CIMNameEnumeration;

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
typedef Array<CIMName> 				CIMNameArray;

} // end namespace OWBI1

#endif
