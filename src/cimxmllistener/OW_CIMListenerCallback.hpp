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
 * @author Dan Nuffer
 */

#ifndef OW_CIM_LISTENER_CALLBACK_HPP_
#define OW_CIM_LISTENER_CALLBACK_HPP_
#include "OW_config.h"
#include "OW_Reference.hpp"

namespace OpenWBEM
{

class CIMInstance;
class String;
/**
 * An abstract base class for CIM Listener callbacks.
 */
class CIMListenerCallback
{
public:
	virtual ~CIMListenerCallback();
	/**
	 * This gets called when the CIM Listener receives an indication.
	 * @param ci The CIM Instance representing the indication
	 * @param listenerPath the path from the http URL, useful for
	 * 	determining what action to take.  I.e. you can have several
	 * 	listerners on a single http server.  This lets you know which
	 * 	one was triggered.
	 */
	void indicationOccurred( CIMInstance& ci, const String& listenerPath )
	{
		doIndicationOccurred( ci, listenerPath );
	}
protected:
	/**
	 * This gets called when the CIM Listener receives an indication.
	 * @param ci The CIM Instance representing the indication
	 * @param listenerPath the path from the http URL, useful for
	 * 	determining what action to take.  I.e. you can have several
	 * 	listerners on a single http server.  This lets you know which
	 * 	one was triggered.
	 */
	virtual void doIndicationOccurred( CIMInstance& ci,
		const String& listenerPath ) = 0;
};
typedef Reference<CIMListenerCallback> CIMListenerCallbackRef;

} // end namespace OpenWBEM

typedef OpenWBEM::CIMListenerCallback OW_CIMListenerCallback OW_DEPRECATED;

#endif
