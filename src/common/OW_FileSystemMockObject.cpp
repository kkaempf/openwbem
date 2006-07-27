/*******************************************************************************
* Copyright (C) 2006 Quest Software, Inc. All rights reserved.
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
#include "OW_FileSystemMockObject.hpp"
#include "OW_Exception.hpp"
#include "OW_File.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(FileSystemMockObjectUnimplemented);
OW_DEFINE_EXCEPTION(FileSystemMockObjectUnimplemented);

FileSystemMockObject::~FileSystemMockObject()
{
}

File 
FileSystemMockObject::openFile(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "openFile");
}
File 
FileSystemMockObject::createFile(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "createFile");
}
File 
FileSystemMockObject::openOrCreateFile(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "openOrCreateFile");
}
int 
FileSystemMockObject::changeFileOwner(const String& filename,	const UserId& userId)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "changeFileOwner");
}
bool 
FileSystemMockObject::exists(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "exists");
}
#ifndef OW_WIN32
bool 
FileSystemMockObject::isExecutable(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "isExecutable");
}
#endif
bool 
FileSystemMockObject::canRead(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "canRead");
}
bool 
FileSystemMockObject::canWrite(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "canWrite");
}
#ifndef OW_WIN32
bool 
FileSystemMockObject::isLink(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "isLink");
}
#endif
bool 
FileSystemMockObject::isDirectory(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "isDirectory");
}
bool 
FileSystemMockObject::changeDirectory(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "changeDirectory");
}
bool 
FileSystemMockObject::makeDirectory(const String& path, int mode)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "makeDirectory");
}
bool 
FileSystemMockObject::getFileSize(const String& path, Int64& size)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "getFileSize");
}
UInt64 
FileSystemMockObject::fileSize(FileHandle fh)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "fileSize");
}
bool 
FileSystemMockObject::removeDirectory(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "removeDirectory");
}
bool 
FileSystemMockObject::removeFile(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "removeFile");
}
bool 
FileSystemMockObject::getDirectoryContents(const String& path, StringArray& dirEntries)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "getDirectoryContents");
}
bool 
FileSystemMockObject::renameFile(const String& oldFileName, const String& newFileName)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "renameFile");
}
size_t 
FileSystemMockObject::read(const FileHandle& hdl, void* bfr, size_t numberOfBytes, Int64 offset)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "read");
}
size_t 
FileSystemMockObject::write(FileHandle hdl, const void* bfr, size_t numberOfBytes, Int64 offset)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "write");
}
Int64 
FileSystemMockObject::seek(const FileHandle& hdl, Int64 offset, int whence)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "seek");
}
Int64 
FileSystemMockObject::tell(const FileHandle& hdl)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "tell");
}
void 
FileSystemMockObject::rewind(const FileHandle& hdl)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "rewind");
}
int 
FileSystemMockObject::close(const FileHandle& hdl)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "close");
}
int 
FileSystemMockObject::flush(FileHandle& hdl)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "flush");
}
String 
FileSystemMockObject::getFileContents(const String& filename)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "getFileContents");
}
StringArray 
FileSystemMockObject::getFileLines(const String& filename)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "getFileLines");
}
String 
FileSystemMockObject::readSymbolicLink(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "readSymbolicLink");
}
String 
FileSystemMockObject::realPath(const String& path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "realPath");
}

std::pair<FileSystem::Path::ESecurity, String> 
FileSystemMockObject::security(String const & path, UserId uid)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "security");
}

std::pair<FileSystem::Path::ESecurity, String> 
FileSystemMockObject::security(String const & path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "security");
}

std::pair<FileSystem::Path::ESecurity, String>
FileSystemMockObject::security(String const & base_dir, String const & rel_path, UserId uid)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "security");
}

std::pair<FileSystem::Path::ESecurity, String> 
FileSystemMockObject::security(String const & base_dir, String const & rel_path)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "security");
}

String 
FileSystemMockObject::dirname(const String& filename)
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "dirname");
}

String 
FileSystemMockObject::getCurrentWorkingDirectory()
{
	OW_THROW(FileSystemMockObjectUnimplementedException, "getCurrentWorkingDirectory");
}


} // end namespace OW_NAMESPACE




