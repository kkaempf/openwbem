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

//
//  OW_IntrusiveReference
//
//  A smart pointer that uses intrusive reference counting.
//
//  Relies on unqualified calls to
//  
//      void OW_IntrusiveReference_add_ref(T * p);
//      void OW_IntrusiveReference_release(T * p);
//
//          (p != 0)
//
//  The object is responsible for destroying itself.
//

template<class T> class OW_IntrusiveReference
{
private:

    typedef OW_IntrusiveReference this_type;

public:

    typedef T element_type;

    OW_IntrusiveReference(): p_(0)
    {
    }

    OW_IntrusiveReference(T * p, bool add_ref = true): p_(p)
    {
        if(p_ != 0 && add_ref) OW_IntrusiveReference_add_ref(p_);
    }

    template<class U> OW_IntrusiveReference(OW_IntrusiveReference<U> const & rhs): p_(rhs.get())
    {
        if(p_ != 0) OW_IntrusiveReference_add_ref(p_);
    }

    OW_IntrusiveReference(OW_IntrusiveReference const & rhs): p_(rhs.p_)
    {
        if(p_ != 0) OW_IntrusiveReference_add_ref(p_);
    }

    ~OW_IntrusiveReference()
    {
        if(p_ != 0) OW_IntrusiveReference_release(p_);
    }

    template<class U> OW_IntrusiveReference & operator=(OW_IntrusiveReference<U> const & rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    OW_IntrusiveReference & operator=(OW_IntrusiveReference const & rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    OW_IntrusiveReference & operator=(T * rhs)
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

    typedef T * (OW_IntrusiveReference::*unspecified_bool_type) () const;

    operator unspecified_bool_type () const
    {
        return p_ == 0? 0: &OW_IntrusiveReference::get;
    }

    bool operator! () const
    {
        return p_ == 0;
    }

    void swap(OW_IntrusiveReference & rhs)
    {
        T * tmp = p_;
        p_ = rhs.p_;
        rhs.p_ = tmp;
    }

private:

    T * p_;
};

template<class T, class U> inline bool operator==(OW_IntrusiveReference<T> const & a, OW_IntrusiveReference<U> const & b)
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(OW_IntrusiveReference<T> const & a, OW_IntrusiveReference<U> const & b)
{
    return a.get() != b.get();
}

template<class T> inline bool operator==(OW_IntrusiveReference<T> const & a, T * b)
{
    return a.get() == b;
}

template<class T> inline bool operator!=(OW_IntrusiveReference<T> const & a, T * b)
{
    return a.get() != b;
}

template<class T> inline bool operator==(T * a, OW_IntrusiveReference<T> const & b)
{
    return a == b.get();
}

template<class T> inline bool operator!=(T * a, OW_IntrusiveReference<T> const & b)
{
    return a != b.get();
}

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96

// Resolve the ambiguity between our op!= and the one in rel_ops

template<class T> inline bool operator!=(OW_IntrusiveReference<T> const & a, OW_IntrusiveReference<T> const & b)
{
    return a.get() != b.get();
}

#endif

template<class T> inline bool operator<(OW_IntrusiveReference<T> const & a, OW_IntrusiveReference<T> const & b)
{
    return std::less<T *>()(a.get(), b.get());
}

template<class T> void swap(OW_IntrusiveReference<T> & lhs, OW_IntrusiveReference<T> & rhs)
{
    lhs.swap(rhs);
}

template<class T, class U> OW_IntrusiveReference<T> static_pointer_cast(OW_IntrusiveReference<U> const & p)
{
    return static_cast<T *>(p.get());
}

template<class T, class U> OW_IntrusiveReference<T> const_pointer_cast(OW_IntrusiveReference<U> const & p)
{
    return const_cast<T *>(p.get());
}

template<class T, class U> OW_IntrusiveReference<T> dynamic_pointer_cast(OW_IntrusiveReference<U> const & p)
{
    return dynamic_cast<T *>(p.get());
}

// If you want your class to be managed by OW_IntrusiveReference, then derive
// from this class.  Note that if multiple inheritance is used, you must derive
// "virtual"ly.
class OW_IntrusiveCountableBase
{
private:

    OW_RefCount use_count_;

    OW_IntrusiveCountableBase(OW_IntrusiveCountableBase const &);
    OW_IntrusiveCountableBase & operator=(OW_IntrusiveCountableBase const &);

protected:

    OW_IntrusiveCountableBase(): use_count_(0)
    {
    }

    virtual ~OW_IntrusiveCountableBase()
    {
    }

public:

    inline friend void OW_IntrusiveReference_add_ref(OW_IntrusiveCountableBase * p)
    {
		p->use_count_.inc();
    }

    inline friend void OW_IntrusiveReference_release(OW_IntrusiveCountableBase * p)
    {
        if(p->use_count_.decAndTest())
			delete p;
    }
};




#endif

