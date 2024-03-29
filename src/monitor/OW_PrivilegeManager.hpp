#ifndef OW_PRIVILEGE_MANAGER_HPP_INCLUDE_GUARD_
#define OW_PRIVILEGE_MANAGER_HPP_INCLUDE_GUARD_

/*******************************************************************************
* Copyright (c) 2002, Networks Associates, Inc. All rights reserved.
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* Copyright (C) 2006, Novell, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Network Associates,
*       nor Quest Software, Inc., nor Novell, Inc., nor the
*       names of its contributors or employees may be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Kevin S. Van Horn
 * @author Douglas Kilpatrick (privman.h)
 * @author Lee Badger (privman.h)
 * @author Dan Nuffer
 * @author Bart Whiteley
 */

#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "blocxx/Cstr.hpp"
#include "OW_Exception.hpp"
#include "OW_LoggerSpec.hpp"
#include "OW_PrivManOpenFlags.h"
#include "blocxx/Process.hpp"
#include "OW_Types.hpp"
#include "blocxx/AutoDescriptor.hpp"
#include "blocxx/FileSystem.hpp"
#include "blocxx/GlobalPtr.hpp"
#include "blocxx/Reference.hpp"

namespace OW_NAMESPACE
{

class PrivilegeManagerImpl;
typedef blocxx::Reference<PrivilegeManagerImpl> PrivilegeManagerImplRef;

OW_DECLARE_EXCEPTION(PrivilegeManager);
OW_DECLARE_EXCEPTION2(FatalPrivilegeManager, PrivilegeManagerException);
OW_DECLARE_EXCEPTION2(MonitorCommunication, FatalPrivilegeManagerException);
OW_DECLARE_EXCEPTION2(InsufficientPrivileges, FatalPrivilegeManagerException);

/**
* This class implements the privilege separation scheme described in these two
* papers:
* - 'Preventing privilege escalation',
*   Niels Provos, Markus Friedl, and Peter Honeyman,
*   Technical Report TR-02-2, University of Michigan, CITI, Aug. 2002
* - 'Privman: a library for partitioning applications',
*   Douglas Kilpatrick,
*   Proceedings, USENIX 2003 Annual Technical Conference, pp. 273-284.
* It is a handle class, that is, objects of this class are reasonably
* inexpensive to copy, and both x and any copy of x refer to the same underlying
* resource.
* A process is limited to having only one monitor, so there can only be
* one PrivilegeManagerImpl instance, which all PrivilegeManager instances
* share.
*/
class OW_MONITOR_API PrivilegeManager
{
public:
	// Whether or not to pass LD_LIBRARY_PATH (or platform's equivalent)
	// to the monitor's environment.  Ordinarily this should be left at its
	// initial value of false -- it's here as a hook for test code.
	//
	static bool use_lib_path;

	/**
	* Null PrivilegeManager object suitable only for reassigning.
	*/
	PrivilegeManager();
	PrivilegeManager(const PrivilegeManager& x);
	PrivilegeManager& operator=(const PrivilegeManager& x);

	bool isNull() const;

	/**
	* Spawns a child process -- the monitor -- that continues to run with
	* elevated privileges, and drops elevated privileges for the client
	* (this process). The client process requests the monitor to carry out any
	* operations that require special (e.g., root) privileges.  Privilege
	* configuration file(s) specifies the client's privileges, that is, the
	* operations it may request, and the arguments it may supply for those
	* operations. The privilege configuration file(s) may also specify the user
	* that the client runs as.
	*
	* @pre The process is running single-threaded, as @c root. Otherwise
	* a null PrivilegeManager is returned.
	*
	* @pre If a privilege configuration file @a app_name exists in directory
	* @a config_dir, both must be secure (@see FileSystem::Path::security).
	*
	* @pre @c SIGPIPE is handled or ignored, so that this signal does not
	* cause program termination.
	*
	* @pre Either @a user_name is null or empty, or the privilege configuration
	* file(s) specifies an unprivileged user.  If the unprivileged user
	* is specified both via @a user_name and the privilege configuration file,
	* then they must specify the same user name.
	*
	* @post Process is running as the specified unprivileged user,
	* a separate monitor process is running, and this object may be used to
	* request operations from the monitor.  If either @a app_name or
	* @a config_dir is null or empty, or if the privileges config files(s)
	* could not be loaded, then this object denies all requests.
	*
	* @param config_dir Absolute path to a secure directory containing either
	* the privilege configuration file or directory identified by app_name.
	*
	* @param app_name Name of privilege configuration file or directory to use.
	*  If a directory is specified, all the files in that directory will be
	*  loaded.
	*
	* @return A PrivilegeManager connected to the monitor.
	*
	* @see privconfig_syntax.txt for a description of the syntax of
	* privilege configuration files.
	*/
	static PrivilegeManager createMonitor(
		char const * config_dir, char const * app_name,
		char const * user_name = 0, LoggerSpec const * plogspec = 0 );

