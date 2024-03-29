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
 * This header is meant for providers convenience so they can just include one
 * file to get all the classes which a typical provider may use.
 * No includes in this header will ever be removed. Some headers may
 * be pulled in transitively, and these may change, so don't rely on transitive
 * includes.
 *
 * @author Dan Nuffer
 */



#ifndef OW_CPP_PROVIDER_INCLUDES_HPP_INCLUDE_GUARD_
#define OW_CPP_PROVIDER_INCLUDES_HPP_INCLUDE_GUARD_


#include "OW_config.h"
#include "blocxx/IntrusiveReference.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_CppAssociatorProviderIFC.hpp"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppSecondaryInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_CppIndicationExportProviderIFC.hpp"
#include "OW_CppIndicationProviderIFC.hpp"
#include "OW_CppReadOnlyInstanceProviderIFC.hpp"
#include "OW_CppSimpleInstanceProviderIFC.hpp"
#include "OW_CppSimpleAssociatorProviderIFC.hpp"
#include "OW_CppQueryProviderIFC.hpp"
#include "OW_QueryProviderInfo.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/Enumeration.hpp"
#include "blocxx/Logger.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_CIM.hpp"
#include "blocxx/Array.hpp"
#include "blocxx/String.hpp"
#include "blocxx/DateTime.hpp"
#include "OW_OperationContext.hpp"
#include "blocxx/StringStream.hpp"
#include "blocxx/Assertion.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_WQLCompile.hpp"
#include "OW_WQLInstancePropertySource.hpp"


#endif
