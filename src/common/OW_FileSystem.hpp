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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_FILESYSTEM_HPP_INCLUDE_GUARD_
#define OW_FILESYSTEM_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_ArrayFwd.hpp"
#include "OW_Exception.hpp"
#include "OW_CommonFwd.hpp"

#ifdef OW_HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifndef MAXPATHLEN
#ifdef PATH_MAX
#define MAXPATHLEN PATH_MAX
#else
#define MAXPATHLEN 1024
#endif
#endif

namespace OpenWBEM
{

OW_DECLARE_APIEXCEPTION(FileSystem, OW_COMMON_API)

/**
 * The purpose of the FileSystem class is to provide an abstraction layer
 * over the platform dependant functionality related to a file system.
 */
namespace FileSystem
{
	/**
	 * Open a file for read/write and return an File object that can be used
	 * for reading and writing.
	 */
	OW_COMMON_API File openFile(const String& path);
	/**
	 * Create the file for the given name.
	 * @param path	The name of the file to create.
	 * @return On success an File object that can be used for reading and
	 * writing. Otherwise a NULL File object.  If the file exists, a NULL
	 * File object will be returned.
	 */
	OW_COMMON_API File createFile(const String& path);
	/**
	 * Opens or creates the file for the given name.
	 * @param path	The name of the file to create.
	 * @return On success an File object that can be used for reading and
	 * writing. Otherwise a null File object.
	 */
	OW_COMMON_API File openOrCreateFile(const String& path);
	/**
	 * Change the given file ownership
	 * @param filename The name of the file to change ownership on.
	 * @param userId The user id to change ownership to
	 * @return 0 on success. Otherwise -1
	 */
	OW_COMMON_API int changeFileOwner(const String& filename,
		const UserId& userId);
	/**
	 * @return true if the file exists (and false otherwise).
	 */
	OW_COMMON_API bool exists(const String& path);
	/**
	 * @return true if the file exists and can be read
	 */
	OW_COMMON_API bool canRead(const String& path);
	/**
	 * @return true if the file exists and can be written
	 */
	OW_COMMON_API bool canWrite(const String& path);
	/**
     * @return true if file exists and is a symbolic link
     */
	OW_COMMON_API bool isLink(const String& path);
	/**
	 * @return true if file exists and is a directory
	 */
	OW_COMMON_API bool isDirectory(const String& path);
	/**
	 * Change to the given directory
	 * @param path	The directory to change to
	 * @return true if the operation succeeds. Otherwise false.
	 */
	OW_COMMON_API bool changeDirectory(const String& path);
	/**
	 * Create a directory
	 * @param path	The name of the directory to create.
	 * @param mode specifies the permissions to use.
	 * @return true if the operation succeeds. Otherwise false.
	 */
	OW_COMMON_API bool makeDirectory(const String& path, int mode=0777);
	/**
	 * Get the size of the file in bytes
	 * @param path	The name of the file to get the size for.
	 * @param size	Put the size of the file in this variable.
	 * @return true if the operation succeeds. Otherwise false.
	 */
	OW_COMMON_API bool getFileSize(const String& path, off_t& size);
	/**
	 * Remove the given directory
	 * @param path	The name of the directory to remove
	 * @return true if the operation succeeds. Otherwise false.
	 */
	OW_COMMON_API bool removeDirectory(const String& path);
	/**
	 * Remove the given file
	 * @param path	The name of the file to remove.
	 * @return true if the operation succeeds. Otherwise false.
	 */
	OW_COMMON_API bool removeFile(const String& path);
	/**
	 * Get the names of the files (and directories) in the given directory
	 * @param path			The name of the directory to get the contents of.
	 * @param dirEntries	The directory contents will be placed in this array.
	 * @return true if the operation succeeds. Otherwise false.
	 */
	OW_COMMON_API bool getDirectoryContents(const String& path,
		StringArray& dirEntries);
	/**
	 * Rename the given file to the new name
	 * @param oldFileName	The name of the file to rename
	 * @param newFileName	The new name for the olFileName
	 * @return true if the operation succeeds. Otherwise false.
	 */
	OW_COMMON_API bool renameFile(const String& oldFileName,
		const String& newFileName);
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
	OW_COMMON_API size_t read(const FileHandle& hdl, void* bfr, size_t numberOfBytes,
		off_t offset=-1L);
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
	OW_COMMON_API size_t write(FileHandle& hdl, const void* bfr,
		size_t numberOfBytes, off_t offset=-1L);
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
	OW_COMMON_API off_t seek(const FileHandle& hdl, off_t offset, int whence);
	/**
	 * @param hdl	The file handle to use in the tell operation.
	 * @return The current position in the file relative to the beginning of
	 * the file on success. Otherwise -1.
	 */
	OW_COMMON_API off_t tell(const FileHandle& hdl);
	/**
	 * Position the file pointer associated with the given file handle to the
	 * beginning of the file.
	 * @param hdl	The file handle to use in the rewind operation.
	 */
	OW_COMMON_API void rewind(const FileHandle& hdl);
	/**
	 * Close file handle.
	 * @param hdl	The file handle to close.
	 * @return 0 on success. Otherwise -1.
	 */
	OW_COMMON_API int close(const FileHandle& hdl);
	/**
	 * Flush any buffered data to the file if buffering supported.
	 * @param hdl	The file handle to flush the buffer on.
	 */
	OW_COMMON_API int flush(FileHandle& hdl);
	/**
	 * Create a file with random data. This is *NOT* suitable for use with SSL initialization,
	 * since the data is generated by the system prng, and has very low entropy.
	 * @param filename  The name of the file to write the random data to
	 */
	OW_COMMON_API void initRandomFile(const String& file);
	/**
	 * Read and return the contents of a text file.  If the file contains a null
	 * character ('\0') then only previous data will be returned.
	 * @param filename The name of the file to read
	 * @exception FileSystemException if the file doesn't exist or reading fails
	 * for any reason.
	 */
	OW_COMMON_API String getFileContents(const String& filename);
	