	/**
	* A variant of that takes arguments of arbitrary string-like types.
	*
	* @pre @a S1, @a S2, and @a S3 are types for which
	* <tt>Cstr::to_char_ptr</tt> is defined.
	*/
	template <typename S1, typename S2, typename S3>
	static PrivilegeManager createMonitor(
		S1 const & config_dir, S2 const & app_name, S3 const & user_name = S3(),
		LoggerSpec const * plogspec = 0)
	{
		return createMonitor(
			blocxx::Cstr::to_char_ptr(config_dir),
			blocxx::Cstr::to_char_ptr(app_name),
			blocxx::Cstr::to_char_ptr(user_name),
			plogspec);
	}

	/**
	 * Creates a PrivilegeManager connected to this process's monitor.
	 *
	 * @pre The process was created by a call to PrivilegeManager::monitoredSpawn().
	 *  and createMonitor() has not been called.
	 *
	 * @return a null PrivilegeManager if a monitor does not exist.
	 */
	static PrivilegeManager connectToMonitor();

	/**
	 * @return An object reference for requesting operations from the monitor.
	 *  If createMonitor() or connectToMontor() has not been called by the
	 *  process, a null PrivilegeManager will be returned.
	 */
	static PrivilegeManager getPrivilegeManager();

	/**
	* If there are no other copies of this object in existence, notifies the
	* monitor process that it is no longer needed, and waits for it
	* to terminate.
	*/
	~PrivilegeManager();

	/// These flags have the same meaning and the same allowed combinations
	/// as for std::ios_base::openmode (see std::filebuf::open()), except
	/// that we have added one more combination corresponding to open mode
	/// @c +>> in Perl: @c in_out_app.
	enum OpenFlags
	{
		in        = E_OW_PRIVMAN_IN,
		out       = E_OW_PRIVMAN_OUT,
		trunc     = E_OW_PRIVMAN_TRUNC,
		app       = E_OW_PRIVMAN_APP,
		ate       = 1 << 4,
		binary    = 1 << 5,
		out_trunc    = out | trunc,
		out_app      = out | app,
		in_out       = in | out,
		in_out_trunc = in | out | trunc,
		in_out_app   = in | out | app,
		iota         = in | out | trunc | app,

		// These flags are used only with the posix open() function.  Their use
		// should be considered non-portable.
		posix_nonblock = 1 << 6
	};

	enum OpenPerms
	{
		no_perms    = 0,
		user_read   = 1 << 8,
		user_write  = 1 << 7,
		user_exec   = 1 << 6,
		// group permissions left out as they are not portable to Windows
		other_read  = 1 << 2,
		other_write = 1 << 1,
		other_exec  = 1 << 0,

		user_rw     = user_read | user_write,
		user_rx     = user_read | user_exec,
		user_all    = user_read | user_write | user_exec,
		other_rw    = other_read | other_write,
		other_rx    = other_read | other_exec,
		other_all   = other_read | other_write | other_exec
	};


	/**
	* Used to access a file that requires special privileges to read or write.
	*
	* @return A @c AutoDescriptor for the file at @a pathname, opened as
	*         indicated by @a flags.
	*
	* @post If the file is created, it has the permissions specified in @e perms,
	* and it is owned by @c root.
	*
	* @param pathname Absolute path to the file to open.
	*
	* @param flags Indicates mode in which to open file.  Must be one of the
	* following values:
	* - @c in: read-only.  File must exist.
	* - @c out: write-only.  File emptied if it exists, and created otherwise.
	* - <tt>out | trunc</tt>: same as @c out.
	* - <tt>out | app</tt>: append-only.  File created if necessary.
	* - <tt>in | out</tt>: read/write, starting at beginning of file.
	*   File must exist.
	* - <tt>in | out | trunc</tt>: read/write.  File emptied if it exists, and
	*   created otherwise.
	*
	* @param perms Permissions to assign to file if the operation creates the
	*   file.
	*
	* @pre Caller must have the required privileges for @a pathname, as follows:
	* - If <tt>flags & in</tt>, then @c open_read privilege is required.
	* - If <tt>flags & app</tt>, then either @c open_write or @c open_append
	*   privilege is required.
	* - If <tt>flags & out</tt> but not <tt>flags & app</tt>, then @c open_write
	*   privilege is required.
	*
	* @throw PrivilegeManagerException
	* @throw IPCIOException
	*/
	blocxx::AutoDescriptor open(
		char const * pathname, OpenFlags flags, OpenPerms perms = no_perms);

