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

#ifndef OW_SHARED_LIBRARY_REFERENCE_HPP_
#define OW_SHARED_LIBRARY_REFERENCE_HPP_

#include "OW_config.h"
#include "OW_Reference.hpp"
#include "OW_SharedLibrary.hpp"

template <class T>
class OW_SharedLibraryReference
{
public:
	OW_SharedLibraryReference(OW_SharedLibraryRef lib, OW_Reference<T> obj)
	: m_sharedLib(lib), m_obj(obj)
	{}

	OW_SharedLibraryReference(OW_SharedLibraryRef lib, T* obj)
	: m_sharedLib(lib), m_obj(OW_Reference<T>(obj))
	{}

	OW_SharedLibraryReference(const OW_SharedLibraryReference<T>& arg)
	: m_sharedLib(arg.m_sharedLib), m_obj(arg.m_obj)
	{
	}

	OW_SharedLibraryReference()
	: m_sharedLib(), m_obj()
	{}

	OW_SharedLibraryReference<T>& operator=(const OW_SharedLibraryReference<T>& arg)
	{
		m_obj = arg.m_obj;
		m_sharedLib = arg.m_sharedLib;
		return *this;
	}

	~OW_SharedLibraryReference()
	{
		try
		{
			m_obj = 0;
			m_sharedLib = 0;
		}
		catch (...)
		{
			// don't let exceptions escape
		}
	}

	OW_SharedLibraryRef getLibRef() const
	{
		return m_sharedLib;
	}

	T* operator->() const
	{
		return &*m_obj;
	}
	
private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (m_obj) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (m_obj) ? 0: &dummy::nonnull; }

	void setNull()
	{
		m_obj = 0;
		m_sharedLib = 0;
	}
	
	template <class U>
	OW_SharedLibraryReference<U> cast_to()
	{
		OW_SharedLibraryReference<U> rval;
		rval.m_obj = m_obj.cast_to<U>();
		rval.m_sharedLib = m_sharedLib;
		return rval;
	}

	template <class U>
	void useRefCountOf(const OW_SharedLibraryReference<U>& arg)
	{
		m_obj.useRefCountOf(arg.m_obj);
	}

	OW_Bool isNull() const
	{
		return m_obj.isNull();
	}
	/* This is so cast_to will work */
	template <class U> friend class OW_SharedLibraryReference;

private:
	OW_SharedLibraryRef m_sharedLib;
	OW_Reference<T> m_obj;
};

#endif
