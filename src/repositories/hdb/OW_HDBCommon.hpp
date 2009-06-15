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

#ifndef OW_HDBCOMMOM_HPP_INCLUDE_GUARD_
#define OW_HDBCOMMOM_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_Exception.hpp"
#include "OW_CIMProperty.hpp"
#include "blocxx/Array.hpp"

#include <cstring>

namespace OW_NAMESPACE
{

OW_DECLARE_APIEXCEPTION(HDB, OW_HDB_API)

#define OW_HDBSIGNATURE "OWHIERARCHICADB"
const int HDBSIGLEN = 16;

// This should generally be kept in sync with the binary serialization version in OW_BinarySerialization.hpp
// The general idea is to have it be a concatenation of release numbers with
// a revision on the end (to prevent problems during development)
// So 3000003 originated from version 3.0.0 rev 4
//
// 8/21/2003 - Changed from 3000003 to 3000004 because of a change in the 
//   structure of CIMInstance.  The name and alias were removed.  Also
//   changed the length of the signature to 16 so the header block could be
//   aligned easier (and slightly smaller).
// 10/25/2003 - 3000005. Changed enumClassNames to send over an enum of strings
//   instead of object paths.
// 2/6/2004 - 3000006. Changed CIMDateTime::{read,write}Object to write each
//   item individually so struct packing doesn't cause incompatibilities.
// 7/22/2004 - 3000008. Changed signatures and added versioning. Now all
//   readObject() calls will be able to read older versions as well as the
//   current.  Introduced MinBinaryProtocolVersion which is the oldest version
//   we can sucessfully read.
// 9/01/2005 - 4000000. Changed key format to use : instead of / to fix a bug.
// 10/12/2005 - 4000001. Fixed association and instance key format wrt associations.
// 4/17/2006 - 4000002. Added namespace field to CIMInstance.
// 4/04/2007 - 4000003. Added code to use flags in AssocDbRecHeader.
const UInt32 HDBVERSION = 4000003;

// This is the oldest version the code can handle.
const UInt32 MinHDBVERSION = 4000001;

/**
 * The HDBHeaderBlock structure represent the header information for
 * the database.
 */
struct OW_HDB_API HDBHeaderBlock
{
	char signature[HDBSIGLEN];
	UInt32 version;
	Int32 firstRoot;
	Int32 lastRoot;
	Int32 firstFree;
};
/**
 * The HDBBlock structure represents nodes within the database.
 */
struct OW_HDB_API HDBBlock
{
	HDBBlock() { memset(this, 0, sizeof(HDBBlock)); }
	UInt32 chkSum;		// The check sum of all following fields
	unsigned char isFree;	// Node is free block
	UInt32 size;				// The size of this block (used in free list)
	UInt32 flags;		// User defined flags
	Int32 nextSib;				// offset of next sibling node in the file
	Int32 prevSib;				// offset of prev sibling node in the file
	Int32 parent;				// offset of the parent node in the file
	Int32 firstChild;			// offset of the first child node for this node
	Int32 lastChild;			// offset of the last child node for this node
	UInt32 keyLength;			// length of the key component of the data
	UInt32 dataLength;		// length of data not including key
	// Data follows
	// The data starts with the key, which is a null terminated string.
	// The length of the non-key data will be dataLength - keyLength;
};
#define OW_HDBLKSZ sizeof(HDBBlock);

String HDBBlockDebugString(const HDBBlock& blk);

//////////////////////////////////////////////////////////////////////////////
class HDBUtilKeyArray
{
public:
	HDBUtilKeyArray(const CIMPropertyArray& props);
	void toString(StringBuffer& out);
private:
	CIMPropertyArray m_props;
};



} // end namespace OW_NAMESPACE

#endif
