/*******************************************************************************
* Copyright (C) 2004 Quest Software, Inc. All rights reserved.
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
typedef blocxx::IntrusiveReference<CIMOMHandleIFC> CIMOMHandleIFCRef;

class RequestHandlerIFC;
typedef blocxx::SharedLibraryReference<blocxx::IntrusiveReference<RequestHandlerIFC> > RequestHandlerIFCRef;

class ServiceIFC;
typedef blocxx::SharedLibraryReference<blocxx::IntrusiveReference<ServiceIFC> > ServiceIFCRef;

class WQLIFC;
typedef blocxx::SharedLibraryReference<blocxx::IntrusiveReference<WQLIFC> > WQLIFCRef;

class RepositoryIFC;
typedef blocxx::IntrusiveReference<RepositoryIFC> RepositoryIFCRef;
typedef blocxx::SharedLibraryReference<RepositoryIFCRef> SharedLibraryRepositoryIFCRef;

class AuthorizerIFC;
typedef blocxx::SharedLibraryReference<blocxx::IntrusiveReference<AuthorizerIFC> > AuthorizerIFCRef;

class Authorizer2IFC;
typedef blocxx::SharedLibraryReference<blocxx::IntrusiveReference<Authorizer2IFC> > Authorizer2IFCRef;

class AuthenticatorIFC;
typedef blocxx::SharedLibraryReference<blocxx::IntrusiveReference<AuthenticatorIFC> > AuthenticatorIFCRef;

class ServiceEnvironmentIFC;
typedef blocxx::IntrusiveReference<ServiceEnvironmentIFC> ServiceEnvironmentIFCRef;

template <typename T> class ResultHandlerIFC;
typedef ResultHandlerIFC<CIMClass> CIMClassResultHandlerIFC;
typedef ResultHandlerIFC<CIMInstance> CIMInstanceResultHandlerIFC;
typedef ResultHandlerIFC<CIMObjectPath> CIMObjectPathResultHandlerIFC;
typedef ResultHandlerIFC<blocxx::String> StringResultHandlerIFC;
typedef ResultHandlerIFC<CIMQualifierType> CIMQualifierTypeResultHandlerIFC;

class CIMProtocolIFC;
typedef blocxx::IntrusiveReference<CIMProtocolIFC> CIMProtocolIFCRef;

class ClientAuthCBIFC;
typedef blocxx::IntrusiveReference<ClientAuthCBIFC> ClientAuthCBIFCRef;


} // end namespace OW_NAMESPACE

#endif


