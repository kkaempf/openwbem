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

#ifndef __OW_HDBNODE_HPP__
#define __OW_HDBNODE_HPP__

#include "OW_config.h"
#include "OW_HDBCommon.hpp"
#include "OW_String.hpp"
#include "OW_Reference.hpp"

class OW_HDBHandle;
class OW_HDB;

//////////////////////////////////////////////////////////////////////////////				 
class OW_HDBNode
{
private:

	struct HDBNodeData
	{
		HDBNodeData();
		HDBNodeData(const HDBNodeData& x);
		~HDBNodeData();
		HDBNodeData& operator= (const HDBNodeData& x);
	
		OW_HDBBlock m_blk;
		OW_String m_key;
		int m_bfrLen;
		unsigned char* m_bfr;
		long m_offset;
		long m_version;
	};

public:

	/**
	 * Create a null OW_HDBNode object.
	 */
	OW_HDBNode() : m_pdata(0) {}

	/**
	 * Create an OW_HDBNode associated with a given key and data.
	 * The node will not be persistant until it is added to the OW_HDB.
	 * @param key		The key associated with the given OW_HDBNode
	 * @param dataLen	The length of the data associated with the node.
	 * @param data		The data associated with the node.
	 */
	OW_HDBNode(const OW_String& key, int dataLen, const unsigned char* data);

	/**
	 * Copy constructor
	 * @param x		The OW_HDBNode to copy this node from.
	 */
	OW_HDBNode(const OW_HDBNode& x) : m_pdata(x.m_pdata) { }

	/**
	 * Assignment operator
	 * @param x		The OW_HDBNode to assign to this one.
	 * @return A reference to this OW_HDBNode.
	 */
	OW_HDBNode& operator= (const OW_HDBNode& x) 
	{
		m_pdata = x.m_pdata;
		return *this;
	}

	/**
	 * @return The value of the flags field on this OW_HDBNode object.
	 */
	int getFlags()
	{
		return m_pdata->m_blk.flags;
	}

	/**
	 * Determine if all of the user defined flags are on.
	 * @param flags	The user defined flags to check.
	 * @return true if all flags are on in this node. Othewise false.
	 */
	OW_Bool areAllFlagsOn(unsigned int flags) const
	{
		return ((m_pdata->m_blk.flags & flags) == flags);
	}

	/**
	 * Determine if some of the user defined flags are on.
	 * @param flags	The user defined flags to check.
	 * @return true if some flags are on in this node. Othewise false.
	 */
	OW_Bool areSomeFlagsOn(unsigned int flags) const
	{
		return ((m_pdata->m_blk.flags & flags) != 0);
	}

	/**
	 * Turn the user defined flags on in this node.
	 * @param hdl
	 * @param flags	The flags to turn on in this node.
	 * @return true if the flags were changed from this operation. Otherwise
	 * false.
	 */
	OW_Bool turnFlagsOn(OW_HDBHandle& hdl, unsigned int flags);

	/**
	 * Turn the user defined flags off in this node.
	 * @param hdl
	 * @param flags	The flags to turn off in this node.
	 * @return true if the flags were changed from this operation. Otherwise
	 * false.
	 */
	OW_Bool turnFlagsOff(OW_HDBHandle& hdl, unsigned int flags);

	/**
	 * @return The key associated with this OW_HDBNode.
	 */
	OW_String getKey() const { return m_pdata->m_key; }

	/**
	 * @return The length of the data associated with this OW_HDBNode.
	 */
	int getDataLen() const { return m_pdata->m_bfrLen; }

	/**
	 * @return A pointer to the data associated with this OW_HDBNode.
	 */
	const unsigned char* getData() const { return m_pdata->m_bfr; }

	/**
	 * @return true if this OW_HDBNode has a parent.
	 */
	OW_Bool hasParent() const { return (m_pdata->m_blk.parent != -1); }

	/**
	 * @return true if this OW_HDBNode has a next sibling.
	 */
	OW_Bool hasNextSibling() const { return (m_pdata->m_blk.nextSib != -1); }

	/**
	 * @return true if this OW_HDBNode has a previous sibling.
	 */
	OW_Bool hasPreviousSibling() const { return (m_pdata->m_blk.prevSib != -1); }

	/**
	 * @return true if this OW_HDBNode has any children.
	 */
	OW_Bool hasChildren() const { return (m_pdata->m_blk.firstChild != -1); }

	/**
	 * @return true if this OW_HDBNode is a root node (has no parent).
	 */
	OW_Bool isRoot() const { return (hasParent() == false); }

	/**
	 * @return true if this OW_HDBNode is a child.
	 */
	OW_Bool isChild() const { return (hasParent() == true); }

	/**
	 * @return true if this OW_HDBNode has any siblings.
	 */
	OW_Bool isSibling() const 
	{ 
		return (hasNextSibling() > 0 || hasPreviousSibling() > 0); 
	}

	/**
	 * @return true if this OW_HDBNode is valid.
	 */
	operator void*() const { return (void*)(m_pdata.isNull() == false); }

private:

	OW_HDBNode(const char* key, OW_HDBHandle& hdl);
	OW_HDBNode(long offset, OW_HDBHandle& hdl);
	void read(long offset, OW_HDBHandle& hdl);
	OW_Bool reload(OW_HDBHandle& hdl);
	long write(OW_HDBHandle& hdl, OW_Bool onlyHeader=false);
	void updateOffsets(OW_HDBHandle& hdl, long offset);
	long getParentOffset() const { return m_pdata->m_blk.parent; }
	long getFirstChildOffset() const { return m_pdata->m_blk.firstChild; }
	long getLastChildOffset() const { return m_pdata->m_blk.lastChild; }
	long getNextSiblingOffset() const { return m_pdata->m_blk.nextSib; }
	long getPrevSiblingOffset() const { return m_pdata->m_blk.prevSib; }
	long getOffset() { return m_pdata->m_offset; }
	OW_Bool remove(OW_HDBHandle& hdl);
	void removeBlock(OW_HDBHandle& hdl, OW_HDBBlock& fblk, long offset);
	void addChild(OW_HDBHandle& hdl, OW_HDBNode& arg);
	OW_Bool updateData(OW_HDBHandle& hdl, int dataLen, unsigned char* data);

	void setNull() { m_pdata = 0; }

	OW_Reference<HDBNodeData> m_pdata;

	friend class OW_HDBHandle;
};

#endif	// __OW_HDBNODE_HPP__

