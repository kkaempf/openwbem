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

#ifndef OW_GENERICHDBREPOSITORY_HPP_INCLUDE_GUARD_
#define OW_GENERICHDBREPOSITORY_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_HDB.hpp"
#include "OW_Array.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMBase.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_Logger.hpp"

namespace OpenWBEM
{

class HDBHandleLock;
class OW_HDB_API GenericHDBRepository
{
public:
	/**
	 * Create a new GenericHDBRepository object.
	 * @param path	The path to where the repository files are or will be
	 *					located.
	 * @exception HDBException
	 */
	GenericHDBRepository(ServiceEnvironmentIFCRef env);
	/**
	 * Destroy this GenericHDBRepository object
	 */
	virtual ~GenericHDBRepository();
	/**
	 * Open this GenericHDBRepository.
	 * @param path	The path to where the repository files are or will be
	 *					located.
	 * @exception Exception
	 * @exception HDBException
	 */
	virtual void open(const String& path);
	/**
	 * Close this GenericHDBRepository.
	 */
	virtual void close();
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
	/**
	 * Create the necessary containers to make a valid path. Fail if the
	 * last container already exists.
	 * @param ns	The name of the namespace to create
	 * @return 0 on success. Otherwise -1 if the bottom most container already
	 * exists.
	 */
	virtual int createNameSpace(const String& ns);
	/**
	 * Delete a container and all of its' children
	 * @param key	The key to the container to remove
	 * @exception IOException If the database is not opened.
	 * @exception HDBException
	 */
	void deleteNameSpace(const String& key);
#endif
	/**
	 * Check whether a namespace exists
	 * @param key	The key to the namespace
	 * @exception IOException If the database is not opened.
	 * @exception HDBException
	 * @return Whether the namespace key exists.
	 */
	bool nameSpaceExists(const String& key);
	/**
	 * @return true if this repository is open. Otherwise false.
	 */
	bool isOpen() { return m_opened; }
	void nodeToCIMObject(CIMBase& cimObj, const HDBNode& node);
	void getCIMObject(CIMBase& cimObj, const String& key,
		HDBHandle hdl);
	void updateCIMObject(const CIMBase& cimObj, HDBNode& node,
		HDBHandle hdl);
	void addCIMObject(const CIMBase& cimObj, const String& key,
		HDBNode& parentNode, HDBHandle hdl, UInt32 nodeFlags=0);
	void addCIMObject(const CIMBase& cimObj, const String& key,
		HDBHandle hdl, UInt32 nodeFlags=0);
	/**
	 * @return An HDBHandle object from the underlying HDB object.
	 * freeHandle must be called later on the handle that is returned from
	 * this method.The HDBHandleLock is a nice way to handle the return
	 * value from this method. It will automatically call free handle when it
	 * goes out of scope.
	 */
	HDBHandle getHandle();
	/**
	 * Free a and that was previously acquired by a call to getHandle. This
	 * method MUST be called on all HDBHandles acquired through getHandle.
	 * @param hdl	A reference to the handle previously acquired through a call
	 *					to getHandle.
	 */
	void freeHandle(HDBHandle& hdl);
	/**
	 * Get the name space node for a given key
	 * @param key	The key for the name space node.
	 * @return On success the HDBNode associated with the given key.
	 * Otherwise a null HDBNode
	 */
	HDBNode getNameSpaceNode(HDBHandleLock& hdl, String key);
	void logFatalError(const String& s) { m_env->getLogger()->logFatalError(s); }
	void logError(const String& s) { m_env->getLogger()->logError(s); }
	void logInfo(const String& s) { m_env->getLogger()->logInfo(s); }
	void logDebug(const String& s) { m_env->getLogger()->logDebug(s); }
protected:
	/**
	 * Throw an IOException is this repository is not open.
	 * @exception IOException If this repository is not open.
	 */
	void throwIfNotOpen()
	{
		if (!isOpen())
			OW_THROW(HDBException, "Database is not open");
	}

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	HDB m_hdb;
	bool m_opened;
	Mutex m_guard;
	Array<HDBHandle> m_handles;
	ServiceEnvironmentIFCRef m_env;
	enum { MAXHANDLES = 10 };
	friend class HDBHandleLock;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

};
/**
 * The HDBHandleLock class helps facilitate the releasing of HDBHandle
 * objects that are acquired through calls to GenericHDBRepository::
 * getHandle. Instances of HDBHandleLock will automatically call
 * GenericHDBRepository::freeHandle when they go out of scope. Instances
 * of this class are essentially smart pointers for HDBHandle instances
 * returned from GenericHDBRepository::getHandle.
 */
class OW_HDB_API HDBHandleLock
{
public:
	HDBHandleLock(GenericHDBRepository* pr, const HDBHandle& hdl) :
		m_pr(pr), m_hdl(hdl) {}
	~HDBHandleLock()
	{
		try
		{
			if (m_pr) m_pr->freeHandle(m_hdl);
		}
		catch (...)
		{
			// don't let exceptions escape
		}
	}
	HDBHandle* operator->() { return &m_hdl; }
	HDBHandle& operator*() { return m_hdl; }
	HDBHandle getHandle() { return m_hdl; }
private:
	GenericHDBRepository* m_pr;
	HDBHandle m_hdl;
};
static const UInt32 HDBNSNODE_FLAG = 0x40000000;
static const UInt32 HDBCLSNODE_FLAG = 0x20000000;
static const UInt32 HDBCLSASSOCNODE_FLAG = 0x08000000;

} // end namespace OpenWBEM

#endif
