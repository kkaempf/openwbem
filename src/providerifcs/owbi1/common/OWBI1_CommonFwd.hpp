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
 * @author Dan Nuffer
 */

#ifndef OWBI1_COMMON_FWD_HPP_INCLUDE_GUARD_
#define OWBI1_COMMON_FWD_HPP_INCLUDE_GUARD_
#include "OWBI1_config.h"
#include "OWBI1_ArrayFwd.hpp"
#include "OWBI1_IntrusiveReference.hpp"
#include "OWBI1_CIMFwd.hpp"

// Yeah I know this is forbidden by the standard, but what am I gonna do?  #include <algorithm> ? I think not.
// If it causes a problem on some compiler, just #ifdef a fix in.
#ifdef OWBI1_WIN32
// someone who cares about windows should fix this to be a forward declaration for less so
// that *everything* doesn't end up including <functional>
#include <functional>
#else
namespace std
{
	template <typename T> class less;
}
#endif

namespace OWBI1
{

struct LogMessage;

class Logger;
typedef IntrusiveReference<Logger> LoggerRef;

class String;
typedef Array<String> StringArray;

class Char16;
typedef Array<Char16> Char16Array;

class DateTime;

class OperationContext;

template <class T> class Enumeration;

class Bool;
typedef Array<Bool>       			BoolArray;

class SessionLanguage;
typedef IntrusiveReference<SessionLanguage> SessionLanguageRef;

class CIMOMHandleIFC;
typedef IntrusiveReference<CIMOMHandleIFC> CIMOMHandleIFCRef;

template <typename T> class ResultHandlerIFC;
typedef ResultHandlerIFC<CIMClass> CIMClassResultHandlerIFC;
typedef ResultHandlerIFC<CIMInstance> CIMInstanceResultHandlerIFC;
typedef ResultHandlerIFC<CIMObjectPath> CIMObjectPathResultHandlerIFC;
typedef ResultHandlerIFC<CIMQualifierType> CIMQualifierTypeResultHandlerIFC;
typedef ResultHandlerIFC<CIMName> CIMNameResultHandlerIFC;

} // end namespace OWBI1

#endif

