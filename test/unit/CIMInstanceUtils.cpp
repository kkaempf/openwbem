/*******************************************************************************
* Copyright (C) 2008, Quest Software, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of 
*       Quest Software, Inc., 
*       nor the names of its contributors or employees may be used to 
*       endorse or promote products derived from this software without 
*       specific prior written permission.
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
// @author Kevin S. Van Horn

#include "CIMInstanceUtils.hpp"
#include "OW_CIMValue.hpp"

namespace CIMInstanceUtils
{
	using OpenWBEM::CIMValue;

	template <typename T>
	CIMProperty cimProp(char const * propName, T const & propValue)
	{
		return CIMProperty(propName, CIMValue(propValue));
	}

	// Some explicit instantiations; add more as needed
	template CIMProperty cimProp(char const *, int const &);
	template CIMProperty cimProp(char const *, bool const &);
	template CIMProperty cimProp(char const *, char const * const &);

	CIMInstance cimInst(
		char const * className, CIMProperty const * properties, std::size_t N)
	{
		CIMInstance inst(className);
		for (size_t i = 0; i < N; ++i)
		{
			inst.setProperty(properties[i]);
		}
		return inst;
	}

	bool basicEqual(CIMInstance const & x, CIMInstance const & y)
	{
		return x.getClassName().equalsIgnoreCase(y.getClassName()) &&
			x.propertiesAreEqualTo(y);
	}

	bool basicEqual(CIMInstanceArray const & x, CIMInstanceArray const & y)
	{
		if (x.size() != y.size())
		{
			return false;
		}
		for (size_t i = 0; i < x.size(); ++i)
		{
			if (!basicEqual(x[i], y[i]))
			{
				return false;
			}
		}
		return true;
	}	

} // CIMInstanceUtils namespace
