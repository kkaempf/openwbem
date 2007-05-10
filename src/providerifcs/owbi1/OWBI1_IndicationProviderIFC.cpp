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

#include "OWBI1_config.h"
#include "OWBI1_IndicationProviderIFC.hpp"

namespace OWBI1
{

///////////////////////////////////////////////////////////////////////////////
BI1IndicationProviderIFC::~BI1IndicationProviderIFC() {}
///////////////////////////////////////////////////////////////////////////////
int
BI1IndicationProviderIFC::mustPoll(
	const ProviderEnvironmentIFCRef &, 
	const WQLSelectStatement &, 
	const String &, 
	const String&, 
	const StringArray&) 
{
	return 0; // means don't poll enumInstances.
}
///////////////////////////////////////////////////////////////////////////////
void
BI1IndicationProviderIFC::authorizeFilter(
	const ProviderEnvironmentIFCRef &, 
	const WQLSelectStatement &, 
	const String &, 
	const String&, 
	const StringArray&, 
	const String &) 
{
}
	
///////////////////////////////////////////////////////////////////////////////
void 
BI1IndicationProviderIFC::getIndicationProviderInfo(IndicationProviderInfo&)
{
}
///////////////////////////////////////////////////////////////////////////////
void
BI1IndicationProviderIFC::activateFilter(
	const ProviderEnvironmentIFCRef&,
	const WQLSelectStatement&, 
	const String&, 
	const String&,
	const StringArray&, 
	bool)
{
}
///////////////////////////////////////////////////////////////////////////////
void
BI1IndicationProviderIFC::deActivateFilter(
	const ProviderEnvironmentIFCRef&,
	const WQLSelectStatement&, 
	const String&, 
	const String&,
	const StringArray&, 
	bool)
{
}
///////////////////////////////////////////////////////////////////////////////
BI1IndicationProviderIFC* 
BI1IndicationProviderIFC::getIndicationProvider() 
{ 
	return this; 
}

} // end namespace OWBI1
