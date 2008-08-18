/*******************************************************************************
* Copyright (C) 2001-2008 QuestSoftware, Inc. All rights reserved.
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
 * @author Kevin S. Van Horn
 */

#include "OW_config.h"
#include "OW_CppIndicationExportXMLHTTPProvider.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_IndicationExporter.hpp"
#include "OW_Format.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_Logger.hpp"
#include "OW_ConfigException.hpp"
#include "OW_Thread.hpp"
#include "OW_Timeout.hpp"
#include "OW_ConfigOpts.hpp"
#include <cmath>
#include <algorithm>

namespace OW_NAMESPACE
{

using namespace WBEMFlags;
using namespace ConfigOpts;
		
namespace
{
	const String COMPONENT_NAME("ow.provider.CppIndicationExportXMLHTTP");
}

#define Prov CppIndicationExportXMLHTTPProvider

///////////////////////////////////////////////////////////////////////////////
Prov::Prov()
{
}

///////////////////////////////////////////////////////////////////////////////
Prov::~Prov()
{
	try
	{
		doShutdown();
		m_bufferingThread.join();
	}
	catch (...)
	{
	}
}

///////////////////////////////////////////////////////////////////////////////
// Export the given indication
void
Prov::exportIndication(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const CIMInstance &indHandlerInst,
	const CIMInstance &indicationInst_)
{
	// get rid of any qualifiers.
	CIMInstance indicationInst(
		indicationInst_.clone(
			E_NOT_LOCAL_ONLY, E_EXCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN));

	Logger logger(COMPONENT_NAME);
	OW_LOG_DEBUG(logger,
		Format(
			"CppIndicationExportXMLHTTPProvider queueing up indication export."
			"  Handler = %1, Indication = %2",
			indHandlerInst.toString(), indicationInst.toString()));

	m_queue.pushBack(ExportIndicationArgs(indHandlerInst, indicationInst));
}

///////////////////////////////////////////////////////////////////////////////
// @return The class names of all the CIM_IndicationHandler sub-classes
// this IndicationExportProvider handles.
StringArray
Prov::getHandlerClassNames()
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
void Prov::initialize(
	ProviderEnvironmentIFCRef const & env)
{
	Config cfg = configValues(env);
	m_queue.setMaxQueueSize(cfg.maxQueueSize);
	m_bufferingThread.initialize(cfg, m_queue);
	m_bufferingThread.start();
}



namespace
{
	char const * const buffering_wait =
		INDICATION_EXPORT_XML_HTTP_BUFFERING_WAIT_SECONDS_opt;
	char const * const max_buffering_delay =
		INDICATION_EXPORT_XML_HTTP_MAX_BUFFERING_DELAY_SECONDS_opt;
	char const * const max_buffer_size =
		INDICATION_EXPORT_XML_HTTP_MAX_BUFFER_SIZE_opt;
	char const * const max_buffered_destinations =
		INDICATION_EXPORT_XML_HTTP_MAX_BUFFERED_DESTINATIONS_opt;
	char const * const max_num_io_threads =
		INDICATION_EXPORT_XML_HTTP_MAX_NUM_IO_THREADS_opt;
	char const * const error_prefix =
		"Error configuring CppIndicationExportXMLHTTPProvider";

	void checkConfig(Prov::Config const & config)
	{
		if (config.bufferingWaitSeconds > config.maxBufferingDelaySeconds)
		{
			Format fmt("%1: %2 must not be greater than %3",
				error_prefix, buffering_wait, max_buffering_delay);
			BLOCXX_THROW(ConfigException, fmt.c_str());
		}
		if (config.maxBufferSize == 0)
		{
			Format fmt("%1: %2 must be greater than zero",
				error_prefix, max_buffer_size);
			BLOCXX_THROW(ConfigException, fmt.c_str());
		}
		if (config.maxNumIoThreads == 0)
		{
			Format fmt("%1: %2 must be greater than zero",
				error_prefix, max_num_io_threads);
			BLOCXX_THROW(ConfigException, fmt.c_str());
		}
	}
}

Prov::Config Prov::configValues(ProviderEnvironmentIFCRef const & env)
{
	String item;
	try
	{
		Config config;

		item = buffering_wait;
		config.bufferingWaitSeconds =
			env->getConfigItem(item, "0.125").toReal32();

		item = max_buffering_delay;
		config.maxBufferingDelaySeconds =
			env->getConfigItem(item, "0.5").toReal32();

		item = max_buffer_size;
		config.maxBufferSize = env->getConfigItem(item, "1000").toUInt32();

		item = max_buffered_destinations;
		config.maxBufferedDestinations = 
			env->getConfigItem(item, "10").toUInt32();

		item = max_num_io_threads;
		config.maxNumIoThreads = env->getConfigItem(item, "10").toUInt32();

		config.maxQueueSize = config.maxNumIoThreads;

		checkConfig(config);

		return config;
	}
	catch (StringConversionException & e)
	{
		String errmsg = String(error_prefix) + ": bad value for " + item;
		BLOCXX_THROW_SUBEX(ConfigException, errmsg.c_str(), e);
	}
}

///////////////////////////////////////////////////////////////////////////////
void
Prov::doShutdown()
{
	try
	{
		m_queue.shutdown();
	}
	catch (...)
	{
	}
	try
	{
		m_bufferingThread.shutdownThreadPool();
	}
	catch (...)
	{
	}
}

void
Prov::shuttingDown(ProviderEnvironmentIFCRef const &)
{
	doShutdown();
}


} // end namespace OW_NAMESPACE

//////////////////////////////////////////////////////////////////////////////
OW_PROVIDERFACTORY(OpenWBEM::Prov, cppindicationexportxmlhttpprovider);

