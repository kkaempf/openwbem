/*******************************************************************************
* Copyright (C) 2004 Quest Software, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */


#include "OW_config.h"
#include "blocxx/FileSystem.hpp"
#include "blocxx/String.hpp"

#include <fstream>

#include <iostream>
#include <iomanip>

#include <sys/time.h>

using namespace OpenWBEM;
using namespace blocxx;
using namespace std;

struct Nothing
{
	const char* name() const { return "Nothing"; }
	void operator()() const
	{
	}
};

struct fstreamCreationDeletion
{
	const char* name() const { return "fstream CreationDeletion"; }
	void operator()() const
	{
		ofstream test("test", std::ios::out | std::ios::app);
	}
};

struct FileSystemExists
{
	const char* name() const { return "FileSystem::Exists()"; }
	void operator()() const
	{
		FileSystem::exists("test");
	}
};

template <typename FunctorT>
void doTiming(const FunctorT& f, size_t reps)
{
	struct timeval begin;
	gettimeofday(&begin, NULL);
	for (size_t i = 0; i < reps; ++i)
	{
		f();
	}

	struct timeval end;
	gettimeofday(&end, NULL);

	struct timeval length;
	timerclear(&length);
	if (end.tv_usec < begin.tv_usec)
	{
		// handle borrow
		length.tv_sec = (end.tv_sec - 1) - begin.tv_sec;
		length.tv_usec = end.tv_usec - begin.tv_usec + 1000000;
	}
	else
	{
		length.tv_sec = end.tv_sec - begin.tv_sec;
		length.tv_usec = end.tv_usec - begin.tv_usec;
	}
	std::cout << f.name() << " x " << reps << '\n';
	std::cout << std::setw(8) << std::setfill(' ') << length.tv_sec << '.' << std::setw(6) << std::setfill('0') << length.tv_usec << std::endl;
}

int main(int argc, const char** argv)
{
	static size_t reps = 100000;
	doTiming(Nothing(), reps);
	doTiming(fstreamCreationDeletion(), reps);
	FileSystem::removeFile("test");
	doTiming(FileSystemExists(), reps);
}

