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

#ifndef OW_HDB_HPP_INCLUDE_GUARD_
#define OW_HDB_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_HDBCommon.hpp"
#include "OW_String.hpp"
#include "OW_Index.hpp"
#include "OW_Reference.hpp"
#include "OW_File.hpp"
#include "OW_HDBNode.hpp"
#include "OW_RWLocker.hpp"
#include "OW_MutexLock.hpp"

#include <cstdio>

class OW_HDB;

//////////////////////////////////////////////////////////////////////////////
class OW_HDBHandle
{
private:

	class OW_HDBHandleData
	{
	public:
		OW_HDBHandleData(OW_HDB* pdb, OW_File file) :
			m_pdb(pdb), m_file(file), m_writeDone(false),
			m_userVal(0L) {}

		OW_HDBHandleData() :
			m_pdb(NULL), m_file(), m_writeDone(false),
			m_userVal(0L) {}

		~OW_HDBHandleData();
	
		OW_HDB* m_pdb;
		OW_File m_file;
		bool m_writeDone;
		OW_Int32 m_userVal;	// Handle user can store any long data here
	};

public:
	/**
	 * Create a new OW_HDBHandle object from another (copy ctor).
	 * @param x		The OW_HDBHandle to make a copy of.
	 */
	OW_HDBHandle(const OW_HDBHandle& x) : m_pdata(x.m_pdata) {}

	OW_HDBHandle();

	/**
	 * Assignment operator
	 * @param x		The OW_HDBHandle to assign to this one.
	 * @return A reference to this OW_HDBHandle
	 */
	OW_HDBHandle& operator= (const OW_HDBHandle& x)
	{
		m_pdata = x.m_pdata;
		return *this;
	}

	/**
	 * Flush the underlying database and index files.
	 */
	void flush();

	/**
	 * Get the OW_HDBNode associated with a key.
	 * @param key	The key of the OW_HDBNode to retrieve.
	 * @return The OW_HDBNode associated with the key on success. Otherwise
	 * a NULL OW_HDBNode if the node doesn't exist.
	 * @exception OW_HDBException
	 */
	OW_HDBNode getNode(const OW_String& key);

	/**
	 * Get the OW_HDBNode that is the parent of another OW_HDBNode.
	 * @param node	The node to get the parent OW_HDBNode for.
	 * @return The OW_HDBNode that is the parent of the given node on success.
	 * Otherwise a NULL OW_HDBNode if there is no parent.
	 * @exception OW_HDBException
	 */
	OW_HDBNode getParent(OW_HDBNode& node);

	/**
	 * Get the OW_HDBNode that is the first child of a given node.
	 * @param node	The node to get the first child from.
	 * @return An OW_HDBNode for the first child of node on success. Otherwise
	 * a NULL OW_HDBNode if there are no children.
	 * @exception OW_HDBException
	 */
	OW_HDBNode getFirstChild(OW_HDBNode& node);

	/**
	 * Get the OW_HDBNode that is the last child of a given node.
	 * @param node	The node to get the last child from.
	 * @return An OW_HDBNode for the last child of node on success. Otherwise
	 * a NULL OW_HDBNode if there are no children.
	 * @exception OW_HDBException
	 */
	OW_HDBNode getLastChild(OW_HDBNode& node);

	/**
	 * Get the OW_HDBNode that is the next sibling of a given node.
	 * @param node	The node to get the next sibling from.
	 * @return An OW_HDBNode for the next sibling of node on success. Otherwise
	 * a NULL OW_HDBNode if there are no siblings.
	 * @exception OW_HDBException
	 */
	OW_HDBNode getNextSibling(OW_HDBNode& node);

	/**
	 * Get the OW_HDBNode that is the previous sibling of a given node.
	 * @param node	The node to get the previous sibling from.
	 * @return An OW_HDBNode for the previous sibling of node on success.
	 * Otherwise a NULL OW_HDBNode if there are no siblings.
	 * @exception OW_HDBException
	 */
	OW_HDBNode getPrevSibling(OW_HDBNode& node);

