#ifndef OW_PRIVILEGE_MANAGER_MOCK_OBJECT_SCOPE_HPP_INCLUDE_GUARD_
#define OW_PRIVILEGE_MANAGER_MOCK_OBJECT_SCOPE_HPP_INCLUDE_GUARD_

/*******************************************************************************
* Copyright (C) 2008, Quest Software, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Network Associates,
*       nor Quest Software, Inc., nor Novell, Inc., nor the
*       names of its contributors or employees may be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Kevin Harris
 */

#include "OW_config.h"
#include "blocxx/Reference.hpp"
#include "OW_PrivilegeManagerMockObject.hpp"

namespace OW_NAMESPACE
{
	/**
	 * A simple privilege manager mock object manipulation class.  This will set
	 * the global privilege manager mock object at construction time and reset
	 * it when this object is destroyed (gone out of scope).
	 */
	class PrivilegeManagerMockObjectScope
	{
	public:
		PrivilegeManagerMockObjectScope(Reference<PrivilegeManagerMockObject> mock);
		virtual ~PrivilegeManagerMockObjectScope();

	private:
		// The old mock object.  This is a raw pointer because the global it
		// is copied from object is also a raw pointer.
		PrivilegeManagerMockObject* old_mock;
		Reference<PrivilegeManagerMockObject> m_mock;
	};
} // end namespace UMI

#endif
