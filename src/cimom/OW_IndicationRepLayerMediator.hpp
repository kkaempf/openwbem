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

/**
 * The purpose of this class is to serve as an always present bridge between
 * OW_IndicationRepLayerImpl and OW_provIndicationRepLayer
 */
class OW_IndicationRepLayerMediator
{
public:
	OW_UInt32 getClassCreationSubscriptionCount();
	OW_UInt32 getClassModificationSubscriptionCount();
	OW_UInt32 getClassDeletionSubscriptionCount();
	OW_UInt32 getInstCreationSubscriptionCount();
	OW_UInt32 getInstModificationSubscriptionCount();
	OW_UInt32 getInstDeletionSubscriptionCount();
	OW_UInt32 getInstReadSubscriptionCount();
	OW_UInt32 getInstMethodCallSubscriptionCount();
	void addSubscription(const OW_String& subName);
	void deleteSubscription(const OW_String& subName);

private:
	OW_Atomic_t m_classCount;
	OW_Atomic_t m_classCreationCount;
	OW_Atomic_t m_classModificationCount;
	OW_Atomic_t m_classDeletionCount;
	OW_Atomic_t m_instCount;
	OW_Atomic_t m_instCreationCount;
	OW_Atomic_t m_instModificationCount;
	OW_Atomic_t m_instDeletionCount;
	OW_Atomic_t m_instReadCount;
	OW_Atomic_t m_instMethodCallCount;
	OW_Atomic_t m_indicationCount;
};


#endif


