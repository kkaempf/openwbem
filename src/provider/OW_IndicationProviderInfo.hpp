/*******************************************************************************
* Copyright (C) 2003 Vintela, Inc. All rights reserved.
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
#ifndef OW_INDICATION_PROVIDER_INFO_HPP_INCLUDE_GUARD_
#define OW_INDICATION_PROVIDER_INFO_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_ProviderInfoBase.hpp"

namespace OpenWBEM
{

struct IndicationProviderInfoEntry
{
	explicit IndicationProviderInfoEntry(String const& indicationName_)
		: indicationName(indicationName_)
	{}
	IndicationProviderInfoEntry(String const& indicationName_, StringArray const& namespaces_, StringArray const& classes_ = StringArray())
		: indicationName(indicationName_)
		, namespaces(namespaces_)
		, classes(classes_)
	{}
	String indicationName;
	StringArray namespaces;
	StringArray classes;
};

class IndicationProviderInfo : public ProviderInfoBase<IndicationProviderInfoEntry>
{
public:
	// pull the names into this class
	using ProviderInfoBase<IndicationProviderInfoEntry>::ClassInfo;
	using ProviderInfoBase<IndicationProviderInfoEntry>::ClassInfoArray;
};

typedef Array<IndicationProviderInfo> IndicationProviderInfoArray;

extern template class ProviderInfoBase<IndicationProviderInfoEntry>;

} // end namespace OpenWBEM

#endif
