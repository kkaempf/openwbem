/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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

#ifndef OWBI1_CIM_SCOPE_REP_HPP_INCLUDE_GUARD_
#define OWBI1_CIM_SCOPE_REP_HPP_INCLUDE_GUARD_

#include "OWBI1_config.h"
#include "OWBI1_COWIntrusiveCountableBase.hpp"
#include "OW_CIMScope.hpp"

namespace OWBI1
{

namespace detail
{

struct CIMScopeRep : public COWIntrusiveCountableBase
{
	CIMScopeRep(const OpenWBEM::CIMScope& scope_) : scope(scope_) {}
	OpenWBEM::CIMScope scope;
	CIMScopeRep* clone() const { return new CIMScopeRep(*this); }

};

inline bool operator<(const CIMScopeRep& x, const CIMScopeRep& y)
{
	return x.scope < y.scope;
}

inline OpenWBEM::CIMScope getOWObj(const CIMScopeRep& in)
{
	return in.scope;
}


} // end namespace detail

} // end namespace OWBI1

#endif





