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

#ifndef OW_PROVIDER_FWD_HPP_INCLUDE_GUARD_
#define OW_PROVIDER_FWD_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/IntrusiveReference.hpp"
#include "blocxx/ArrayFwd.hpp"

namespace OW_NAMESPACE
{

class ProviderEnvironmentIFC;
typedef blocxx::IntrusiveReference<ProviderEnvironmentIFC> ProviderEnvironmentIFCRef;

class ProviderRegistrationEnvironmentIFC;
typedef blocxx::IntrusiveReference<ProviderRegistrationEnvironmentIFC> ProviderRegistrationEnvironmentIFCRef;

class InstanceProviderIFC;
typedef blocxx::IntrusiveReference<InstanceProviderIFC> InstanceProviderIFCRef;

class SecondaryInstanceProviderIFC;
typedef blocxx::IntrusiveReference<SecondaryInstanceProviderIFC> SecondaryInstanceProviderIFCRef;
typedef blocxx::Array<SecondaryInstanceProviderIFCRef> SecondaryInstanceProviderIFCRefArray;

class MethodProviderIFC;
typedef blocxx::IntrusiveReference<MethodProviderIFC> MethodProviderIFCRef;

class PolledProviderIFC;
typedef blocxx::IntrusiveReference<PolledProviderIFC> PolledProviderIFCRef;

class ProviderBaseIFC;
typedef blocxx::IntrusiveReference<ProviderBaseIFC> ProviderBaseIFCRef;
typedef blocxx::Array<ProviderBaseIFCRef> ProviderBaseIFCRefArray;

class AssociatorProviderIFC;
typedef blocxx::IntrusiveReference< AssociatorProviderIFC > AssociatorProviderIFCRef;

class IndicationProviderIFC;
typedef blocxx::IntrusiveReference< IndicationProviderIFC > IndicationProviderIFCRef;
typedef blocxx::Array<IndicationProviderIFCRef> IndicationProviderIFCRefArray;

class IndicationExportProviderIFC;
typedef blocxx::IntrusiveReference<IndicationExportProviderIFC> IndicationExportProviderIFCRef;
typedef blocxx::Array<IndicationExportProviderIFCRef> IndicationExportProviderIFCRefArray;

class QueryProviderIFC;
typedef blocxx::IntrusiveReference<QueryProviderIFC> QueryProviderIFCRef;
class QueryProviderInfo;

} // end namespace OW_NAMESPACE

#endif



