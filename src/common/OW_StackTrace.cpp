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
#include "OW_StackTrace.hpp"
#include "OW_Exec.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Format.hpp"
#include "OW_Array.hpp"
#include <fstream>

extern "C"
{
#include <unistd.h> // for getpid()
}

using std::ifstream;
using std::ofstream;
using std::flush;

#ifndef DEFAULT_GDB_PATH
#define DEFAULT_GDB_PATH "/usr/bin/gdb"
#endif

// static
void OW_StackTrace::getStackTrace()
{
#ifdef OW_ENABLE_STACK_TRACE_ON_EXCEPTIONS
	if (getenv("OW_STACKTRACE"))
	{
		ifstream file(DEFAULT_GDB_PATH);
		if (file)
		{
			file.close();
			OW_String scriptName("/tmp/owgdb-");
			OW_String outputName("/tmp/owgdbout-");
			// TODO: don't use getppid, get it from somewhere else!
			outputName += OW_String(OW_UInt32(::getpid()));
			scriptName += OW_String(OW_UInt32(::getpid())) + ".sh";
			OW_String exeName("/proc/");
			exeName += OW_String(OW_UInt32(::getppid())) + "/exe";
			
			ofstream scriptFile(scriptName.c_str(), std::ios::out);
			scriptFile << "#!/bin/sh\n"
				<< "gdb " << exeName << " " << ::getppid() << " << EOS > " << outputName << " 2>&1\n"
				<< "thread apply all bt\n"
				<< "detach\n"
				<< "q\n"
				<< "EOS\n" << flush;
			scriptFile.close();

			OW_Array<OW_String> command;
			command.push_back( "/bin/sh" );
			command.push_back( scriptName );
			OW_Exec::safeSystem(command);


			ifstream outputFile(outputName.c_str(), std::ios::in);

			OW_String output;
			while (outputFile)
			{
				output += OW_String::getLine(outputFile) + "\n";
			}

			outputFile.close();
			unlink(outputName.c_str());
			unlink(scriptName.c_str());

			retval = new OW_StackTrace(output);
		}
	}
	if (retval)
	{
		std::cerr << *retval << endl;
	}
#endif // OW_ENABLE_STACK_TRACE_ON_EXCEPTIONS
}

OW_StackTrace::OW_StackTrace(const OW_String& trace)
	: m_trace(trace)
{
}

