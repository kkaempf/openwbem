/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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

/**
 * @author Dan Nuffer
 */

#include "OW_config.h"
#if defined(OW_USE_DL)
#include "OW_dlSharedLibrary.hpp"
#include "OW_Format.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include <dlfcn.h>

#include "OW_Types.h"

#if defined(OW_USE_FAKE_LIBS)
/////////////////////////////////////////////////////////////////////////////
#include "OW_FileSystem.hpp"
#include "OW_File.hpp"
#include "OW_Array.hpp"

#define OW_FAKELIB_HEADING "FAKE"
#define OW_FAKELIB_HEADING_LENGTH 4

// NOTE: To make a fake library, create a text file with all functions that
// should be contained, in a list like:
// FAKE=true
// library_function1=some_unique_global_function_name
// library_function2=some_other_unique_global_function_name
// library_function3=some_final_unique_global_function_name
// ...
//
// The first entry MUST begin with FAKE, and there can be NO characters before
// this entry.
//
// Most dynamic libraries in openwbem are expected to have a getOWVersion
// function, so...
extern "C" const char* getOWVersion()
{ 
	return OW_VERSION;
}
// Now a line should be in your fake library which says:
// getOWVersion=getOWVersion
/////////////////////////////////////////////////////////////////////////////
#endif

#include <iostream>

namespace OpenWBEM
{

// static
int dlSharedLibrary::m_call_dlclose = 1;

Mutex dlSharedLibrary_guard;

dlSharedLibrary::dlSharedLibrary(void * libhandle, const String& libName)
	: SharedLibrary(), m_libhandle( libhandle ), m_libName(libName)
{
#if defined(OW_USE_FAKE_LIBS)
	// Find out if it is a fake library.
	m_fakeLibrary = dlSharedLibrary::isFakeLibrary(libName);

	if( m_fakeLibrary )
	{
		initializeSymbolMap();
	}
#endif /* defined(OW_USE_FAKE_LIBS) */
}
  
dlSharedLibrary::~dlSharedLibrary()
{
	if (m_call_dlclose)
	{
		dlclose( m_libhandle );
	}
}
bool dlSharedLibrary::doGetFunctionPointer(const String& functionName,
		void** fp) const
{
	MutexLock l(dlSharedLibrary_guard);
#if defined(OW_USE_FAKE_LIBS)
	String realFunctionName = functionName;
	// If this is a fake library, extract convert the requested function
	// name into the proper function name for the main executable.
	if( m_fakeLibrary )
	{
		Map<String,String>::const_iterator symIter = m_symbolMap.find(functionName);
		if( symIter == m_symbolMap.end() )
		{
			return false;
		}
		realFunctionName = symIter->second;
	}
	// FIXME! Remove this, when things are sufficiently debugged.
	std::cout << "faked=" << functionName << " realFunctionName=" << realFunctionName << std::endl;
	*fp = dlsym( m_libhandle, realFunctionName.c_str() );
#else
	*fp = dlsym( m_libhandle, functionName.c_str() );
#endif /* defined(OW_USE_FAKE_LIBS) */
	  
	if (!*fp)
	{
		return false;
	}
	return true;
}

bool dlSharedLibrary::isFakeLibrary(const String& library_path)
{
#if defined(OW_USE_FAKE_LIBS)
  if( FileSystem::canRead(library_path) )
  {
    // Read the beginning of the file and see if it
    // contains the fake library heading.
    File libfile = FileSystem::openFile(library_path);

    if( libfile )
    {
      char buffer[(OW_FAKELIB_HEADING_LENGTH) + 1];
      size_t num_read = libfile.read(buffer,(OW_FAKELIB_HEADING_LENGTH));
      if( num_read == (OW_FAKELIB_HEADING_LENGTH) )
      {
	// Null terminate it.
	buffer[OW_FAKELIB_HEADING_LENGTH] = '\0';
	if( String(OW_FAKELIB_HEADING) == buffer )
	{
	  // Yes, it's a fake library.
	  return true;
	}
      }
    }
  }  
#endif
  return false;
}

#if defined(OW_USE_FAKE_LIBS)
void dlSharedLibrary::initializeSymbolMap()
{
	if( ! m_fakeLibrary )
	{
		return;
	}
	// Read the contents of the file to find out what the function names
	// (normally available from dlsym) are mapped to functions in the main
	// program. 
	StringArray lines = FileSystem::getFileLines(m_libName);

	for( StringArray::const_iterator iter = lines.begin();
		iter != lines.end();
		++iter )
	{
		// Skip commented lines.  
		if( iter->startsWith('#') )
		{
			continue;
		}
		StringArray current_line = iter->tokenize("=");
		// Skip invalid lines.
		if( current_line.size() != 2 )
		{
			continue;
		}
		// Add the data into the map.
		String option = String(current_line[0]).trim();
		String value = String(current_line[1]).trim();
		m_symbolMap[option] = value;
	}      
}
#endif /* defined(OW_USE_FAKE_LIBS) */

} // end namespace OpenWBEM

#endif

