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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_GENERICHDBREPOSITORY_HPP_INCLUDE_GUARD_
#define OW_GENERICHDBREPOSITORY_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_HDB.hpp"
#include "OW_Array.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMBase.hpp"
#include "OW_CIMOMEnvironment.hpp"

#define OW_ROOT_CONTAINER "root"

class OW_HDBHandleLock;

class OW_GenericHDBRepository
{
public:

	/**
	 * Create a new OW_GenericHDBRepository object.
	 * @param path	The path to where the repository files are or will be
	 *					located.
	 * @exception OW_HDBException
	 */
	OW_GenericHDBRepository(OW_CIMOMEnvironmentRef env);

	/**
	 * Destroy this OW_GenericHDBRepository object
	 */
	virtual ~OW_GenericHDBRepository();

	/**
	 * Open this OW_GenericHDBRepository.
	 * @param path	The path to where the repository files are or will be
	 *					located.
	 * @exception OW_Exception
	 * @exception OW_HDBException
	 */
	virtual void open(const OW_String& path);

	/**
	 * Close this OW_GenericHDBRepository.
	 */
	virtual void close();

	/**
	 * Create the necessary containers to make a valid path. Fail if the
	 * last container already exists.
	 * @param nameComps	The names of the idividual containers where the 1st
	 * element in the array is the topmost container.
	 * @return 0 on success. Otherwise -1 if the bottom most container already
	 * exists.
	 */
	virtual int createNameSpace(const OW_StringArray& nameComps,
		OW_Bool rootCheck=true);

	/**
	 * Delete a container and all of its' children
	 * @param key	The key to the container to remove
	 * @exception OW_IOException If the database is not opened.
	 * @exception OW_HDBException
	 */
	void deleteNameSpace(const OW_String& key);

	/**
	 * Check whether a namespace exists
	 * @param key	The key to the namespace
	 * @exception OW_IOException If the database is not opened.
	 * @exception OW_HDBException
	 * @return Whether the namespace key exists.
	 */
	bool nameSpaceExists(const OW_String& key);

	/**
	 * @return true if this repository is open. Otherwise false.
	 */
	OW_Bool isOpen() { return m_opened; }

	void nodeToCIMObject(OW_CIMBase& cimObj, const OW_HDBNode& node);

	void getCIMObject(OW_CIMBase& cimObj, const OW_String& key,
		OW_HDBHandle hdl);

	void updateCIMObject(const OW_CIMBase& cimObj, OW_HDBNode& node,
		OW_HDBHandle hdl);

	void addCIMObject(const OW_CIMBase& cimObj, const OW_String& key,
		OW_HDBNode& parentNode, OW_HDBHandle hdl, OW_UInt32 nodeFlags=0);

	void addCIMObject(const OW_CIMBase& cimObj, const OW_String& key,
		OW_HDBHandle hdl, OW_UInt32 nodeFlags=0);

	/**
	 * @return An OW_HDBHandle object from the underlying OW_HDB object.
	 * freeHandle must be called later on the handle that is returned from
	 * this method.The OW_HDBHandleLock is a nice way to handle the return
	 * value from this method. It will automatically call free handle when it
	 * goes out of scope.
	 */
	OW_HDBHandle getHandle();

	/**
	 * Free a and that was previously acquired by a call to getHandle. This
	 * method MUST be called on all OW_HDBHandles acquired through getHandle.
	 * @param hdl	A reference to the handle previously acquired through a call
	 *					to getHandle.
	 */
	void freeHandle(OW_HDBHandle& hdl);

	/**
	 * Get the name space node for a given key
	 * @param key	The key for the name space node.
	 * @return On success the OW_HDBNode associated with the given key.
	 * Otherwise a null OW_HDBNode
	 */
	OW_HDBNode getNameSpaceNode(OW_HDBHandleLock& hdl, const OW_String& key);

	void logError(const OW_String& s) { m_env->logError(s); }
	void logCustInfo(const OW_String& s) { m_env->logCustInfo(s); }
	void logDebug(const OW_String& s) { m_env->logDebug(s); }

protected:

	/**
	 * Throw an OW_IOException is this repository is not open.
	 * @exception OW_IOException If this repository is not open.
	 */
	void throwIfNotOpen()
	{
		if(!isOpen())
			OW_THROW(OW_HDBException, "Database is not open");
	}

	OW_HDB m_hdb;
	OW_Bool m_opened;
	OW_Mutex m_guard;
	OW_Array<OW_HDBHandle> m_handles;
	OW_CIMOMEnvironmentRef m_env;

	enum { MAXHANDLES = 10 };

	friend class OW_HDBHandleLock;
};

/**
 * The OW_HDBHandleLock class helps facilitate the releasing of OW_HDBHandle
 * objects that are acquired through calls to OW_GenericHDBRepository::
 * getHandle. Instances of OW_HDBHandleLock will automatically call
 * OW_GenericHDBRepository::freeHandle when they go out of scope. Instances
 * of this class are essentially smart pointers for OW_HDBHandle instances
 * returned from OW_GenericHDBRepository::getHandle.
 */
class OW_HDBHandleLock
{
public:
	OW_HDBHandleLock(OW_GenericHDBRepository* pr, const OW_HDBHandle& hdl) :
		m_pr(pr), m_hdl(hdl) {}

	~OW_HDBHandleLock()
	{
		try
		{
			if(m_pr) m_pr->freeHandle(m_hdl);
		}
		catch (...)
		{
			// don't let exceptions escape
		}
	}
	OW_HDBHandle* operator->() { return &m_hdl; }
	OW_HDBHandle& operator*() { return m_hdl; }
	OW_HDBHandle getHandle() { return m_hdl; }
private:
	OW_GenericHDBRepository* m_pr;
	OW_HDBHandle m_hdl;
};

#define OW_HDBNSNODE_FLAG			0x40000000
#define OW_HDBCLSNODE_FLAG			0x20000000
#define OW_HDBCLSASSOCNODE_FLAG	0x08000000

#endif


