/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef OW_INDICATION_REP_LAYER_MEDIATOR_HPP_
#define OW_INDICATION_REP_LAYER_MEDIATOR_HPP_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_AtomicOps.hpp"

namespace OpenWBEM
{

/**
 * The purpose of this class is to serve as an always present bridge between
 * IndicationRepLayerImpl and provIndicationRepLayer
 */
class IndicationRepLayerMediator
{
public:
	UInt32 getClassCreationSubscriptionCount();
	UInt32 getClassModificationSubscriptionCount();
	UInt32 getClassDeletionSubscriptionCount();
	UInt32 getInstCreationSubscriptionCount();
	UInt32 getInstModificationSubscriptionCount();
	UInt32 getInstDeletionSubscriptionCount();
	UInt32 getInstReadSubscriptionCount();
	UInt32 getInstMethodCallSubscriptionCount();
	void addSubscription(const String& subName);
	void deleteSubscription(const String& subName);
private:
	Atomic_t m_classCount;
	Atomic_t m_classCreationCount;
	Atomic_t m_classModificationCount;
	Atomic_t m_classDeletionCount;
	Atomic_t m_instCount;
	Atomic_t m_instCreationCount;
	Atomic_t m_instModificationCount;
	Atomic_t m_instDeletionCount;
	Atomic_t m_instReadCount;
	Atomic_t m_instMethodCallCount;
	Atomic_t m_indicationCount;
};

} // end namespace OpenWBEM

#endif
