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

#include "OW_config.h"
#include "OW_HDB.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_Format.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

#include <cstring>

static OW_UInt32 calcCheckSum(unsigned char* src, OW_Int32 len);

//////////////////////////////////////////////////////////////////////////////
OW_HDB::OW_HDB() :
	m_hdrBlock(), m_fileName(), m_version(0), m_hdlCount(0),
	m_opened(false), m_pindex(NULL), m_indexGuard(), m_guard()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_HDB::~OW_HDB()
{
	try
	{
		if(m_hdlCount > 0)
		{
			// cerr << "*** OW_HDB::~OW_HDB - STILL OUTSTANDING HANDLES ***" << endl;
		}
		close();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDB::close()
{
	if(m_opened)
	{
		m_pindex->close();
		m_pindex = 0;
		m_opened = false;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDB::open(const char* fileName)
{
	OW_MutexLock l(m_guard);
	if(m_opened)
	{
		return;
	}

	m_hdlCount = 0;
	m_version = 0;
	m_fileName = fileName;

	OW_String fname = m_fileName + ".dat";

	createFile();
	if(!checkFile())
	{
		OW_String msg("Failed to open file: ");
		msg += fname;
		OW_THROW(OW_HDBException, msg.c_str());
	}
	m_fileName = fname;
	m_opened = true;
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDB::createFile()
{
	OW_HDBHeaderBlock b = { OW_HDBSIGNATURE, OW_HDBVERSION, -1L, -1L, -1L };
	m_hdrBlock = b;

	OW_File f = OW_FileSystem::createFile(m_fileName + ".dat");
	if(!f)
	{
		return false;
	}

	if(f.write(&m_hdrBlock, sizeof(m_hdrBlock), 0) != sizeof(m_hdrBlock))
	{
		f.close();
		OW_THROW(OW_HDBException, "Failed to write header of HDB");
	}

	f.close();
	m_pindex = OW_Index::createIndexObject();
	m_pindex->open(m_fileName.c_str());
	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDB::checkFile()
{
	OW_File f = OW_FileSystem::openFile(m_fileName + ".dat");
	if(!f)
	{
		OW_String msg("Failed to open file: ");
		msg += m_fileName;
		OW_THROW(OW_HDBException, msg.c_str());
	}

	if(f.read(&m_hdrBlock, sizeof(m_hdrBlock), 0) != sizeof(m_hdrBlock))
	{
		f.close();
		OW_String msg("Failed to read HDB header from file: ");
		msg += m_fileName;
		OW_THROW(OW_HDBException, msg.c_str());
	}
	f.close();

	if(::strncmp(m_hdrBlock.signature, OW_HDBSIGNATURE, OW_HDBSIGLEN))
	{
		OW_String msg("Invalid format for HDB file: ");
		msg += m_fileName;
		OW_THROW(OW_HDBException, msg.c_str());
	}

	if (m_hdrBlock.version != OW_HDBVERSION)
	{
		OW_THROW(OW_HDBException, format("Invalid version (%1) for file (%2). Expected (%3)", m_hdrBlock.version, m_fileName, OW_HDBVERSION).c_str());
	}

	m_pindex = OW_Index::createIndexObject();
	m_pindex->open(m_fileName.c_str());
	return true;
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_HDB::incVersion()
{
	OW_MutexLock l(m_guard);
	m_version++;
	return m_version;
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBHandle
OW_HDB::getHandle()
{
	OW_MutexLock l(m_guard);
	if(!m_opened)
	{
		OW_THROW(OW_HDBException, "Can't get handle from closed OW_HDB");
	}

	OW_File file = OW_FileSystem::openFile(m_fileName);
	if(!file)
	{
		return OW_HDBHandle();
	}

	m_hdlCount++;
	return OW_HDBHandle(this, file);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDB::decHandleCount()
{
	OW_MutexLock l(m_guard);
	m_hdlCount--;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDB::setOffsets(OW_File file, OW_Int32 firstRootOffset, OW_Int32 lastRootOffset,
	OW_Int32 firstFreeOffset)
{
	OW_MutexLock l(m_guard);
	m_hdrBlock.firstRoot = firstRootOffset;
	m_hdrBlock.lastRoot = lastRootOffset;
	m_hdrBlock.firstFree = firstFreeOffset;
	if(file.write(&m_hdrBlock, sizeof(m_hdrBlock), 0) != sizeof(m_hdrBlock))
	{
		OW_THROW(OW_HDBException, "Failed to update offset on HDB");
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDB::setFirstRootOffSet(OW_File file, OW_Int32 offset)
{
	setOffsets(file, offset, m_hdrBlock.lastRoot, m_hdrBlock.firstFree);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDB::setLastRootOffset(OW_File file, OW_Int32 offset)
{
	setOffsets(file, m_hdrBlock.firstRoot, offset, m_hdrBlock.firstFree);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDB::setFirstFreeOffSet(OW_File file, OW_Int32 offset)
{
	setOffsets(file, m_hdrBlock.firstRoot, m_hdrBlock.lastRoot, offset);
}

//////////////////////////////////////////////////////////////////////////////
// Find a block in the free list that is large enough to hold the given
// size. If no block in the free list is large enough or the free list
// is empty, then the offset to the end of the file is returned
OW_Int32
OW_HDB::findBlock(OW_File file, OW_Int32 size)
{
	OW_MutexLock l(m_guard);
	OW_Int32 offset = -1;
	OW_HDBBlock fblk;

	// If the free list is not empty, then search it for a block
	// big enough to hold the given size
	if(m_hdrBlock.firstFree != -1)
	{
		OW_Int32 coffset = m_hdrBlock.firstFree;
		while(true)
		{
			readBlock(fblk, file, coffset);

			// If the current block size is greater than or equal to the
			// size being requested, then we found a block in the file
			// we can use.
			if(fblk.size >= static_cast<OW_UInt32>(size))
			{
				offset = coffset;
				break;
			}

			if((coffset = fblk.nextSib) == -1L)
			{
				break;
			}
		}
	}

	// If offset is no longer -1, then we must have found a block
	// of adequate size.
	if(offset != -1)
	{
		// Remove the block from the free list
		removeBlockFromFreeList(file, fblk);
	}
	else
	{
		// We didn't find a block that was big enough, so let's just allocate
		// a chunk at the end of the file.
		if(file.seek(0L, SEEK_END) == -1L)
		{
			OW_THROW(OW_HDBException, "Failed to seek to end of file");
		}

		if((offset = file.tell()) == -1L)
		{
			OW_THROW(OW_HDBException, "Failed to get offset in file");
		}
	}

	return offset;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDB::removeBlockFromFreeList(OW_File file, OW_HDBBlock& fblk)
{
	OW_MutexLock l(m_guard);
	OW_HDBBlock cblk;

	// If block has a next sibling, then set it's previous sibling pointer
	// to the given blocks previous pointer
	if(fblk.nextSib != -1)
	{
		readBlock(cblk, file, fblk.nextSib);

		cblk.prevSib = fblk.prevSib;
		writeBlock(cblk, file, fblk.nextSib);
	}

	// If block has a previous sibling, then set it's next sibling pointer
	// to the given blocks next pointer
	if(fblk.prevSib != -1)
	{
		readBlock(cblk, file, fblk.prevSib);

		cblk.nextSib = fblk.nextSib;
		writeBlock(cblk, file, fblk.prevSib);
	}
	else		// Block must be the 1st one in the free list
	{
		// If no previous sibling, assume this was the 1st in the
		// free list, so set the head pointer
		if(m_hdrBlock.firstFree != -1)
		{
			setFirstFreeOffSet(file, fblk.nextSib);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Add a block to the free list.
void
OW_HDB::addBlockToFreeList(OW_File file, const OW_HDBBlock& parmblk,
	OW_Int32 offset)
{
	OW_MutexLock l(m_guard);
	OW_HDBBlock fblk = parmblk;
	fblk.isFree = true;

	// If the free list is empty, set the free list head pointer only
	if(m_hdrBlock.firstFree == -1)
	{
		fblk.nextSib = -1;
		fblk.prevSib = -1;

		writeBlock(fblk, file, offset);

		setFirstFreeOffSet(file, offset);
		return;
	}

	OW_HDBBlock cblk;
	cblk.size = 0;
	OW_Int32 coffset = m_hdrBlock.firstFree;
	OW_Int32 loffset = 0;

	// Find insertion point in free list
	while(coffset != -1)
	{
		loffset = coffset;
		readBlock(cblk, file, coffset);

		if(fblk.size <= cblk.size)
		{
			break;
		}

		coffset = cblk.nextSib;
	}

	if(coffset == -1)		// Append to end of free list?
	{
		cblk.nextSib = offset;

		writeBlock(cblk, file, loffset);

		fblk.prevSib = loffset;
		fblk.nextSib = -1;

		writeBlock(fblk, file, offset);
	}
	else						// Insert before last node read
	{
		if(cblk.prevSib == -1)			// If this Was the 1st on the list
		{											// Set the free list head pointer
			setFirstFreeOffSet(file, offset);		
		}
		else
		{
			// Read the previous node from last read to set it's next
			// sibling pointer
			OW_HDBBlock tblk;
			readBlock(tblk, file, cblk.prevSib);

			tblk.nextSib = offset;
			writeBlock(tblk, file, cblk.prevSib);
		}

		fblk.nextSib = coffset;
		fblk.prevSib = cblk.prevSib;
		writeBlock(fblk, file, offset);

		// Set the prev sib pointer in last read to the node being added.
		cblk.prevSib = offset;
		writeBlock(cblk, file, coffset);
	}
}

//////////////////////////////////////////////////////////////////////////////
// addRootNode will insert the given node into the files root node list.
// Upon return the file pointer should be positioned immediately after
// the given node in the file.
void
OW_HDB::addRootNode(OW_File file, OW_HDBBlock& fblk, OW_Int32 offset)
{
	OW_MutexLock l(m_guard);
	fblk.parent = -1;
	fblk.nextSib = -1;

	if(m_hdrBlock.firstRoot == -1)
	{
		setOffsets(file, offset, offset, m_hdrBlock.firstFree);
		fblk.prevSib = -1;
	}
	else
	{
		fblk.prevSib = m_hdrBlock.lastRoot;

		OW_HDBBlock cblk;
		readBlock(cblk, file, m_hdrBlock.lastRoot);

		cblk.nextSib = offset;
		writeBlock(cblk, file, m_hdrBlock.lastRoot);

		setLastRootOffset(file, offset);
	}

	writeBlock(fblk, file, offset);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_HDB::writeBlock(OW_HDBBlock& fblk, OW_File file, OW_Int32 offset)
{
	fblk.chkSum = 0;
	OW_UInt32 chkSum = calcCheckSum(reinterpret_cast<unsigned char*>(&fblk), sizeof(fblk));
	fblk.chkSum = chkSum;

	int cc = file.write(&fblk, sizeof(fblk), offset);
	if(cc != sizeof(fblk))
	{
		OW_THROW(OW_HDBException, "Failed to write block");
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC	
void
OW_HDB::readBlock(OW_HDBBlock& fblk, OW_File file, OW_Int32 offset)
{
	int cc = file.read(&fblk, sizeof(fblk), offset);
	if(cc != sizeof(fblk))
	{
		OW_THROW(OW_HDBException, "Failed to read block");
	}

	OW_UInt32 chkSum = fblk.chkSum;
	fblk.chkSum = 0;
	fblk.chkSum = calcCheckSum(reinterpret_cast<unsigned char*>(&fblk), sizeof(fblk));
	if(chkSum != fblk.chkSum)
	{
		OW_THROW(OW_HDBException, "CORRUPT DATA? Invalid check sum in node");
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_IndexEntry
OW_HDB::findFirstIndexEntry(const char* key)
{
	if(!m_opened)
	{
		OW_THROW(OW_HDBException, "HDB is not opened");
	}
	OW_MutexLock il(m_indexGuard);
	return m_pindex->findFirst(key);
}

//////////////////////////////////////////////////////////////////////////////
OW_IndexEntry
OW_HDB::findNextIndexEntry()
{
	if(!m_opened)
	{
		OW_THROW(OW_HDBException, "HDB is not opened");
	}

	OW_MutexLock il(m_indexGuard);
	return m_pindex->findNext();
}

//////////////////////////////////////////////////////////////////////////////
OW_IndexEntry
OW_HDB::findPrevIndexEntry()
{
	if(!m_opened)
	{
		OW_THROW(OW_HDBException, "HDB is not opened");
	}

	OW_MutexLock il(m_indexGuard);
	return m_pindex->findPrev();
}

//////////////////////////////////////////////////////////////////////////////
OW_IndexEntry
OW_HDB::findIndexEntry(const char* key)
{
	if(!m_opened)
	{
		OW_THROW(OW_HDBException, "HDB is not opened");
	}

	OW_MutexLock il(m_indexGuard);
	return m_pindex->find(key);
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDB::addIndexEntry(const char* key, OW_Int32 offset)
{
	if(!m_opened)
	{
		OW_THROW(OW_HDBException, "HDB is not opened");
	}

	OW_MutexLock il(m_indexGuard);
	return m_pindex->add(key, offset);
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDB::removeIndexEntry(const char* key)
{
	if(!m_opened)
	{
		OW_THROW(OW_HDBException, "HDB is not opened");
	}
	OW_MutexLock il(m_indexGuard);
	return m_pindex->remove(key);
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDB::updateIndexEntry(const char* key, OW_Int32 newOffset)
{
	if(!m_opened)
	{
		OW_THROW(OW_HDBException, "HDB is not opened");
	}
	OW_MutexLock il(m_indexGuard);
	return m_pindex->update(key, newOffset);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDB::flushIndex()
{
	if(m_opened)
	{
		OW_MutexLock il(m_indexGuard);
		m_pindex->flush();
	}
}

//////////////////////////////////////////////////////////////////////////////
static OW_UInt32
calcCheckSum(unsigned char* src, OW_Int32 len)
{
	OW_UInt32 cksum = 0;
	OW_Int32 i;

	for(i = 0; i < len; i++)
	{
		cksum += src[i];
	}

	return cksum;
}

//////////////////////////////////////////////////////////////////////////////
// ROUTINES FOR OW_HDBHandle class
OW_HDBHandle::OW_HDBHandleData::~OW_HDBHandleData()
{
	try
	{
		m_file.close();
		m_pdb->decHandleCount();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBHandle::OW_HDBHandle() :
	m_pdata(NULL)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBHandle::OW_HDBHandle(OW_HDB* pdb, OW_File file) :
	m_pdata(new OW_HDBHandleData(pdb, file))
{
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_HDBHandle::registerWrite()
{
	m_pdata->m_writeDone = true;
	return m_pdata->m_pdb->incVersion();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDBHandle::flush()
{
	if(m_pdata->m_writeDone)
	{
		m_pdata->m_pdb->flushIndex();
		m_pdata->m_file.flush();
		m_pdata->m_writeDone = false;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_HDBHandle::getFirstRoot()
{
	if(m_pdata->m_pdb->getFirstRootOffSet() > 0)
	{
		return OW_HDBNode(m_pdata->m_pdb->getFirstRootOffSet(), *this);
	}
	return OW_HDBNode();
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_HDBHandle::getNode(const OW_String& key)
{
	if(!key.empty())
	{
		return OW_HDBNode(key.c_str(), *this);
	}
	return OW_HDBNode();
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_HDBHandle::getParent(OW_HDBNode& node)
{
	if(node)
	{
		if(node.reload(*this))
		{
			if(node.getParentOffset() > 0)
			{
				return OW_HDBNode(node.getParentOffset(), *this);
			}
		}
	}

	return OW_HDBNode();
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_HDBHandle::getFirstChild(OW_HDBNode& node)
{
	if(node)
	{
		if(node.reload(*this))
		{
			if(node.getFirstChildOffset() > 0)
			{
				return OW_HDBNode(node.getFirstChildOffset(), *this);
			}
		}
	}

	return OW_HDBNode();
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_HDBHandle::getLastChild(OW_HDBNode& node)
{
	if(node)
	{
		if(node.reload(*this))
		{
			if(node.getLastChildOffset() > 0)
			{
				return OW_HDBNode(node.getLastChildOffset(), *this);
			}
		}
	}

	return OW_HDBNode();
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_HDBHandle::getNextSibling(OW_HDBNode& node)
{
	if(node)
	{
		if(node.reload(*this))
		{
			if(node.getNextSiblingOffset() > 0)
			{
				return OW_HDBNode(node.getNextSiblingOffset(), *this);
			}
		}
	}

	return OW_HDBNode();
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_HDBHandle::getPrevSibling(OW_HDBNode& node)
{
	if(node)
	{
		if(node.reload(*this))
		{
			if(node.getPrevSiblingOffset() > 0)
			{
				return OW_HDBNode(node.getPrevSiblingOffset(), *this);
			}
		}
	}

	return OW_HDBNode();
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDBHandle::addRootNode(OW_HDBNode& node)
{
	bool cc = false;
	if(node)
	{
		if(node.getOffset() > 0)
		{
			OW_THROW(OW_HDBException, "node is already on file");
		}

		if(m_pdata->m_pdb->findIndexEntry(node.getKey().c_str()))
		{
			OW_THROW(OW_HDBException, "key for node is already in index");
		}

		node.write(*this);
		cc = true;
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDBHandle::addChild(OW_HDBNode& parentNode, OW_HDBNode& childNode)
{
	bool cc = false;
	if(parentNode && childNode)
	{
		if(childNode.getOffset() > 0)
		{
			OW_THROW(OW_HDBException, "child node already has a parent");
		}

		if(parentNode.getOffset() <= 0)
		{
			OW_THROW(OW_HDBException, "parent node is not on file");
		}

		if(m_pdata->m_pdb->findIndexEntry(childNode.getKey().c_str()))
		{
			OW_THROW(OW_HDBException, "key for node is already in index");
		}

		if(parentNode.reload(*this))
		{
			parentNode.addChild(*this, childNode);
			cc = true;
		}
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDBHandle::addChild(const OW_String& parentKey, OW_HDBNode& childNode)
{
	if(parentKey.empty())
	{
		return false;
	}

	OW_HDBNode pnode = OW_HDBNode(parentKey.c_str(), *this);
	if(pnode)
	{
		return addChild(pnode, childNode);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDBHandle::removeNode(OW_HDBNode& node)
{
	bool cc = false;
	if(node && node.getOffset() > 0)
	{
		if(node.reload(*this))
		{
			node.remove(*this);
			cc = true;
		}
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDBHandle::removeNode(const OW_String& key)
{
	bool cc = false;
	if(!key.empty())
	{
		OW_HDBNode node(key.c_str(), *this);
		if(node)
		{
			node.remove(*this);
		}
		cc = true;
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDBHandle::updateNode(OW_HDBNode& node, OW_Int32 dataLen, unsigned char* data)
{
	bool cc = false;
	if(node)
	{
		// If node is already on file, then get a writelock on db
		if(node.getOffset() > 0)
		{
			if(node.reload(*this))
			{
				node.updateData(*this, dataLen, data);
				cc = true;
			}
		}
		else
		{
			// Node isn't yet on file. No need for a write lock
			node.updateData(*this, dataLen, data);
			cc = true;
		}
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDBHandle::turnFlagsOn(OW_HDBNode& node, OW_UInt32 flags)
{
	if(node)
	{
		if(node.getOffset() > 0)
		{
			if(node.reload(*this))
			{
            node.turnFlagsOn(*this, flags);
			}
		}
		else
		{
			node.turnFlagsOn(*this, flags);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HDBHandle::turnFlagsOff(OW_HDBNode& node, OW_UInt32 flags)
{
	if(node)
	{
		if(node.getOffset() > 0)
		{
			if(node.reload(*this))
			{
				node.turnFlagsOff(*this, flags);
			}
		}
		else
		{
			node.turnFlagsOff(*this, flags);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_IndexEntry
OW_HDBHandle::findFirstIndexEntry(const char* key)
{
	return m_pdata->m_pdb->findFirstIndexEntry(key);
}

//////////////////////////////////////////////////////////////////////////////
OW_IndexEntry
OW_HDBHandle::findNextIndexEntry()
{
	return m_pdata->m_pdb->findNextIndexEntry();
}

//////////////////////////////////////////////////////////////////////////////
OW_IndexEntry
OW_HDBHandle::findPrevIndexEntry()
{
	return m_pdata->m_pdb->findPrevIndexEntry();
}

//////////////////////////////////////////////////////////////////////////////
OW_IndexEntry
OW_HDBHandle::findIndexEntry(const char* key)
{
	return m_pdata->m_pdb->findIndexEntry(key);
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDBHandle::addIndexEntry(const char* key, OW_Int32 offset)
{
	return m_pdata->m_pdb->addIndexEntry(key, offset);
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDBHandle::removeIndexEntry(const char* key)
{
	return m_pdata->m_pdb->removeIndexEntry(key);
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_HDBHandle::updateIndexEntry(const char* key, OW_Int32 newOffset)
{
	return m_pdata->m_pdb->updateIndexEntry(key, newOffset);
}




