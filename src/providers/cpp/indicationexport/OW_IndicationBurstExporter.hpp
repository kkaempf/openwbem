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

#ifndef OW_INDICATION_BURST_EXPORTER_HPP
#define OW_INDICATION_BURST_EXPORTER_HPP
#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_Types.hpp"
#include "blocxx/IntrusiveCountableBase.hpp"

namespace OW_NAMESPACE
{

struct IndicationBurstExporter : IntrusiveCountableBase
{
	virtual ~IndicationBurstExporter();

	virtual void initialize(UInt32 maxNumIoThreads) = 0;
	virtual void sendBurst(
		CIMInstance const & handler, Array<CIMInstance> const & indications)
		= 0;
	// Implementation must be thread safe.
	virtual void shutdown() = 0;
};

typedef IntrusiveReference<IndicationBurstExporter> IndicationBurstExporterRef;

struct IndicationBurstExporterImpl : public IndicationBurstExporter
{
	IndicationBurstExporterImpl();
	virtual ~IndicationBurstExporterImpl();

	virtual void initialize(UInt32 maxNumIoThreads);
	virtual void sendBurst(
		CIMInstance const & handler, Array<CIMInstance> const & indications);
	virtual void shutdown();

private:
	LoggerRef m_logger;
	ThreadPoolRef m_pool;
};

} // end namespace OW_NAMESPACE

#endif
