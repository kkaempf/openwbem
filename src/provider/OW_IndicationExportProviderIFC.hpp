/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_INDICATIONEXPORTPROVIDERIFC_HPP_
#define OW_INDICATIONEXPORTPROVIDERIFC_HPP_
#include "OW_config.h"
#include "OW_ProviderBaseIFC.hpp"
#include "OW_Array.hpp"
#include "OW_Reference.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"

namespace OpenWBEM
{

class IndicationExportProviderIFC : public ProviderBaseIFC
{
public:
	virtual ~IndicationExportProviderIFC();
	/**
	 * @return The class names of all the CIM_IndicationHandler sub-classes
	 * this IndicationExportProviderIFC handles.
	 */
	virtual StringArray getHandlerClassNames() = 0;
	
	/**
	 * Export the given indication
	 */
	virtual void exportIndication(const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& indHandlerInst, const CIMInstance& indicationInst) = 0;
};
typedef Reference<IndicationExportProviderIFC>
		IndicationExportProviderIFCRef;
typedef Array<IndicationExportProviderIFCRef>
		IndicationExportProviderIFCRefArray;

} // end namespace OpenWBEM

#endif
