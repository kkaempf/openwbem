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

#ifndef OW_HDB_HPP_INCLUDE_GUARD_
#define OW_HDB_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_HDBCommon.hpp"
#include "OW_String.hpp"
#include "OW_Index.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_File.hpp"
#include "OW_HDBNode.hpp"
#include "OW_RWLocker.hpp"
#include "OW_MutexLock.hpp"
#include <cstdio>

namespace OpenWBEM
{

class HDB;
//////////////////////////////////////////////////////////////////////////////
class HDBHandle
{
private:
	class HDBHandleData : public IntrusiveCountableBase
	{
	public:
		HDBHandleData(HDB* pdb, File file) :
			m_pdb(pdb), m_file(file), m_writeDone(false),
			m_userVal(0L) {}
		HDBHandleData() :
			m_pdb(NULL), m_file(), m_writeDone(false),
			m_userVal(0L) {}
		~HDBHandleData();
	
		HDB* m_pdb;
		File m_file;
		bool m_writeDone;
		Int32 m_userVal;	// Handle user can store any long data here
	};
	typedef IntrusiveReference<HDBHandleData> HDBHandleDataRef;
public:
	/**
	 * Create a new HDBHandle object from another (copy ctor).
	 * @param x		The HDBHandle to make a copy of.
	 */
	HDBHandle(const HDBHandle& x) : m_pdata(x.m_pdata) {}
	HDBHandle();
	/**
	 * Assignment operator
	 * @param x		The HDBHandle to assign to this one.
	 * @return A reference to this HDBHandle
	 */
	HDBHandle& operator= (const HDBHandle& x)
	{
		m_pdata = x.m_pdata;
		return *this;
	}
	/**
	 * Flush the underlying database and index files.
	 */
	void flush();
	/**
	 * Get the HDBNode associated with a key.
	 * @param key	The key of the HDBNode to retrieve.
	 * @return The HDBNode associated with the key on success. Otherwise
	 * a NULL HDBNode if the node doesn't exist.
	 * @exception HDBException
	 */
	HDBNode getNode(const String& key);
	/**
	 * Get the HDBNode that is the parent of another HDBNode.
	 * @param node	The node to get the parent HDBNode for.
	 * @return The HDBNode that is the parent of the given node on success.
	 * Otherwise a NULL HDBNode if there is no parent.
	 * @exception HDBException
	 */
	HDBNode getParent(HDBNode& node);
	/**
	 * Get the HDBNode that is the first child of a given node.
	 * @param node	The node to get the first child from.
	 * @return An HDBNode for the first child of node on success. Otherwise
	 * a NULL HDBNode if there are no children.
	 * @exception HDBException
	 */
	HDBNode getFirstChild(HDBNode& node);
	/**
	 * Get the HDBNode that is the last child of a given node.
	 * @param node	The node to get the last child from.
	 * @return An HDBNode for the last child of node on success. Otherwise
	 * a NULL HDBNode if there are no children.
	 * @exception HDBException
	 */
	HDBNode getLastChild(HDBNode& node);
	/**
	 * Get the HDBNode that is the next sibling of a given node.
	 * @param node	The node to get the next sibling from.
	 * @return An HDBNode for the next sibling of node on success. Otherwise
	 * a NULL HDBNode if there are no siblings.
	 * @exception HDBException
	 */
	HDBNode getNextSibling(HDBNode& node);
	/**
	 * Get the HDBNode that is the previous sibling of a given node.
	 * @param node	The node to get the previous sibling from.
	 * @return An HDBNode for the previous sibling of node on success.
	 * Otherwise a NULL HDBNode if there are no siblings.
	 * @exception HDBException
	 */
	HDBNode getPrevSibling(HDBNode& node);
	/**
	 * @return The first root node of the database if there is one. Othewise
	 * a NULL HDBNode.
	 * @exception HDBException
	 */
	HDBNode getFirstRoot();
	/**
	 * Add a node to the database that has no parent (root node).
	 * @param node	The node to add as a root node to the database.
	 * @return true if the node was added to the database. Otherwise false.
	 * @exception HDBException
	 */
	bool addRootNode(HDBNode& node);
	/**
	 * Add a node as a child to another node.
	 * @param parentNode		The node to add the child node to.
	 * @param childNode		The child node to add to parentNode.
	 * @return true if the child node was added to the parent. Otherwise false.
	 * @exception HDBException
	 */
	bool addChild(HDBNode& parentNode, HDBNode& childNode);
	/**
	 * Add a node as a child to another node referenced by a given key.
	 * @param parentKey	The key to the parent node.
	 * @param childNode	The child node to add to the parentNode
	 * @return true if the child node was added to the parent. false if the
	 * parent does not exist.
	 */
	bool addChild(const String& parentKey, HDBNode& childNode);
	/**
	 * Remove a node and all of its' children.
	 * @param node	The node to remove.
	 * @return true if the node was removed. Otherwise false.
	 * @exception HDBException
	 */
	bool removeNode(HDBNode& node);
	/**
	 * Remove the node associated with a given key and all of its' children.
	 * @param key	The key of the node to remove.
	 * @return true if the node was removed. Otherwise false.
	 * @exception HDBException
	 */
	bool removeNode(const String& key);
	/**
	 * Update the data associated with a node.
	 * @param node		The node to update the data on.
	 * @param dataLen	The length of the data that will be associated with node.
	 * @param data		The data that will be associated with node.
	 * @return true if the update was successful. Otherwise false.
	 * @exception HDBException
	 */
	bool updateNode(HDBNode& node, Int32 dataLen, const unsigned char* data);
	/**
	 * Turn the user defined flags on in this node.
	 * @param node		The node to turn the flags on in
	 * @param flags	The flags to turn on in this node.
	 */
	void turnFlagsOn(HDBNode& node, UInt32 flags);
	/**
	 * Turn the user defined flags off in this node.
	 * @param node		The node to turn the flags off in
	 * @param flags	The flags to turn off in this node.
	 */
	void turnFlagsOff(HDBNode& node, UInt32 flags);
	/**
	 * Set the user value for this handle. The user value is not used by the
	 * HDBHandle object. It allows the user to store any value for later
	 * retrieval. It is being used to help facilitate the caching of
	 * HDBHandle object.
	 * @param value	The new value for the user data field.
	 */
	void setUserValue(Int32 value) { m_pdata->m_userVal = value; }
	/**
	 * @return The user value for this HDBHandle object.
	 * @see setUserValue
	 */
	Int32 getUserValue() const { return m_pdata->m_userVal; }
	/**
	 * @return true if the is a valid HDBHandle. Otherwise false.
	 */
private:
	struct dummy
	{
		void nonnull() {};
	};
	typedef void (dummy::*safe_bool)();
public:
	operator safe_bool () const
		{  return (m_pdata) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (m_pdata) ? 0: &dummy::nonnull; }
private:
	HDBHandle(HDB* pdb, File file);
	File getFile() { return m_pdata->m_file; }
	HDB* getHDB() { return m_pdata->m_pdb; }
	Int32 registerWrite();
	IndexEntry findFirstIndexEntry(const char* key=NULL);
	IndexEntry findNextIndexEntry();
	IndexEntry findPrevIndexEntry();
	IndexEntry findIndexEntry(const char* key);
	bool addIndexEntry(const char* key, Int32 offset);
	bool removeIndexEntry(const char* key);
	bool updateIndexEntry(const char* key, Int32 newOffset);
	friend class HDB;
	friend class HDBNode;
	HDBHandleDataRef m_pdata;
};
//////////////////////////////////////////////////////////////////////////////
class HDB
{
public:
	
