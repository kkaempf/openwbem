/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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

#ifndef OW_SERVICEIFC_NAMES_HPP_INCLUDE_GUARD_
#define OW_SERVICEIFC_NAMES_HPP_INCLUDE_GUARD_
#include "OW_config.h"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OpenWBEM
{

namespace ServiceIFCNames
{

static const char* const HTTPServer = "HTTPServer";
static const char* const XMLListener = "XMLListener";
static const char* const WQL = "WQL";
static const char* const SimpleAuthorizer = "SimpleAuthorizer";
static const char* const XMLExecute = "XMLExecute";
static const char* const BinaryRequestHandler = "BinaryRequestHandler";
static const char* const AuthorizerManager = "AuthorizerManager";
static const char* const ProviderManager = "ProviderManager";
static const char* const CIMRepository = "CIMRepository";
static const char* const CIMServer = "CIMServer";
static const char* const AuthManager = "AuthManager";
static const char* const PollingManager = "PollingManager";
static const char* const WQLFilterRep = "WQLFilterRep";
static const char* const IndicationServer = "IndicationServer";
static const char* const IndicationRepLayer = "IndicationRepLayer";

} // end namespace ServiceIFCNames

} // end namespace OpenWBEM


#endif

