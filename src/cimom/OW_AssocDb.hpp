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

#ifndef OW_ASSOCDB_HPP_INCLUDE_GUARD_
#define OW_ASSOCDB_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Index.hpp"
#include "OW_Array.hpp"
#include "OW_String.hpp"
#include "OW_File.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_SortedVectorSet.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_ResultHandlerIFC.hpp"

/**
 * The OW_AssocDbEntry represents an entry in the association database.
 */
class OW_AssocDbEntry
{
public:
	OW_AssocDbEntry(std::istream& istrm);

	OW_AssocDbEntry() 
		: m_objectName(OW_CIMNULL)
		, m_offset(-1L)
	{}

	OW_AssocDbEntry(const OW_CIMObjectPath& objectName,
		const OW_String& role,
		const OW_String& resultRole);

	//OW_Bool operator == (const OW_AssocDbEntry& arg) const;

	struct entry
	{
		entry ()
			: m_associatedObject(OW_CIMNULL)
			, m_associationPath(OW_CIMNULL)
		{}

		OW_String m_assocClass;
		OW_String m_resultClass;
		OW_CIMObjectPath m_associatedObject; // value for associtor(Name)s
		OW_CIMObjectPath m_associationPath;  // value for reference(Name)s
		
		void writeObject(std::ostream& ostrm) const;
		void readObject(std::istream& istrm);
	};

	void writeObject(std::ostream& ostrm) const;
	void readObject(std::istream& istrm);

	OW_Int32 getOffset() const { return m_offset; }
	void setOffset(OW_Int32 offset) { m_offset = offset; }

	static OW_String makeKey(const OW_CIMObjectPath& objectName, const OW_String& role,
		const OW_String& resultRole);
	
	OW_String makeKey() const;

private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (m_objectName) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (m_objectName) ? 0: &dummy::nonnull; }

public:

	OW_CIMObjectPath m_objectName; // part 1 of key
	OW_String m_role; // part 2 of key
	OW_String m_resultRole; // part 3 of key

	OW_Array<entry> m_entries;

	OW_Int32 m_offset;
};

std::ostream& operator << (std::ostream& ostrm, const OW_AssocDbEntry& arg);

typedef OW_Array<OW_AssocDbEntry> OW_AssocDbEntryArray;

bool operator==(const OW_AssocDbEntry::entry& lhs, const OW_AssocDbEntry::entry& rhs);

//////////////////////////////////////////////////////////////////////////////

class OW_AssocDb;

//////////////////////////////////////////////////////////////////////////////
typedef OW_ResultHandlerIFC<OW_AssocDbEntry::entry> OW_AssocDbEntryResultHandlerIFC;


class OW_AssocDbHandle
{
private:

	struct AssocDbHandleData
	{
		AssocDbHandleData();
		AssocDbHandleData(const AssocDbHandleData& arg);
		AssocDbHandleData(OW_AssocDb* pdb, OW_File file);
		~AssocDbHandleData();
		AssocDbHandleData& operator= (const AssocDbHandleData& arg);

		OW_AssocDb* m_pdb;
		OW_File m_file;
	};

public:

	OW_AssocDbHandle() : m_pdata(NULL) {}

	OW_AssocDbHandle(const OW_AssocDbHandle& arg) : m_pdata(arg.m_pdata) {}

	OW_AssocDbHandle& operator= (const OW_AssocDbHandle& arg)
	{
		m_pdata = arg.m_pdata;
		return *this;
	}

	/**
	 * @return true if there are association entries in the association
	 * database for the given target object.
	 */
	OW_Bool hasAssocEntries(const OW_String& ns, const OW_CIMObjectPath& instanceName);

	/**
	 * Add an OW_AssocDbEntry& to the database.
	 * @param newEntry	The OW_AssocDbEntry to add to the database.
	 */
	//void addEntry(const OW_AssocDbEntry& newEntry);
	void addEntry(const OW_CIMObjectPath& objectName, 
		const OW_String& assocClassName, const OW_String& resultClass,
		const OW_String& role, const OW_String& resultRole, 
		const OW_CIMObjectPath& associatedObject, 
		const OW_CIMObjectPath& assocClassPath);

	/**
	 * Add all entries to the database that are reference by the
	 * given association.
	 * @param assocKey	The key to the association instance.
	 * @param 	assocInstance The instance of the association referenced by
	 *				assocKey
	 */
	void addEntries(const OW_String& ns, const OW_CIMInstance& assocInstance);
	void addEntries(const OW_String& ns, const OW_CIMClass& assocClass);

	/**
	 * Remove an OW_AssocDbEntry& from the database.
	 * @param entryToDelete	The OW_AssocDbEntry to delete from the database.
	 */
	//void deleteEntry(const OW_AssocDbEntry& entryToDelete);
	void deleteEntry(const OW_CIMObjectPath& objectName, 
		const OW_String& assocClassName, const OW_String& resultClass,
		const OW_String& role, const OW_String& resultRole, 
		const OW_CIMObjectPath& associatedObject, 
		const OW_CIMObjectPath& assocClassPath);

	/**
	 * Remove all OW_AssocDbEntry objects specified in an array.
	 * @param entryra	The Array OW_AssocDbEntry objects to delete.
	 */
	//void deleteEntries(const OW_AssocDbEntryArray& entryra);

	/**
	 * Remove all entries from the database that are reference by the
	 * given association.
	 * @param assocKey	The key to the association instance.
	 * @param 	assocInstance The instance of the association referenced by
	 *				assocKey
	 */
	void deleteEntries(const OW_String& ns, const OW_CIMInstance& assocInstance);
	void deleteEntries(const OW_String& ns, const OW_CIMClass& assocClass);

