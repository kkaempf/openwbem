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

#ifndef __OW_FILESYSTEM_HPP__
#define __OW_FILESYSTEM_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include <cstdio>
#include <sys/param.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

class OW_File;

/**
 * The purpose of the OW_FileSystem class is to provide an abstraction layer
 * over the platform dependant functionality related to a file system.
 */
class OW_FileSystem
{
public:

	/**
	 * Open a file for read/write and return an OW_File object that can be used
	 * for reading and writing.
	 */
	static OW_File openFile(const OW_String& path);

	/**
	 * Create the file for the given name.
	 * @param path	The name of the file to create.
	 * @return On success an OW_File object that can be used for reading and
	 * writing. Otherwise a null OW_File object.
	 */
	static OW_File createFile(const OW_String& path);

	/**
	 * Change the given file ownership
	 * @param filename The name of the file to change ownership on.
	 * @param userId The user id to change ownership to
	 * @return 0 on success. Otherwise -1
	 */
	static int changeFileOwner(const OW_String& filename,
		const OW_UserId& userId);

	/**
	 * @return true if the file exists (and false otherwise).
	 */
	static OW_Bool exists(const OW_String& path);

	/**
	 * @return true if the file exists and can be read
	 */
	static OW_Bool canRead(const OW_String& path);

	/**
	 * @return true if the file exists and can be written
	 */
	static OW_Bool canWrite(const OW_String& path);

	/**
	 * @return true if file exists and is a directory
	 */
	static OW_Bool isDirectory(const OW_String& path);

	/**
	 * Change to the given directory
	 * @param path	The directory to change to
	 * @return true if the operation succeeds. Otherwise false.
	 */
	static OW_Bool changeDirectory(const OW_String& path);

	/**
	 * Create a directory
	 * @param path	The name of the directory to create.
	 * @return true if the operation succeeds. Otherwise false.
	 */
	static OW_Bool makeDirectory(const OW_String& path);

	/**
	 * Get the size of the file in bytes
	 * @param path	The name of the file to get the size for.
	 * @param size	Put the size of the file in this variable.
	 * @return true if the operation succeeds. Otherwise false.
	 */
	static OW_Bool getFileSize(const OW_String& path, OW_UInt32& size);

	/**
	 * Remove the given directory
	 * @param path	The name of the directory to remove
	 * @return true if the operation succeeds. Otherwise false.
	 */
	static OW_Bool removeDirectory(const OW_String& path);

	/**
	 * Remove the given file
	 * @param path	The name of the file to remove.
	 * @return true if the operation succeeds. Otherwise false.
	 */
	static OW_Bool removeFile(const OW_String& path);

	/**
	 * Get the names of the files (and directories) in the given directory
	 * @param path			The name of the directory to get the contents of.
	 * @param dirEntries	The directory contents will be placed in this array.
	 * @return true if the operation succeeds. Otherwise false.
	 */
	static OW_Bool getDirectoryContents(const OW_String& path,
		OW_StringArray& dirEntries);

	/**
	 * Rename the given file to the new name
	 * @param oldFileName	The name of the file to rename
	 * @param newFileName	The new name for the olFileName
	 * @return true if the operation succeeds. Otherwise false.
	 */
	static OW_Bool renameFile(const OW_String& oldFileName,
		const OW_String& newFileName);

	/**
	 * Read data from file.
	 * @param hdl				The file handle to perform the read operation on.
	 * @param bfr				The location where the read operation will place
	 *								what is read.
	 * @param numberOfBytes	The number of bytes to read.
	 * @param offset			The offset to seek to in the file before the read
	 *								operation is done. -1 will use the current offset.
	 * @return The number of bytes read. If EOF or an error occurs, a short
	 * count or zero is returned.
	 */
	static size_t read(OW_FileHandle& hdl, void* bfr, size_t numberOfBytes,
		long offset=-1L);

	/**
	 * Write data to a file.
	 * @param hdl				The file handle to perform the write operation on.
	 * @param bfr				The locaction to get the contents to write.
	 * @param numberOfBytes	The number of bytes to write.
	 * @param offset			The offset to seek to in the file before the write
	 *								operation is done. -1 will use the current offset.
	 * @return The number of bytes written. If an error occurs, a short count
	 * or zero is returned.
	 */
	static size_t write(OW_FileHandle& hdl, const void* bfr,
		size_t numberOfBytes, long offset=-1L);

	/**
	 * Seek to a given offset within the file.
	 * @param hdl			The file handle to use in the seek operation.
	 * @param offset		The offset to seek to relative to the whence parm.
	 * @param whence		Can be one of the follwing values:
	 *							SEEK_SET - Seek relative to the beginning of the file.
	 *							SEEK_CUR - Seek relative to the current position.
	 *							SEEK_END - Seek relative to the end of the file (bwd).
	 * @return The the current location in the file relative to the beginning
	 * of the file on success. Other -1.
	 */
	static int seek(OW_FileHandle& hdl, OW_off_t offset, int whence=SEEK_SET);

	/**
	 * @param hdl	The file handle to use in the tell operation.
	 * @return The current position in the file relative to the beginning of
	 * the file on success. Otherwise -1.
	 */
	static OW_off_t tell(OW_FileHandle& hdl);

	/**
	 * Position the file pointer associated with the given file handle to the
	 * beginning of the file.
	 * @param hdl	The file handle to use in the rewind operation.
	 */
	static void rewind(OW_FileHandle& hdl);

