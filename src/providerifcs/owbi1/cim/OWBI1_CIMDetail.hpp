/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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

#ifndef OWBI1_CIM_DETAIL_HPP_INCLUDE_GUARD_
#define OWBI1_CIM_DETAIL_HPP_INCLUDE_GUARD_

#include "OWBI1_config.h"
#include "OW_Types.hpp"

// this header contains internal implementation details. DO NOT USE IT! (unless of course you're working on the OWBI1 implementation :-)

namespace OWBI1
{

namespace detail
{

template <typename T>
struct getRepT
{
	typedef void type;
};

template <>
struct getRepT<CIMObjectPath>
{
	typedef CIMObjectPathRep type;
};

template <>
struct getRepT<CIMDateTime>
{
	typedef CIMDateTimeRep type;
};

template <>
struct getRepT<CIMClass>
{
	typedef CIMClassRep type;
};

template <>
struct getRepT<CIMInstance>
{
	typedef CIMInstanceRep type;
};

template <>
struct getRepT<CIMFlavor>
{
	typedef CIMFlavorRep type;
};

template <>
struct getRepT<CIMScope>
{
	typedef CIMScopeRep type;
};

template <>
struct getRepT<CIMQualifier>
{
	typedef CIMQualifierRep type;
};

/////////////////////////////////////////////////////////////////////////////
template <typename T1, typename T2>
void unwrapArray(T1& out, const T2& in)
{
	out.resize(in.size());
	for (size_t i = 0; i < in.size(); ++i)
	{
		out[i] = getOWObj(*in[i].getRep());
	}
}

/////////////////////////////////////////////////////////////////////////////
template <typename T1, typename T2>
T1 unwrapArray(const T2& in)
{
	T1 rv(in.size());
	for (size_t i = 0; i < in.size(); ++i)
	{
		rv[i] = getOWObj(*in[i].getRep());
	}
	return rv;
}

/////////////////////////////////////////////////////////////////////////////
template <typename T1, typename T2>
void wrapArray(T1& out, const T2& in)
{
	out.resize(in.size());
	for (size_t i = 0; i < in.size(); ++i)
	{
		typedef typename getRepT<typename T1::value_type>::type rep_t;
		out[i] = typename T1::value_type(new rep_t(in[i]));
	}
}

/////////////////////////////////////////////////////////////////////////////
template <typename T1, typename T2>
T1 wrapArray(const T2& in)
{
	T1 rv(in.size());
	wrapArray(rv, in);
	return rv;
}

} // end namespace detail

} // end namespace OWBI1

#endif


