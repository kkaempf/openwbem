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

namespace OpenWBEM
{

static UInt32 calcCheckSum(unsigned char* src, Int32 len);
//////////////////////////////////////////////////////////////////////////////
HDB::HDB() :
	m_hdrBlock(), m_fileName(), m_version(0), m_hdlCount(0),
	m_opened(false), m_pindex(NULL), m_indexGuard(), m_guard()
{
}
//////////////////////////////////////////////////////////////////////////////
HDB::~HDB()
{
	try
	{
		if (m_hdlCount > 0)
		{
			// cerr << "*** HDB::~HDB - STILL OUTSTANDING HANDLES ***" << endl;
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
HDB::close()
{
	if (m_opened)
	{
		m_pindex->close();
		m_pindex = 0;
		m_opened = false;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
HDB::open(const char* fileName)
{
	MutexLock l(m_guard);
	if (m_opened)
	{
		return;
	}
	m_hdlCount = 0;
	m_version = 0;
	m_fileName = fileName;
	String fname = m_fileName + ".dat";
	createFile();
	if (!checkFile())
	{
		String msg("Failed to open file: ");
		msg += fname;
		OW_THROW(HDBException, msg.c_str());
	}
	m_fileName = fname;
	m_opened = true;
}
//////////////////////////////////////////////////////////////////////////////
bool
HDB::createFile()
{
	HDBHeaderBlock b = { OW_HDBSIGNATURE, HDBVERSION, -1L, -1L, -1L };
	m_hdrBlock = b;
	File f = FileSystem::createFile(m_fileName + ".dat");
	if (!f)
	{
		return false;
	}
	if (f.write(&m_hdrBlock, sizeof(m_hdrBlock), 0) != sizeof(m_hdrBlock))
	{
		f.close();
		OW_THROW(HDBException, "Failed to write header of HDB");
	}
	f.close();
	m_pindex = Index::createIndexObject();
	m_pindex->open(m_fileName.c_str());
	return true;
}
//////////////////////////////////////////////////////////////////////////////
bool
HDB::checkFile()
{
	File f = FileSystem::openFile(m_fileName + ".dat");
	if (!f)
	{
		String msg("Failed to open file: ");
		msg += m_fileName;
		OW_THROW(HDBException, msg.c_str());
	}
	if (f.read(&m_hdrBlock, sizeof(m_hdrBlock), 0) != sizeof(m_hdrBlock))
	{
		f.close();
		String msg("Failed to read HDB header from file: ");
		msg += m_fileName;
		OW_THROW(HDBException, msg.c_str());
	}
	f.close();
	if (::strncmp(m_hdrBlock.signature, OW_HDBSIGNATURE, HDBSIGLEN))
	{
		String msg("Invalid Format for HDB file: ");
		msg += m_fileName;
		OW_THROW(HDBException, msg.c_str());
	}
	if (m_hdrBlock.version < MinHDBVERSION || m_hdrBlock.version > HDBVERSION)
	{
		OW_THROW(HDBException, Format("Invalid version (%1) for file (%2). Expected (%3)", m_hdrBlock.version, m_fileName, HDBVERSION).c_str());
	}
	m_pindex = Index::createIndexObject();
	m_pindex->open(m_fileName.c_str());
	return true;
}
//////////////////////////////////////////////////////////////////////////////
Int32
HDB::incVersion()
{
	MutexLock l(m_guard);
	m_version++;
	return m_version;
}
//////////////////////////////////////////////////////////////////////////////
HDBHandle
HDB::getHandle()
{
	MutexLock l(m_guard);
	if (!m_opened)
	{
		OW_THROW(HDBException, "Can't get handle from closed HDB");
	}
	const File& file = FileSystem::openFile(m_fileName);
	if (!file)
	{
		return HDBHandle();
	}
	m_hdlCount++;
	return HDBHandle(this, file);
}
//////////////////////////////////////////////////////////////////////////////
void
HDB::decHandleCount()
{
	MutexLock l(m_guard);
	m_hdlCount--;
}
//////////////////////////////////////////////////////////////////////////////
void
HDB::setOffsets(File& file, Int32 firstRootOffset, Int32 lastRootOffset,
	Int32 firstFreeOffset)
{
	MutexLock l(m_guard);
	m_hdrBlock.firstRoot = firstRootOffset;
	m_hdrBlock.lastRoot = lastRootOffset;
	m_hdrBlock.firstFree = firstFreeOffset;
	if (file.write(&m_hdrBlock, sizeof(m_hdrBlock), 0) != sizeof(m_hdrBlock))
	{
		OW_THROW(HDBException, "Failed to update offset on HDB");
	}
}
//////////////////////////////////////////////////////////////////////////////
void
HDB::setFirstRootOffSet(File& file, Int32 offset)
{
	setOffsets(file, offset, m_hdrBlock.lastRoot, m_hdrBlock.firstFree);
}
//////////////////////////////////////////////////////////////////////////////
void
HDB::setLastRootOffset(File& file, Int32 offset)
{
	setOffsets(file, m_hdrBlock.firstRoot, offset, m_hdrBlock.firstFree);
}
//////////////////////////////////////////////////////////////////////////////
void
HDB::setFirstFreeOffSet(File& file, Int32 offset)
{
	setOffsets(file, m_hdrBlock.firstRoot, m_hdrBlock.lastRoot, offset);
}
//////////////////////////////////////////////////////////////////////////////
// Find a block in the free list that is large enough to hold the given
// size. If no block in the free list is large enough or the free list
// is empty, then the offset to the end of the file is returned
Int32
HDB::findBlock(File& file, Int32 size)
{
	MutexLock l(m_guard);
	Int32 offset = -1;
	HDBBlock fblk;
	// If the free list is not empty, then search it for a block
	// big enough to hold the given size
	if (m_hdrBlock.firstFree != -1)
	{
		Int32 coffset = m_hdrBlock.firstFree;
		while (true)
		{
			readBlock(fblk, file, coffset);
			// If the current block size is greater than or equal to the
			// size being requested, then we found a block in the file
			// we can use.
			if (fblk.size >= static_cast<UInt32>(size))
			{
				offset = coffset;
				break;
			}
			if ((coffset = fblk.nextSib) == -1L)
			{
				break;
			}
		}
	}
	// If offset is no longer -1, then we must have found a block
	// of adequate size.
	if (offset != -1)
	{
		// Remove the block from the free list
		removeBlockFromFreeList(file, fblk);
	}
	else
	{
		// We didn't find a block that was big enough, so let's just allocate
		// a chunk at the end of the file.
		if (file.seek(0L, SEEK_END) == -1L)
		{
			OW_THROW(HDBException, "Failed to seek to end of file");
		}
		if ((offset = file.tell()) == -1L)
		{
			OW_THROW(HDBException, "Failed to get offset in file");
		}
	}
	return offset;
}
//////////////////////////////////////////////////////////////////////////////
void
HDB::removeBlockFromFreeList(File& file, HDBBlock& fblk)
{
	MutexLock l(m_guard);
	HDBBlock cblk;
	// If block has a next sibling, then set it's previous sibling pointer
	// to the given blocks previous pointer
	if (fblk.nextSib != -1)
	{
		readBlock(cblk, file, fblk.nextSib);
		cblk.prevSib = fblk.prevSib;
		writeBlock(cblk, file, fblk.nextSib);
	}
	// If block has a previous sibling, then set it's next sibling pointer
	// to the given blocks next pointer
	if (fblk.prevSib != -1)
	{
		readBlock(cblk, file, fblk.prevSib);
		cblk.nextSib = fblk.nextSib;
		writeBlock(cblk, file, fblk.prevSib);
	}
	else		// Block must be the 1st one in the free list
	{
		// If no previous sibling, assume this was the 1st in the
		// free list, so set the head pointer
		if (m_hdrBlock.firstFree != -1)
		{
			setFirstFreeOffSet(file, fblk.nextSib);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
// Add a block to the free list.
void
HDB::addBlockToFreeList(File& file, const HDBBlock& parmblk,
	Int32 offset)
{
	MutexLock l(m_guard);
	HDBBlock fblk = parmblk;
	fblk.isFree = true;
	// If the free list is empty, set the free list head pointer only
	if (m_hdrBlock.firstFree == -1)
	{
		fblk.nextSib = -1;
		fblk.prevSib = -1;
		writeBlock(fblk, file, offset);
		setFirstFreeOffSet(file, offset);
		return;
	}
	HDBBlock cblk;
	cblk.size = 0;
	Int32 coffset = m_hdrBlock.firstFree;
	Int32 loffset = 0;
	// Find insertion point in free list
	while (coffset != -1)
	{
		loffset = coffset;
		readBlock(cblk, file, coffset);
		if (fblk.size <= cblk.size)
		{
			break;
		}
		coffset = cblk.nextSib;
	}
	if (coffset == -1)		// Append to end of free list?
	{
		cblk.nextSib = offset;
		writeBlock(cblk, file, loffset);
		fblk.prevSib = loffset;
		fblk.nextSib = -1;
		writeBlock(fblk, file, offset);
	}
	else						// Insert before last node read
	{
		if (cblk.prevSib == -1)			// If this Was the 1st on the list
		{											// Set the free list head pointer
			setFirstFreeOffSet(file, offset);		
		}
		else
		{
			// Read the previous node from last read to set it's next
			// sibling pointer
			HDBBlock tblk;
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
HDB::addRootNode(File& file, HDBBlock& fblk, Int32 offset)
{
	MutexLock l(m_guard);
	fblk.parent = -1;
	fblk.nextSib = -1;
	if (m_hdrBlock.firstRoot == -1)
	{
		setOffsets(file, offset, offset, m_hdrBlock.firstFree);
		fblk.prevSib = -1;
	}
	else
	{
		fblk.prevSib = m_hdrBlock.lastRoot;
		HDBBlock cblk;
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
HDB::writeBlock(HDBBlock& fblk, File& file, Int32 offset)
{
	fblk.chkSum = 0;
	UInt32 chkSum = calcCheckSum(reinterpret_cast<unsigned char*>(&fblk), sizeof(fblk));
	fblk.chkSum = chkSum;
	int cc = file.write(&fblk, sizeof(fblk), offset);
	if (cc != sizeof(fblk))
	{
		OW_THROW(HDBException, "Failed to write block");
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC	
void
HDB::readBlock(HDBBlock& fblk, const File& file, Int32 offset)
{
	int cc = file.read(&fblk, sizeof(fblk), offset);
	if (cc != sizeof(fblk))
	{
		OW_THROW(HDBException, "Failed to read block");
	}
	UInt32 chkSum = fblk.chkSum;
	fblk.chkSum = 0;
	fblk.chkSum = calcCheckSum(reinterpret_cast<unsigned char*>(&fblk), sizeof(fblk));
	if (chkSum != fblk.chkSum)
	{
		OW_THROW(HDBException, "CORRUPT DATA? Invalid check sum in node");
	}
}
//////////////////////////////////////////////////////////////////////////////
IndexEntry
HDB::findFirstIndexEntry(const char* key)
{
	if (!m_opened)
	{
		OW_THROW(HDBException, "HDB is not opened");
	}
	MutexLock il(m_indexGuard);
	return m_pindex->findFirst(key);
}
//////////////////////////////////////////////////////////////////////////////
IndexEntry
HDB::findNextIndexEntry()
{
	if (!m_opened)
	{
		OW_THROW(HDBException, "HDB is not opened");
	}
	MutexLock il(m_indexGuard);
	return m_pindex->findNext();
}
//////////////////////////////////////////////////////////////////////////////
IndexEntry
HDB::findPrevIndexEntry()
{
	if (!m_opened)
	{
		OW_THROW(HDBException, "HDB is not opened");
	}
	MutexLock il(m_indexGuard);
	return m_pindex->findPrev();
}
//////////////////////////////////////////////////////////////////////////////
IndexEntry
HDB::findIndexEntry(const char* key)
{
	if (!m_opened)
	{
		OW_THROW(HDBException, "HDB is not opened");
	}
	MutexLock il(m_indexGuard);
	return m_pindex->find(key);
}
//////////////////////////////////////////////////////////////////////////////
bool
HDB::addIndexEntry(const char* key, Int32 offset)
{
	if (!m_opened)
	{
		OW_THROW(HDBException, "HDB is not opened");
	}
	MutexLock il(m_indexGuard);
	return m_pindex->add(key, offset);
}
//////////////////////////////////////////////////////////////////////////////
bool
HDB::removeIndexEntry(const char* key)
{
	if (!m_opened)
	{
		OW_THROW(HDBException, "HDB is not opened");
	}
	MutexLock il(m_indexGuard);
	return m_pindex->remove(key);
}
//////////////////////////////////////////////////////////////////////////////
bool
HDB::updateIndexEntry(const char* key, Int32 newOffset)
{
	if (!m_opened)
	{
		OW_THROW(HDBException, "HDB is not opened");
	}
	MutexLock il(m_indexGuard);
	return m_pindex->update(key, newOffset);
}
//////////////////////////////////////////////////////////////////////////////
void
HDB::flushIndex()
{
	if (m_opened)
	{
		MutexLock il(m_indexGuard);
		m_pindex->flush();
	}
}
//////////////////////////////////////////////////////////////////////////////
static UInt32
calcCheckSum(unsigned char* src, Int32 len)
{
	UInt32 cksum = 0;
	Int32 i;
	for (i = 0; i < len; i++)
	{
		cksum += src[i];
	}
	return cksum;
}
//////////////////////////////////////////////////////////////////////////////
// ROUTINES FOR HDBHandle class
HDBHandle::HDBHandleData::~HDBHandleData()
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
HDBHandle::HDBHandle() :
	m_pdata(NULL)
{
}
//////////////////////////////////////////////////////////////////////////////
HDBHandle::HDBHandle(HDB* pdb, const File& file) :
	m_pdata(new HDBHandleData(pdb, file))
{
}
//////////////////////////////////////////////////////////////////////////////
Int32
HDBHandle::registerWrite()
{
	m_pdata->m_writeDone = true;
	return m_pdata->m_pdb->incVersion();
}
//////////////////////////////////////////////////////////////////////////////
void
HDBHandle::flush()
{
	if (m_pdata->m_writeDone)
	{
		m_pdata->m_pdb->flushIndex();
		m_pdata->m_file.flush();
		m_pdata->m_writeDone = false;
	}
}
//////////////////////////////////////////////////////////////////////////////
HDBNode
HDBHandle::getFirstRoot()
{
	if (m_pdata->m_pdb->getFirstRootOffSet() > 0)
	{
		return HDBNode(m_pdata->m_pdb->getFirstRootOffSet(), *this);
	}
	return HDBNode();
}
//////////////////////////////////////////////////////////////////////////////
HDBNode
HDBHandle::getNode(const String& key)
{
	if (!key.empty())
	{
		return HDBNode(key.c_str(), *this);
	}
	return HDBNode();
}
//////////////////////////////////////////////////////////////////////////////
HDBNode
HDBHandle::getParent(HDBNode& node)
{
	if (node)
	{
		if (node.reload(*this))
		{
			if (node.getParentOffset() > 0)
			{
				return HDBNode(node.getParentOffset(), *this);
			}
		}
	}
	return HDBNode();
}
//////////////////////////////////////////////////////////////////////////////
HDBNode
HDBHandle::getFirstChild(HDBNode& node)
{
	if (node)
	{
		if (node.reload(*this))
		{
			if (node.getFirstChildOffset() > 0)
			{
				return HDBNode(node.getFirstChildOffset(), *this);
			}
		}
	}
	return HDBNode();
}
//////////////////////////////////////////////////////////////////////////////
HDBNode
HDBHandle::getLastChild(HDBNode& node)
{
	if (node)
	{
		if (node.reload(*this))
		{
			if (node.getLastChildOffset() > 0)
			{
				return HDBNode(node.getLastChildOffset(), *this);
			}
		}
	}
	return HDBNode();
}
//////////////////////////////////////////////////////////////////////////////
HDBNode
HDBHandle::getNextSibling(HDBNode& node)
{
	if (node)
	{
		if (node.reload(*this))
		{
			if (node.getNextSiblingOffset() > 0)
			{
				return HDBNode(node.getNextSiblingOffset(), *this);
			}
		}
	}
	return HDBNode();
}
//////////////////////////////////////////////////////////////////////////////
HDBNode
HDBHandle::getPrevSibling(HDBNode& node)
{
	if (node)
	{
		if (node.reload(*this))
		{
			if (node.getPrevSiblingOffset() > 0)
			{
				return HDBNode(node.getPrevSiblingOffset(), *this);
			}
		}
	}
	return HDBNode();
}
//////////////////////////////////////////////////////////////////////////////
bool
HDBHandle::addRootNode(HDBNode& node)
{
	bool cc = false;
	if (node)
	{
		if (node.getOffset() > 0)
		{
			OW_THROW(HDBException, "node is already on file");
		}
		if (m_pdata->m_pdb->findIndexEntry(node.getKey().c_str()))
		{
			OW_THROW(HDBException, "key for node is already in index");
		}
		node.write(*this);
		cc = true;
	}
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
bool
HDBHandle::addChild(HDBNode& parentNode, HDBNode& childNode)
{
	bool cc = false;
	if (parentNode && childNode)
	{
		if (childNode.getOffset() > 0)
		{
			OW_THROW(HDBException, "child node already has a parent");
		}
		if (parentNode.getOffset() <= 0)
		{
			OW_THROW(HDBException, "parent node is not on file");
		}
		if (m_pdata->m_pdb->findIndexEntry(childNode.getKey().c_str()))
		{
			OW_THROW(HDBException, "key for node is already in index");
		}
		if (parentNode.reload(*this))
		{
			parentNode.addChild(*this, childNode);
			cc = true;
		}
	}
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
bool
HDBHandle::addChild(const String& parentKey, HDBNode& childNode)
{
	if (parentKey.empty())
	{
		return false;
	}
	HDBNode pnode = HDBNode(parentKey.c_str(), *this);
	if (pnode)
	{
		return addChild(pnode, childNode);
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////
bool
HDBHandle::removeNode(HDBNode& node)
{
	bool cc = false;
	if (node && node.getOffset() > 0)
	{
		if (node.reload(*this))
		{
			node.remove(*this);
			cc = true;
		}
	}
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
bool
HDBHandle::removeNode(const String& key)
{
	bool cc = false;
	if (!key.empty())
	{
		HDBNode node(key.c_str(), *this);
		if (node)
		{
			node.remove(*this);
		}
		cc = true;
	}
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
bool
HDBHandle::updateNode(HDBNode& node, Int32 dataLen, const unsigned char* data)
{
	bool cc = false;
	if (node)
	{
		// If node is already on file, then get a writelock on db
		if (node.getOffset() > 0)
		{
			if (node.reload(*this))
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
HDBHandle::turnFlagsOn(HDBNode& node, UInt32 flags)
{
	if (node)
	{
		if (node.getOffset() > 0)
		{
			if (node.reload(*this))
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
HDBHandle::turnFlagsOff(HDBNode& node, UInt32 flags)
{
	if (node)
	{
		if (node.getOffset() > 0)
		{
			if (node.reload(*this))
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
IndexEntry
HDBHandle::findFirstIndexEntry(const char* key)
{
	return m_pdata->m_pdb->findFirstIndexEntry(key);
}
//////////////////////////////////////////////////////////////////////////////
IndexEntry
HDBHandle::findNextIndexEntry()
{
	return m_pdata->m_pdb->findNextIndexEntry();
}
//////////////////////////////////////////////////////////////////////////////
IndexEntry
HDBHandle::findPrevIndexEntry()
{
	return m_pdata->m_pdb->findPrevIndexEntry();
}
//////////////////////////////////////////////////////////////////////////////
IndexEntry
HDBHandle::findIndexEntry(const char* key)
{
	return m_pdata->m_pdb->findIndexEntry(key);
}
//////////////////////////////////////////////////////////////////////////////
bool
HDBHandle::addIndexEntry(const char* key, Int32 offset)
{
	return m_pdata->m_pdb->addIndexEntry(key, offset);
}
//////////////////////////////////////////////////////////////////////////////
bool
HDBHandle::removeIndexEntry(const char* key)
{
	return m_pdata->m_pdb->removeIndexEntry(key);
}
//////////////////////////////////////////////////////////////////////////////
bool
HDBHandle::updateIndexEntry(const char* key, Int32 newOffset)
{
	return m_pdata->m_pdb->updateIndexEntry(key, newOffset);
}

} // end namespace OpenWBEM

