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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#ifndef OW_SELECT_ENGINE_HPP_
#define OW_SELECT_ENGINE_HPP_
#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_SelectableCallbackIFC.hpp"
#include "OW_Array.hpp"
#include "OW_Exception.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OpenWBEM
{

OW_DECLARE_APIEXCEPTION(Select, OW_COMMON_API)
class OW_COMMON_API SelectEngine
{
public:
	void addSelectableObject(const SelectableIFCRef& obj,
		const SelectableCallbackIFCRef& cb);
	void go(); // Throws SelectException on error
	void stop();
private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	Array<SelectableIFCRef> m_selectableObjs;
	Array<SelectableCallbackIFCRef> m_callbacks;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	bool m_stopFlag;
};

class OW_COMMON_API SelectEngineStopper : public SelectableCallbackIFC
{
public:
	SelectEngineStopper(SelectEngine& engine);
protected:
	virtual void doSelected(SelectableIFCRef& selectedObject);
private:
	SelectEngine& m_engine;
};

} // end namespace OpenWBEM

#endif
