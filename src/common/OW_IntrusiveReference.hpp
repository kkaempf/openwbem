#ifndef OW_INTRUSIVE_REFERENCE_HPP_INCLUDE_GUARD_
#define OW_INTRUSIVE_REFERENCE_HPP_INCLUDE_GUARD_
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
//
//  Copyright (c) 2001, 2002 Peter Dimov
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
#include "OW_config.h"
#include "OW_RefCount.hpp"
#include <functional>           // for std::less

namespace OpenWBEM
{

//
//  IntrusiveReference
//
//  A smart pointer that uses intrusive reference counting.
//
//  Relies on unqualified calls to
//  
//      void IntrusiveReference_add_ref(T * p);
//      void IntrusiveReference_release(T * p);
//
//          (p != 0)
//
//  The object is responsible for destroying itself.
//
template<class T> class IntrusiveReference
{
private:
    typedef IntrusiveReference this_type;
public:
    typedef T element_type;

    IntrusiveReference(): p_(0)
    {
    }
    IntrusiveReference(T * p, bool add_ref = true): p_(p)
    {
        if(p_ != 0 && add_ref) IntrusiveReference_add_ref(p_);
    }
    template<class U> IntrusiveReference(IntrusiveReference<U> const & rhs): p_(rhs.get())
    {
        if(p_ != 0) IntrusiveReference_add_ref(p_);
    }
    IntrusiveReference(IntrusiveReference const & rhs): p_(rhs.p_)
    {
        if(p_ != 0) IntrusiveReference_add_ref(p_);
    }
    ~IntrusiveReference()
    {
        if(p_ != 0) IntrusiveReference_release(p_);
    }
    template<class U> IntrusiveReference & operator=(IntrusiveReference<U> const & rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }
    IntrusiveReference & operator=(IntrusiveReference const & rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }
    IntrusiveReference & operator=(T * rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }
    T * get() const
    {
        return p_;
    }
    T & operator*() const
    {
        return *p_;
    }
    T * operator->() const
    {
        return p_;
    }
    typedef T * (IntrusiveReference::*unspecified_bool_type) () const;
    operator unspecified_bool_type () const
    {
        return p_ == 0? 0: &IntrusiveReference::get;
    }
    bool operator! () const
    {
        return p_ == 0;
    }
    void swap(IntrusiveReference & rhs)
    {
        T * tmp = p_;
        p_ = rhs.p_;
        rhs.p_ = tmp;
    }
private:
    T * p_;
};
template<class T, class U> inline bool operator==(IntrusiveReference<T> const & a, IntrusiveReference<U> const & b)
{
    return a.get() == b.get();
}
template<class T, class U> inline bool operator!=(IntrusiveReference<T> const & a, IntrusiveReference<U> const & b)
{
    return a.get() != b.get();
}
template<class T> inline bool operator==(IntrusiveReference<T> const & a, T * b)
{
    return a.get() == b;
}
template<class T> inline bool operator!=(IntrusiveReference<T> const & a, T * b)
{
    return a.get() != b;
}
template<class T> inline bool operator==(T * a, IntrusiveReference<T> const & b)
{
    return a == b.get();
}
template<class T> inline bool operator!=(T * a, IntrusiveReference<T> const & b)
{
    return a != b.get();
}
#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96
// Resolve the ambiguity between our op!= and the one in rel_ops
template<class T> inline bool operator!=(IntrusiveReference<T> const & a, IntrusiveReference<T> const & b)
{
    return a.get() != b.get();
}
#endif
template<class T> inline bool operator<(IntrusiveReference<T> const & a, IntrusiveReference<T> const & b)
{
    return std::less<T *>()(a.get(), b.get());
}
template<class T> void swap(IntrusiveReference<T> & lhs, IntrusiveReference<T> & rhs)
{
    lhs.swap(rhs);
}
template<class T, class U> IntrusiveReference<T> static_pointer_cast(IntrusiveReference<U> const & p)
{
    return static_cast<T *>(p.get());
}
template<class T, class U> IntrusiveReference<T> const_pointer_cast(IntrusiveReference<U> const & p)
{
    return const_cast<T *>(p.get());
}
template<class T, class U> IntrusiveReference<T> dynamic_pointer_cast(IntrusiveReference<U> const & p)
{
    return dynamic_cast<T *>(p.get());
}

/**
 * If you want your class to be managed by IntrusiveReference, then derive
 * from this class.  Note that if multiple inheritance is used, you must derive
 * "virtual"ly.
 */
class IntrusiveCountableBase
{
private:
    RefCount use_count_;
    IntrusiveCountableBase(IntrusiveCountableBase const &);
    IntrusiveCountableBase & operator=(IntrusiveCountableBase const &);
protected:
    IntrusiveCountableBase(): use_count_(0)
    {
    }
    virtual ~IntrusiveCountableBase()
    {
    }
public:
    inline friend void IntrusiveReference_add_ref(IntrusiveCountableBase * p)
    {
		p->use_count_.inc();
    }
    inline friend void IntrusiveReference_release(IntrusiveCountableBase * p)
    {
        if(p->use_count_.decAndTest())
			delete p;
    }
};

} // end namespace OpenWBEM

#endif
