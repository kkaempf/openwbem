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

#ifndef OW_WBEM_FLAGS_HPP_INCLUDE_GUARD_
#define OW_WBEM_FLAGS_HPP_INCLUDE_GUARD_

#include "OW_config.h"

namespace OW_WBEMFlags
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

}


#endif				

