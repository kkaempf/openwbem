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

#include "OW_config.h"
#include "OW_Timeout.hpp"
#include "OW_DateTime.hpp"
#include "OW_Infinity.hpp"


namespace OW_NAMESPACE
{

/////////////////////////////////////////////////////////////////////////////
// static
Timeout 
Timeout::absolute(const DateTime& dt)
{
	return Timeout(E_ABSOLUTE, dt);
}
	
/////////////////////////////////////////////////////////////////////////////
// static 
Timeout 
Timeout::relative(float seconds)
{
	return Timeout(E_RELATIVE, seconds);
}
	
/////////////////////////////////////////////////////////////////////////////
// static 
Timeout 
Timeout::relativeWithReset(float seconds)
{
	return Timeout(E_RELATIVE_WITH_RESET, seconds);
}
	
/////////////////////////////////////////////////////////////////////////////
//static 
Timeout Timeout::infinite = relative(INFINITY);

Timeout::ETimeoutType 
Timeout::getType() const
{
	return m_type;
}

DateTime
Timeout::getAbsolute() const
{
	return m_absolute;
}

float 
Timeout::getRelative() const
{
	return m_seconds;
}

Timeout::Timeout()
{
}
	
Timeout::Timeout(ETimeoutType type, const DateTime& dt)
	: m_type(type)
	, m_absolute(dt)
{
}

Timeout::Timeout(ETimeoutType type, float seconds)
	: m_type(type)
	, m_seconds(seconds)
{
}

bool operator==(const Timeout& x, const Timeout& y)
{
	if (x.getType() != y.getType())
	{
		return false;
	}
	if (x.getType() == Timeout::E_ABSOLUTE)
	{
		return x.getAbsolute() == y.getAbsolute();
	}
	else
	{
		return x.getRelative() == y.getRelative();
	}
}

bool operator!=(const Timeout& x, const Timeout& y)
{
	return !(x == y);
}

} // end namespace OW_NAMESPACE




