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

#include "OW_config.h"
#include "OW_UUID.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Types.h"

#include <sys/time.h> // for gettimeofday
#include <string.h> // for memcmp
#include <stdlib.h> // for rand

// typedefs
typedef OW_UInt64 uuid_time_t;
struct uuid_node_t 
{
	char nodeId[6];
};

struct uuid_state
{
	uuid_time_t timestamp;
	uuid_node_t nodeId;
	OW_UInt16 clockSequence;
};




// static generator state
static uuid_state g_state;

static OW_NonRecursiveMutex g_guard;

void get_system_time(uuid_time_t *uuid_time)
{
	struct timeval tp;

	gettimeofday(&tp, (struct timezone *)0);

	/* Offset between UUID formatted times and Unix formatted times.
	   UUID UTC base time is October 15, 1582.
	   Unix base time is January 1, 1970.
	 */
	*uuid_time = (tp.tv_sec * 10000000) + (tp.tv_usec * 10) +
		(((unsigned long long) 0x01B21DD2) << 32) + 0x13814000;
};

const OW_UInt16 UUIDS_PER_TICK = 1024;

/* get-current_time -- get time as 60 bit 100ns ticks since whenever.
   Compensate for the fact that real clock resolution is
   less than 100ns. */
void get_current_time(uuid_time_t * timestamp) {
	uuid_time_t                time_now;
	static uuid_time_t  time_last;
	static OW_UInt16   uuids_this_tick;
	static int                   inited = 0;

	if (!inited) {
		get_system_time(&time_now);
		uuids_this_tick = UUIDS_PER_TICK;
		inited = 1;
	};

	while (1) {
		get_system_time(&time_now);

		/* if clock reading changed since last UUID generated... */
		if (time_last != time_now) {
			/* reset count of uuids gen'd with this clock reading */
			uuids_this_tick = 0;
			break;
		};
		if (uuids_this_tick < UUIDS_PER_TICK) {
			uuids_this_tick++;
			break;
		};
		/* going too fast for our clock; spin */
	};
	/* add the count of uuids to low order bits of the clock reading */
	*timestamp = time_now + uuids_this_tick;
};

void get_ieee_node_identifier(uuid_node_t *node) {
	node->nodeId[0] = 0;
	node->nodeId[1] = 1;
	node->nodeId[2] = 2;
	node->nodeId[3] = 3;
	node->nodeId[4] = 4;
	node->nodeId[5] = 5;
}

/////////////////////////////////////////////////////////////////////////////
OW_UUID::OW_UUID()
{
	OW_NonRecursiveMutexLock l(g_guard);

	uuid_time_t timestamp;
	get_current_time(&timestamp);

	uuid_node_t node;
	get_ieee_node_identifier(&node);

	uuid_time_t last_time = g_state.timestamp;
	OW_UInt16 clockseq = g_state.clockSequence;
	uuid_node_t last_node = g_state.nodeId;

	// If clock went backwards, or node ID changed (e.g., net card swap) change clockseq
	if (memcmp(&node, &last_node, sizeof(uuid_node_t)))
		clockseq = rand(); // TODO: don't use rand()
	else if (timestamp < last_time)
		++clockseq;

	// save the state for next time
	g_state.timestamp = last_time;
	g_state.clockSequence = clockseq;
	g_state.nodeId = last_node;

	l.release();

	// stuff fields into the UUID
	// do time_low 
        OW_UInt32 tmp = static_cast<OW_UInt32>(timestamp & 0xFFFFFFFF);
        m_uuid[3] = static_cast<OW_UInt8>(tmp);
        tmp >>= 8;
        m_uuid[2] = static_cast<OW_UInt8>(tmp);
        tmp >>= 8;
        m_uuid[1] = static_cast<OW_UInt8>(tmp);
        tmp >>= 8;
        m_uuid[0] = static_cast<OW_UInt8>(tmp);

	// do time_mid
        tmp = static_cast<OW_UInt16>((timestamp >> 32) & 0xFFFF);
        m_uuid[5] = static_cast<OW_UInt8>(tmp);
        tmp >>= 8;
        m_uuid[4] = static_cast<OW_UInt8>(tmp);

	// do time_hi_and_version
        tmp = static_cast<OW_UInt16>(((timestamp >> 48) & 0x0FFF) | (1 << 12));
        m_uuid[7] = static_cast<OW_UInt8>(tmp);
        tmp >>= 8;
        m_uuid[6] = static_cast<OW_UInt8>(tmp);

	// do clk_seq_low
        tmp = clockseq & 0xFF;
        m_uuid[9] = static_cast<OW_UInt8>(tmp);

	// do clk_seq_hi_res
        tmp = (clockseq & 0x3F00) >> 8 | 0x80;
        m_uuid[8] = static_cast<OW_UInt8>(tmp);

        memcpy(m_uuid+10, &node, 6);

}

/////////////////////////////////////////////////////////////////////////////
OW_UUID::OW_UUID(const OW_String& uuidStr)
{
	(void)uuidStr;
}

/////////////////////////////////////////////////////////////////////////////
OW_String 
OW_UUID::toString() const
{
	return "";
}

/////////////////////////////////////////////////////////////////////////////
bool operator==(const OW_UUID& x, const OW_UUID& y)
{
	return memcmp(x.m_uuid, y.m_uuid, sizeof(x.m_uuid)) == 0;
}

/////////////////////////////////////////////////////////////////////////////
bool operator<(const OW_UUID& x, const OW_UUID& y)
{
	return memcmp(x.m_uuid, y.m_uuid, sizeof(x.m_uuid)) < 0;
}

/////////////////////////////////////////////////////////////////////////////
bool operator!=(const OW_UUID& x, const OW_UUID& y)
{
	return !(x == y);
}




