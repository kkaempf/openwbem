/*******************************************************************************
* Copyright (C) 2003-2004 Quest Software, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#ifndef OW_INDICATION_REP_LAYER_MEDIATOR_HPP_
#define OW_INDICATION_REP_LAYER_MEDIATOR_HPP_
#include "OW_config.h"
#include "blocxx/String.hpp"
#include "blocxx/AtomicOps.hpp"
#include "blocxx/IntrusiveCountableBase.hpp"

namespace OW_NAMESPACE
{

/**
 * The purpose of this class is to serve as an always present bridge between
 * IndicationRepLayerImpl and provIndicationRepLayer
 */
class OW_CIMOMCOMMON_API IndicationRepLayerMediator : public blocxx::IntrusiveCountableBase
{
public:
	blocxx::UInt32 getClassCreationSubscriptionCount();
	blocxx::UInt32 getClassModificationSubscriptionCount();
	blocxx::UInt32 getClassDeletionSubscriptionCount();
	blocxx::UInt32 getInstCreationSubscriptionCount();
	blocxx::UInt32 getInstModificationSubscriptionCount();
	blocxx::UInt32 getInstDeletionSubscriptionCount();
	blocxx::UInt32 getInstReadSubscriptionCount();
	blocxx::UInt32 getInstMethodCallSubscriptionCount();
	void addSubscription(const blocxx::String& subName);
	void deleteSubscription(const blocxx::String& subName);
private:
	blocxx::Atomic_t m_classCount;
	blocxx::Atomic_t m_classCreationCount;
	blocxx::Atomic_t m_classModificationCount;
	blocxx::Atomic_t m_classDeletionCount;
	blocxx::Atomic_t m_instCount;
	blocxx::Atomic_t m_instCreationCount;
	blocxx::Atomic_t m_instModificationCount;
	blocxx::Atomic_t m_instDeletionCount;
	blocxx::Atomic_t m_instReadCount;
	blocxx::Atomic_t m_instMethodCallCount;
	blocxx::Atomic_t m_indicationCount;
};

} // end namespace OW_NAMESPACE

#endif
