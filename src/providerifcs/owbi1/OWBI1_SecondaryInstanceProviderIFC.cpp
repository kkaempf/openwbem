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

#include "OW_config.h"
#include "OWBI1_SecondaryInstanceProviderIFC.hpp"

namespace OWBI1
{

///////////////////////////////////////////////////////////////////////////////
BI1SecondaryInstanceProviderIFC::~BI1SecondaryInstanceProviderIFC() 
{
}

///////////////////////////////////////////////////////////////////////////////
void
BI1SecondaryInstanceProviderIFC::filterInstances(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMName& className,
	CIMInstanceArray& instances,
	const CIMPropertyList& propertyList,
	const CIMClass& requestedClass,
	const CIMClass& cimClass )
{
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
void
BI1SecondaryInstanceProviderIFC::createInstance(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMInstance& cimInstance )
{
}

///////////////////////////////////////////////////////////////////////////////
void
BI1SecondaryInstanceProviderIFC::modifyInstance(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMInstance& modifiedInstance,
	const CIMInstance& previousInstance,
	const CIMPropertyList& propertyList,
	const CIMClass& theClass)
{
}

///////////////////////////////////////////////////////////////////////////////
void 
BI1SecondaryInstanceProviderIFC::deleteInstance(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMObjectPath& cop)
{
}

#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

///////////////////////////////////////////////////////////////////////////////
BI1SecondaryInstanceProviderIFC* 
BI1SecondaryInstanceProviderIFC::getSecondaryInstanceProvider() 
{ 
	return this; 
}

} // end namespace OWBI1


