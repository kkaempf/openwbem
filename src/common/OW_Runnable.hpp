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

#ifndef OW_RUNNABLE_HPP_INCLUDE_GUARD_
#define OW_RUNNABLE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Reference.hpp"

/**
 * There are two methods for creating a thread of execution in the OW systems.
 * One is to derive from OW_Thread and implement the run method and call start
 * on instances of the class to get the thread running.
 * The other method is to derive from OW_Runnable and pass references of the
 * derived class to the static run method on OW_Thread that takes an
 * OW_RunnableRef class. This technique allows the caller to run the
 * OW_Runnable object as a separate thread or in the same thread based on
 * the separateThread argument passed to the OW_Thread::run method.
 *
 * Example:
 *
 *		class MyRunnable : public OW_Runnable
 *		{
 *			virtual void run()
 *			{
 *				Some meaningful stuff for MyRunnable
 *			}
 *		};
 *
 *		void foo()
 *		{
 *			OW_RunnableRef rref(new MyRunnable);
 *			bool sepThreadFlag = (if wanted in separate thread) ? true : false;
 *			OW_Thread::run(rref, sepThreadFlag);
 *
 *			// If sepThreadFlag was true, then when we return from foo,
 *			// the MyRunnable object is still running.
 *		}
 */
class OW_Runnable
{
public:
	virtual ~OW_Runnable();
	virtual void run() = 0;
};

typedef OW_Reference<OW_Runnable> OW_RunnableRef;


#endif

