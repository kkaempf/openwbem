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

#include "OW_IndicationBufferingThread.hpp"
#include "blocxx/ClockImpl.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_Logger.hpp"
#include "OW_Format.hpp"
#include "OW_NoSuchPropertyException.hpp"
#include <list>
#include <cmath>
#include <algorithm>
#include <iostream> // DEBUG

using namespace std;
using namespace OpenWBEM;
using namespace OpenWBEM::MTQueueEnum;

namespace
{
	String COMPONENT_NAME("ow.provider.CppIndicationExportXMLHTTPProvider");
}

IndicationBufferingThread::ExportIndicationArgs::ExportIndicationArgs(
	CIMInstance const & handler0, CIMInstance const & indication0)
: handler(handler0),
  indication(indication0)
{
}

IndicationBufferingThread::ExportIndicationArgs::ExportIndicationArgs()
{
}

IndicationBufferingThread::IndicationBufferingThread()
{
	this->ctor(
		IndicationBurstExporterRef(new IndicationBurstExporterImpl()),
		blocxx::ClockRef(new blocxx::ClockImpl()));
}

IndicationBufferingThread::IndicationBufferingThread(
	IndicationBurstExporterRef const & burstExporter,
	blocxx::ClockRef const & clock)
{
	this->ctor(burstExporter, clock);
}

void IndicationBufferingThread::ctor(
	IndicationBurstExporterRef const & burstExporter,
	blocxx::ClockRef const & clock)
{
	m_burstExporter = burstExporter;
	m_clock = clock;
}

void IndicationBufferingThread::initialize(
	Config const & config,
	MTSourceIfc<ExportIndicationArgs> & requestSource)
{
	m_requestSource = &requestSource;
	m_bufferingWaitSeconds = config.bufferingWaitSeconds;
	m_maxBufferingDelaySeconds = config.maxBufferingDelaySeconds;
	m_maxBufferSize = config.maxBufferSize;
	m_maxBufferedDestinations = config.maxBufferedDestinations;
	m_burstExporter->initialize(config.maxNumIoThreads);
}

void IndicationBufferingThread::shutdownThreadPool()
{
	// thread-safe call
	m_burstExporter->shutdown();
}

namespace
{
	CIMName const destination("Destination");

	struct DestBuffering : public IntrusiveCountableBase
	{
		CIMInstance handler;
		CIMInstanceArray buffer;
		DateTime endTime;
		DateTime timeout;
	};

	typedef IntrusiveReference<DestBuffering> DestBufferingRef;

	String handlerDest(CIMInstance const & handler)
	{
		return handler.getPropertyT(destination).getValue().toString();
	}

	bool equalAsHandlers(CIMInstance const & x, CIMInstance const & y)
	{
		return (
			x.getClassName().equalsIgnoreCase(y.getClassName()) &&
			handlerDest(x) == handlerDest(y));
	}

	void checkHasDestination(CIMInstance const & handler)
	{
		handler.getPropertyT(destination);
	}
} // anon ns

// Note: this uses linear search, but it should be OK for reasonable
// values of maxBufferedDestinations (no more than 10 or 20).
//
class IndicationBufferingThread::BufferingMap
{
	typedef std::list<DestBufferingRef> List;
	typedef List::iterator iterator;

public:
	// PROMISE: If this object has a DestBuffering object with a handler
	// field equivalent to handler, removes that object and returns it.
	// Otherwise creates a new DestBuffering object with its handler
	// field set to handler, and returns that.
	// THROWS: NoSuchPropertyException if handler lacks the
	// "Destination" property.
	//
	DestBufferingRef findAndRemove(CIMInstance const & handler)
	{
		checkHasDestination(handler);
		iterator end = m_destBuffers.end();
		for (iterator it = m_destBuffers.begin(); it != end; ++it)
		{
			DestBufferingRef b = *it;
			if (equalAsHandlers(handler, b->handler))
			{
				m_destBuffers.erase(it);
				return b;
			}
		}
		DestBufferingRef b(new DestBuffering());
		b->handler = handler;
		return b;
	}

	void insert(DestBufferingRef b)
	{
		// NOTE: This is more efficient than it looks.  In practice,
		// b is always inserted at the beginning or end of the list.
		// The for loop is just there as defensive programming.
		DateTime btimeout = b->timeout;
		if (m_destBuffers.empty() || btimeout >= latest().timeout)
		{
			m_destBuffers.push_back(b);
		}
		else
		{
			iterator end = m_destBuffers.end();
			iterator it = m_destBuffers.begin();
			for ( ; it != end && (*it)->timeout < btimeout; ++it)
			{
			}
			m_destBuffers.insert(it, b);
		}
	}

	bool empty() const
	{
		return m_destBuffers.empty();
	}

	size_t size() const
	{
		return m_destBuffers.size();
	}

	DestBuffering & earliest()
	{
		return **m_destBuffers.begin();
	}

	void popEarliest()
	{
		m_destBuffers.erase(m_destBuffers.begin());
	}

private:
	DestBuffering & latest()
	{
		iterator end = m_destBuffers.end();
		--end;
		return **end;
	}

	// INVARIANT: elements stored in order of increasing timeout field.
	//
	List m_destBuffers;
};

void IndicationBufferingThread::sendBurst(
	CIMInstance const & handler, CIMInstanceArray const & indications)
{
	try
	{
		m_burstExporter->sendBurst(handler, indications);
	}
	catch (std::exception & e)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_ERROR(logger,
			Format("Exception caught when sending an indication burst: %1",
				e.what()));
	}
}

void IndicationBufferingThread::processIndication(
	ExportIndicationArgs const & req, BufferingMap & bmap, DateTime now)
{
	try
	{
		bool maxDestsReached = (bmap.size() >= m_maxBufferedDestinations);
		DestBufferingRef rb = bmap.findAndRemove(req.handler);
		DestBuffering & b = *rb;
		if (b.buffer.empty())
		{
			b.endTime = now + m_maxBufferingDelaySeconds;
		}
		b.buffer.push_back(req.indication);
		b.timeout =
			(b.buffer.size() >= m_maxBufferSize || maxDestsReached ?
				now : min(b.endTime, now + m_bufferingWaitSeconds));
		bmap.insert(rb);
	}
	catch (NoSuchPropertyException & e)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_ERROR(logger,
			"Indication handler instance lacks required property");
	}
}

Int32 IndicationBufferingThread::run()
{
	BufferingMap bmap;
	ExportIndicationArgs req;
	EPopResult res;
	res = m_requestSource->popFront(Timeout::infinite, req);
	while (res != E_SHUT_DOWN)
	{
		DateTime now = m_clock->now();
		if (res == E_VALUE)
		{
			this->processIndication(req, bmap, now);
		}
		while (!bmap.empty() && bmap.earliest().timeout <= now)
		{
			DestBuffering const & b = bmap.earliest();
			this->sendBurst(b.handler, b.buffer);
			bmap.popEarliest();
		}

		Timeout timeout =
			(bmap.empty() ?
				Timeout::infinite :
				Timeout::absolute(bmap.earliest().timeout));
		res = m_requestSource->popFront(timeout, req);
	}
	m_burstExporter->shutdown();
	return 0;
}