	/**
	* Variant of @c open that takes argument of arbitrary string-like type.
	* @pre @a S is a type for which <tt>Cstr::to_char_ptr</tt> is defined.
	*/
	template <typename S>
	blocxx::AutoDescriptor open(
		S const & pathname, OpenFlags flags, OpenPerms perms = no_perms)
	{
		return this->open(blocxx::Cstr::to_char_ptr(pathname), flags, perms);
	}

	/**
	 * Perform a stat() on the supplied pathname to obtain file information for
	 * the supplied pathname (if a regular file or directory), or the file to
	 * which it refers (for a symlink).
	 *
	 * @return a completed FileInformation structure.
	 *
	 * @pre Caller must have the stat privilege for the supplied pathname.
	 *
	 * @throw FileSystemException if the file did not exist, the pathname is a
	 * dangling symlink, or anything else that could cause a stat() to fail.
	 *
	 * @throw IPCIOException if the monitor communication fails
	 *
	 * @throw PrivilegeManagerException for insufficient privileges or other
	 * monitor errors
	 */
	blocxx::FileSystem::FileInformation stat(const char* pathname);


	/**
	 * Variant of @c open that takes argument of arbitrary string-like type.
	 * @pre @a S is a type for which <tt>Cstr::to_char_ptr</tt> is defined.
	 */
	template <typename S>
	blocxx::FileSystem::FileInformation stat(const S& pathname)
	{
		return this->stat(blocxx::Cstr::to_char_ptr(pathname));
	}

	/**
	 * Perform an lstat() on a file to obtain file information for the supplied
	 * pathname.  If the path is a link, information is obtained for the link
	 * itself instead of the linked file.
	 *
	 * @return a completed FileInformation structure.
	 *
	 * @pre Caller must have the stat privilege for the supplied pathname.
	 *
	 * @throw FileSystemException if the file did not exist, the pathname is a
	 * dangling symlink, or anything else that could cause a stat() to fail.
	 *
	 * @throw IPCIOException if the monitor communication fails
	 *
	 * @throw PrivilegeManagerException for insufficient privileges or other
	 * monitor errors
	 */
	blocxx::FileSystem::FileInformation lstat(const char* pathname);

	/**
	 * Variant of @c open that takes argument of arbitrary string-like type.
	 * @pre @a S is a type for which <tt>Cstr::to_char_ptr</tt> is defined.
	 */
	template <typename S>
	blocxx::FileSystem::FileInformation lstat(const S& pathname)
	{
		return this->lstat(blocxx::Cstr::to_char_ptr(pathname));
	}

	enum ReadDirOptions
	{
		E_KEEP_SPECIAL, E_OMIT_SPECIAL
	};


	// An enum that describes what type of error is stored in the error code.
	// Any error with an error code less than MONITOR_ERROR_START can be assumed
	// to be an errno.  Everything equal or greater to MONITOR_FATAL_ERROR_START
	// should be assume to be a fatal monitor error, most likely due to errors
	// in privileges or security.
	enum ErrorCodes
	{
		MONITOR_ERROR_START = 65536,
		E_UNKNOWN,
		E_OPERATION_FAILED,
		MONITOR_FATAL_ERROR_START = MONITOR_ERROR_START * 2,
		E_INVALID_OPERATION,
		E_INVALID_PATH,
		E_INVALID_SIZE,
		E_INVALID_PARAMETER,
		E_INVALID_SECURITY,
		E_INSUFFICIENT_PRIVILEGES,
		E_ALREADY_INITIALIZED
	};

	/**
	* @return The entries in the directory @a pathname.  If @a opt ==
	* @c E_KEEP_SPECIAL, then the result includes the special entries for the
	* directory itself and its parent directory.  If @a opt == @c E_OMIT_SPECIAL,
	* then these special entries are omitted.
	*
	* @pre @a pathname is a valid directory path.
	*
	* @pre Caller has @c read_dir privilege for @a pathname.
	*
	* @throw PrivilegeManagerException
	* @throw IPCIOException
	*/
	blocxx::StringArray readDirectory(char const * pathname, ReadDirOptions opt);

