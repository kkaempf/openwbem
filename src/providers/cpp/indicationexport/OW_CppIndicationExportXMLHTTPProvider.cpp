/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#include "OW_CppIndicationExportXMLHTTPProvider.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_IndicationExporter.hpp"
#include "OW_Format.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_MutexLock.hpp"
#include "OW_Logger.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
		
namespace
{
	const String COMPONENT_NAME("ow.provider.CppIndicationExportXMLHTTP");
}

///////////////////////////////////////////////////////////////////////////////
CppIndicationExportXMLHTTPProvider::CppIndicationExportXMLHTTPProvider()
	: m_cancelled(false)
{
}

///////////////////////////////////////////////////////////////////////////////
CppIndicationExportXMLHTTPProvider::~CppIndicationExportXMLHTTPProvider()
{
}

///////////////////////////////////////////////////////////////////////////////
// Export the given indication
void
CppIndicationExportXMLHTTPProvider::exportIndication(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMInstance &indHandlerInst,
	const CIMInstance &indicationInst_)
{
	// get rid of any qualifiers.
	CIMInstance indicationInst(indicationInst_.clone(E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN));

	LoggerRef logger = env->getLogger(COMPONENT_NAME);
	OW_LOG_DEBUG(logger, Format("CppIndicationExportXMLHTTPProvider "
		"exporting indication.  Handler = %1, Indication = %2",
		indHandlerInst.toString(), indicationInst.toString()));

	String listenerUrl;
	indHandlerInst.getProperty("Destination").getValue().get(listenerUrl);

	// this guy parses it out.
	URL url(listenerUrl);

	if (indHandlerInst.getClassName().equalsIgnoreCase("CIM_IndicationHandlerXMLHTTPS"))
	{
		if (!url.scheme.equals(URL::CIMXML_WBEMS))
		{
			url.scheme = URL::CIMXML_WBEMS;
			listenerUrl = url.toString();
		}
	}

	{
		MutexLock lock(m_guard);
		if (m_cancelled)
		{
			return;
		}
		m_httpClient = new HTTPClient(listenerUrl);
	}
	IndicationExporter exporter = IndicationExporter(m_httpClient);

	// the OW 2.0 HTTPXMLCIMListener uses the HTTP path to differentiate different
	// subscriptions.  This is stored in namespace name of the URL.
	if (!url.namespaceName.empty())
	{
		m_httpClient->setHTTPPath('/' + url.namespaceName);
	}

	exporter.exportIndication(ns, indicationInst);
}
///////////////////////////////////////////////////////////////////////////////
// @return The class names of all the CIM_IndicationHandler sub-classes
// this IndicationExportProvider handles.
StringArray
CppIndicationExportXMLHTTPProvider::getHandlerClassNames()
{
	StringArray rv;
	rv.append("CIM_IndicationHandlerXMLHTTP"); // name in the 2.5 schema
	rv.append("CIM_IndicationHandlerXMLHTTPS"); // used by OW 2.0 for HTTPS indications
	rv.append("CIM_IndicationHandlerCIMXML"); // new name in the 2.6 schema
	// new in the 2.8 schema
	rv.append("CIM_ListenerDestinationCIMXML");

	return rv;
}

///////////////////////////////////////////////////////////////////////////////
void
CppIndicationExportXMLHTTPProvider::doCooperativeCancel()
{
	MutexLock lock(m_guard);
	m_cancelled = true;
	if (m_httpClient)
	{
		m_httpClient->close();
	}
}


} // end namespace OpenWBEM

//////////////////////////////////////////////////////////////////////////////
OW_PROVIDERFACTORY_NOID(OpenWBEM::CppIndicationExportXMLHTTPProvider, XMLHTTP);

