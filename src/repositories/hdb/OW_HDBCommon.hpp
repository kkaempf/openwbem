/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_HDBCOMMOM_HPP_INCLUDE_GUARD_
#define OW_HDBCOMMOM_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Exception.hpp"

#include <cstring>

class OW_HDBException : public OW_Exception
{
public:

	OW_HDBException() : OW_Exception() {}
	OW_HDBException(const char* file, int line, const char* msg) :
		OW_Exception(file, line, msg) {}
	OW_HDBException(const char* msg) : OW_Exception(msg) {}
	OW_HDBException(const OW_HDBException& e) : OW_Exception(e) {}
		
	virtual const char* type() const { return "OW_HDBException"; }
};

#define OW_HDBSIGNATURE "OWHIERARCHICADB"
const int OW_HDBSIGLEN = 16;


// The general idea is to have it be a concatenation of release numbers with
// a revision on the end (to prevent problems during development)
// So 3000003 originated from version 3.0.0 rev 4
//
// 8/21/2003 - Changed from 3000003 to 3000004 because of a change in the 
//   structure of OW_CIMInstance.  The name and alias were removed.  Also
//   changed the length of the signature to 16 so the header block could be
//   aligned easier (and slightly smaller).
// 10/25/2003 - 3000005. Changed enumClassNames to send over an enum of strings
//   instead of object paths.
const OW_UInt32 OW_HDBVERSION = 3000005;

/**
 * The OW_HDBHeaderBlock structure represent the header information for
 * the database.
 */
struct OW_HDBHeaderBlock
{
	char signature[OW_HDBSIGLEN];
	OW_UInt32 version;
	OW_Int32 firstRoot;
	OW_Int32 lastRoot;
	OW_Int32 firstFree;
};

/**
 * The OW_HDBBlock structure represents nodes within the database.
 */
struct OW_HDBBlock
{
	OW_HDBBlock() { memset(this, 0, sizeof(OW_HDBBlock)); }

	OW_UInt32 chkSum;		// The check sum of all following fields
	unsigned char isFree;	// Node is free block
	OW_UInt32 size;				// The size of this block (used in free list)
	OW_UInt32 flags;		// User defined flags
	OW_Int32 nextSib;				// offset of next sibling node in the file
	OW_Int32 prevSib;				// offset of prev sibling node in the file
	OW_Int32 parent;				// offset of the parent node in the file
	OW_Int32 firstChild;			// offset of the first child node for this node
	OW_Int32 lastChild;			// offset of the last child node for this node
	OW_UInt32 keyLength;			// length of the key component of the data
	OW_UInt32 dataLength;		// length of data not including key
	// Data follows
	// The data starts with the key, which is a null terminated string.
	// The length of the non-key data will be dataLength - keyLength;
};

#define OW_HDBLKSZ sizeof(HDBBlock);

#endif