	/**
	* Variant of @c readDirectory that takes argument of arbitrary string-like
	* type.
	* @pre @a S is a type for which <tt>Cstr::to_char_ptr</tt> is defined.
	*/
	template <typename S>
	blocxx::StringArray readDirectory(S const & pathname, ReadDirOptions opt)
	{
		return this->readDirectory(blocxx::Cstr::to_char_ptr(pathname), opt);
	}

	/**
	 * Return the contents of the symlink specified by @a pathname.
	 *
	 * @pre @pathname is a valid symlink path
	 * @pre Caller has @c read_link privilege for @a pathname.
	 *
	 * @throw PrivilegeManagerException
	 * @throw IPCIOException
	 */
	blocxx::String readLink(char const * pathname);

	template <typename S>
	blocxx::String readLink(S const & pathname)
	{
		return this->readLink(blocxx::Cstr::to_char_ptr(pathname));
	}

#if 0
	/**
	* @return True iff only @c root or @a user have write permissions for
	* @a path and all ancestor directories.
	*
	* @pre Caller must have @c checkPath privilege for @a path.
	*
	* @throw PrivilegeManagerException
	* @throw IPCIOException
	*/
	bool checkPath(char const * path, char const * user);

	/**
	* Variant of @c checkPath that takes arguments of arbitrary string-like types.
	* @pre @a S1 and @a S2 are types for which <tt>Cstr::to_char_ptr</tt> is defined.
	*/
	template <typename S1, typename S2>
	bool checkPath(S1 const & path, S2 const & user)
	{
		return this->checkPath(Cstr::to_char_ptr(path), Cstr::to_char_ptr(user));
	}
#endif

	/**
	* @post The file or directory at @a oldpath is renamed to @a newpath.
	*
	* @pre @a oldpath and @a newpath are on the same filesystem.
	*
	* @pre Caller must have @c rename_from privilege for @a oldpath and
	* rename_to privilege for  @a newpath.
	*
	* @throw PrivilegeManagerException
	* @throw IPCIOException
	*/
	void rename(char const * oldpath, char const * newpath);

	/**
	* Variant of @c rename that takes arguments of arbitrary string-like types.
	* @pre @a S1 and @a S2 are types for which <tt>Cstr::to_char_ptr</tt> is defined.
	*/
	template <typename S1, typename S2>
	void rename(S1 const & oldpath, S2 const & newpath)
	{
		this->rename(blocxx::Cstr::to_char_ptr(oldpath), blocxx::Cstr::to_char_ptr(newpath));
	}

	/**
	* @post The file at @path no longer exists (if successful), errno set.
	* @pre Caller must have @c remove_file privilege for @a path.
	*
	* @return true if the file was successfully removed.
	*
	* @throw PrivilegeManagerException
	* @throw IPCIOException
	*/
	bool removeFile(char const * path);

	/**
	* Variant of @c removeFile that takes argument of arbitrary string-like type.
	* @pre @a S is a type for which <tt>Cstr::to_char_ptr</tt> is defined.
	*/
	template <typename S>
	bool removeFile(S const & path)
	{
		return this->removeFile(blocxx::Cstr::to_char_ptr(path));
	}

	/**
	* @post The directory at @path no longer exists (if successful), errno set.
	* @pre Caller must have @c remove_dir privilege for @a path.
	*
	* @return true if the directory was successfully removed.
	*
	* @throw PrivilegeManagerException
	* @throw IPCIOException
	*/
	bool removeDirectory(char const * path);

	/**
	* Variant of @c removeDirectory that takes argument of arbitrary string-like type.
	* @pre @a S is a type for which <tt>Cstr::to_char_ptr</tt> is defined.
	*/
	template <typename S>
	bool removeDirectory(S const & path)
	{
		return this->removeDirectory(blocxx::Cstr::to_char_ptr(path));
	}

