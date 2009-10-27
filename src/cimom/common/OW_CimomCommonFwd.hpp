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

/**
 * NOTE: This header is internal and subject to change
 */

#ifndef OW_CIMOM_COMMON_FWD_HPP_INCLUDE_GUARD_
#define OW_CIMOM_COMMON_FWD_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/IntrusiveReference.hpp"
#include "blocxx/SharedLibraryReference.hpp"

namespace OW_NAMESPACE
{

class CIMServer;
class ProviderManager;
typedef blocxx::IntrusiveReference<ProviderManager> ProviderManagerRef;
class PollingManager;
typedef blocxx::IntrusiveReference<PollingManager> PollingManagerRef;
class IndicationRepLayer;
class IndicationServer;
typedef blocxx::SharedLibraryReference<blocxx::IntrusiveReference<IndicationServer> > IndicationServerRef;
class AuthorizerManager;
typedef blocxx::IntrusiveReference<AuthorizerManager> AuthorizerManagerRef;
class IndicationRepLayerMediator;
typedef blocxx::IntrusiveReference<IndicationRepLayerMediator> IndicationRepLayerMediatorRef;
class EmbeddedCIMOMEnvironment;
typedef blocxx::IntrusiveReference<EmbeddedCIMOMEnvironment> EmbeddedCIMOMEnvironmentRef;
class CIMOMEnvironment;
typedef blocxx::IntrusiveReference<CIMOMEnvironment> CIMOMEnvironmentRef;
class LocalCIMOMHandle;
typedef blocxx::IntrusiveReference<LocalCIMOMHandle> LocalCIMOMHandleRef;
class ProviderIFCLoaderBase;
typedef blocxx::IntrusiveReference<ProviderIFCLoaderBase> ProviderIFCLoaderRef;




} // end namespace OW_NAMESPACE

#endif

