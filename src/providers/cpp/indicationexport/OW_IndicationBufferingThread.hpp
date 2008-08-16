#ifndef OW_INDICATION_BUFFERING_THREAD_HPP
#define OW_INDICATION_BUFFERING_THREAD_HPP
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
 * @author Kevin S. Van Horn
 */

#include "OW_config.h"
#include "OW_Thread.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_MTQueue.hpp"
#include "OW_Clock.hpp"
#include "OW_IndicationBurstExporter.hpp"

namespace OW_NAMESPACE
{

class IndicationBufferingThread : public Thread
{
public:
	// Parameters that control how to buffer indications so that
	// multiple indications for the same destination can be sent together.
	struct Config
	{
		// The object buffers indications for a given destination until
		// either
		// - no indication for that destination has arrived in the last
		//   bufferingWaitSeconds seconds; or
		// - maxBufferingDelaySeconds seconds have passed since the first
		//   unsent (currently buffered) indication for that destination
		//   arrived; or
		// - there are maxBufferSize or more unsent
		//   (buffered) indications for that destination.
		// When any of these conditions occurs, the object sends all of
		// the buffered indications for the destination as a single request.
		//
		// Exception: If we are already buffering for maxBufferedDestinations
		// destinations, then indications arriving for a destination
		// not already buffered are sent immediately without buffering.
		//
		blocxx::Real32 bufferingWaitSeconds;
		blocxx::Real32 maxBufferingDelaySeconds;
		blocxx::UInt32 maxBufferSize;
		blocxx::UInt32 maxBufferedDestinations;

		// Maximum number of I/O threads the object can use at any one time.
		// These threads communicate a burst of indications to a given
		//destination.
		//
		blocxx::UInt32 maxNumIoThreads;
	};

	struct ExportIndicationArgs
	{
		ExportIndicationArgs(
			CIMInstance const & handler0, CIMInstance const & indication0);
		ExportIndicationArgs();
		CIMInstance handler;
		CIMInstance indication;
	};

	IndicationBufferingThread();

	// For unit testing.
	IndicationBufferingThread(
		IndicationBurstExporterRef const & burstExporter,
		ClockRef const & clock);

	void initialize(
		Config const & config,
		MTSourceIfc<ExportIndicationArgs> & requestSource);

	void shutdownThreadPool();

	virtual Int32 run();

private:
	void ctor(
		IndicationBurstExporterRef const & burstExporter,
		ClockRef const & clock);

	void sendBurst(
		CIMInstance const & handler, CIMInstanceArray const & indications);

	class BufferingMap;

	void processIndication(
		ExportIndicationArgs const &, BufferingMap &, DateTime now);

	IndicationBurstExporterRef m_burstExporter;
	ClockRef m_clock;
	MTSourceIfc<ExportIndicationArgs> * m_requestSource; // reference
	Real32 m_bufferingWaitSeconds;
	Real32 m_maxBufferingDelaySeconds;
	UInt32 m_maxBufferSize;
	UInt32 m_maxBufferedDestinations;
};

} // namespace

#endif
