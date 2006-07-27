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

#ifndef OW_TIMEOUT_HPP_INCLUDE_GUARD_
#define OW_TIMEOUT_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "OW_DateTime.hpp"

namespace OW_NAMESPACE
{

/**
 * A timeout can be absolute, which means that it will happen at the specified DateTime.
 * A timeout can be relative, which means that it will happen at the specified interval after the call is made.
 * A timeout can be relative with reset, which means that it will happen at the specified interval after "no activity" has occurred.
 *   "no activity" is dependent on the operation.
 * A timeout can be infinite.
 */
class Timeout
{
public:
	static Timeout absolute(const DateTime& dt);
	static Timeout relative(float seconds);
	static Timeout relativeWithReset(float seconds);

	static Timeout infinite;

	enum ETimeoutType
	{
		E_ABSOLUTE,
		E_RELATIVE,
		E_RELATIVE_WITH_RESET
	};

	ETimeoutType getType() const;
	DateTime getAbsolute() const;
	float getRelative() const;

private:
	// have to use static factory functions to create a Timeout instance
	Timeout();
	Timeout(ETimeoutType type, const DateTime& dt);
	Timeout(ETimeoutType type, float seconds);

	ETimeoutType m_type;
	DateTime m_absolute;
	float m_seconds;
};

bool operator==(const Timeout& x, const Timeout& y);
bool operator!=(const Timeout& x, const Timeout& y);

} // end namespace OW_NAMESPACE

#endif


