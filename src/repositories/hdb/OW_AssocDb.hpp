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
#ifndef OW_ASSOCDB_HPP_INCLUDE_GUARD_
#define OW_ASSOCDB_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_Index.hpp"
#include "OW_Array.hpp"
#include "OW_String.hpp"
#include "OW_File.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_SortedVectorSet.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_ResultHandlerIFC.hpp"

namespace OpenWBEM
{

/**
 * The AssocDbEntry represents an entry in the association database.
 */
class AssocDbEntry
{
public:
	AssocDbEntry(std::istream& istrm);
	AssocDbEntry() 
		: m_objectName(CIMNULL)
		, m_offset(-1L)
	{}
	AssocDbEntry(const CIMObjectPath& objectName,
		const String& role,
		const String& resultRole);
	struct entry
	{
		entry ()
			: m_associatedObject(CIMNULL)
			, m_associationPath(CIMNULL)
		{}
		String m_assocClass;
		String m_resultClass;
		CIMObjectPath m_associatedObject; // value for associtor(Name)s
		CIMObjectPath m_associationPath;  // value for reference(Name)s
		
		void writeObject(std::ostream& ostrm) const;
		void readObject(std::istream& istrm);
	};
	void writeObject(std::ostream& ostrm) const;
	void readObject(std::istream& istrm);
	Int32 getOffset() const { return m_offset; }
	void setOffset(Int32 offset) { m_offset = offset; }
	static String makeKey(const CIMObjectPath& objectName, const String& role,
		const String& resultRole);
	
	String makeKey() const;
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
	CIMObjectPath m_objectName; // part 1 of key
	String m_role; // part 2 of key
	String m_resultRole; // part 3 of key
	Array<entry> m_entries;
	Int32 m_offset;
};
std::ostream& operator << (std::ostream& ostrm, const AssocDbEntry& arg);
typedef Array<AssocDbEntry> AssocDbEntryArray;
bool operator==(const AssocDbEntry::entry& lhs, const AssocDbEntry::entry& rhs);
//////////////////////////////////////////////////////////////////////////////
class AssocDb;
//////////////////////////////////////////////////////////////////////////////
typedef ResultHandlerIFC<AssocDbEntry::entry> AssocDbEntryResultHandlerIFC;
class AssocDbHandle
{
private:
	struct AssocDbHandleData
	{
		AssocDbHandleData();
		AssocDbHandleData(const AssocDbHandleData& arg);
		AssocDbHandleData(AssocDb* pdb, File file);
		~AssocDbHandleData();
		AssocDbHandleData& operator= (const AssocDbHandleData& arg);
		AssocDb* m_pdb;
		File m_file;
	};
public:
	AssocDbHandle() : m_pdata(NULL) {}
	AssocDbHandle(const AssocDbHandle& arg) : m_pdata(arg.m_pdata) {}
	AssocDbHandle& operator= (const AssocDbHandle& arg)
	{
		m_pdata = arg.m_pdata;
		return *this;
	}
	/**
	 * @return true if there are association entries in the association
	 * database for the given target object.
	 */
	bool hasAssocEntries(const String& ns, const CIMObjectPath& instanceName);
	/**
	 * Add an AssocDbEntry& to the database.
	 * @param newEntry	The AssocDbEntry to add to the database.
	 */
	//void addEntry(const AssocDbEntry& newEntry);
	void addEntry(const CIMObjectPath& objectName, 
		const String& assocClassName, const String& resultClass,
		const String& role, const String& resultRole, 
		const CIMObjectPath& associatedObject, 
		const CIMObjectPath& assocClassPath);
	/**
	 * Add all entries to the database that are reference by the
	 * given association.
	 * @param assocKey	The key to the association instance.
	 * @param 	assocInstance The instance of the association referenced by
	 *				assocKey
	 */
	void addEntries(const String& ns, const CIMInstance& assocInstance);
	void addEntries(const String& ns, const CIMClass& assocClass);
	/**
	 * Remove an AssocDbEntry& from the database.
	 * @param entryToDelete	The AssocDbEntry to delete from the database.
	 */
	//void deleteEntry(const AssocDbEntry& entryToDelete);
	void deleteEntry(const CIMObjectPath& objectName, 
		const String& assocClassName, const String& resultClass,
		const String& role, const String& resultRole, 
		const CIMObjectPath& associatedObject, 
		const CIMObjectPath& assocClassPath);
	/**
	 * Remove all AssocDbEntry objects specified in an array.
	 * @param entryra	The Array AssocDbEntry objects to delete.
	 */
	//void deleteEntries(const AssocDbEntryArray& entryra);
	/**
	 * Remove all entries from the database that are reference by the
	 * given association.
	 * @param assocKey	The key to the association instance.
	 * @param 	assocInstance The instance of the association referenced by
	 *				assocKey
	 */
	void deleteEntries(const String& ns, const CIMInstance& assocInstance);
	void deleteEntries(const String& ns, const CIMClass& assocClass);
	/**
	 * Delete all entries in the association database that referenct the
	 * given target object name.
	 * @param objectName	The name of the object to delete the entries for.
	 *									This is the string version of the object path for
	 *									the target object.
	 */
	//void deleteEntries(const String& objectName);
	/**
	 * Get all of the AssocDbEntries that fit the given criterion.
	 * @param objectName	The target object that all entries must have.
	 * @param assocClasses	If specified, the association class name from all
	 *								entries must be found in this array.
	 * @param propertyName	All entries must have this property name.
	 * @return An AssocDbEntryArray that contains all of the entries that
	 * meet the given criterion.
	 */
	void getAllEntries(const CIMObjectPath& objectName,
		const SortedVectorSet<String>* passocClasses,
		const SortedVectorSet<String>* presultClasses,
		const String& role,
		const String& resultRole,
		AssocDbEntryResultHandlerIFC& result);
	File getFile() const { return m_pdata->m_file; }
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
	