	/**
	 * Create a new HDB object.
	 */
	HDB();
	/**
	 * Destroy this HDB object.
	 */
	~HDB();
	/**
	 * Open this HDB object up for business.
	 * @param fileName	The file name associated with the database
	 * @exception HDBException if an error occurs opening/creating files.
	 */
	void open(const char* fileName);
	/**
	 * Close this HDB object
	 */
	void close();
	/**
	 * Create a new HDBHandle that is associated with this HDB object.
	 * @return An HDBHandle object that can be used to operate on this
	 * HDB object.
	 * @exception HDBException if this HDB is not opened.
	 */
	HDBHandle getHandle();
	/**
	 * @return true if this HDB is currently opened.
	 */
private:
	struct dummy
	{
		void nonnull() {};
	};
	typedef void (dummy::*safe_bool)();
public:
	operator safe_bool () const
		{  return (m_opened) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (m_opened) ? 0: &dummy::nonnull; }
	/**
	 * @return The number of outstanding handles on this HDB
	 */
	int getHandleCount() const { return m_hdlCount; }
	/**
	 * @return The file name for this HDB object
	 */
	String getFileName() const { return m_fileName; }
	/**
	 * Write the given HDBBlock.
	 * @param fblk	The HDBBlock to write.
	 * @param file The file object to write fblk to.
	 * @param offset	The offset to write the block to.
	 * @return The number of bytes written on success. Otherwise -1
	 */
	static void writeBlock(HDBBlock& fblk, File file, Int32 offset);
	/**
	 * Read the given HDBBlock.
	 * @param fblk	The HDBBlock to read.
	 * @param file The file object to read fblk from.
	 * @param offset	The offset to read the block from.
	 * @return The number of bytes read on success. Otherwise -1
	 * @exception HDBException
	 */
	static void readBlock(HDBBlock& fblk, File file, Int32 offset);
private:
	bool createFile();
	bool checkFile();
	void setOffsets(File file, Int32 firstRootOffset, Int32 lastRootOffset,
		Int32 firstFreeOffset);
	void setFirstRootOffSet(File file, Int32 offset);
	void setLastRootOffset(File file, Int32 offset);
	void setFirstFreeOffSet(File file, Int32 offset);
	Int32 getFirstRootOffSet() { return m_hdrBlock.firstRoot; }
	Int32 getLastRootOffset() { return m_hdrBlock.lastRoot; }
	Int32 getFirstFreeOffSet() { return m_hdrBlock.firstFree; }
	Int32 findBlock(File file, int size);
	void removeBlockFromFreeList(File file, HDBBlock& fblk);
	void addRootNode(File file, HDBBlock& fblk, Int32 offset);
	void addBlockToFreeList(File file, const HDBBlock& parmblk,
		Int32 offset);
	Int32 getVersion() { return m_version; }
	Int32 incVersion();
	void decHandleCount();
	IndexEntry findFirstIndexEntry(const char* key=NULL);
	IndexEntry findNextIndexEntry();
	IndexEntry findPrevIndexEntry();
	IndexEntry findIndexEntry(const char* key);
	bool addIndexEntry(const char* key, Int32 offset);
	bool removeIndexEntry(const char* key);
	bool updateIndexEntry(const char* key, Int32 newOffset);
	void flushIndex();
	HDBHeaderBlock m_hdrBlock;
	String m_fileName;
	Int32 m_version;
	int m_hdlCount;
	bool m_opened;
	IndexRef m_pindex;
	Mutex m_indexGuard;
	Mutex m_guard;
	friend class HDBNode;
	friend class HDBHandle;
	friend class HDBHandle::HDBHandleData;
};

} // end namespace OpenWBEM

#endif