	/**
	* Spawns a monitored child process running as the same user as the calling
	* process.
	*
	* @return A @c Process object for the child process.
	*
	* @param exec_path Absolute path of the executable to run in the child
	* process.
	*
	* @param argv Null-terminated argument list for the child process.
	* (@see Exec::spawn for details).
	*
	* @param envp The null-terminated environment for the child process.
	* (@see Exec::spawn for details). If envp != Secure::minimalEnvironment(),
	* then the privilege configuration statement must include an
	* <env_specification> that allows for all the variables and values
	* designated by envp.
	*
	* @param app_name The name of the privilege configuration file or directory used by the
	* child process.  This is looked for in the same configuration directory
	* that was specified when the PrivilegeManager instance was created.
	*
	* @pre Caller must have @c monitored_spawn privilege for
	* (@a exec_path, @a app_name).
	*
	* @pre The unprivileged user specified in file @a app_name must have
	* execute permission on @a exec_path (including necessary permissions
	* to traverse the path).
	*
	* @pre Both @a exec_path and the configuration file specified by
	* @a app_name must be secure.
	*
	* @throw PrivilegeManagerException
	* @throw IPCIOException
	*/
	blocxx::ProcessRef monitoredSpawn(
		char const * exec_path,
		char const * app_name,
		char const * const argv[], char const * const envp[]
		);

	/**
	* Variant of @c monitoredSpawn for which @a exec_path and @a appname have
	* arbitrary string-like types, and @a argv and @a envp have arbitrary
	* string-array-like types.
	*
	* @pre @a S1 and S2 are types for which <tt>Cstr::to_char_ptr</tt> is
	* defined.
	*
	* @pre Specializations of the <tt>Cstr::CstrArr</tt> class template are
	* defined for types @a SA1 and @a SA2.
	*/
	template <typename S1, typename S2, typename SA1, typename SA2>
	blocxx::ProcessRef monitoredSpawn(
		S1 const & exec_path, S2 const & appname,
		SA1 const & argv, SA2 const & envp
		)
	{
		blocxx::Cstr::CstrArr<SA1> sa_argv(argv);
		blocxx::Cstr::CstrArr<SA2> sa_envp(envp);
		return this->monitoredSpawn(
			blocxx::Cstr::to_char_ptr(exec_path), blocxx::Cstr::to_char_ptr(appname),
			sa_argv.sarr, sa_envp.sarr
			);
	}

	/**
	* Spawns a monitored child process running as the same user as the calling
	* process.  The monitor runs as the specified user.
	*
	* @return A @c Process object for the child process.
	*
	* @param exec_path Absolute path of the executable to run in the child
	* process.
	*
	* @param app_name The name of the privilege configuration file or directory used by the
	* child process.  This is looked for in the same configuration directory
	* that was specified when the PrivilegeManager instance was created.
	*
	* @param argv Null-terminated argument list for the child process.
	* (@see Exec::spawn for details).
	*
	* @param envp The null-terminated environment for the child process.
	* (@see Exec::spawn for details). If envp != Secure::minimalEnvironment(),
	* then the privilege configuration statement must include an
	* <env_specification> that allows for all the variables and values
	* designated by envp.
	*
	* @param user The monitor for the child process runs as this user.
	* If @a user is null or empty, the monitor runs as the
	* same user as the calling process. There must be an entry for
	* @a user in the password file.
	*
	* @pre Caller must have @c monitored_user_spawn privilege for
	* (@a exec_path, @a app_name).
	*
	* @pre The unprivileged user specified in file @a app_name must have
	* execute permission on @a exec_path (including necessary permissions
	* to traverse the path).
	*
	* @pre Both @a exec_path and the configuration file specified by
	* @a app_name must be secure.
	*
	* @throw PrivilegeManagerException
	* @throw IPCIOException
	*/
	blocxx::ProcessRef monitoredUserSpawn(
		char const * exec_path,
		char const * app_name,
		char const * const argv[], char const * const envp[],
		char const * user
		);

	/**
	* Variant of @c monitoredUserSpawn for which @a exec_path and @a
	* appname have arbitrary string-like types, and @a argv and @a
	* envp have arbitrary string-array-like types.
	*
	* @pre @a S1, S2, and S3 are types for which
	* <tt>Cstr::to_char_ptr</tt> is defined.
	*
	* @pre Specializations of the <tt>Cstr::CstrArr</tt> class template are
	* defined for types @a SA1 and @a SA2.
	*/
	template <typename S1, typename S2, typename SA1, typename SA2, typename S3>
	blocxx::ProcessRef monitoredUserSpawn(
		S1 const & exec_path, S2 const & appname,
		SA1 const & argv, SA2 const & envp, S3 const & user
		)
	{
		blocxx::Cstr::CstrArr<SA1> sa_argv(argv);
		blocxx::Cstr::CstrArr<SA2> sa_envp(envp);
		return this->monitoredUserSpawn(
			blocxx::Cstr::to_char_ptr(exec_path), blocxx::Cstr::to_char_ptr(appname),
			sa_argv.sarr, sa_envp.sarr, blocxx::Cstr::to_char_ptr(user)
			);
	}


