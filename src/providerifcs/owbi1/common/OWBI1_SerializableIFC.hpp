/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

#ifndef OWBI1_SERIALIZABLEIFC_HPP_INCLUDE_GUARD_
#define OWBI1_SERIALIZABLEIFC_HPP_INCLUDE_GUARD_
#include "OWBI1_config.h"
#include "OWBI1_CommonFwd.hpp"
#include <iosfwd>

namespace OWBI1
{

class OWBI1_OWBI1PROVIFC_API SerializableIFC
{
public:
	virtual ~SerializableIFC();
	/**
	 * Read this object from an input stream. The object must have been
	 * previously written through a call to writeObject.
	 *
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream& istrm) = 0;
	/**
	 * Write this object to an output stream. The intent is for the object to
	 * be retrieved later through a call to readObject.
	 *
	 * @param ostrm	The output stream to write the object to.
	 */
	virtual void writeObject(std::ostream& ostrm) const = 0;
};

} // end namespace OWBI1

#endif


