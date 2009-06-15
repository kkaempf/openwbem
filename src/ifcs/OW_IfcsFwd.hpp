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

#ifndef OW_IFCS_FWD_HPP_INCLUDE_GUARD_
#define OW_IFCS_FWD_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/IntrusiveReference.hpp"
#include "blocxx/SharedLibraryReference.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_CIMFwd.hpp"

namespace OW_NAMESPACE
{

class CIMOMHandleIFC;
typedef IntrusiveReference<CIMOMHandleIFC> CIMOMHandleIFCRef;

class RequestHandlerIFC;
typedef SharedLibraryReference<IntrusiveReference<RequestHandlerIFC> > RequestHandlerIFCRef;

class ServiceIFC;
typedef SharedLibraryReference<IntrusiveReference<ServiceIFC> > ServiceIFCRef;

class WQLIFC;
typedef SharedLibraryReference<IntrusiveReference<WQLIFC> > WQLIFCRef;

class RepositoryIFC;
typedef IntrusiveReference<RepositoryIFC> RepositoryIFCRef;
typedef SharedLibraryReference<RepositoryIFCRef> SharedLibraryRepositoryIFCRef;

class AuthorizerIFC;
typedef SharedLibraryReference<IntrusiveReference<AuthorizerIFC> > AuthorizerIFCRef;

class Authorizer2IFC;
typedef SharedLibraryReference<IntrusiveReference<Authorizer2IFC> > Authorizer2IFCRef;

class AuthenticatorIFC;
typedef SharedLibraryReference<IntrusiveReference<AuthenticatorIFC> > AuthenticatorIFCRef;

class ServiceEnvironmentIFC;
typedef IntrusiveReference<ServiceEnvironmentIFC> ServiceEnvironmentIFCRef;

template <typename T> class ResultHandlerIFC;
typedef ResultHandlerIFC<CIMClass> CIMClassResultHandlerIFC;
typedef ResultHandlerIFC<CIMInstance> CIMInstanceResultHandlerIFC;
typedef ResultHandlerIFC<CIMObjectPath> CIMObjectPathResultHandlerIFC;
typedef ResultHandlerIFC<String> StringResultHandlerIFC;
typedef ResultHandlerIFC<CIMQualifierType> CIMQualifierTypeResultHandlerIFC;

class CIMProtocolIFC;
typedef IntrusiveReference<CIMProtocolIFC> CIMProtocolIFCRef;

class ClientAuthCBIFC;
typedef IntrusiveReference<ClientAuthCBIFC> ClientAuthCBIFCRef;


} // end namespace OW_NAMESPACE

#endif


