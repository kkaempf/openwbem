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

#ifndef __OW_PLATFORM_HPP__
#define __OW_PLATFORM_HPP__

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Exception.hpp"
#include "OW_IOException.hpp"

DEFINE_EXCEPTION(Daemon);

class OW_Platform
{
public:

	enum
	{
		SHUTDOWN,
		REINIT
	};

	struct Options
	{
		Options()
		: debug(false)
		, configFile(false)
		, configFilePath()
		, help(false)
		, error(false)
		{}

		OW_Bool debug;
		OW_Bool configFile;
		OW_String configFilePath;
		OW_Bool help;
		OW_Bool error;
	};

	static Options daemonInit( int argc, char* argv[] );

	// Throws OW_DaemonException on error
	static void daemonize(const OW_Bool& dbgFlg, const OW_String& daemonName);
	static int daemonShutdown(const OW_String& daemonName);
	static void initSig() { plat_upipe = OW_UnnamedPipe::createUnnamedPipe(); }
	static void pushSig(int sig)
	{
		if (plat_upipe->writeInt(sig) == -1)
			OW_THROW(OW_IOException, "Failed writing signal to pipe");
	}
	static int popSig()
	{
		int tmp = -2;
		if (plat_upipe->readInt(&tmp) < 0)
			return -1;
		return tmp;
	}
	static void shutdownSig() { plat_upipe = 0; }

	static OW_SelectableIFCRef getSigSelectable()
	{
		return plat_upipe;
	}
private:
	OW_Platform(); // prevent instantiation.
	static OW_Reference<OW_UnnamedPipe> plat_upipe;
};

#endif	// __OW_PLATFORM_HPP__
