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
 * This header is meant for providers convenience so they can just include one
 * file to get all the classes which a typical provider may use.
 * No includes in this header will ever be removed. Some headers may
 * be pulled in transitively, and these may change, so don't rely on transitive
 * includes.
 *
 * @author Dan Nuffer
 */



#ifndef OWBI1_PROVIDER_INCLUDES_HPP_INCLUDE_GUARD_
#define OWBI1_PROVIDER_INCLUDES_HPP_INCLUDE_GUARD_


#include "OWBI1_config.h"
#include "OWBI1_IntrusiveReference.hpp"
#include "OWBI1_ProviderEnvironmentIFC.hpp"
#include "OWBI1_BI1AssociatorProviderIFC.hpp"
#include "OWBI1_BI1InstanceProviderIFC.hpp"
#include "OWBI1_BI1SecondaryInstanceProviderIFC.hpp"
#include "OWBI1_BI1MethodProviderIFC.hpp"
#include "OWBI1_BI1PolledProviderIFC.hpp"
#include "OWBI1_BI1IndicationExportProviderIFC.hpp"
#include "OWBI1_BI1IndicationProviderIFC.hpp"
#include "OWBI1_BI1ReadOnlyInstanceProviderIFC.hpp"
#include "OWBI1_BI1SimpleInstanceProviderIFC.hpp"
#include "OWBI1_BI1SimpleAssociatorProviderIFC.hpp"
#include "OWBI1_Format.hpp"
#include "OWBI1_Enumeration.hpp"
#include "OWBI1_Logger.hpp"
#include "OWBI1_CIMOMHandleIFC.hpp"
#include "OWBI1_ResultHandlerIFC.hpp"
#include "OWBI1_CIM.hpp"
#include "OWBI1_Array.hpp"
#include "OWBI1_String.hpp"
#include "OWBI1_DateTime.hpp"
#include "OWBI1_OperationContext.hpp"
#include "OWBI1_StringStream.hpp"
#include "OWBI1_Assertion.hpp"


#endif
