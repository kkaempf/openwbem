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
#include "OW_config.h"
#include "OW_CIM.hpp"
#include "OW_CppIndicationExportXMLHTTPProvider.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_IndicationExporter.hpp"
#include "OW_Format.hpp"

namespace
{
	// anonymous namespace is to prevent possible linkage problems or identifier
	// conflict whens the library is dynamically loaded

///////////////////////////////////////////////////////////////////////////////
OW_CppIndicationExportXMLHTTPProvider::~OW_CppIndicationExportXMLHTTPProvider()
{
}

///////////////////////////////////////////////////////////////////////////////
// Export the given indication
void
OW_CppIndicationExportXMLHTTPProvider::exportIndication(
	const OW_ProviderEnvironmentIFCRef& env, OW_CIMInstance &indHandlerInst,
	OW_CIMInstance &indicationInst)
{
	env->getLogger()->logDebug(format("OW_CppIndicationExportXMLHTTPProvider "
		"exporting indication.  Handler = %1, Indication = %2",
		indHandlerInst.toString(), indicationInst.toString()));

	if (indHandlerInst.getClassName().
			equalsIgnoreCase("CIM_IndicationHandlerXMLHTTP")
		 || indHandlerInst.getClassName().
			equalsIgnoreCase("CIM_IndicationHandlerXMLHTTPS")
		 || indHandlerInst.getClassName().
			equalsIgnoreCase("CIM_IndicationHandlerCIM-XML"))
	{
		OW_String listenerUrl;
		indHandlerInst.getProperty("Destination").getValue().get(listenerUrl);
		if (indHandlerInst.getClassName().
			 equalsIgnoreCase("CIM_IndicationHandlerXMLHTTPS"))
		{
			OW_URL url(listenerUrl);
			if (!url.protocol.equals("https"))
			{
				url.protocol = "https";
				listenerUrl = url.toString();
			}
		}

		OW_IndicationExporter exporter(OW_CIMProtocolIFCRef(
			new OW_HTTPClient(listenerUrl)));
		exporter.exportIndication(indicationInst);
	}
	else
	{
		env->getLogger()->logError(
			format("OW_CppIndicationExportXMLHTTPProvider::exportIndication "
				"called with wrong indication handler class: %1",
				indHandlerInst.getClassName()));
	}
}

///////////////////////////////////////////////////////////////////////////////
// @return The class names of all the CIM_IndicationHandler sub-classes
// this OW_IndicationExportProvider handles.
OW_StringArray
OW_CppIndicationExportXMLHTTPProvider::getHandlerClassNames()
{
	OW_StringArray rv;
	rv.append("CIM_IndicationHandlerXMLHTTP");
	rv.append("CIM_IndicationHandlerXMLHTTPS");
	rv.append("CIM_IndicationHandlerCIM-XML"); // new name in the 2.6 schema
	return rv;
}

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
OW_PROVIDERFACTORY(OW_CppIndicationExportXMLHTTPProvider);