	/**
	 * @return The first root node of the database if there is one. Othewise
	 * a NULL OW_HDBNode.
	 * @exception OW_HDBException
	 */
	OW_HDBNode getFirstRoot();

	/**
	 * Add a node to the database that has no parent (root node).
	 * @param node	The node to add as a root node to the database.
	 * @return true if the node was added to the database. Otherwise false.
	 * @exception OW_HDBException
	 */
	bool addRootNode(OW_HDBNode& node);

	/**
	 * Add a node as a child to another node.
	 * @param parentNode		The node to add the child node to.
	 * @param childNode		The child node to add to parentNode.
	 * @return true if the child node was added to the parent. Otherwise false.
	 * @exception OW_HDBException
	 */
	bool addChild(OW_HDBNode& parentNode, OW_HDBNode& childNode);

	/**
	 * Add a node as a child to another node referenced by a given key.
	 * @param parentKey	The key to the parent node.
	 * @param childNode	The child node to add to the parentNode
	 * @return true if the child node was added to the parent. false if the
	 * parent does not exist.
	 */
	bool addChild(const OW_String& parentKey, OW_HDBNode& childNode);

	/**
	 * Remove a node and all of its' children.
	 * @param node	The node to remove.
	 * @return true if the node was removed. Otherwise false.
	 * @exception OW_HDBException
	 */
	bool removeNode(OW_HDBNode& node);

	/**
	 * Remove the node associated with a given key and all of its' children.
	 * @param key	The key of the node to remove.
	 * @return true if the node was removed. Otherwise false.
	 * @exception OW_HDBException
	 */
	bool removeNode(const OW_String& key);

	/**
	 * Update the data associated with a node.
	 * @param node		The node to update the data on.
	 * @param dataLen	The length of the data that will be associated with node.
	 * @param data		The data that will be associated with node.
	 * @return true if the update was successful. Otherwise false.
	 * @exception OW_HDBException
	 */
	bool updateNode(OW_HDBNode& node, OW_Int32 dataLen, unsigned char* data);

	/**
	 * Turn the user defined flags on in this node.
	 * @param node		The node to turn the flags on in
	 * @param flags	The flags to turn on in this node.
	 */
	void turnFlagsOn(OW_HDBNode& node, OW_UInt32 flags);

	/**
	 * Turn the user defined flags off in this node.
	 * @param node		The node to turn the flags off in
	 * @param flags	The flags to turn off in this node.
	 */
	void turnFlagsOff(OW_HDBNode& node, OW_UInt32 flags);

	/**
	 * Set the user value for this handle. The user value is not used by the
	 * OW_HDBHandle object. It allows the user to store any value for later
	 * retrieval. It is being used to help facilitate the caching of
	 * OW_HDBHandle object.
	 * @param value	The new value for the user data field.
	 */
	void setUserValue(OW_Int32 value) { m_pdata->m_userVal = value; }

	/**
	 * @return The user value for this OW_HDBHandle object.
	 * @see setUserValue
	 */
	OW_Int32 getUserValue() const { return m_pdata->m_userVal; }

	/**
	 * @return true if the is a valid OW_HDBHandle. Otherwise false.
	 */
private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (!m_pdata.isNull()) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (!m_pdata.isNull()) ? 0: &dummy::nonnull; }

private:

	OW_HDBHandle(OW_HDB* pdb, OW_File file);

	OW_File getFile() { return m_pdata->m_file; }
	OW_HDB* getHDB() { return m_pdata->m_pdb; }
	OW_Int32 registerWrite();

	OW_IndexEntry findFirstIndexEntry(const char* key=NULL);
	OW_IndexEntry findNextIndexEntry();
	OW_IndexEntry findPrevIndexEntry();
	OW_IndexEntry findIndexEntry(const char* key);
	bool addIndexEntry(const char* key, OW_Int32 offset);
	bool removeIndexEntry(const char* key);
	bool updateIndexEntry(const char* key, OW_Int32 newOffset);

	friend class OW_HDB;
	friend class OW_HDBNode;

	OW_Reference<OW_HDBHandleData> m_pdata;
};

