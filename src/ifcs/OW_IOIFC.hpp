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
#ifndef __OW_IOIFC_HPP__
#define __OW_IOIFC_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Bool.hpp"

class OW_IOIFC
{
public:

	virtual ~OW_IOIFC() {}

	/**
	 * Read a specified number of bytes from the device that is exposing the
	 * OW_IOIFC interface.
	 *
	 * @param dataIn A pointer to a location in memory to put the bytes that
	 *		have been read.
	 * @param dataInLen The number of bytes being requested from the device.
	 * @param errorAsException If true and an error occurs durring the read
	 *		operation, then throw an exception.
	 * @return The number of bytes actually read from the device, or -1 on
	 *		error.
	 */
	virtual int read(void* dataIn, int dataInLen,
			OW_Bool errorAsException=false) = 0;
			
	/**
	 * Write a specified number of bytes to the device that is exposing the
	 * OW_IOIFC interface.
	 *
	 * @param dataOut A pointer to a location in memory that contains the bytes
	 *		that will be written to the device.
	 * @param dataOutLen The length of the data pointed to by the dataOut parm.
	 * @param errorAsException If true and an error occurs durring the write
	 *		operation, then throw an exception.
	 * @return The number of bytes actually written to the device. or -1 on
	 *		error
	 */
	virtual int write(const void* dataOut, int dataOutLen,
			OW_Bool errorAsException=false) = 0;
};

#endif	// __OW_IOIFC_HPP__

