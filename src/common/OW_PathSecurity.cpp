/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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
 * @author Kevin S. Van Horn
 */

#ifndef OW_WIN32

#include "OW_Assertion.hpp"
#include "OW_FileSystem.hpp"
#include "OW_Format.hpp"
#include "OW_String.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace OW_NAMESPACE
{

namespace
{
	using namespace FileSystem::Path;

	unsigned const MAX_SYMBOLIC_LINKS = 100;

	// Conceptually, this class consists of two values:
	// - A resolved part, which can have path components added or removed
	//   at the end, and
	// - an unresolved part, which can have path components added or removed
	//   at the beginning.
	//
	class PartiallyResolvedPath
	{
	public:
		// PROMISE: Resolved part is base_dir and unresolved part is empty.
		// REQUIRE: base_dir is in canonical form.
		PartiallyResolvedPath(char const * base_dir);

		// Prepends the components of path to the unresolved part of the path.
		// Note that path may be empty.
		// REQUIRE: path does not start with '/'.
		void multi_push_unresolved(char const * path);

		// Discards the first component of the unresolved part.
		// REQUIRE: unresolved part nonempty.
		void pop_unresolved();

		// RETURNS: true iff the unresolved part is empty
		bool unresolved_empty() const;

		// RETURNS: true iff the first component of the unresolved part is ".".
		bool unresolved_starts_with_curdir() const;

		// RETURNS: true iff the first component of the unresolved part is "..".
		bool unresolved_starts_with_parent() const;

		// RETURNS: true iff push_unresolved(path) has ever been called
		// with an empty unresolved part and path ending in '/'.
		bool dir_specified() const;

		// Transfers the first component of the unresolved part to the end
		// of the resolved part.
		// REQUIRE: unresolved part is nonempty.
		void xfer_component();

		// Discards the last component of the resolved part.
		// If resolved part is "/", does nothing (parent of "/" is "/").
		void pop_resolved();

		// Resets the resolved part ot "/".
		void reset_resolved();

		// RETURNS: the resolved part, as a String.
		String get_resolved() const;

		// Calls lstat on the resolved part.
		void lstat_resolved(struct stat & st) const;

		// REQUIRE: resolved part is not "/", and last component is a symbolic
		// link.
		// PROMISE: Reads the symbolic link and assigns it to path (including
		// a terminating '\0' character).
		void read_symlink(std::vector<char> & path);

	private:

		// INVARIANT: Holds an absolute path with no duplicate '/' chars,
		// no "." or ".." components, no component (except possibly the last)
		// that is a symlink, and no terminating '/' unless the whole path is
		// "/". 
		mutable std::vector<char> m_resolved;

		// INVARIANT: holds a relative path with no repeated or terminating '/'
		// chars, stored in reverse order.
		std::vector<char> m_unresolved;

		bool m_dir_specified;
	};

	PartiallyResolvedPath::PartiallyResolvedPath(char const * base_dir)
	: m_resolved(base_dir, base_dir + std::strlen(base_dir)),
	  m_unresolved(),
	  m_dir_specified(false)
	{
	}

	void PartiallyResolvedPath::multi_push_unresolved(char const * path)
	{
		OW_ASSERT(path && *path != '/');
		if (*path == '\0')
		{
			return;
		}
		char const * end = path;
		while (*end != '\0')
		{
			++end;
		}
		if (end != path && *(end - 1) == '/')
		{
			m_dir_specified = true;
		}
		m_unresolved.push_back('/');
		bool last_separator = true;
		while (end != path)
		{
			char c = *--end;
			bool separator = (c == '/');
			if (!(separator && last_separator))
			{
				m_unresolved.push_back(c);
			}
			last_separator = separator;
		}
	}

	void PartiallyResolvedPath::pop_unresolved()
	{
		OW_ASSERT(!m_unresolved.empty());
		while (!m_unresolved.empty() && m_unresolved.back() != '/')
		{
			m_unresolved.pop_back();
		}
		while (!m_unresolved.empty() && m_unresolved.back() == '/')
		{
			m_unresolved.pop_back();
		}
	}

	inline bool PartiallyResolvedPath::unresolved_empty() const
	{
		return m_unresolved.empty();
	}

	bool PartiallyResolvedPath::unresolved_starts_with_curdir() const
	{
		std::size_t n = m_unresolved.size();
		return (
			n > 0 && m_unresolved[n - 1] == '.' &&
			(n == 1 || m_unresolved[n - 2] == '/')
		);
	}

	bool PartiallyResolvedPath::unresolved_starts_with_parent() const
	{
		std::size_t n = m_unresolved.size();
		return (
			n >= 2 && m_unresolved[n - 1] == '.' && m_unresolved[n - 2] == '.'
			&& (n == 2 || m_unresolved[n - 3] == '/')
		);
	}

	inline bool PartiallyResolvedPath::dir_specified() const
	{
		return m_dir_specified;
	}

	void PartiallyResolvedPath::xfer_component()
	{
		OW_ASSERT(!m_unresolved.empty());
		std::size_t n = m_resolved.size();
		OW_ASSERT(n > 0 && (n == 1 || m_resolved[n - 1] != '/'));
		if (n > 1)
		{
			m_resolved.push_back('/');
		}
		char c;
		while (!m_unresolved.empty() && (c = m_unresolved.back()) != '/')
		{
			m_unresolved.pop_back();
			m_resolved.push_back(c);
		}
		while (!m_unresolved.empty() && m_unresolved.back() == '/')
		{
			m_unresolved.pop_back();
		}
	}

	void PartiallyResolvedPath::pop_resolved()
	{
		std::size_t n = m_resolved.size();
		OW_ASSERT(n > 0 && m_resolved[0] == '/');
		if (n == 1)
		{
			return; // parent of "/" is "/"
		}
		OW_ASSERT(m_resolved.back() != '/');
		while (m_resolved.back() != '/')
		{
			m_resolved.pop_back();
		}
		// pop off path separator too, unless we are back to the root dir
		if (m_resolved.size() > 1)
		{
			m_resolved.pop_back();
		}
	}

	inline void PartiallyResolvedPath::reset_resolved()
	{
		std::vector<char>(1, '/').swap(m_resolved);
	}

	class NullTerminate
	{
		std::vector<char> & m_buf;
	public:
		NullTerminate(std::vector<char> & buf)
		: m_buf(buf)
		{
			m_buf.push_back('\0');
		}

		~NullTerminate()
		{
			m_buf.pop_back();
		}
	};

	inline String PartiallyResolvedPath::get_resolved() const
	{
		NullTerminate x(m_resolved);
		return String(&m_resolved[0]);
	}

#ifdef OW_NETWARE
  #define LSTAT ::stat
  #define S_ISLNK(x) false
  #define READLINK(path, buf, size) 0
  #define READLINK_ALLOWED false
#else
  #define LSTAT ::lstat
  #define READLINK(path, buf, size) ::readlink((path), (buf), (size))
  #define READLINK_ALLOWED true
#endif

	void wrapped_lstat(char const * path, struct stat & st)
	{
		if (LSTAT(path, &st) < 0)
		{
			OW_THROW_ERRNO_MSG(FileSystemException, path);
		}
	}

	void PartiallyResolvedPath::lstat_resolved(struct stat & st) const
	{
		NullTerminate x(m_resolved);
		wrapped_lstat(&m_resolved[0], st);
	}

	void PartiallyResolvedPath::read_symlink(std::vector<char> & path)
	{
		OW_ASSERT(READLINK_ALLOWED);
		NullTerminate x(m_resolved);
		std::vector<char> buf(MAXPATHLEN + 1);
		while (true)
		{
			char const * symlink_path = &m_resolved[0];
			int rv = READLINK(symlink_path, &buf[0], buf.size());
			// Note that if the link value is too big to fit into buf, but
			// there is no other error, then rv == buf.size(); in particular,
			// we do NOT get rv < 0 with errno == ENAMETOOLONG (this refers
			// to the input path, not the link value returned).
			if (rv < 0)
			{
				OW_THROW_ERRNO_MSG(FileSystemException, symlink_path);
			}
			else if (static_cast<unsigned>(rv) == buf.size())
			{
				buf.resize(2 * buf.size());
			}
			else
			{
				path.swap(buf);
				return;
			}
		}
	}

	inline bool group_ok(::gid_t gid)
	{
#ifdef OW_SOLARIS
		return gid == 0 /* root */ || gid == 3 /* sys */;
#else
		return gid == 0 /* root */;
#endif
	}

	inline bool check_grp_oth(struct stat const & x)
	{
		::mode_t badmsk = group_ok(x.st_gid) ? S_IWOTH : (S_IWGRP | S_IWOTH);
		return !(x.st_mode & badmsk);
	}

	bool file_ok(struct stat const & x, ::uid_t uid, bool full_path)
	{
		// Note: originally this disallowed multiple hard links to a file,
		// but that restriction is not necessary, as the permissions for a
		// file are associated with its inode, and not with its directory
		// entries.  Note also that it's not a problem if someone does an
		// unlink of an alternate path to the file, as this just removes
		// the alternate directory entry -- the file itself is not actually
		// deleted until there are no hard links at all to it.
		return (
			// owned by root or uid (or bin on HP-UX)
			(x.st_uid == 0 || 
#ifdef OW_HPUX
			 // on HP-UX, many system dirs & files are owned by the bin user, which has a uid of 2.
			 x.st_uid == 2 ||
#endif
			 x.st_uid == uid) &&
			// either
			// - file is a symbolic link;
			// - no write privileges for group and other (certain system
			//   groups excepted); or
			// - this is a parent directory of the file/dir we are
			//   interested in, and its sticky bit is set so that files in this
			//   directory can only be renamed or deleted by the owner of the
			//   file, owner of the directory, and root.
			(S_ISLNK(x.st_mode) || check_grp_oth(x) ||
				S_ISDIR(x.st_mode) && !full_path && (x.st_mode & S_ISVTX)
			)
		);
	}

	char const * strip_leading_slashes(char const * path)
	{
		while (*path == '/')
		{
			++path;
		}
		return path;
	}

	std::pair<ESecurity, String>
	path_security(
		char const * base_dir, char const * rel_path, ::uid_t uid, bool bdsecure
	)
	{
		OW_ASSERT(base_dir[0] == '/');
		OW_ASSERT(
			base_dir[1] == '\0' || base_dir[std::strlen(base_dir) - 1] != '/');
		OW_ASSERT(rel_path[0] != '/');

		struct stat st;
		PartiallyResolvedPath prp(base_dir);
		bool secure = bdsecure;
		ESecurity status_if_secure = E_SECURE_DIR;
		unsigned num_symbolic_links = 0;

		prp.multi_push_unresolved(rel_path);
		while (!prp.unresolved_empty())
		{
			if (prp.unresolved_starts_with_curdir())
			{
				prp.pop_unresolved();
			}
			else if (prp.unresolved_starts_with_parent())
			{
				prp.pop_unresolved();
				prp.pop_resolved();
			}
			else
			{
				prp.xfer_component();
				prp.lstat_resolved(st);
				secure &= file_ok(st, uid, prp.unresolved_empty());
				if (S_ISREG(st.st_mode))
				{
					status_if_secure = E_SECURE_FILE;
					if (!prp.unresolved_empty() || prp.dir_specified())
					{
						OW_THROW_ERRNO_MSG1(
							FileSystemException, prp.get_resolved(), ENOTDIR);
					}
				}
				else if (S_ISLNK(st.st_mode))
				{
					if (++num_symbolic_links > MAX_SYMBOLIC_LINKS)
					{
						OW_THROW_ERRNO_MSG1(
							FileSystemException, prp.get_resolved(), ELOOP);
					}
					std::vector<char> slpath_vec;
					prp.read_symlink(slpath_vec);
					char const * slpath = &slpath_vec[0];
					if (slpath[0] == '/')
					{
						prp.reset_resolved();
						slpath = strip_leading_slashes(slpath);
					}
					else
					{
						prp.pop_resolved();
					}
					prp.multi_push_unresolved(slpath);
				}
				else if (!S_ISDIR(st.st_mode))
				{
					String msg = prp.get_resolved() + 
						" is not a directory, symbolic link, nor regular file";
					OW_THROW(FileSystemException, msg.c_str());
				}
			}
		}
		ESecurity sec = secure ? status_if_secure : E_INSECURE;
		return std::make_pair(sec, prp.get_resolved());
	}

	std::pair<ESecurity, String> path_security(char const * path, UserId uid)
	{
		if (path[0] != '/')
		{
			OW_THROW(FileSystemException,
				Format("%1 is not an absolute path", path).c_str());
		}
		char const * relpath = strip_leading_slashes(path);
		struct stat st;
		wrapped_lstat("/", st);
		bool bdsecure = file_ok(st, uid, *relpath == '\0');
		return path_security("/", relpath, uid, bdsecure);
	}

} // anonymous namespace

std::pair<ESecurity, String> 
FileSystem::Path::security(String const & path, UserId uid)
{
	String abspath =
		path.startsWith("/") ? path : getCurrentWorkingDirectory() + "/" + path;
	return path_security(abspath.c_str(), uid);
}

std::pair<ESecurity, String>
FileSystem::Path::security(
	String const & base_dir, String const & rel_path, UserId uid)
{
	return path_security(base_dir.c_str(), rel_path.c_str(), uid, true);
}

std::pair<ESecurity, String> 
FileSystem::Path::security(String const & path)
{
	return security(path, ::geteuid());
}

std::pair<ESecurity, String>
FileSystem::Path::security(
	String const & base_dir, String const & rel_path)
{
	return security(base_dir, rel_path, ::geteuid());
}

} // end namespace OW_NAMESPACE

#endif