//////////////////////////////////////////////////////////////////////////////
class OW_HDB
{
public:
	
	/**
	 * Create a new OW_HDB object.
	 */
	OW_HDB();

	/**
	 * Destroy this OW_HDB object.
	 */
	~OW_HDB();

	/**
	 * Open this OW_HDB object up for business.
	 * @param fileName	The file name associated with the database
	 * @exception OW_HDBException if an error occurs opening/creating files.
	 */
	void open(const char* fileName);

	/**
	 * Close this OW_HDB object
	 */
	void close();

	/**
	 * Create a new OW_HDBHandle that is associated with this OW_HDB object.
	 * @return An OW_HDBHandle object that can be used to operate on this
	 * OW_HDB object.
	 * @exception OW_HDBException if this OW_HDB is not opened.
	 */
	OW_HDBHandle getHandle();

	/**
	 * @return true if this OW_HDB is currently opened.
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
	 * @return The number of outstanding handles on this OW_HDB
	 */
	int getHandleCount() const { return m_hdlCount; }

	/**
	 * @return The file name for this OW_HDB object
	 */
	OW_String getFileName() const { return m_fileName; }


	/**
	 * Write the given OW_HDBBlock.
	 * @param fblk	The OW_HDBBlock to write.
	 * @param file The file object to write fblk to.
	 * @param offset	The offset to write the block to.
	 * @return The number of bytes written on success. Otherwise -1
	 */
	static void writeBlock(OW_HDBBlock& fblk, OW_File file, OW_Int32 offset);

	/**
	 * Read the given OW_HDBBlock.
	 * @param fblk	The OW_HDBBlock to read.
	 * @param file The file object to read fblk from.
	 * @param offset	The offset to read the block from.
	 * @return The number of bytes read on success. Otherwise -1
	 * @exception OW_HDBException
	 */
	static void readBlock(OW_HDBBlock& fblk, OW_File file, OW_Int32 offset);

private:

	bool createFile();
	bool checkFile();
	void setOffsets(OW_File file, OW_Int32 firstRootOffset, OW_Int32 lastRootOffset,
		OW_Int32 firstFreeOffset);
	void setFirstRootOffSet(OW_File file, OW_Int32 offset);
	void setLastRootOffset(OW_File file, OW_Int32 offset);
	void setFirstFreeOffSet(OW_File file, OW_Int32 offset);
	OW_Int32 getFirstRootOffSet() { return m_hdrBlock.firstRoot; }
	OW_Int32 getLastRootOffset() { return m_hdrBlock.lastRoot; }
	OW_Int32 getFirstFreeOffSet() { return m_hdrBlock.firstFree; }
	OW_Int32 findBlock(OW_File file, int size);
	void removeBlockFromFreeList(OW_File file, OW_HDBBlock& fblk);
	void addRootNode(OW_File file, OW_HDBBlock& fblk, OW_Int32 offset);
	void addBlockToFreeList(OW_File file, const OW_HDBBlock& parmblk,
		OW_Int32 offset);

	OW_Int32 getVersion() { return m_version; }
	OW_Int32 incVersion();
	void decHandleCount();

	OW_IndexEntry findFirstIndexEntry(const char* key=NULL);
	OW_IndexEntry findNextIndexEntry();
	OW_IndexEntry findPrevIndexEntry();
	OW_IndexEntry findIndexEntry(const char* key);
	bool addIndexEntry(const char* key, OW_Int32 offset);
	bool removeIndexEntry(const char* key);
	bool updateIndexEntry(const char* key, OW_Int32 newOffset);
	void flushIndex();

	OW_HDBHeaderBlock m_hdrBlock;
	OW_String m_fileName;
	OW_Int32 m_version;
	int m_hdlCount;
	bool m_opened;
	OW_IndexRef m_pindex;
	OW_Mutex m_indexGuard;
	OW_Mutex m_guard;

	friend class OW_HDBNode;
	friend class OW_HDBHandle;
	friend class OW_HDBHandle::OW_HDBHandleData;
};

#endif