	/**
	* Spawns a child process that has no monitor.
	*
	* @return A @c Process object for the child process.
	*
	* @param execpath The path of the executable the child is to run.  Must be
	* an absolute path.
	*
	* @param argv Null-terminated argument list for the child process.
	* (@see Exec::spawn for details).
	*
	* @param envp Null-terminated environment for the child process.
	* (@see Exec::spawn for details). If envp != Secure::minimalEnvironment(),
	* then the privilege configuration statement must include an
	* <env_specification> that allows for all the variables and values
	* designated by envp.
	*
	* @param user The child process runs as this user.  If @a user is null
	* or empty, the child process runs as the same user as the calling process.
	* There must be an entry for @a user in the password file.
	*
	* @param working_dir A directory to @c chdir to before running the
	* program.  If empty or null, no @c chdir is done.
	*
	* @pre Caller must have @c user_spawn privilege for (@a execpath, @a user).
	*
	* @pre The user must have execute permission on @a exec_path (including
	* necessary permissions to traverse the path).
	*
	* @pre @a exec_path must be secure.
	*
	* @throw PrivilegeManagerException
	* @throw IPCIOException
	*/
	blocxx::ProcessRef userSpawn(
		char const * execpath,
		char const * const argv[], char const * const envp[],
		char const * user,
		char const * working_dir = 0
		);

	/**
	* A variant of @c userSpawn for which @a execpath and @a user
	* may have arbitrary string-like types, and @ argv and @a envp has arbitrary
	* string-array-like types.
	*
	* @pre @a S1, @a S2, and @ S3 are types for which
	* <tt>Cstr::to_char_ptr</tt> is defined.
	*
	* @pre Specializations of the <tt>Cstr::CstrArr</tt> class template are
	* defined for types @a SA1 and @a SA2.
	*/
	template <typename S1, typename S2, typename S3, typename SA1, typename SA2>
	blocxx::ProcessRef userSpawn(
		S1 const & execpath, SA1 const & argv, SA2 const & envp,
		S2 const & user, S3 const & working_dir
		)
	{
		blocxx::Cstr::CstrArr<SA1> sa_argv(argv);
		blocxx::Cstr::CstrArr<SA2> sa_envp(envp);
		return this->userSpawn(
			blocxx::Cstr::to_char_ptr(execpath), sa_argv.sarr, sa_envp.sarr,
			blocxx::Cstr::to_char_ptr(user), blocxx::Cstr::to_char_ptr(working_dir)
			);
	}

	// Default arguments don't work if type of default argument is a template
	// parameter, since you can't do type deduction.  Thus, we just
	// define another function with one less argument.
	//
	template <typename S1, typename S2, typename SA1, typename SA2>
	inline blocxx::ProcessRef userSpawn(
		S1 const & execpath, SA1 const & argv, SA2 const & envp,
		S2 const & user
		)
	{
		return userSpawn(execpath, argv, envp, user, (char const *)0);
	}

private:
	int kill(blocxx::ProcId pid, int sig);

	blocxx::Process::Status pollStatus(blocxx::ProcId pid);

	friend class MonitorChildImpl;

	static PrivilegeManager setInstance(const PrivilegeManagerImplRef& p_impl);

	static PrivilegeManagerImplRef init(
		char const * config_dir, char const * app_name, char const * user_name,
		LoggerSpec const * plogspec);

	PrivilegeManager(PrivilegeManagerImplRef p_impl);
	PrivilegeManagerImplRef m_impl;
	PrivilegeManagerImplRef pimpl() const;
};

	struct NullPMFactory
	{
		static void* create()
		{
			return 0;
		}
	};

	typedef blocxx::GlobalPtr<PrivilegeManagerMockObject, NullPMFactory> PrivilegeManagerMockObject_t;
	/**
	 * If this object is non-null, the default functionality of the
	 * PrivilegeManager class will be replaced by calls to
	 * g_privilegeManagerMockObject's member functions. This is to be used
	 * for unit tests. Not all functions may be
	 * implemented, if you need one that isn't, then please implement it!
	 * Modifying this variable will affect all
	 * threads, it should not be used in a threaded program.
	 */
	extern PrivilegeManagerMockObject_t g_privilegeManagerMockObject;
} // namespace OW_NAMESPACE
#endif
