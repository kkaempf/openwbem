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
#include "OW_GenericHDBRepository.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMException.hpp"
#include "OW_RepositoryStreams.hpp"

#define HDL_NOTINUSE		-1
#define HDL_NOTCACHED	-2

//////////////////////////////////////////////////////////////////////////////
OW_GenericHDBRepository::OW_GenericHDBRepository(OW_CIMOMEnvironmentRef env)
	: m_hdb()
	, m_opened(false)
	, m_guard()
	, m_handles()
	, m_env(env)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_GenericHDBRepository::~OW_GenericHDBRepository()
{
	close();
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBHandle
OW_GenericHDBRepository::getHandle()
{
	OW_MutexLock ml(m_guard);
	for(int i = 0; i < int(m_handles.size()); i++)
	{
		if(m_handles[i].getUserValue() == HDL_NOTINUSE)
		{
			m_handles[i].setUserValue(i);		// Set user value to index
			return m_handles[i];
		}
	}

	OW_HDBHandle hdl = m_hdb.getHandle();
	if(m_handles.size() < MAXHANDLES)
	{
		hdl.setUserValue(m_handles.size());
		m_handles.append(hdl);
	}
	else
	{
		hdl.setUserValue(HDL_NOTCACHED);
	}

	return hdl;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_GenericHDBRepository::freeHandle(OW_HDBHandle& hdl)
{
	OW_MutexLock ml(m_guard);
	long uv = hdl.getUserValue();
	if(uv > HDL_NOTINUSE && uv < long(m_handles.size()))
	{
		// Handle is from the cache, so flag it as not in use.
		hdl.flush();
		m_handles[uv].setUserValue(HDL_NOTINUSE);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_GenericHDBRepository::open(const OW_String& path)
{
	OW_MutexLock ml(m_guard);
	close();
	m_hdb.open(path.c_str());
	m_opened = true;

	// Create root container
	OW_HDBHandleLock hdl(this, getHandle());
	OW_String contk(OW_ROOT_CONTAINER);
	contk.toLowerCase();
	OW_HDBNode node = hdl->getNode(contk);
	if(!node)
	{
		node = OW_HDBNode(contk, contk.length()+1,
			(const unsigned char*) contk.c_str());
		hdl->turnFlagsOn(node, OW_HDBNSNODE_FLAG);
		hdl->addRootNode(node);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_GenericHDBRepository::close()
{
	OW_MutexLock ml(m_guard);

	if(!m_opened)
	{
		return;
	}

	m_opened = false;
	for(int i = 0; i < int(m_handles.size()); i++)
	{
		if(m_handles[i].getUserValue() > HDL_NOTINUSE)
		{
			//cerr << "OW_GenericHDBRepository::close HANDLES ARE STILL IN USE!!!!"
			//	<< endl;
			break;
		}
	}

	m_handles.clear();
	m_hdb.close();
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_GenericHDBRepository::getNameSpaceNode(OW_HDBHandleLock& hdl,
	const OW_String& key)
{
	OW_String ck(key);
	if(ck.length() == 0)
	{
		ck = OW_ROOT_CONTAINER;
	}

	OW_HDBNode node = hdl->getNode(ck.toLowerCase());
	if(node)
	{
		if(!node.areAllFlagsOn(OW_HDBNSNODE_FLAG))
		{
			OW_THROW(OW_IOException, "logic error. Expected namespace node");
		}
	}

	return node;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_GenericHDBRepository::createNameSpace(const OW_StringArray& nameComps,
	OW_Bool rootCheck)
{
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node;
	OW_HDBNode pnode;
	OW_String ks;

	if(nameComps.size() == 0)
	{
		return -1;
	}

	if(rootCheck)
	{
		if(!nameComps[0].equalsIgnoreCase(OW_ROOT_CONTAINER))
		{
			pnode = this->getNameSpaceNode(hdl, OW_ROOT_CONTAINER);
			if(!pnode)
			{
				OW_THROW(OW_IOException, "root container node is missing!");
			}
			ks = OW_ROOT_CONTAINER;
		}
		else
		{
			if(nameComps.size() == 1)
			{
				return -1;
			}
		}
	}

	size_t sz = nameComps.size() - 1;
	for(size_t i = 0; i < sz; i++)
	{
		if(ks.length() > 0)
		{
			ks += "/";
		}

		ks += nameComps[i].toString().toLowerCase();
		node = hdl->getNode(ks);
		if(!node)
		{
			node = OW_HDBNode(ks, ks.length()+1,
				(const unsigned char*) ks.c_str());
			hdl->turnFlagsOn(node, OW_HDBNSNODE_FLAG);

			if(!pnode)
			{
				hdl->addRootNode(node);
			}
			else
			{
				hdl->addChild(pnode, node);
			}
		}
		else
		{
			if(!node.areAllFlagsOn(OW_HDBNSNODE_FLAG))
			{
				OW_THROW(OW_IOException,
					"logic error. read namespace node that is not a namespace");
			}
		}

		pnode = node;
	}

	OW_ASSERT(pnode);

	if(ks.length() > 0)
	{
		ks += "/";
	}

	ks += nameComps[sz].toString().toLowerCase();
	node = hdl->getNode(ks);
	if(node)
	{
		if(!node.areAllFlagsOn(OW_HDBNSNODE_FLAG))
		{
			OW_THROW(OW_IOException,
				"logic error. read namespace node that is not a namespace");
		}

		return -1;
	}

	node = OW_HDBNode(ks, ks.length()+1, (const unsigned char*) ks.c_str());
	hdl->turnFlagsOn(node, OW_HDBNSNODE_FLAG);
	hdl->addChild(pnode, node);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_GenericHDBRepository::deleteNameSpace(const OW_String& key)
{
	throwIfNotOpen();
	if(key.equalsIgnoreCase(OW_ROOT_CONTAINER))
	{
		OW_THROWCIMMSG(OW_CIMException::ACCESS_DENIED,
			"cannot delete root namespace");
	}

	OW_String k(key.toString().toLowerCase());

	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = hdl->getNode(k);
	if(node)
	{
		if(!node.areAllFlagsOn(OW_HDBNSNODE_FLAG))
		{
			OW_THROW(OW_IOException, "logic error. deleting non-namespace node");
		}

		hdl->removeNode(node);
	}
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_GenericHDBRepository::nameSpaceExists(const OW_String& key)
{
	throwIfNotOpen();

	OW_String k(key.toString().toLowerCase());

	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = hdl->getNode(k);
	if(node)
	{
		if(!node.areAllFlagsOn(OW_HDBNSNODE_FLAG))
		{
			return false;
		}

		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_GenericHDBRepository::nodeToCIMObject(OW_CIMBase& cimObj,
	const OW_HDBNode& node)
{
	if(node)
	{
		OW_RepositoryIStream istrm(node.getDataLen(), node.getData());
		cimObj.readObject(istrm);
	}
	else
	{
		cimObj.setNull();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_GenericHDBRepository::getCIMObject(OW_CIMBase& cimObj, const OW_String& key,
	OW_HDBHandle hdl)
{
	nodeToCIMObject(cimObj, hdl.getNode(key));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_GenericHDBRepository::updateCIMObject(const OW_CIMBase& cimObj,
	OW_HDBNode& node, OW_HDBHandle hdl)
{
	OW_RepositoryOStream ostrm;
	cimObj.writeObject(ostrm);
	hdl.updateNode(node, ostrm.length(), ostrm.getData());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_GenericHDBRepository::addCIMObject(const OW_CIMBase& cimObj,
	const OW_String& key, OW_HDBNode& parentNode, OW_HDBHandle hdl,
	OW_UInt32 nodeFlags)
{
	OW_RepositoryOStream ostrm;
	cimObj.writeObject(ostrm);
	OW_HDBNode node(key, ostrm.length(), ostrm.getData());
	node.turnFlagsOn(hdl, nodeFlags);
	hdl.addChild(parentNode, node);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_GenericHDBRepository::addCIMObject(const OW_CIMBase& cimObj,
	const OW_String& key, OW_HDBHandle hdl, OW_UInt32 nodeFlags)
{
	OW_RepositoryOStream ostrm;
	cimObj.writeObject(ostrm);
	OW_HDBNode node(key, ostrm.length(), ostrm.getData());
	node.turnFlagsOn(hdl, nodeFlags);
	hdl.addRootNode(node);
}

