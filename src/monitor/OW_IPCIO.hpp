#ifndef OW_IPCIO_HPP_INCLUDE_GUARD_
#define OW_IPCIO_HPP_INCLUDE_GUARD_

/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
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
#include "OW_Exception.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_Types.hpp"
#include "OW_PosixUnnamedPipe.hpp"
#include "OW_IOIFCStreamBuffer.hpp"
#include "OW_AutoDescriptor.hpp"
#include "OW_Reference.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(IPCIO);


/***** TODO: timeouts ****/

/// The PrivilegeManager class uses this class to handle communications
/// between the monitor and client.
//
class IPCIO
{
public:
	/// @param peer_descriptor socket descriptor for communicating with peer
	//
	IPCIO(AutoDescriptor peer_descriptor, Timeout const & timeout);

	~IPCIO();

	enum EOFAction
	{
		E_THROW_ON_EOF, E_RETURN_FALSE_ON_EOF
	};

	enum EBuffering
	{
		E_BUFFERED, E_UNBUFFERED
	};

	/**
	* Reads specified amount of data from peer.
	*
	* @param buf Where to store characters read.
	* @param count How many characters to read.
	* @param eof_action What to do if 0 bytes read (EOF immediately encountered).
	* @param ebuffering If set to @c E_BUFFERED, then use buffered input.
	*
	* @return true if @a count bytes read; false if 0 bytes read (EOF) and
	* @a eof_action is @c E_RETURN_FALSE_ON_EOF.
	*
	* @pre If @a ebuffering != @c E_BUFFERED, there must be no data remaining in
	* the internal input buffer.
	*
	* @throw IPCIOException if I/O error occurred or fewer than @a count bytes
	* read (unless should return @c false).
	*/
	bool sgetn(
		char * buf, std::streamsize count,
		EOFAction eof_action = E_THROW_ON_EOF, EBuffering eb = E_BUFFERED
	);

	/**
	* Writes specified amount of data to peer.
	* @param buf Source of characters to write
	* @param count How many characters to write.
	* @throw IPCIOException on I/O error.
	*/
	void sputn(char const * buf, std::streamsize count);

	/**
	* Sends a @c FileHandle to the peer.
	* @param h The @c FileHandle to send.
	* @post output buffer flushed.
	* @throw IPCIOException on I/O error.
	*/
	void put_handle(FileHandle h);

	/**
	* Gets a @c FileHandle from the peer.
	*
	* @return The @c FileHandle sent by the peer.
	*
	* @pre There is no data in the internal input buffer.
	*
	* @throw IPCIOException on I/O error or if the peer does not send a
	* @c FileHandle or if there is data in the input buffer.
	*/
	AutoDescriptor get_handle();

	void close();

	/**
	* Finishes writing to peer any buffered write data.
	* @throw IOException
	*/
	void put_sync();

	/**
	* Checks that there is no data left in the input buffer, i.e., that the
	* entire message sent from the peer has been read.
	* @throw IPCIOException if the input buffer is nonempty.
	*/
	void get_sync();

private:
	IPCIO(IPCIO const &); // unimplemented
	void operator=(IPCIO const &); // unimplemented

	IntrusiveReference<PosixUnnamedPipe> m_pipe;
	Reference<IOIFCStreamBuffer> m_streambuf;
};

/**
* Writes a single value of type @a T to the peer.
* @pre @a T is a POD type containing no pointers.
* @throw IPCIOException if the write fails.
*/
template <typename T>
void ipcio_put(IPCIO & io, T const & val)
{
	io.sputn(reinterpret_cast<char const *>(&val), sizeof(val));
}

/**
* Reads from the peer a single value of type @a T and assigns it to @x.
* @pre @a T is a POD type.
* @return true if read succeeds, false if @c EOF immediately encountered and
* @a eof_action is @c E_RETURN_FALSE_ON_EOF.
* @throw IPCIOException if the read fails (unless should return false).
*/
template <typename T>
bool ipcio_get(
	IPCIO & io, T & x, IPCIO::EOFAction eof_action = IPCIO::E_THROW_ON_EOF,
	IPCIO::EBuffering eb = IPCIO::E_BUFFERED
)
{
	return io.sgetn(reinterpret_cast<char *>(&x), sizeof(x), eof_action, eb);
}

/**
* Writes a single @c String value to the peer.
* @throw IPCIOException if the write fails.
*/
void ipcio_put(IPCIO & io, String const & s);

/**
* Converts @a s to a @c String and writes it to the peer.  @c NULL is converted
* to the empty string.
* @throw IPCIOException if the write fails.
*/
void ipcio_put(IPCIO & io, char const * s);

/**
* Converts the first @a len characters of @a s to a @c String and writes it to
* the peer.
* @pre @a s != NULL.
* @throw IPCIOException
*/
void ipcio_put(IPCIO & io, char const * s, std::size_t len);

/**
* Reads from the peer a single @c String value and assigns it to @a s, truncating
* it if necessary so that the assigned value does not exceed a length of
* @a max_len.
*
* @return true if the read succeeds, false if @c EOF immediately encountered and
* @a eof_action is @c E_RETURN_FALSE_ON_EOF.
*
* @note Setting @a max_len to a reasonable value guards against attempts by a
* hostile (compromised) peer to cause the process to run out of memory.  You
* can check for truncation by using a @a max_len one larger than your desired
* maximum length, and checking the length of @a s on return.
*
* @throw IPCIOException if the read fails for any reason (unless should return
* false).
*/
bool ipcio_get(
	IPCIO & io, String & s, std::size_t max_len = std::size_t(-1),
	IPCIO::EOFAction eof_action = IPCIO::E_THROW_ON_EOF,
	IPCIO::EBuffering eb = IPCIO::E_BUFFERED
);

} // namespace OW_NAMESPACE

#endif
