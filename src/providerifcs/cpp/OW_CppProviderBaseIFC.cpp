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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CppProviderBaseIFC.hpp"

namespace OpenWBEM
{

/////////////////////////////////////////////////////////////////////////////
CppProviderBaseIFC::CppProviderBaseIFC() 
	: m_dt(0)
	, m_persist(false) 
{
}

/////////////////////////////////////////////////////////////////////////////
CppProviderBaseIFC::CppProviderBaseIFC(const CppProviderBaseIFC& arg)
	: IntrusiveCountableBase(arg)
	, m_dt(arg.m_dt)
	, m_persist(arg.m_persist) 
{
}

/////////////////////////////////////////////////////////////////////////////
CppProviderBaseIFC::~CppProviderBaseIFC() 
{
}

/////////////////////////////////////////////////////////////////////////////
void CppProviderBaseIFC::updateAccessTime() 
{ 
	m_dt.setToCurrent(); 
}

/////////////////////////////////////////////////////////////////////////////
void 
CppProviderBaseIFC::initialize(const ProviderEnvironmentIFCRef&) {}
/////////////////////////////////////////////////////////////////////////////
CppInstanceProviderIFC* 
CppProviderBaseIFC::getInstanceProvider() 
{ 
	return 0; 
}

/////////////////////////////////////////////////////////////////////////////
CppSecondaryInstanceProviderIFC* 
CppProviderBaseIFC::getSecondaryInstanceProvider() 
{ 
	return 0; 
}

/////////////////////////////////////////////////////////////////////////////
CppMethodProviderIFC* 
CppProviderBaseIFC::getMethodProvider() 
{ 
	return 0; 
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
/////////////////////////////////////////////////////////////////////////////
CppAssociatorProviderIFC* 
CppProviderBaseIFC::getAssociatorProvider() 
{ 
	return 0; 
}
#endif

/////////////////////////////////////////////////////////////////////////////
CppIndicationExportProviderIFC* 
CppProviderBaseIFC::getIndicationExportProvider() 
{ 
	return 0; 
}

/////////////////////////////////////////////////////////////////////////////
CppPolledProviderIFC* 
CppProviderBaseIFC::getPolledProvider() 
{ 
	return 0; 
}

/////////////////////////////////////////////////////////////////////////////
CppIndicationProviderIFC* 
CppProviderBaseIFC::getIndicationProvider() 
{ 
	return 0; 
}

/////////////////////////////////////////////////////////////////////////////
DateTime 
CppProviderBaseIFC::getLastAccessTime() const  
{ 
	return m_dt; 
}

/////////////////////////////////////////////////////////////////////////////
bool 
CppProviderBaseIFC::canUnload() 
{ 
	return true; 
}

/////////////////////////////////////////////////////////////////////////////
bool 
CppProviderBaseIFC::getPersist() const 
{ 
	return m_persist; 
}

/////////////////////////////////////////////////////////////////////////////
void 
CppProviderBaseIFC::setPersist(bool persist) 
{ 
	m_persist = persist; 
}

void CppProviderBaseIFC::shuttingDown()
{
}

} // end namespace OpenWBEM