	void addOrDeleteEntries(const String& ns, const CIMInstance& assocInstance, bool add);
	void addOrDeleteEntries(const String& ns, const CIMClass& assocClass, bool add);
	AssocDbHandle(AssocDb* pdb, File file) :
		m_pdata(new AssocDbHandleData(pdb, file)) {}
	Reference<AssocDbHandleData> m_pdata;
	friend class AssocDb;
};
// The following structure represents the format of header that
// preceeds all records in the associations db
// TODO: Fix all the code that uses this thing that assumes it's all packed data.
struct AssocDbRecHeader
{
	AssocDbRecHeader() { memset(this, 0, sizeof(*this)); }
	UInt32 chkSum;
	Int32 nextFree;
	UInt32 blkSize;
	UInt32 flags;
	size_t dataSize;
};
#define OW_ASSOCSIGNATURE "OWASSOCIATORFILE"
#define OW_ASSOCSIGLEN 17
// The following structure represents the file header for the
// associations database
struct AssocDbHeader
{
	char signature[OW_ASSOCSIGLEN];
	Int32 firstFree;
};
class AssocDb
{
public:
	/**
	 * Create a new AssocDb object.
	 */
	AssocDb(ServiceEnvironmentIFCRef env);
	/**
	 * Destroy this AssocDb object.
	 */
	~AssocDb();
	/**
	 * Open this AssocDb object up for business.
	 * @param fileName	The file name associated with the database
	 * @exception HDBException if an error occurs opening/creating files.
	 */
	void open(const String& fileName);
	/**
	 * Close this AssocDb object
	 */
	void close();
	/**
	 * Create a new HDBHandle that is associated with this HDB object.
	 * @return An HDBHandle object that can be used to operate on this
	 * HDB object.
	 * @exception HDBException if this HDB is not opened.
	 */
	AssocDbHandle getHandle();
	/**
	 * @return true if this AssocDb is currently opened.
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
	 * @return The number of outstanding handles on this AssocDb
	 */
	int getHandleCount() const { return m_hdlCount; }
	/**
	 * @return The file name for this HDB object
	 */
	String getFileName() const { return m_fileName; }
private:
	AssocDbEntry findEntry(const String& objectKey,
		AssocDbHandle& hdl);
	AssocDbEntry nextEntry(AssocDbHandle& hdl);
	void deleteEntry(const AssocDbEntry& entry, AssocDbHandle& hdl);
	void deleteEntry(const CIMObjectPath& objectName, 
		const String& assocClassName, const String& resultClass,
		const String& role, const String& resultRole, 
		const CIMObjectPath& associatedObject, 
		const CIMObjectPath& assocClassPath, AssocDbHandle& hdl);
	void addEntry(const AssocDbEntry& entry, AssocDbHandle& hdl);
	void addEntry(const CIMObjectPath& objectName, 
		const String& assocClassName, const String& resultClass,
		const String& role, const String& resultRole, 
		const CIMObjectPath& associatedObject, 
		const CIMObjectPath& assocClassPath, AssocDbHandle& hdl);
	void decHandleCount();
	MutexLock getDbLock() { return MutexLock(m_guard); }
	AssocDbEntry readEntry(Int32 offset, AssocDbHandle& hdl);
	void addToFreeList(Int32 offset, AssocDbHandle& hdl);
	AssocDbRecHeader getNewBlock(Int32& offset, UInt32 blkSize,
		AssocDbHandle& hdl);
	bool createFile();
	bool checkFile();
	AssocDbHeader m_hdrBlock;
	IndexRef m_pIndex;
	String m_fileName;
	int m_hdlCount;
	bool m_opened;
	Mutex m_guard;
	ServiceEnvironmentIFCRef m_env;
	friend class AssocDbHandle;
	friend struct AssocDbHandle::AssocDbHandleData;
};

} // end namespace OpenWBEM

#endif
