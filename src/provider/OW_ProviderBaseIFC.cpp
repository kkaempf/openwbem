/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#include "OW_ProviderBaseIFC.hpp"
#include "OW_InstanceProviderIFC.hpp"
#include "OW_SecondaryInstanceProviderIFC.hpp"
#include "OW_MethodProviderIFC.hpp"
#include "OW_IndicationProviderIFC.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_PolledProviderIFC.hpp"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_AssociatorProviderIFC.hpp"
#endif

namespace OW_NAMESPACE
{

///////////////////////////////////////////////////////////////////////////////
ProviderBaseIFC::~ProviderBaseIFC() 
{
}

InstanceProviderIFC* 
ProviderBaseIFC::getInstanceProvider()
{
	return 0;
}

MethodProviderIFC* 
ProviderBaseIFC::getMethodProvider()
{
	return 0;
}

IndicationExportProviderIFC* 
ProviderBaseIFC::getIndicationExportProvider()
{
	return 0;
}

IndicationProviderIFC* 
ProviderBaseIFC::getIndicationProvider()
{
	return 0;
}

PolledProviderIFC* 
ProviderBaseIFC::getPolledProvider()
{
	return 0;
}

SecondaryInstanceProviderIFC* 
ProviderBaseIFC::getSecondaryInstanceProvider()
{
	return 0;
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
AssociatorProviderIFC* 
ProviderBaseIFC::getAssociatorProvider()
{
	return 0;
}

#endif
QueryProviderIFC* 
ProviderBaseIFC::getQueryProvider()
{
	return 0;
}

} // end namespace OW_NAMESPACE

