/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_HDBNODE_HPP_INCLUDE_GUARD_
#define OW_HDBNODE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_HDBCommon.hpp"
#include "OW_String.hpp"
#include "OW_Reference.hpp"

namespace OpenWBEM
{

class HDBHandle;
class HDB;
//////////////////////////////////////////////////////////////////////////////				
class HDBNode
{
private:
	struct HDBNodeData
	{
		HDBNodeData();
		HDBNodeData(const HDBNodeData& x);
		~HDBNodeData();
		HDBNodeData& operator= (const HDBNodeData& x);
	
		HDBBlock m_blk;
		String m_key;
		Int32 m_bfrLen;
		unsigned char* m_bfr;
		Int32 m_offset;
		Int32 m_version;
	};
public:
	/**
	 * Create a null HDBNode object.
	 */
	HDBNode() : m_pdata(0) {}
	/**
	 * Create an HDBNode associated with a given key and data.
	 * The node will not be persistant until it is added to the HDB.
	 * @param key		The key associated with the given HDBNode
	 * @param dataLen	The length of the data associated with the node.
	 * @param data		The data associated with the node.
	 */
	HDBNode(const String& key, int dataLen, const unsigned char* data);
	/**
	 * Copy constructor
	 * @param x		The HDBNode to copy this node from.
	 */
	HDBNode(const HDBNode& x) : m_pdata(x.m_pdata) { }
	/**
	 * Assignment operator
	 * @param x		The HDBNode to assign to this one.
	 * @return A reference to this HDBNode.
	 */
	HDBNode& operator= (const HDBNode& x)
	{
		m_pdata = x.m_pdata;
		return *this;
	}
	/**
	 * @return The value of the flags field on this HDBNode object.
	 */
	UInt32 getFlags()
	{
		return m_pdata->m_blk.flags;
	}
	/**
	 * Determine if all of the user defined flags are on.
	 * @param flags	The user defined flags to check.
	 * @return true if all flags are on in this node. Othewise false.
	 */
	bool areAllFlagsOn(UInt32 flags) const
	{
		return ((m_pdata->m_blk.flags & flags) == flags);
	}
	/**
	 * Determine if some of the user defined flags are on.
	 * @param flags	The user defined flags to check.
	 * @return true if some flags are on in this node. Othewise false.
	 */
	bool areSomeFlagsOn(UInt32 flags) const
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
	bool turnFlagsOn(HDBHandle& hdl, UInt32 flags);
	/**
	 * Turn the user defined flags off in this node.
	 * @param hdl
	 * @param flags	The flags to turn off in this node.
	 * @return true if the flags were changed from this operation. Otherwise
	 * false.
	 */
	bool turnFlagsOff(HDBHandle& hdl, UInt32 flags);
	/**
	 * @return The key associated with this HDBNode.
	 */
	String getKey() const { return m_pdata->m_key; }
	/**
	 * @return The length of the data associated with this HDBNode.
	 */
	Int32 getDataLen() const { return m_pdata->m_bfrLen; }
	/**
	 * @return A pointer to the data associated with this HDBNode.
	 */
	const unsigned char* getData() const { return m_pdata->m_bfr; }
	/**
	 * @return true if this HDBNode has a parent.
	 */
	bool hasParent() const { return (m_pdata->m_blk.parent != -1); }
	/**
	 * @return true if this HDBNode has a next sibling.
	 */
	bool hasNextSibling() const { return (m_pdata->m_blk.nextSib != -1); }
	/**
	 * @return true if this HDBNode has a previous sibling.
	 */
	bool hasPreviousSibling() const { return (m_pdata->m_blk.prevSib != -1); }
	/**
	 * @return true if this HDBNode has any children.
	 */
	bool hasChildren() const { return (m_pdata->m_blk.firstChild != -1); }
	/**
	 * @return true if this HDBNode is a root node (has no parent).
	 */
	bool isRoot() const { return (hasParent() == false); }
	/**
	 * @return true if this HDBNode is a child.
	 */
	bool isChild() const { return (hasParent() == true); }
	/**
	 * @return true if this HDBNode has any siblings.
	 */
	bool isSibling() const
	{
		return (hasNextSibling() > 0 || hasPreviousSibling() > 0);
	}
	/**
	 * @return true if this HDBNode is valid.
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
	HDBNode(const char* key, HDBHandle& hdl);
	HDBNode(Int32 offset, HDBHandle& hdl);
	void read(Int32 offset, HDBHandle& hdl);
	bool reload(HDBHandle& hdl);
	enum EWriteHeaderFlag
	{
		E_WRITE_ALL,
		E_WRITE_ONLY_HEADER
	};
	Int32 write(HDBHandle& hdl, EWriteHeaderFlag onlyHeader = E_WRITE_ALL);
	void updateOffsets(HDBHandle& hdl, Int32 offset);
	Int32 getParentOffset() const { return m_pdata->m_blk.parent; }
	Int32 getFirstChildOffset() const { return m_pdata->m_blk.firstChild; }
	Int32 getLastChildOffset() const { return m_pdata->m_blk.lastChild; }
	Int32 getNextSiblingOffset() const { return m_pdata->m_blk.nextSib; }
	Int32 getPrevSiblingOffset() const { return m_pdata->m_blk.prevSib; }
	Int32 getOffset() const { return m_pdata->m_offset; }
	bool remove(HDBHandle& hdl);
	void removeBlock(HDBHandle& hdl, HDBBlock& fblk, Int32 offset);
	void addChild(HDBHandle& hdl, HDBNode& arg);
	bool updateData(HDBHandle& hdl, int dataLen, const unsigned char* data);
	void setNull() { m_pdata = 0; }
	Reference<HDBNodeData> m_pdata;
	friend class HDBHandle;
};

} // end namespace OpenWBEM

#endif