	/**
	 * Close file handle.
	 * @param hdl	The file handle to close.
	 * @return 0 on success. Otherwise -1.
	 */
	static int close(OW_FileHandle& hdl);

	/**
	 * Flush any buffered data to the file if buffering supported.
	 * @param hdl	The file handle to flush the buffer on.
	 */
	static int flush(OW_FileHandle& hdl);

	/**
	 * Create a file with random data suitable for use with SSL initialization
	 * @param filename  The name of the file to write the random data to
	 */
	static void initRandomFile(const OW_String& file);
};

class OW_FileNullPtr;

/**
 * The purpose of the OW_File class is to provide an abstraction layer
 * over the platform dependant functionality related to a file.
 */
class OW_File
{
public:

	/**
	 * Create a NULL OW_File object.
	 */
	OW_File() : m_hdl(0)
	{
	}

	/**
	 * Copy constructor
	 * @param x	The OW_File object to copy.
	 */
	OW_File(const OW_File& x) : m_hdl(x.m_hdl) 
	{
	}

	/**
	 * Assignment operator
	 * @param x	The OW_File object to copy.
	 * @return A reference to this OW_File object.
	 */
	OW_File& operator= (const OW_File& x)
	{
		m_hdl = x.m_hdl;
		return *this;
	}

	/**
	 * NULL Assignment operator.
	 * This method allows OW_File object to be directly assigned a NULL value.
	 * For example:
	 * 	OW_File fl = .....;
	 *		fl = 0;
	 *		if(fl)
	 *		{
	 *			...
	 *		}
	 *
	 * @return A reference to this object.
	 */
	OW_File& operator= (const OW_FileNullPtr*)
	{
		m_hdl = 0;
		return *this;
	}

	/**
	 * Read from the underlying file.
	 * @param bfr				The location where the read operation will place
	 *								what is read.
	 * @param numberOfBytes	The number of bytes to read.
	 * @param offset			The offset to seek to in the file before the read
	 *								operation is done. -1 will use the current offset.
	 * @return The number of bytes read. If EOF or an error occurs, a short
	 * count or zero is returned.
	 */
	size_t read(void* bfr, size_t numberOfBytes, long offset=-1L)
	{
		return OW_FileSystem::read(m_hdl, bfr, numberOfBytes, offset);
	}

	/**
	 * Write to the underlying file
	 * @param bfr				The locaction to get the contents to write.
	 * @param numberOfBytes	The number of bytes to write.
	 * @param offset			The offset to seek to in the file before the write
	 *								operation is done. -1 will use the current offset.
	 * @return The number of bytes written. If an error occurs, a short count
	 * or zero is returned.
	 */
	size_t write(const void* bfr, size_t numberOfBytes, long offset=-1L)
	{
		return OW_FileSystem::write(m_hdl, bfr, numberOfBytes, offset);
	}

	/**
	 * Seek to a given offset within the file.
	 * @param offset		The offset to seek to relative to the whence parm.
	 * @param whence		Can be one of the follwing values:
	 *							SEEK_SET - Seek relative to the beginning of the file.
	 *							SEEK_CUR - Seek relative to the current position.
	 *							SEEK_END - Seek relative to the end of the file (bwd).
	 * @return The the current location in the file relative to the beginning
	 * of the file on success. Other -1.
	 */
	int seek(OW_off_t offset, int whence=SEEK_SET)
	{
		return OW_FileSystem::seek(m_hdl, offset, whence);
	}

	/**
	 * @return The current position in the file relative to the beginning of
	 * the file on success. Otherwise -1.
	 */
	OW_off_t tell()
	{
		return OW_FileSystem::tell(m_hdl);
	}

	/**
	 * Position the file pointer to the beginning of the file.
	 */
	void rewind()
	{
		OW_FileSystem::rewind(m_hdl);
	}

	/**
	 * Close the underlying file object.
	 * @return 0 on success. Otherwise -1.
	 */
	int close()
	{
		int rv = OW_FileSystem::close(m_hdl);
		m_hdl = 0;
		return rv;
	}

	/**
	 * Flush any buffered data to the file.
	 * @return 0 on success. Otherwise -1.
	 */
	int flush()
	{
		return OW_FileSystem::flush(m_hdl);
	}

	/**
	 * @return true if this is a valid OW_File object.
	 */
	operator void*() const 
	{ 
		return (void*)(m_hdl != 0); 
	}

	/**
	 * Equality operator.
	 * @param rhs The OW_File object to compare this object to.
	 * @return true if this OW_File represents the same file as rhs.
	 */
	bool operator==(const OW_File& rhs) 
	{ 
		return m_hdl == rhs.m_hdl; 
	}

	/**
	 * NULL Equality operator.
	 * This allows OW_File objects to be compared directly to zero.
	 * @return true if this OW_File is a NULL object.
	 */
	bool operator==(const OW_FileNullPtr*) 
	{ 
		return m_hdl == 0; 
	}

	/**
	 * NULL inequality operator.
	 * This allows OW_File objects to be compared directly to zero.
	 * @return true if this OW_File is not a NULL object.
	 */
	bool operator!=(const OW_FileNullPtr*) 
	{ 
		return m_hdl != 0; 
	}

private:

	OW_File(OW_FileHandle& hdl) : m_hdl(hdl) 
	{
	}

	OW_FileHandle	m_hdl;

	friend class OW_FileSystem;
};

#endif	// __OW_FILESYSTEM_HPP__

