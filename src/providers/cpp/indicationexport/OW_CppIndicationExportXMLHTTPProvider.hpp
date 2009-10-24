/*******************************************************************************
* Copyright (C) 2001-2008 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef OW_CPPINDICATIONEXPORTXMLHTTPPROVIDER_HPP_INCLUDE_GUARD_
#define OW_CPPINDICATIONEXPORTXMLHTTPPROVIDER_HPP_INCLUDE_GUARD_

/**
 * @author Dan Nuffer
 * @author Kevin S. Van Horn
 */

#include "OW_config.h"
#include "OW_CppIndicationExportProviderIFC.hpp"
#include "OW_CIMInstance.hpp"
#include "blocxx/MTQueue.hpp"
#include "OW_IndicationBufferingThread.hpp"

namespace OW_NAMESPACE
{

class CppIndicationExportXMLHTTPProvider : public CppIndicationExportProviderIFC
{
public:
	CppIndicationExportXMLHTTPProvider();

	virtual ~CppIndicationExportXMLHTTPProvider();

	/**
	 * Export the given indication
	 */
	virtual void exportIndication(const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance &indHandlerInst, const CIMInstance &indicationInst);

	/**
	 * @return The class names of all the CIM_CppIndicationHandler sub-classes
	 * this IndicationExportProvider handles.
	 */
	virtual StringArray getHandlerClassNames();

	/**
	 * Called by the CIMOM when the provider is initialized
	 *
	 * @param hdl The handle to the cimom
	 *
	 * @throws CIMException
	 */
	virtual void initialize(const ProviderEnvironmentIFCRef&);

	virtual void doShutdown();
	virtual void shuttingDown(ProviderEnvironmentIFCRef const &);

	// The following are public only to allow unit testing.

	struct Config : public IndicationBufferingThread::Config
	{
		// Max number of unprocessed exportIndication requests
		// that can queue up before exportIndication() blocks.  (An internal
		// thread processes the requests.)
		blocxx::UInt32 maxQueueSize;
	};

	Config configValues(ProviderEnvironmentIFCRef const & env);

private:
	typedef IndicationBufferingThread::ExportIndicationArgs
		ExportIndicationArgs;

	blocxx::MTQueue<ExportIndicationArgs> m_queue;
	IndicationBufferingThread m_bufferingThread;
	bool m_threadStarted;
};

typedef blocxx::IntrusiveReference<CppIndicationExportXMLHTTPProvider>
	CppIndicationExportXMLHTTPProviderRef;

} // end namespace OW_NAMESPACE

#endif
