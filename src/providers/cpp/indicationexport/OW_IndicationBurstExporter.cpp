/*******************************************************************************
* Copyright (C) 2008 Quest Software, Inc. All rights reserved.
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

/**
 * @author Kevin S. Van Horn
 */

#include "OW_IndicationBurstExporter.hpp"
#include "blocxx/ThreadPool.hpp"
#include "blocxx/Logger.hpp"
#include "blocxx/Timeout.hpp"
#include "OW_SendIndicationBurstTask.hpp"

using namespace OpenWBEM;
using namespace blocxx;

namespace
{
	char const * const COMPONENT_NAME =
		"ow.owcimomd.indication-burst-exporter";
}

IndicationBurstExporter::~IndicationBurstExporter()
{
}

IndicationBurstExporterImpl::IndicationBurstExporterImpl()
: m_logger(0),
  m_pool(0)
{
}

IndicationBurstExporterImpl::~IndicationBurstExporterImpl()
{
	try
	{
		this->shutdown();
	}
	catch (...)
	{
	}
}

void IndicationBurstExporterImpl::initialize(UInt32 maxNumIoThreads)
{
	m_logger = new Logger(COMPONENT_NAME);
	m_pool = new ThreadPool(
		ThreadPool::DYNAMIC_SIZE_NO_QUEUE,
		maxNumIoThreads, maxNumIoThreads, *m_logger,
		"Send Indication Burst Tasks"
	);
}

void IndicationBurstExporterImpl::shutdown()
{
	// Dan tells me that ThreadPool::shutdown is designed to be
	// thread safe, so this method should also be thread safe.
	// We operate under the usual assumption that there is a memory barrier
	// (or thread creation) between the call to initialize and the call
	// to this method, so that the test for m_pool non-null is safe.
	if (m_pool)
	{
		m_pool->shutdown(
			ThreadPool::E_DISCARD_WORK_IN_QUEUE,
			Timeout::relative(0), // time to wait before cooperative cancel
			Timeout::infinite);   // time to wait before definitive cancel
	}
}

void IndicationBurstExporterImpl::sendBurst(
	CIMInstance const & handler, Array<CIMInstance> const & indications)
{
	IndicationExporterRef ie(IndicationExporterImpl::create(handler));
	RunnableRef task(new SendIndicationBurstTask(ie, indications));
	m_pool->addWork(task);
}
