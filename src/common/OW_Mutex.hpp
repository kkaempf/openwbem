/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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

#ifndef OW_MUTEX_INCLUDE_GUARD_HPP_
#define OW_MUTEX_INCLUDE_GUARD_HPP_

#include "OW_config.h"
#include "OW_ThreadTypes.hpp"


class OW_Mutex
{
public:

	/**
	 * Create a new recursive OW_Mutex object.
	 */
	OW_Mutex();

	/**
	 * Create a new OW_Mutex object.
	 * @param recursive	If true then this is a recursive mutext. Otherwise
	 * a thread will block indefinately if it calls acquire more than once.
	 */
	//OW_Mutex(bool recursive);

	/**
	 * Destroy this OW_Mutex object.
	 */
	~OW_Mutex();

	/**
	 * Acquire ownership of this OW_Mutex object.
	 * If this is a recursive mutex, this call will block if another thread has
	 * ownership of this OW_Mutex. When it returns, the current thread will be
	 * the owner of this OW_Mutex object.
	 * If this is not a recursive mutex, the behaviour is similar to recursive
	 * with the exception that more than one call to acquire will block the
	 * thread indefinately.
	 */
	void acquire();

	/**
	 * Release ownership of this OW_Mutex object. If another thread is waiting
	 * to acquire the ownership of this mutex it will stop blocking and acquire
	 * ownership when this call returns.
	 * @return ?
	 */
	bool release();

protected:

	OW_Mutex_t m_mutex;
	OW_Thread_t volatile m_owner;
	int volatile m_refCount;
	bool m_isRecursive;

private:
	OW_Mutex(const OW_Mutex&);
	OW_Mutex operator = (const OW_Mutex&);

	static OW_Thread_t zeroThread();
	static OW_Thread_t NULLTHREAD;

	friend class OW_Condition;
};


#endif // __OW_MUTEX_HPP__
