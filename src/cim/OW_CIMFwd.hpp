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

#ifndef OW_CIM_FWD_HPP_INCLUDE_GUARD
#define OW_CIM_FWD_HPP_INCLUDE_GUARD
#include "OW_config.h"
#include "blocxx/ArrayFwd.hpp"
#include "OW_CommonFwd.hpp"	// for Enumeration<> and because this file used to contain forward declarations for some stuff in common.

namespace OW_NAMESPACE
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

typedef blocxx::Enumeration<CIMObjectPath> CIMObjectPathEnumeration;
typedef blocxx::Enumeration<CIMQualifierType> CIMQualifierTypeEnumeration;
typedef blocxx::Enumeration<CIMClass> CIMClassEnumeration;
typedef blocxx::Enumeration<CIMInstance> CIMInstanceEnumeration;

typedef blocxx::Array<CIMDateTime> 			CIMDateTimeArray;
typedef blocxx::Array<CIMBase> 				CIMBaseArray;
typedef blocxx::Array<CIMDataType> 			CIMDataTypeArray;
typedef blocxx::Array<CIMScope> 			CIMScopeArray;
typedef blocxx::Array<CIMUrl> 				CIMUrlArray;
typedef blocxx::Array<CIMNameSpace>			CIMNameSpaceArray;
typedef blocxx::Array<CIMElement> 			CIMElementArray;
typedef blocxx::Array<CIMFlavor> 			CIMFlavorArray;
typedef blocxx::Array<CIMProperty> 			CIMPropertyArray;
typedef blocxx::Array<CIMObjectPath> 		CIMObjectPathArray;
typedef blocxx::Array<CIMQualifierType> 	CIMQualifierTypeArray;
typedef blocxx::Array<CIMQualifier> 		CIMQualifierArray;
typedef blocxx::Array<CIMMethod> 			CIMMethodArray;
typedef blocxx::Array<CIMClass> 			CIMClassArray;
typedef blocxx::Array<CIMParameter> 		CIMParameterArray;
typedef blocxx::Array<CIMParamValue> 		CIMParamValueArray;
typedef blocxx::Array<CIMInstance> 			CIMInstanceArray;
typedef blocxx::Array<CIMValue> 			CIMValueArray;
typedef blocxx::Array<CIMName> 				CIMNameArray;

} // end namespace OW_NAMESPACE

#endif
