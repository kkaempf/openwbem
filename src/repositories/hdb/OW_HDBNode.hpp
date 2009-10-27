/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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

#ifndef OW_HDBNODE_HPP_INCLUDE_GUARD_
#define OW_HDBNODE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_HDBCommon.hpp"
#include "blocxx/String.hpp"
#include "blocxx/IntrusiveReference.hpp"
#include "blocxx/IntrusiveCountableBase.hpp"
#include "blocxx/SafeBool.hpp"

namespace OW_NAMESPACE
{

class HDBHandle;
class HDB;
//////////////////////////////////////////////////////////////////////////////
class OW_HDB_API HDBNode
{
private:
	struct HDBNodeData : public blocxx::IntrusiveCountableBase
	{
		HDBNodeData();
		HDBNodeData(const HDBNodeData& x);
		~HDBNodeData();
		HDBNodeData& operator= (const HDBNodeData& x);

		HDBBlock m_blk;
		blocxx::String m_key;
		blocxx::Int32 m_bfrLen;
		unsigned char* m_bfr;
		blocxx::Int32 m_offset;
		blocxx::Int32 m_version;
	};
	typedef blocxx::IntrusiveReference<HDBNodeData> HDBNodeDataRef;

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
	HDBNode(const blocxx::String& key, int dataLen, const unsigned char* data);
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
	blocxx::UInt32 getFlags()
	{
		return m_pdata->m_blk.flags;
	}
	/**
	 * Determine if all of the user defined flags are on.
	 * @param flags	The user defined flags to check.
	 * @return true if all flags are on in this node. Othewise false.
	 */
	bool areAllFlagsOn(blocxx::UInt32 flags) const
	{
		return ((m_pdata->m_blk.flags & flags) == flags);
	}
	/**
	 * Determine if some of the user defined flags are on.
	 * @param flags	The user defined flags to check.
	 * @return true if some flags are on in this node. Othewise false.
	 */
	bool areSomeFlagsOn(blocxx::UInt32 flags) const
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
	bool turnFlagsOn(HDBHandle& hdl, blocxx::UInt32 flags);
	/**
	 * Turn the user defined flags off in this node.
	 * @param hdl
	 * @param flags	The flags to turn off in this node.
	 * @return true if the flags were changed from this operation. Otherwise
	 * false.
	 */
	bool turnFlagsOff(HDBHandle& hdl, blocxx::UInt32 flags);
	/**
	 * @return The key associated with this HDBNode.
	 */
	blocxx::String getKey() const { return m_pdata->m_key; }
	/**
	 * @return The length of the data associated with this HDBNode.
	 */
	blocxx::Int32 getDataLen() const { return m_pdata->m_bfrLen; }
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
		return ( hasNextSibling() || hasPreviousSibling() );
	}

	/**
	 * @return true if this HDBNode is valid.
	 */
	BLOCXX_SAFE_BOOL_IMPL(HDBNode, HDBNodeDataRef, HDBNode::m_pdata, m_pdata)

private:
	HDBNode(const char* key, HDBHandle& hdl);
	HDBNode(blocxx::Int32 offset, HDBHandle& hdl);
	void read(blocxx::Int32 offset, HDBHandle& hdl);
	bool reload(HDBHandle& hdl);
	enum EWriteHeaderFlag
	{
		E_WRITE_ALL,
		E_WRITE_ONLY_HEADER
	};
	blocxx::Int32 write(HDBHandle& hdl, EWriteHeaderFlag onlyHeader = E_WRITE_ALL);
	void updateOffsets(HDBHandle& hdl, blocxx::Int32 offset);
	blocxx::Int32 getParentOffset() const { return m_pdata->m_blk.parent; }
	blocxx::Int32 getFirstChildOffset() const { return m_pdata->m_blk.firstChild; }
	blocxx::Int32 getLastChildOffset() const { return m_pdata->m_blk.lastChild; }
	blocxx::Int32 getNextSiblingOffset() const { return m_pdata->m_blk.nextSib; }
	blocxx::Int32 getPrevSiblingOffset() const { return m_pdata->m_blk.prevSib; }
	blocxx::Int32 getOffset() const { return m_pdata->m_offset; }
	bool remove(HDBHandle& hdl);
	void removeBlock(HDBHandle& hdl, HDBBlock& fblk, blocxx::Int32 offset);
	void addChild(HDBHandle& hdl, HDBNode& arg);
	bool updateData(HDBHandle& hdl, const HDBNode& parentNode, int dataLen, const unsigned char* data);
	void updateParent(HDBHandle& hdl, const HDBNode& parentNode);
	void detachFromParent(HDBHandle& hdl);
	void addToNewParent(HDBHandle& hdl, HDBNode& parentNode);
	void setNull() { m_pdata = 0; }
	blocxx::String debugString() const;

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	HDBNodeDataRef m_pdata;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	friend class HDBHandle;
};

} // end namespace OW_NAMESPACE

#endif
