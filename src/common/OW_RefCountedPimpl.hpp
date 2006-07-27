#ifndef REF_COUNTED_PIMPL_HPP
#define REF_COUNTED_PIMPL_HPP

/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Quest Software, Inc., nor the
*       names of its contributors or employees may be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/// @author Kevin S. Van Horn

#include "OW_config.h"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_IntrusiveReference.hpp"

namespace OW_NAMESPACE
{

	/**
	* PURPOSE: Make it easy to define immutable classes or handle classes,
	* making it inexpensive to copy them, while keeping implementation details
	* out of the header file.  This is done by declaring
	* <PRE>
	*   class FooImpl;
	*   class Foo : private RefcountedPimpl<FooImpl>
	*   {
	*      ...
	*   };
	* </PRE>
	* and using @c pimpl() in @c Foo member functions to access the
	* internal data.  The @c Foo object is a wrapper around a reference-counted
	* pointer to a @c FooImpl object.
	*
	* @pre @a Impl is derived from <tt>OpenWBEM::IntrusiveCountableBase</tt>.
	*/
	template <typename Impl>
	class RefCountedPimpl
	{
	protected:
		// These member functions will only get instantiated where they are
		// used -- in the implementation code for the class.  Thus the full
		// definition of Impl need not be visible in code that merely uses
		// a class derived from RefCountedPimpl<Impl>.

		/// @post Object holds null reference
		// 
		RefCountedPimpl()
		: pImpl(0)
		{
		}

		/// @post Object holds reference to *p.
		//
		RefCountedPimpl(Impl * p)
		: pImpl(p)
		{
		}

		/// @post Object holds reference to *p.
		/// @pre The dynamic type of *p is @a Impl or a subclass of @a Impl.
		//
		RefCountedPimpl(IntrusiveCountableBase * p)
		: pImpl(p)
		{
		}

		/// @return A pointer to the @a Impl object held by <tt>*this</tt>.
		//
		Impl * pimpl() const
		{
			return static_cast<Impl *>(pImpl.getPtr());
		}

		/// @return True if and only if this object holds a null pointer.
		//
		bool null() const
		{
			return !pImpl;
		}

	private:
		// IntrusiveCountableBase is used instead of Impl because we want to
		// be able to hide the full definition of Impl while still having
		// the implicit copy ctor, copy assignment, and dtor available.
		//
		IntrusiveReference<IntrusiveCountableBase> pImpl;
	};

}

#endif
