/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

#ifndef OW_WBEM_FLAGS_HPP_INCLUDE_GUARD_
#define OW_WBEM_FLAGS_HPP_INCLUDE_GUARD_
#include "OW_config.h"

namespace OW_NAMESPACE
{

namespace WBEMFlags
{
//////////////////////////////////////////////////////
enum EDeepFlag
{
	E_SHALLOW,
	E_DEEP
};
//////////////////////////////////////////////////////
enum ELocalOnlyFlag
{
	E_NOT_LOCAL_ONLY,
	E_LOCAL_ONLY
};
//////////////////////////////////////////////////////
enum EIncludeQualifiersFlag
{
	E_EXCLUDE_QUALIFIERS,
	E_INCLUDE_QUALIFIERS
};
//////////////////////////////////////////////////////
enum EIncludeClassOriginFlag
{
	E_EXCLUDE_CLASS_ORIGIN,
	E_INCLUDE_CLASS_ORIGIN
};
//////////////////////////////////////////////////////
enum EEnumSubclassesFlag
{
	E_DONT_ENUM_SUBCLASSES,
	E_ENUM_SUBCLASSES
};
//////////////////////////////////////////////////////
enum EOperationFlag
{
	E_CREATE_NAMESPACE,        // 0
	E_DELETE_NAMESPACE,        // 1
	E_ENUM_NAMESPACE,          // 2
	E_GET_QUALIFIER_TYPE,      // 3
	E_ENUM_QUALIFIER_TYPES,    // 4
	E_DELETE_QUALIFIER_TYPE,   // 5
	E_SET_QUALIFIER_TYPE,      // 6
	E_GET_CLASS,               // 7
	E_DELETE_CLASS,            // 8
	E_CREATE_CLASS,            // 9
	E_MODIFY_CLASS,            // 10
	E_ENUM_CLASSES,            // 11
	E_ENUM_CLASS_NAMES,        // 12
	E_ENUM_INSTANCES,          // 13
	E_ENUM_INSTANCE_NAMES,     // 14
	E_GET_INSTANCE,            // 15
	E_DELETE_INSTANCE,         // 16
	E_CREATE_INSTANCE,         // 17
	E_MODIFY_INSTANCE,         // 18
	E_SET_PROPERTY,            // 19
	E_GET_PROPERTY,            // 20
	E_INVOKE_METHOD_NO_LOCK,   // 21
	E_INVOKE_METHOD_READ_LOCK, // 22
	E_INVOKE_METHOD_WRITE_LOCK,// 23
	E_ASSOCIATOR_NAMES,        // 24
	E_ASSOCIATORS,             // 25
	E_ASSOCIATORS_CLASSES,     // 26
	E_REFERENCE_NAMES,         // 27
	E_REFERENCES,              // 28
	E_REFERENCES_CLASSES,      // 29
	E_EXEC_QUERY,              // 30
	E_EXPORT_INDICATION        // 31
};
/////////////////////////////////////////////////////////////////////////////
enum EOperationResultFlag
{
	E_FAILED,
	E_SUCCESS
};

}

} // end namespace OW_NAMESPACE

#endif				
