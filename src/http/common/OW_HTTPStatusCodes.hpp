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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#ifndef OW_HTTPSTATUSCODES_H_INCLUDE_GUARD_
#define OW_HTTPSTATUSCODES_H_INCLUDE_GUARD_
#include "OW_config.h"

namespace OW_NAMESPACE
{

// Status codes.
const int SC_CONTINUE = 100;
const int SC_SWITCHING_PROTOCOLS = 101;
const int SC_OK = 200;
const int SC_CREATED = 201;
const int SC_ACCEPTED = 202;
const int SC_NON_AUTHORITATIVE_INFORMATION = 203;
const int SC_NO_CONTENT = 204;
const int SC_RESET_CONTENT = 205;
const int SC_PARTIAL_CONTENT = 206;
const int SC_MULTIPLE_CHOICES = 300;
const int SC_MOVED_PERMANENTLY = 301;
const int SC_MOVED_TEMPORARILY = 302;
const int SC_SEE_OTHER = 303;
const int SC_NOT_MODIFIED = 304;
const int SC_USE_PROXY = 305;
const int SC_BAD_REQUEST = 400;
const int SC_UNAUTHORIZED = 401;
const int SC_PAYMENT_REQUIRED = 402;
const int SC_FORBIDDEN = 403;
const int SC_NOT_FOUND = 404;
const int SC_METHOD_NOT_ALLOWED = 405;
const int SC_NOT_ACCEPTABLE = 406;
const int SC_PROXY_AUTHENTICATION_REQUIRED = 407;
const int SC_REQUEST_TIMEOUT = 408;
const int SC_CONFLICT = 409;
const int SC_GONE = 410;
const int SC_LENGTH_REQUIRED = 411;
const int SC_PRECONDITION_FAILED = 412;
const int SC_REQUEST_ENTITY_TOO_LARGE = 413;
const int SC_REQUEST_URI_TOO_LONG = 414;
const int SC_UNSUPPORTED_MEDIA_TYPE = 415;
const int SC_INTERNAL_SERVER_ERROR = 500;
const int SC_NOT_IMPLEMENTED = 501;
const int SC_BAD_GATEWAY = 502;
const int SC_SERVICE_UNAVAILABLE = 503;
const int SC_GATEWAY_TIMEOUT = 504;
const int SC_HTTP_VERSION_NOT_SUPPORTED = 505;
const int SC_NOT_EXTENDED = 510;

} // end namespace OW_NAMESPACE

#endif
