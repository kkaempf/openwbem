/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_LOCALCIMOMHANDLE_HPP_INCLUDE_GUARD_
#define OW_LOCALCIMOMHANDLE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_RepositoryIFC.hpp"
#include "blocxx/RWLocker.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_RepositoryCIMOMHandle.hpp"

namespace OW_NAMESPACE
{

/**
 * The LocalCIMOMHandle class is a derivitive of the CIMOMHandleIFC that
 * is used by all components that need access to CIM Services.
 */
class OW_CIMOMCOMMON_API LocalCIMOMHandle : public RepositoryCIMOMHandle
{
public:

	enum EInitialLockFlag
	{
		E_NONE,
		E_READ,
		E_WRITE
	};

	/**
	 * Create a new LocalCIMOMHandle with a given repository interface
	 * and user access contol information.
	 * @param env A reference to an CIMOMEnvironment object.
	 * @param pRepos A reference to a Repository that will be used by this
	 *		LocalCIMOMHandle.
	 * @param context The operation context that will be associated with
	 *		this LocalCIMOMHandle.
	 * @param initialLock The type of lock to acquire in the constructor
	 *              and release in the destructor.
	 */
	LocalCIMOMHandle(CIMOMEnvironmentRef env, RepositoryIFCRef pRepos,
		OperationContext& context, EInitialLockFlag initialLock = E_NONE);

	virtual ~LocalCIMOMHandle();

	/**
	 * @return A reference to the CIMOMEnvironment used by this object.
	 */
	CIMOMEnvironmentRef getEnvironment() const { return m_env; }

	/**
	 * @return The features of the CIMOM this CIMOMHandleIFC is connected to as
	 * an CIMFeatures object.
	 */
	virtual CIMFeatures getServerFeatures();
	/**
	 * Export a given instance of an indication.
	 * This will cause all CIMListerners that are interested in this type
	 * of indication to be notified.
	 * @param instance	The indication instance to use in the notification.
	 */
	void exportIndication(const CIMInstance& instance,
		const blocxx::String& instNS);

private:
	CIMOMEnvironmentRef m_env;
	EInitialLockFlag m_initialLock;
};

} // end namespace OW_NAMESPACE

#endif