	/**
	 * Delete all entries in the association database that referenct the
	 * given target object name.
	 * @param objectName	The name of the object to delete the entries for.
	 *									This is the string version of the object path for
	 *									the target object.
	 */
	//void deleteEntries(const OW_String& objectName);

	/**
	 * Get all of the OW_AssocDbEntries that fit the given criterion.
	 * @param objectName	The target object that all entries must have.
	 * @param assocClasses	If specified, the association class name from all
	 *								entries must be found in this array.
	 * @param propertyName	All entries must have this property name.
	 * @return An OW_AssocDbEntryArray that contains all of the entries that
	 * meet the given criterion.
	 */
	void getAllEntries(const OW_CIMObjectPath& objectName,
		const OW_SortedVectorSet<OW_String>* passocClasses,
		const OW_SortedVectorSet<OW_String>* presultClasses,
		const OW_String& role,
		const OW_String& resultRole,
		OW_AssocDbEntryResultHandlerIFC& result);

	OW_File getFile() const { return m_pdata->m_file; }

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
	
	void addOrDeleteEntries(const OW_String& ns, const OW_CIMInstance& assocInstance, bool add);
	void addOrDeleteEntries(const OW_String& ns, const OW_CIMClass& assocClass, bool add);
	OW_AssocDbHandle(OW_AssocDb* pdb, OW_File file) :
		m_pdata(new AssocDbHandleData(pdb, file)) {}

	OW_Reference<AssocDbHandleData> m_pdata;

	friend class OW_AssocDb;
};


// The following structure represents the format of header that
// preceeds all records in the associations db
// TODO: Fix all the code that uses this thing that assumes it's all packed data.
struct AssocDbRecHeader
{
	AssocDbRecHeader() { memset(this, 0, sizeof(*this)); }

	OW_UInt32 chkSum;
	OW_Int32 nextFree;
	OW_UInt32 blkSize;
	OW_UInt32 flags;
	size_t dataSize;
};

#define OW_ASSOCSIGNATURE "OWASSOCIATORFILE"
#define OW_ASSOCSIGLEN 17

// The following structure represents the file header for the
// associations database
struct OW_AssocDbHeader
{
	char signature[OW_ASSOCSIGLEN];
	OW_Int32 firstFree;
};

class OW_AssocDb
{
public:

	/**
	 * Create a new OW_AssocDb object.
	 */
	OW_AssocDb(OW_CIMOMEnvironmentRef env);

	/**
	 * Destroy this OW_AssocDb object.
	 */
	~OW_AssocDb();

	/**
	 * Open this OW_AssocDb object up for business.
	 * @param fileName	The file name associated with the database
	 * @exception OW_HDBException if an error occurs opening/creating files.
	 */
	void open(const OW_String& fileName);

	/**
	 * Close this OW_AssocDb object
	 */
	void close();

	/**
	 * Create a new OW_HDBHandle that is associated with this OW_HDB object.
	 * @return An OW_HDBHandle object that can be used to operate on this
	 * OW_HDB object.
	 * @exception OW_HDBException if this OW_HDB is not opened.
	 */
	OW_AssocDbHandle getHandle();

	/**
	 * @return true if this OW_AssocDb is currently opened.
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
	 * @return The number of outstanding handles on this OW_AssocDb
	 */
	int getHandleCount() const { return m_hdlCount; }

	/**
	 * @return The file name for this OW_HDB object
	 */
	OW_String getFileName() const { return m_fileName; }

private:

	OW_AssocDbEntry findEntry(const OW_String& objectKey,
		OW_AssocDbHandle& hdl);
	OW_AssocDbEntry nextEntry(OW_AssocDbHandle& hdl);
	void deleteEntry(const OW_AssocDbEntry& entry, OW_AssocDbHandle& hdl);
	void deleteEntry(const OW_CIMObjectPath& objectName, 
		const OW_String& assocClassName, const OW_String& resultClass,
		const OW_String& role, const OW_String& resultRole, 
		const OW_CIMObjectPath& associatedObject, 
		const OW_CIMObjectPath& assocClassPath, OW_AssocDbHandle& hdl);

	void addEntry(const OW_AssocDbEntry& entry, OW_AssocDbHandle& hdl);
	void addEntry(const OW_CIMObjectPath& objectName, 
		const OW_String& assocClassName, const OW_String& resultClass,
		const OW_String& role, const OW_String& resultRole, 
		const OW_CIMObjectPath& associatedObject, 
		const OW_CIMObjectPath& assocClassPath, OW_AssocDbHandle& hdl);

	void decHandleCount();
	OW_MutexLock getDbLock() { return OW_MutexLock(m_guard); }

	OW_AssocDbEntry readEntry(OW_Int32 offset, OW_AssocDbHandle& hdl);
	void addToFreeList(OW_Int32 offset, OW_AssocDbHandle& hdl);
	AssocDbRecHeader getNewBlock(OW_Int32& offset, OW_UInt32 blkSize,
		OW_AssocDbHandle& hdl);

	OW_Bool createFile();
	OW_Bool checkFile();

	OW_AssocDbHeader m_hdrBlock;
	OW_IndexRef m_pIndex;
	OW_String m_fileName;
	int m_hdlCount;
	OW_Bool m_opened;
	OW_Mutex m_guard;
	OW_CIMOMEnvironmentRef m_env;

	friend class OW_AssocDbHandle;
	friend struct OW_AssocDbHandle::AssocDbHandleData;
};

#endif

