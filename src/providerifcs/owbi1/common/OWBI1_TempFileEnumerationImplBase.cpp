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

#include "OWBI1_config.h"
#include "OWBI1_TempFileEnumerationImplBase.hpp"
#include "OWBI1_EnumerationException.hpp"
#include "OW_File.hpp"
#include "OW_FileSystem.hpp"
#include "OWBI1_Types.hpp"
#include "OW_TempFileStream.hpp"
#include "OWBI1_String.hpp"

namespace OWBI1
{
using namespace OpenWBEM;

namespace
{
const UInt32 TEMPFILE_ENUMERATION_SIG = 0x4f57454e; // "OWEN"
}

TempFileEnumerationImplBase::TempFileEnumerationImplBase()
	: m_size(0), m_data(new OpenWBEM::TempFileStream())
{
	UInt32 enumSig = TEMPFILE_ENUMERATION_SIG;
	m_data->write(reinterpret_cast<const char*>(&enumSig), sizeof(enumSig));
	if (!m_data->good())
	{
		OWBI1_THROW(EnumerationException, "Failed to write signature to "
			"enumeration tempfile.");
	}
	// now we have to read the sig so that the temp file stream is
	// positioned correctly
	UInt32 tmpSig;
	m_data->read(reinterpret_cast<char*>(&tmpSig), sizeof(tmpSig));
	if (!m_data->good())
	{
		OWBI1_THROW(EnumerationException, "Failed to read signature from "
			"enumeration tempfile.");
	}
}

TempFileEnumerationImplBase::TempFileEnumerationImplBase(String const& filename)
	: m_size(readSize(filename)), m_data(new OpenWBEM::TempFileStream(filename.c_str()))
{
	// now we have to read the sig so that the temp file stream is
	// positioned correctly
	UInt32 tmpSig;
	m_data->read(reinterpret_cast<char*>(&tmpSig), sizeof(tmpSig));
	if (!m_data->good())
	{
		OWBI1_THROW(EnumerationException, "Failed to read signature of "
			"enumeration tempfile.");
	}
	if (tmpSig != TEMPFILE_ENUMERATION_SIG)
	{
		OWBI1_THROW(EnumerationException, "Signature of enumeration tempfile is not valid.");
	}
}
TempFileEnumerationImplBase::~TempFileEnumerationImplBase()
{
	delete m_data;
}
bool
TempFileEnumerationImplBase::hasMoreElements() const
{
	
	return m_size > 0;
}
size_t
TempFileEnumerationImplBase::numberOfElements() const
{
	return m_size;
}
void
TempFileEnumerationImplBase::clear()
{
	m_size = 0;
	m_data->reset();
}
String
TempFileEnumerationImplBase::releaseFile()
{
	// Append the size onto the end of the stream so we can recover it
	// when constructing from the file
	m_data->write(reinterpret_cast<char*>(&m_size), sizeof(m_size));
	if (!m_data->good())
	{
		OWBI1_THROW(EnumerationException, "Failed to write size to "
			"enumeration tempfile.");
	}
	String rval = m_data->releaseFile().c_str();
	clear();
	return rval;
}

bool
TempFileEnumerationImplBase::usingTempFile() const
{
	return m_data->usingTempFile();
}

size_t 
TempFileEnumerationImplBase::readSize(String const& filename)
{
	size_t size;
	// open the file and read the size that is written to the end of it.
	File f = FileSystem::openFile(filename.c_str());
	if (!f)
	{
		OWBI1_THROW(EnumerationException, "Failed to open file");
	}
	
	// Check that the correct signature is on the file
	UInt32 fileSig;
	if (f.read(reinterpret_cast<char*>(&fileSig), sizeof(fileSig)) != sizeof(fileSig))
	{
		OWBI1_THROW(EnumerationException, "Failure to read enumeration "
			"signature");
	}
	if (fileSig != TEMPFILE_ENUMERATION_SIG)
	{
		OWBI1_THROW(EnumerationException, "Attempted to construct an "
			"enumeration from a file that does not have the correct "
			"signature");
	}
	
	off_t whence = f.seek(-static_cast<off_t>(sizeof(size)), SEEK_END);
	if (whence == -1)
	{
		OWBI1_THROW(EnumerationException, "Failure to seek");
	}
	if (f.read(reinterpret_cast<char*>(&size), sizeof(size), whence) != sizeof(size))
	{
		OWBI1_THROW(EnumerationException, "Failure to read enumeration "
			"size");
	}
	if (f.close() == -1)
	{
		OWBI1_THROW(EnumerationException, "Failure to close enumeration "
			"file");
	}
	return size;
}

void
TempFileEnumerationImplBase::throwIfEmpty() const
{
	if (!hasMoreElements())
	{
		OWBI1_THROW (EnumerationException, "Attempt to Extract from empty Enum");
	}
}


} // end namespace OWBI1