	/**
	 * Read and return the lines of a test file. If the file contains a null
	 * character ('\0') then only previous data will be returned.
	 * @param filename The name of the file to read
	 * @exception FileSystemException if the file doesn't exist or reading fails
	 * for any reason.
	 */
	OW_COMMON_API StringArray getFileLines(const String& filename);

	/**
	 * Read the value of a symbolic link
	 * @param path Path to the symbolic link
	 * @return The target of the symbolic link
	 * @exception FileSystemException: ENOTDIR, ENOENT, EACCES, ELOOP, EINVAL, EIO, EFAULT, ENOMEM
	 */
	OW_COMMON_API String readSymbolicLink(const String& path);

	namespace Path
	{
		/**
		 * Convert path to the canonicalized absolute pathname by expanding all
		 * symbolic links and resolving references to /./, /../ and extra /
		 * characters. If path is relative, it will be interpreted as relative to
		 * the current working directory. This function is similar to the SuSv3
		 * function, however it's easier to use and thread safe.
		 * @param path The path to canonicalize.
		 * @return The canonicalized version of path.
		 * @throws FileSystemException EACCESS, EIO, ELOOP, ENOENT, ENOTDIR
		 */
		OW_COMMON_API String realPath(const String& path);

		/**
		 * Take a string that contains a pathname, and return a string that is
		 * a pathname of the parent directory of that file. Trailing '/'
		 * characters in the path are not counted as part of the path.
		 * If path does not contain a '/', then dirname() shall return the
		 * string ".". If path an empty string, dirname() shall return the
		 * string ".".
		 * @param filename The file pathname
		 * @return The pathname of the parent directory of filename.
		 */
		OW_COMMON_API String dirname(const String& filename);

		/**
		 * Get the process's current working directory.
		 * Calls to chdir() or fchdir() will modify this. Multi-threaded applications must
		 * exercise caution changing the current working directory.
		 * @throws FileSystemException: ENOENT-The current working directory has been unlinked.
		 */
		OW_COMMON_API String getCurrentWorkingDirectory();


	} // end namespace Path

} // end namespace FileSystem

} // end namespace OpenWBEM

#endif
