/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_SelectEngine.hpp"
#include "OW_Select.hpp"
#include "OW_ExceptionIds.hpp"

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION_WITH_ID(Select);
//////////////////////////////////////////////////////////////////////////////
void
SelectEngine::addSelectableObject(SelectableIFCRef obj,
	SelectableCallbackIFCRef cb)
{
	m_selectableObjs.push_back(obj);
	m_callbacks.push_back(cb);
}
//////////////////////////////////////////////////////////////////////////////
void
SelectEngine::go()
{
	m_stopFlag = false;
	do
	{
		SelectTypeArray selObjs;
		for (size_t i = 0; i < m_selectableObjs.size(); ++i)
		{
			selObjs.push_back(m_selectableObjs[i]->getSelectObj());
		}
		int selected = Select::select(selObjs);
		if (selected == Select::SELECT_ERROR)
		{
			OW_THROW(SelectException, "Select Error");
		}
		else if (selected == Select::SELECT_TIMEOUT
				 || selected == Select::SELECT_INTERRUPTED)
		{
			continue;
		}
		else
		{
			m_callbacks[selected]->selected(m_selectableObjs[selected]);
		}
	} while (!m_stopFlag);
}
//////////////////////////////////////////////////////////////////////////////
void
SelectEngine::stop()
{
	m_stopFlag = true;
}

} // end namespace OpenWBEM

