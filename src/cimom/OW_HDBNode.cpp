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

#include "OW_config.h"
#include "OW_HDBNode.hpp"
#include "OW_HDB.hpp"
#include "OW_AutoPtr.hpp"

#include <cstring>

#define SZ(x)	sizeof((x))

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode::HDBNodeData::HDBNodeData() :
	m_blk(), m_key(), m_bfrLen(0), m_bfr(NULL), m_offset(-1L), m_version(0)
{
	::memset(&m_blk, 0, SZ(m_blk));
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode::HDBNodeData::HDBNodeData(const HDBNodeData& x) :
	m_blk(x.m_blk), m_key(x.m_key), m_bfrLen(x.m_bfrLen), m_bfr(NULL),
	m_offset(x.m_offset), m_version(0)
{
	if(m_bfrLen > 0)
	{
		m_bfr = new unsigned char[m_bfrLen];
		::memcpy(m_bfr, x.m_bfr, m_bfrLen);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode::HDBNodeData::~HDBNodeData()
{
	delete [] m_bfr;
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode::HDBNodeData& 
OW_HDBNode::HDBNodeData::operator= (const OW_HDBNode::HDBNodeData& x)
{
	m_blk = x.m_blk;
	m_key = x.m_key;
	m_version = x.m_version;
	delete [] m_bfr;
	m_bfr = NULL;
	m_bfrLen = x.m_bfrLen;
	if(m_bfrLen > 0)
	{
		m_bfr = new unsigned char[m_bfrLen];
		::memcpy(m_bfr, x.m_bfr, m_bfrLen);
	}
	m_offset = x.m_offset;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode::OW_HDBNode(const char* key, OW_HDBHandle& hdl) :
	m_pdata(NULL)
{
	if(!key || !hdl)
	{
		return;
	}

	OW_IndexEntry ientry = hdl.findIndexEntry(key);
	if(!ientry)
		return;

	read(ientry.offset, hdl);
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode::OW_HDBNode(long offset, OW_HDBHandle& hdl) :
	m_pdata(NULL)
{
	if(!hdl || offset <= 0)
	{
		return;
	}

	read(offset, hdl);
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode::OW_HDBNode(const OW_String& key, int dataLen, 
	const unsigned char* data) :
	m_pdata(NULL)
{
	if(key.length() == 0)
	{
		return;
	}

	m_pdata = new HDBNodeData;
	m_pdata->m_offset = -1;
	m_pdata->m_version = 0;
	m_pdata->m_key = key;

	if(dataLen && data != NULL)
	{
		m_pdata->m_bfr = new unsigned char[dataLen];
		::memcpy(m_pdata->m_bfr, data, dataLen);
	}
	else
	{
		dataLen = 0;
	}

	m_pdata->m_bfrLen = dataLen;
	m_pdata->m_blk.isFree = false;
	m_pdata->m_blk.keyLength = m_pdata->m_key.length()+1;
	m_pdata->m_blk.size = 0;
	m_pdata->m_blk.nextSib = -1;
	m_pdata->m_blk.prevSib = -1;
	m_pdata->m_blk.parent = -1;
	m_pdata->m_blk.firstChild = -1;
	m_pdata->m_blk.lastChild = -1;
	m_pdata->m_blk.dataLength = m_pdata->m_key.length() + 1 + dataLen;
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_HDBNode::read(long offset, OW_HDBHandle& hdl)
{
	if(offset <= 0 || !hdl)
	{
		OW_THROW(OW_HDBException, "Invalid offset to read node from");
	}

	OW_File file = hdl.getFile();
	OW_HDBBlock fblk;

	// Dereference existing data
	setNull();
	
	if(OW_HDB::readBlock(fblk, file, offset) != SZ(fblk))
	{
		OW_THROW(OW_HDBException, "Failed to read node header");
	}

	// If previously deleted, don't do anything
	if(fblk.isFree)
	{
		return;
	}

	char* kbfr = new char[fblk.keyLength];
	if(file.read(kbfr, fblk.keyLength) != size_t(fblk.keyLength))
	{
		delete [] kbfr;
		OW_THROW(OW_HDBException, "Failed to read key for node");
	}

	// Let OW_String take ownership of the allocated memory. It will
	// delete it later
	OW_String key(true, kbfr);

	unsigned char* bfr = NULL;
	int dataLen = fblk.dataLength - fblk.keyLength;
	if(dataLen > 0)
	{
		bfr = new unsigned char[dataLen];
		if(file.read(bfr, dataLen) != size_t(dataLen))
		{
			delete [] bfr;
			OW_THROW(OW_HDBException, "Failed to data for node");
		}
	}

	m_pdata = new HDBNodeData;
	m_pdata->m_offset = offset;
	m_pdata->m_bfr = bfr;
	m_pdata->m_bfrLen = dataLen;
	m_pdata->m_blk = fblk;
	m_pdata->m_key = key;
	m_pdata->m_version = hdl.getHDB()->getVersion();
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_HDBNode::reload(OW_HDBHandle& hdl)
{
	// If this is a null node, don't attempt to re-load
	if(m_pdata.isNull())
	{
		return false;
	}

	// If this node has never been written, don't attempt to re-load
	if(m_pdata->m_offset <= 0)
	{
		return true;
	}

	// If no writes were done to the database before the last
	// read, don't re-load
	if(hdl.getHDB()->getVersion() == m_pdata->m_version)
	{
		return true;
	}

	OW_File file = hdl.getFile();
	OW_HDBBlock fblk;
	long offset = m_pdata->m_offset;

	if(OW_HDB::readBlock(fblk, file, offset) != SZ(fblk))
	{
		setNull();
		return false;
	}

	// If previously deleted, don't do anything
	if(fblk.isFree)
	{
		setNull();
		return false;
	}

	OW_AutoPtr<char> kbfr(new char[fblk.keyLength]);
	if(file.read(kbfr.get(), fblk.keyLength) != size_t(fblk.keyLength))
	{
		return false;
	}

	if(!m_pdata->m_key.equals(kbfr.get()))
	{
		// If the key has changed, assume this node was deleted and the
		// space is now occupied by a different node.
		setNull();
		return false;
	}

	int dataLen = fblk.dataLength - fblk.keyLength;
	if(fblk.dataLength != m_pdata->m_blk.dataLength)
	{
		// Length of data for node has changed, so delete the old
		// data buffer
		delete [] m_pdata->m_bfr;
		m_pdata->m_bfr = NULL;
	}
	
	// If there is data for the node, then allocate the buffer for it
	// and read the data
	if(dataLen > 0)
	{
		if(m_pdata->m_bfr == NULL)
		{
			m_pdata->m_bfr = new unsigned char[dataLen];
		}

		if(file.read(m_pdata->m_bfr, dataLen) != size_t(dataLen))
		{
			setNull();		// this will delete m_pdata->m_bfr;
			return false;
		}
	}

	m_pdata->m_bfrLen = dataLen;
	m_pdata->m_blk = fblk;
	m_pdata->m_version = hdl.getHDB()->getVersion();
	return true;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool 
OW_HDBNode::turnFlagsOn(OW_HDBHandle& hdl, unsigned int flags)
{
	if(m_pdata.isNull())
		return false;

	OW_Bool cc = false;
	flags |= m_pdata->m_blk.flags;
	if(flags != m_pdata->m_blk.flags)
	{
		m_pdata->m_blk.flags = flags;
		if(m_pdata->m_offset > 0)
		{
			write(hdl);
		}
		cc = true;
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool 
OW_HDBNode::turnFlagsOff(OW_HDBHandle& hdl, unsigned int flags)
{
	if(m_pdata.isNull())
		return false;

	OW_Bool cc = false;
	flags = m_pdata->m_blk.flags & (~flags);
	if(flags != m_pdata->m_blk.flags)
	{
		m_pdata->m_blk.flags = flags;
		if(m_pdata->m_offset > 0)
		{
			write(hdl);
		}
		cc = true;
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool 
OW_HDBNode::updateData(OW_HDBHandle& hdl, int dataLen, unsigned char* data)
{
	OW_Bool cc = false;
	if(!m_pdata.isNull())
	{
		// If this node was read from file, make sure we are dealing with
		// an up to date version.
		if(m_pdata->m_offset > 0)
		{
			if(!reload(hdl))
			{
				// Node is no longer on file
				return false;
			}
		}

		cc = true;

		// If the data len has changed - we'll be using a different buffer
		if(dataLen != m_pdata->m_bfrLen)
		{
			delete [] m_pdata->m_bfr;
			m_pdata->m_bfr = NULL;
			m_pdata->m_bfrLen = 0;
		}

		if(dataLen > 0 && data != NULL)
		{
			if(m_pdata->m_bfr == NULL)
			{
				m_pdata->m_bfr = new unsigned char[dataLen];
			}
			m_pdata->m_bfrLen = dataLen;
			::memcpy(m_pdata->m_bfr, data, dataLen);
		}

		// If this node is already on file, then write changes to file
		if(m_pdata->m_offset > 0)
		{
			write(hdl);
		}
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
long
OW_HDBNode::write(OW_HDBHandle& hdl, OW_Bool onlyHeader)
{
	if(m_pdata.isNull())
	{
		OW_THROW(OW_HDBException, "Cannot write null node");
	}

	OW_Bool newRecord = false;
	m_pdata->m_blk.keyLength = m_pdata->m_key.length() + 1;
	m_pdata->m_blk.dataLength = m_pdata->m_bfrLen + m_pdata->m_key.length() + 1;

	OW_File file = hdl.getFile();
	OW_HDB* phdb = hdl.getHDB();
	int totalSize = m_pdata->m_blk.dataLength + SZ(m_pdata->m_blk);

	m_pdata->m_blk.isFree = false;
	if(m_pdata->m_offset <= 0)	// Is this a new node?
	{
		newRecord = true;
		m_pdata->m_blk.size = totalSize;
		m_pdata->m_offset = phdb->findBlock(file, totalSize);

		if(m_pdata->m_blk.parent <= 0)
		{
			phdb->addRootNode(file, m_pdata->m_blk, m_pdata->m_offset);
		}
		else
		{
			if(OW_HDB::writeBlock(m_pdata->m_blk, file, m_pdata->m_offset)
				!= SZ(m_pdata->m_blk))
			{
				OW_THROW(OW_HDBException, "Failed to write node header");
			}
		}

		// Add this entry to the index
		if (!hdl.addIndexEntry(m_pdata->m_key.c_str(), m_pdata->m_offset))
		{
			OW_THROW(OW_HDBException, "Failed to write index entry");
		}
	}
	else
	{
		// If the size required to hold this node has increased,
		// then delete the old block and add a new one.
		if(totalSize > int(m_pdata->m_blk.size))
		{
			OW_HDBBlock node = m_pdata->m_blk;
			phdb->addBlockToFreeList(file, node, m_pdata->m_offset); // delete old
			m_pdata->m_blk.size = totalSize;
			updateOffsets(hdl, phdb->findBlock(file, totalSize));
		}

		if(OW_HDB::writeBlock(m_pdata->m_blk, file, m_pdata->m_offset)
			!= SZ(m_pdata->m_blk))
		{
			OW_THROW(OW_HDBException, "Failed to write node header");
		}
	}

	if(onlyHeader == false || newRecord == true)
	{
		// Now write key and data for node
		if(file.write(m_pdata->m_key.c_str(), m_pdata->m_key.length()+1)
			!= m_pdata->m_key.length()+1)
		{
			OW_THROW(OW_HDBException, "Failed to write node key");
		}

		if(file.write(m_pdata->m_bfr, m_pdata->m_bfrLen) 
			!= size_t(m_pdata->m_bfrLen))
		{
			OW_THROW(OW_HDBException, "Failed to write node data");
		}
	}

	m_pdata->m_version = hdl.registerWrite();
	return m_pdata->m_offset;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDBNode::updateOffsets(OW_HDBHandle& hdl, long offset)
{
	if(offset <= 0L || m_pdata.isNull() || !hdl)
	{
		return;
	}

	OW_HDB* pdb = hdl.getHDB();
	OW_File file = hdl.getFile();
	OW_HDBBlock fblk;
	if(m_pdata->m_blk.prevSib > 0)
	{
		if(OW_HDB::readBlock(fblk, file, m_pdata->m_blk.prevSib) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to read block for offset update");
		}
		fblk.nextSib = offset;
		if(OW_HDB::writeBlock(fblk, file, m_pdata->m_blk.prevSib) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to write block for offset update");
		}
	}

	if(m_pdata->m_blk.nextSib > 0)
	{
		if(OW_HDB::readBlock(fblk, file, m_pdata->m_blk.nextSib) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to read block for offset update");
		}
		fblk.prevSib = offset;
		if(OW_HDB::writeBlock(fblk, file, m_pdata->m_blk.nextSib) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to write block for offset update");
		}
	}

	if(m_pdata->m_blk.parent > 0)
	{
		if(OW_HDB::readBlock(fblk, file, m_pdata->m_blk.parent) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to read block for offset update");
		}

		OW_Bool doUpdate = false;
		if(fblk.firstChild == m_pdata->m_offset)
		{
			fblk.firstChild = offset;
			doUpdate = true;
		}

		if(fblk.lastChild == m_pdata->m_offset)
		{
			fblk.lastChild = offset;
			doUpdate = true;
		}

		if(doUpdate)
		{
			if(OW_HDB::writeBlock(fblk, file, m_pdata->m_blk.parent) != SZ(fblk))
			{
				OW_THROW(OW_HDBException, 
					"Failed to write block for offset update");
			}
		}
	}
	else	// No parent. Must be a root node
	{
		if(pdb->getFirstRootOffSet() == m_pdata->m_offset)
		{
			pdb->setFirstRootOffSet(file, offset);
		}
		if(pdb->getLastRootOffset() == m_pdata->m_offset)
		{
			pdb->setLastRootOffset(file, offset);
		}
	}

	// Now update the parent offset on all children
	long coffset = m_pdata->m_blk.firstChild;
	while(coffset > 0)
	{
		if(OW_HDB::readBlock(fblk, file, coffset) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to read block for offset update");
		}
		fblk.parent = offset;

		if(OW_HDB::writeBlock(fblk, file, coffset) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to write block for offset update");
		}
		coffset = fblk.nextSib;
	}

	// Update the index entry associated with this node
	hdl.updateIndexEntry(m_pdata->m_key.c_str(), offset);

	m_pdata->m_offset = offset;
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_HDBNode::addChild(OW_HDBHandle& hdl, OW_HDBNode& arg)
{
	if(!arg)
	{
		return;
	}

	// If this node has never been written, then write it before
	// we create a child. If we don't do this, we don't have
	// a parent offset to put in the child node
	if(m_pdata->m_offset <= 0)
	{
		write(hdl);
	}

	// If this node has already been written to the file, and it is the
	// child of another parent, throw and exception.
	// If it is already a child of this node, then just write it out
	// and return.
	if(arg.m_pdata->m_offset > 0)
	{
		// If this node is already the parent, then just
		// write the child node and return
		if(arg.m_pdata->m_blk.parent == m_pdata->m_offset)
		{
			arg.write(hdl);
			return;
		}

		OW_THROW(OW_HDBException, "Child node already has a parent");
	}

	arg.m_pdata->m_blk.prevSib = m_pdata->m_blk.lastChild;
	arg.m_pdata->m_blk.nextSib = -1;
	arg.m_pdata->m_blk.parent = m_pdata->m_offset;

	long newNodeOffset = arg.write(hdl);
	OW_File file = hdl.getFile();

	// If this node already had children, then update the last node in the
	// child list to point to the new node.
	if(m_pdata->m_blk.lastChild > 0)
	{
		OW_HDBBlock node;
		if(OW_HDB::readBlock(node, file, m_pdata->m_blk.lastChild) != SZ(node))
		{
			OW_THROW(OW_HDBException, "Failed to read prev sibling from file");
		}

		// Update next sibling pointer on node
		node.nextSib = newNodeOffset;	

		// Write prev sibling node
		if(OW_HDB::writeBlock(node, file, m_pdata->m_blk.lastChild) != SZ(node))
		{
			OW_THROW(OW_HDBException, "Failed to write node header");
		}
	}

	m_pdata->m_blk.lastChild = arg.m_pdata->m_offset;
	if(m_pdata->m_blk.firstChild <= 0)
	{
		m_pdata->m_blk.firstChild = m_pdata->m_blk.lastChild;
	}

	write(hdl);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_HDBNode::remove(OW_HDBHandle& hdl)
{
	// If node hasn't been written yet, don't do anything.
	if(m_pdata->m_offset <= 0)
	{
		return false;
	}

	OW_File file = hdl.getFile();
	OW_HDB* pdb = hdl.getHDB();
	OW_HDBBlock fblk;

	// Remove all children
	long coffset = m_pdata->m_blk.lastChild;
	long toffset;
	while(coffset > 0)
	{
		if(OW_HDB::readBlock(fblk, file, coffset) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to read child block for removal");
		}

		toffset = coffset;			// Save offset for deletion class
		coffset = fblk.prevSib;		// Save offset for next read

		// Remove node and all of its children. This call will modify fblk.
		removeBlock(hdl, fblk, toffset);
	}

	// Set pointer on next sibling if it exists
	if(m_pdata->m_blk.nextSib > 0)
	{
		if(OW_HDB::readBlock(fblk, file, m_pdata->m_blk.nextSib) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to read sibling to remove node");
		}

		fblk.prevSib = m_pdata->m_blk.prevSib;
		if(OW_HDB::writeBlock(fblk, file, m_pdata->m_blk.nextSib) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to write sibling to remove node");
		}
	}

	// Set pointer on prev sibling if it exists
	if(m_pdata->m_blk.prevSib > 0)
	{
		if(OW_HDB::readBlock(fblk, file, m_pdata->m_blk.prevSib) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to read sibling to remove node");
		}

		fblk.nextSib = m_pdata->m_blk.nextSib;
		if(OW_HDB::writeBlock(fblk, file, m_pdata->m_blk.prevSib) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to write sibling to remove node");
		}
	}

	// If it has a parent, insure parent doesn't contain it's offset
	if(m_pdata->m_blk.parent > 0)
	{
		// Read parent block
		if(OW_HDB::readBlock(fblk, file, m_pdata->m_blk.parent) != SZ(fblk))
		{
			OW_THROW(OW_HDBException, "Failed to read parent to remove node");
		}

		OW_Bool changed = false;

		// If this was the first child in the list, then update the 
		// first child pointer in the parent block
		if(fblk.firstChild == m_pdata->m_offset)
		{
			changed = true;
			fblk.firstChild = m_pdata->m_blk.nextSib;
		}

		// If this was the last child in the list, then update the 
		// last child pointer in the parent block
		if(fblk.lastChild == m_pdata->m_offset)
		{
			changed = true;
			fblk.lastChild = m_pdata->m_blk.prevSib;
		}

		// If any offsets changed in the parent block, then update the parent
		if(changed)
		{
			if(OW_HDB::writeBlock(fblk, file, m_pdata->m_blk.parent) != SZ(fblk))
			{
				OW_THROW(OW_HDBException, "Failed to write parent to remove node");
			}
		}
	}
	else
	{
		// Must be a root node
		if(pdb->getFirstRootOffSet() == m_pdata->m_offset)
		{
			pdb->setFirstRootOffSet(file, m_pdata->m_blk.nextSib);
		}

		if(pdb->getLastRootOffset() == m_pdata->m_offset)
		{
			pdb->setLastRootOffset(file, m_pdata->m_blk.prevSib);
		}
	}

	// Add block to free list
	pdb->addBlockToFreeList(file, m_pdata->m_blk, m_pdata->m_offset);

	// Remove the index entry for this node
	hdl.removeIndexEntry(m_pdata->m_key.c_str());

	m_pdata->m_offset = -1;
	m_pdata->m_blk.isFree = true;
	m_pdata->m_blk.parent = -1;
	m_pdata->m_blk.firstChild = -1;
	m_pdata->m_blk.lastChild = -1;
	m_pdata->m_blk.prevSib = -1;
	m_pdata->m_blk.nextSib = -1;
	m_pdata->m_blk.size = 0;
	hdl.registerWrite();
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// removeBlock is used to add a OW_HDBBlock and all of it's children to the
// DB's deletion list. The next sibling and previous sibling fields are
// ignored. The intention is for this method to be called from the 
// OW_HFileNodeImpl remove method only! This method is recursive, so all stack
// optimizations are welcome.
#define LMAX(a, b)	((int(a) > int(b)) ? (a) : (b))
void
OW_HDBNode::removeBlock(OW_HDBHandle& hdl, OW_HDBBlock& fblk, long offset)
{
	OW_AutoPtr<unsigned char> 
		pbfr(new unsigned char[LMAX(SZ(fblk), fblk.keyLength)]);

	OW_File file = hdl.getFile();
	long coffset = fblk.lastChild;
	if(coffset > 0)
	{
		while(coffset > 0)
		{
			if(OW_HDB::readBlock(*((OW_HDBBlock*)pbfr.get()), file, coffset) 
				!= SZ(fblk))
			{
				OW_THROW(OW_HDBException, "Failed to read block for removal");
			}

			// Save current offset for call to removeBlock
			long toffset = coffset;

			// Get pointer to previous sibling 
			coffset = ((OW_HDBBlock*)pbfr.get())->prevSib;

			// Recursive call to removeBlock. *pblk.get() will be modified.
			removeBlock(hdl, *((OW_HDBBlock*)pbfr.get()), toffset);
		}
	}

	// Need to read the key for this block to update the index
	if(file.read(pbfr.get(), fblk.keyLength, offset + SZ(fblk)) 
		!= size_t(fblk.keyLength))
	{
		OW_THROW(OW_HDBException, "Failed to read node's key for removal");
	}

	// Remove this node's key from the index
	hdl.removeIndexEntry((const char*)pbfr.get());

	// Add the block to the free list
	hdl.getHDB()->addBlockToFreeList(file, fblk, offset);
}

