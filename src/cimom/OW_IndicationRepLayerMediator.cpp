/*******************************************************************************
* Copyright (C) 2003 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_IndicationRepLayerMediator.hpp"
#include "OW_MutexLock.hpp"

///////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_IndicationRepLayerMediator::getClassCreationSubscriptionCount()
{
	return OW_AtomicGet(m_indicationCount) + OW_AtomicGet(m_classCount) + OW_AtomicGet(m_classCreationCount);
}

///////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_IndicationRepLayerMediator::getClassModificationSubscriptionCount()
{
	return OW_AtomicGet(m_indicationCount) + OW_AtomicGet(m_classCount) + OW_AtomicGet(m_classModificationCount);
}

///////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_IndicationRepLayerMediator::getClassDeletionSubscriptionCount()
{
	return OW_AtomicGet(m_indicationCount) + OW_AtomicGet(m_classCount) + OW_AtomicGet(m_classDeletionCount);
}

///////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_IndicationRepLayerMediator::getInstCreationSubscriptionCount()
{
	return OW_AtomicGet(m_indicationCount) + OW_AtomicGet(m_instCount) + OW_AtomicGet(m_instCreationCount);
}

///////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_IndicationRepLayerMediator::getInstModificationSubscriptionCount()
{
	return OW_AtomicGet(m_indicationCount) + OW_AtomicGet(m_instCount) + OW_AtomicGet(m_instModificationCount);
}

///////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_IndicationRepLayerMediator::getInstDeletionSubscriptionCount()
{
	return OW_AtomicGet(m_indicationCount) + OW_AtomicGet(m_instCount) + OW_AtomicGet(m_instDeletionCount);
}

///////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_IndicationRepLayerMediator::getInstReadSubscriptionCount()
{
	return OW_AtomicGet(m_indicationCount) + OW_AtomicGet(m_instCount) + OW_AtomicGet(m_instReadCount);
}

///////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_IndicationRepLayerMediator::getInstMethodCallSubscriptionCount()
{
	return OW_AtomicGet(m_indicationCount) + OW_AtomicGet(m_instCount) + OW_AtomicGet(m_instMethodCallCount);
}

///////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerMediator::addSubscription(const OW_String& subName)
{
	OW_String name(subName);
	name.toLowerCase();

	if (name == "cim_instcreation")
	{
		OW_AtomicInc(m_instCreationCount);
	}
	else if (name == "cim_instmodification")
	{
		OW_AtomicInc(m_instModificationCount);
	}
	else if (name == "cim_instdeletion")
	{
		OW_AtomicInc(m_instDeletionCount);
	}
	else if (name == "cim_instmethodcall")
	{
		OW_AtomicInc(m_instMethodCallCount);
	}
	else if (name == "cim_instread")
	{
		OW_AtomicInc(m_instReadCount);
	}
	else if (name == "cim_instindication")
	{
		OW_AtomicInc(m_instCount);
	}
	else if (name == "cim_classcreation")
	{
		OW_AtomicInc(m_classCreationCount);
	}
	else if (name == "cim_classmodification")
	{
		OW_AtomicInc(m_classModificationCount);
	}
	else if (name == "cim_classdeletion")
	{
		OW_AtomicInc(m_classDeletionCount);
	}
	else if (name == "cim_classindication")
	{
		OW_AtomicInc(m_classCount);
	}
	else if (name == "cim_indication")
	{
		OW_AtomicInc(m_indicationCount);
	}
}

///////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerMediator::deleteSubscription(const OW_String& subName)
{
	OW_String name(subName);
	name.toLowerCase();

	if (name == "cim_instcreation")
	{
		OW_AtomicDec(m_instCreationCount);
	}
	else if (name == "cim_instmodification")
	{
		OW_AtomicDec(m_instModificationCount);
	}
	else if (name == "cim_instdeletion")
	{
		OW_AtomicDec(m_instDeletionCount);
	}
	else if (name == "cim_instmethodcall")
	{
		OW_AtomicDec(m_instMethodCallCount);
	}
	else if (name == "cim_instread")
	{
		OW_AtomicDec(m_instReadCount);
	}
	else if (name == "cim_instindication")
	{
		OW_AtomicDec(m_instCount);
	}
	else if (name == "cim_classcreation")
	{
		OW_AtomicDec(m_classCreationCount);
	}
	else if (name == "cim_classmodification")
	{
		OW_AtomicDec(m_classModificationCount);
	}
	else if (name == "cim_classdeletion")
	{
		OW_AtomicDec(m_classDeletionCount);
	}
	else if (name == "cim_classindication")
	{
		OW_AtomicDec(m_classCount);
	}
	else if (name == "cim_indication")
	{
		OW_AtomicDec(m_indicationCount);
	}
}


