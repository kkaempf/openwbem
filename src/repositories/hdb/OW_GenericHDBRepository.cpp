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
#include "OW_config.h"
#include "OW_GenericHDBRepository.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMException.hpp"
#include "OW_DataStreams.hpp"
#include "OW_IOException.hpp"
#include "OW_Format.hpp"

namespace OpenWBEM
{

static const int HDL_NOTINUSE = -1;
static const int HDL_NOTCACHED = -2;
//////////////////////////////////////////////////////////////////////////////
GenericHDBRepository::GenericHDBRepository(ServiceEnvironmentIFCRef env)
	: m_hdb()
	, m_opened(false)
	, m_guard()
	, m_handles()
	, m_env(env)
{
}
//////////////////////////////////////////////////////////////////////////////
GenericHDBRepository::~GenericHDBRepository()
{
	try
	{
		close();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
HDBHandle
GenericHDBRepository::getHandle()
{
	MutexLock ml(m_guard);
	for(int i = 0; i < int(m_handles.size()); i++)
	{
		if(m_handles[i].getUserValue() == HDL_NOTINUSE)
		{
			m_handles[i].setUserValue(i);		// Set user value to index
			return m_handles[i];
		}
	}
	HDBHandle hdl = m_hdb.getHandle();
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
GenericHDBRepository::freeHandle(HDBHandle& hdl)
{
	MutexLock ml(m_guard);
	Int32 uv = hdl.getUserValue();
	if(uv > HDL_NOTINUSE && uv < Int32(m_handles.size()))
	{
		// Handle is from the cache, so flag it as not in use.
		hdl.flush();
		m_handles[uv].setUserValue(HDL_NOTINUSE);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
GenericHDBRepository::open(const String& path)
{
	MutexLock ml(m_guard);
	close();
	m_hdb.open(path.c_str());
	m_opened = true;
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	// Create root namespace
	createNameSpace("root");
#endif
	//HDBHandleLock hdl(this, getHandle());
	//String contk("root");
	//contk.toLowerCase();
	//HDBNode node = hdl->getNode(contk);
	//if(!node)
	//{
	//	node = HDBNode(contk, contk.length()+1,
	//		reinterpret_cast<const unsigned char*>(contk.c_str()));
	//	hdl->turnFlagsOn(node, HDBNSNODE_FLAG);
	//	hdl->addRootNode(node);
	//}
}
//////////////////////////////////////////////////////////////////////////////
void
GenericHDBRepository::close()
{
	MutexLock ml(m_guard);
	if(!m_opened)
	{
		return;
	}
	m_opened = false;
	for(int i = 0; i < int(m_handles.size()); i++)
	{
		if(m_handles[i].getUserValue() > HDL_NOTINUSE)
		{
			//cerr << "GenericHDBRepository::close HANDLES ARE STILL IN USE!!!!"
			//	<< endl;
			break;
		}
	}
	m_handles.clear();
	m_hdb.close();
}
//////////////////////////////////////////////////////////////////////////////
HDBNode
GenericHDBRepository::getNameSpaceNode(HDBHandleLock& hdl,
	String ck)
{
	if(ck.empty())
	{
		return HDBNode();
	}
	HDBNode node = hdl->getNode(ck);
	if(node)
	{
		if(!node.areAllFlagsOn(HDBNSNODE_FLAG))
		{
			OW_THROW(IOException, "logic error. Expected namespace node");
		}
	}
	return node;
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
int
GenericHDBRepository::createNameSpace(String ns)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	HDBNode node;
	if(ns.empty())
	{
		return -1;
	}
	node = hdl->getNode(ns);
	if(!node)
	{
		// create the namespace
		node = HDBNode(ns, ns.length()+1,
			reinterpret_cast<const unsigned char*>(ns.c_str()));
		hdl->turnFlagsOn(node, HDBNSNODE_FLAG);
		hdl->addRootNode(node);
		logDebug(Format("created namespace %1", ns));
	}
	else
	{
		// it already exists, return -1.
		if(!node.areAllFlagsOn(HDBNSNODE_FLAG))
		{
			OW_THROW(IOException,
				"logic error. read namespace node that is not a namespace");
		}
		return -1;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
void
GenericHDBRepository::deleteNameSpace(String key)
{
	throwIfNotOpen();
	if(key.equals("root"))
	{
		OW_THROWCIMMSG(CIMException::FAILED,
			"cannot delete root namespace");
	}
	HDBHandleLock hdl(this, getHandle());
	HDBNode node = hdl->getNode(key);
	if(node)
	{
		if(!node.areAllFlagsOn(HDBNSNODE_FLAG))
		{
			OW_THROW(IOException, "logic error. deleting non-namespace node");
		}
		hdl->removeNode(node);
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, Format("Unable to delete namespace %1", key).c_str());
	}
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool
GenericHDBRepository::nameSpaceExists(String key)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	HDBNode node = hdl->getNode(key);
	if(node)
	{
		if(!node.areAllFlagsOn(HDBNSNODE_FLAG))
		{
			return false;
		}
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////
void
GenericHDBRepository::nodeToCIMObject(CIMBase& cimObj,
	const HDBNode& node)
{
	if(node)
	{
		DataIStream istrm(node.getDataLen(), node.getData());
		cimObj.readObject(istrm);
	}
	else
	{
		cimObj.setNull();
	}
}
//////////////////////////////////////////////////////////////////////////////
void
GenericHDBRepository::getCIMObject(CIMBase& cimObj, const String& key,
	HDBHandle hdl)
{
	nodeToCIMObject(cimObj, hdl.getNode(key));
}
//////////////////////////////////////////////////////////////////////////////
void
GenericHDBRepository::updateCIMObject(const CIMBase& cimObj,
	HDBNode& node, HDBHandle hdl)
{
	DataOStream ostrm;
	cimObj.writeObject(ostrm);
	hdl.updateNode(node, ostrm.length(), ostrm.getData());
}
//////////////////////////////////////////////////////////////////////////////
void
GenericHDBRepository::addCIMObject(const CIMBase& cimObj,
	const String& key, HDBNode& parentNode, HDBHandle hdl,
	UInt32 nodeFlags)
{
	DataOStream ostrm;
	cimObj.writeObject(ostrm);
	HDBNode node(key, ostrm.length(), ostrm.getData());
	node.turnFlagsOn(hdl, nodeFlags);
	hdl.addChild(parentNode, node);
}
//////////////////////////////////////////////////////////////////////////////
void
GenericHDBRepository::addCIMObject(const CIMBase& cimObj,
	const String& key, HDBHandle hdl, UInt32 nodeFlags)
{
	DataOStream ostrm;
	cimObj.writeObject(ostrm);
	HDBNode node(key, ostrm.length(), ostrm.getData());
	node.turnFlagsOn(hdl, nodeFlags);
	hdl.addRootNode(node);
}

} // end namespace OpenWBEM

