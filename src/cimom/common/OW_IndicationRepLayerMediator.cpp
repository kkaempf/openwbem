/*******************************************************************************
* Copyright (C) 2003 Vintela, Inc. All rights reserved.
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
#include "OW_config.h"
#include "OW_IndicationRepLayerMediator.hpp"
#include "OW_MutexLock.hpp"

namespace OpenWBEM
{

///////////////////////////////////////////////////////////////////////////////
UInt32
IndicationRepLayerMediator::getClassCreationSubscriptionCount()
{
	return AtomicGet(m_indicationCount) + AtomicGet(m_classCount) + AtomicGet(m_classCreationCount);
}
///////////////////////////////////////////////////////////////////////////////
UInt32
IndicationRepLayerMediator::getClassModificationSubscriptionCount()
{
	return AtomicGet(m_indicationCount) + AtomicGet(m_classCount) + AtomicGet(m_classModificationCount);
}
///////////////////////////////////////////////////////////////////////////////
UInt32
IndicationRepLayerMediator::getClassDeletionSubscriptionCount()
{
	return AtomicGet(m_indicationCount) + AtomicGet(m_classCount) + AtomicGet(m_classDeletionCount);
}
///////////////////////////////////////////////////////////////////////////////
UInt32
IndicationRepLayerMediator::getInstCreationSubscriptionCount()
{
	return AtomicGet(m_indicationCount) + AtomicGet(m_instCount) + AtomicGet(m_instCreationCount);
}
///////////////////////////////////////////////////////////////////////////////
UInt32
IndicationRepLayerMediator::getInstModificationSubscriptionCount()
{
	return AtomicGet(m_indicationCount) + AtomicGet(m_instCount) + AtomicGet(m_instModificationCount);
}
///////////////////////////////////////////////////////////////////////////////
UInt32
IndicationRepLayerMediator::getInstDeletionSubscriptionCount()
{
	return AtomicGet(m_indicationCount) + AtomicGet(m_instCount) + AtomicGet(m_instDeletionCount);
}
///////////////////////////////////////////////////////////////////////////////
UInt32
IndicationRepLayerMediator::getInstReadSubscriptionCount()
{
	return AtomicGet(m_indicationCount) + AtomicGet(m_instCount) + AtomicGet(m_instReadCount);
}
///////////////////////////////////////////////////////////////////////////////
UInt32
IndicationRepLayerMediator::getInstMethodCallSubscriptionCount()
{
	return AtomicGet(m_indicationCount) + AtomicGet(m_instCount) + AtomicGet(m_instMethodCallCount);
}
///////////////////////////////////////////////////////////////////////////////
void
IndicationRepLayerMediator::addSubscription(const String& subName)
{
	String name(subName);
	name.toLowerCase();
	if (name == "cim_instcreation")
	{
		AtomicInc(m_instCreationCount);
	}
	else if (name == "cim_instmodification")
	{
		AtomicInc(m_instModificationCount);
	}
	else if (name == "cim_instdeletion")
	{
		AtomicInc(m_instDeletionCount);
	}
	else if (name == "cim_instmethodcall")
	{
		AtomicInc(m_instMethodCallCount);
	}
	else if (name == "cim_instread")
	{
		AtomicInc(m_instReadCount);
	}
	else if (name == "cim_instindication")
	{
		AtomicInc(m_instCount);
	}
	else if (name == "cim_classcreation")
	{
		AtomicInc(m_classCreationCount);
	}
	else if (name == "cim_classmodification")
	{
		AtomicInc(m_classModificationCount);
	}
	else if (name == "cim_classdeletion")
	{
		AtomicInc(m_classDeletionCount);
	}
	else if (name == "cim_classindication")
	{
		AtomicInc(m_classCount);
	}
	else if (name == "cim_indication")
	{
		AtomicInc(m_indicationCount);
	}
}
///////////////////////////////////////////////////////////////////////////////
void
IndicationRepLayerMediator::deleteSubscription(const String& subName)
{
	String name(subName);
	name.toLowerCase();
	if (name == "cim_instcreation")
	{
		AtomicDec(m_instCreationCount);
	}
	else if (name == "cim_instmodification")
	{
		AtomicDec(m_instModificationCount);
	}
	else if (name == "cim_instdeletion")
	{
		AtomicDec(m_instDeletionCount);
	}
	else if (name == "cim_instmethodcall")
	{
		AtomicDec(m_instMethodCallCount);
	}
	else if (name == "cim_instread")
	{
		AtomicDec(m_instReadCount);
	}
	else if (name == "cim_instindication")
	{
		AtomicDec(m_instCount);
	}
	else if (name == "cim_classcreation")
	{
		AtomicDec(m_classCreationCount);
	}
	else if (name == "cim_classmodification")
	{
		AtomicDec(m_classModificationCount);
	}
	else if (name == "cim_classdeletion")
	{
		AtomicDec(m_classDeletionCount);
	}
	else if (name == "cim_classindication")
	{
		AtomicDec(m_classCount);
	}
	else if (name == "cim_indication")
	{
		AtomicDec(m_indicationCount);
	}
}

} // end namespace OpenWBEM

