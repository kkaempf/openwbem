/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#ifndef OW_LOCAL_AUTHENTICATION_COMMON_HPP_INCLUDE_GUARD
#define OW_LOCAL_AUTHENTICATION_COMMON_HPP_INCLUDE_GUARD
#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_CommonFwd.hpp"

namespace OW_NAMESPACE
{

namespace LocalAuthenticationCommon
{

OW_DECLARE_EXCEPTION(LocalAuthentication);

/**
 * Initialize the local auth directory. This creates any directories necessary, assigns the necessary permissions, and
 * if any old authentication files are present, they are erased.
 *
 * @param local_auth_dir The directory to initialize.
 *
 * @return String The real path to the directory that was created.
 */
blocxx::String initializeDir(blocxx::String const & localAuthDir);
blocxx::String createFile(blocxx::String const & localAuthDir, const blocxx::String& uid, const blocxx::String& cookie);

static const char* const REMOVE_CMD = "remove";
static const char* const INITIALIZE_CMD = "initialize";
static const char* const CREATE_CMD = "create";

} // end namespace LocalAuthenticationCommon

} // end namespace OW_NAMESPACE

#endif

