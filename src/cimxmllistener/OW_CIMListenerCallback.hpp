/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
#ifndef OW_CIM_LISTENER_CALLBACK_HPP_
#define OW_CIM_LISTENER_CALLBACK_HPP_

#include "OW_config.h"
#include "OW_Reference.hpp"

class OW_CIMInstance;
class OW_String;

/**
 * An abstract base class for CIM Listener callbacks.
 */

class OW_CIMListenerCallback
{
public:
	virtual ~OW_CIMListenerCallback();

	/**
	 * This gets called when the CIM Listener receives an indication.
	 * @param ci The CIM Instance representing the indication
	 * @param listenerPath the path from the http URL, useful for
	 * 	determining what action to take.  I.e. you can have several
	 * 	listerners on a single http server.  This lets you know which
	 * 	one was triggered.
	 */
	void indicationOccurred( OW_CIMInstance& ci, const OW_String& listenerPath )
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
	virtual void doIndicationOccurred( OW_CIMInstance& ci,
		const OW_String& listenerPath ) = 0;
};

typedef OW_Reference<OW_CIMListenerCallback> OW_CIMListenerCallbackRef;

#endif
