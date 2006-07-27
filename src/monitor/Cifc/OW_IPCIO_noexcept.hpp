#ifndef OW_IPCIO_NOEXCEPT_HPP_INCLUDE_GUARD_
#define OW_IPCIO_NOEXCEPT_HPP_INCLUDE_GUARD_

/*******************************************************************************
* Copyright (C) 2006, Quest Software, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Quest Software, Inc., nor the
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
 */

#include "OW_config.h"
#include <vector>
#include <memory>
#include <ios>

namespace OW_NAMESPACE
{

/// OW_privman_noexcept uses this class to handle communications
/// between the monitor and client.  This class is intended for use only
/// on the client side.
//
class IPCIO_noexcept
{
public:
/// The class operations never throw exceptions.  If a runtime error occurs,
/// ok() becomes false and remains false thereafter.  Any operation carried
/// out when ok() is false is a no-op.

	IPCIO_noexcept();

	~IPCIO_noexcept();

	/// @pre !connected() and @a descr >= 0.
	//
	void connect(int descr);

	/// @return Whether @c connect has been called with a nonnegative descriptor
	//
	bool connected() const
	{
		return m_connected;
	}

	/// @return connected() and no runtime error has occurred.
	//
	bool ok() const
	{
		return m_ok;
	}

	enum EBuffering
	{
		E_BUFFERED, E_UNBUFFERED
	};

	/**
	* Reads specified amount of data from peer.
	*
	* @param buf Where to store characters read.
	* @param count How many characters to read.
	* @param eb If set to @c E_BUFFERED, then use buffered input.
	*
	* @pre If @a eb != @c E_BUFFERED, there must be no data remaining in
	* the internal input buffer.
	*
	* @post @c ok() becomes false if I/O error occurred or fewer than
	* @a count bytes read.
	*/
	void sgetn(char * buf, std::streamsize count, EBuffering eb = E_BUFFERED);

	/**
	* Writes specified amount of data to peer.
	* @param buf Source of characters to write
	* @param count How many characters to write.
	* @post @c ok() becomes false if I/O error occured.
	*/
	void sputn(char const * buf, std::streamsize count);

	/**
	* Gets a file descriptor from the peer.
	*
	* @return The descriptor sent by the peer.
	*
	* @pre There is no data in the internal input buffer.
	*
	* @post @c ok() becomes false on I/O error or if the peer does not send a
	* descriptor or if there is data in the input buffer.
	*/
	int get_handle();

	/**
	* Finishes writing to peer any buffered write data, and sets ok() to
	* false if the operation fails.
	*/
	void put_sync();

	/**
	* Checks that there is no data left in the input buffer, i.e., that the
	* entire message sent from the peer has been read.  Sets ok() to false
	* if the operation fails.
	*/
	void get_sync();

private:
	IPCIO_noexcept(IPCIO_noexcept const &); // disallowed
	void operator=(IPCIO_noexcept const &); // disallowed
	class Buffer;
	
	int m_descriptor;
	bool m_connected;
	bool m_ok;
	std::auto_ptr<Buffer> m_buffer;
};

/**
* Writes a single value of type @a T to the peer.
* @pre @a T is a POD type containing no pointers.
*/
template <typename T>
void ipcio_put(IPCIO_noexcept & io, T const & val)
{
	io.sputn(reinterpret_cast<char const *>(&val), sizeof(val));
}

/**
* Reads from the peer a single value of type @a T and assigns it to @x.
* @pre @a T is a POD type.
*/
template <typename T>
void ipcio_get(
	IPCIO_noexcept & io, T & x,
	IPCIO_noexcept::EBuffering eb = IPCIO_noexcept::E_BUFFERED)
{
	io.sgetn(reinterpret_cast<char *>(&x), sizeof(x), eb);
}

/**
* Writes @a s to the peer as if it were a @c String.
* @c NULL is converted to "".
*/
void ipcio_put_str(IPCIO_noexcept & io, char const * s);

/**
* Reads from the peer a single @c String value and assigns its null-terminated
* character sequence to v.
*/
void ipcio_get_str(IPCIO_noexcept & io, std::vector<char> & v);

} // namespace OW_NAMESPACE

#endif
