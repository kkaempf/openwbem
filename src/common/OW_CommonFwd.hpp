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

#ifndef OW_COMMON_FWD_HPP_INCLUDE_GUARD_
#define OW_COMMON_FWD_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_ArrayFwd.hpp"
#include "OW_IntrusiveReference.hpp"

// Yeah I know this is forbidden by the standard, but what am I gonna do?  #include <algorithm> ? I think not.
// If it causes a problem on some compiler, just #ifdef a fix in.
namespace std
{
	template <typename T> class less;
}

namespace OpenWBEM
{

struct LogMessage;
class Logger;
typedef IntrusiveReference<Logger> LoggerRef;

class LogAppender;
typedef IntrusiveReference<LogAppender> LogAppenderRef;

class String;
typedef Array<String> StringArray;

class Char16;
typedef Array<Char16> Char16Array;

template <class Key, class T, class Compare>
class SortedVectorMapDataCompare;

template<class Key, class T, class Compare = SortedVectorMapDataCompare<Key, T, std::less<Key> > >
class SortedVectorMap;

namespace ConfigFile
{
	typedef SortedVectorMap<String, String> ConfigMap;
}

class StringBuffer;
class DateTime;
class OperationContext;

template <class T> class Enumeration;
typedef Enumeration<String> StringEnumeration;

class Thread;
typedef IntrusiveReference<Thread> ThreadRef;
class ThreadPool;
typedef IntrusiveReference<ThreadPool> ThreadPoolRef;

class Bool;
typedef Array<Bool>       			BoolArray;

struct CIMFeatures;
class NonRecursiveMutexLock;
class NonRecursiveMutex;

class UnnamedPipe;
typedef IntrusiveReference<UnnamedPipe> UnnamedPipeRef;

class File;
class MD5;
class UserInfo;

class SharedLibraryLoader;
typedef IntrusiveReference<SharedLibraryLoader> SharedLibraryLoaderRef;
class TmpFile;

class Socket;
class SocketBaseImpl;
typedef IntrusiveReference<SocketBaseImpl> SocketBaseImplRef;
class ServerSocket;

class TempFileStream;


} // end namespace OpenWBEM

#endif

