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
#ifndef OW_ASSOCDB2_HPP_INCLUDE_GUARD_
#define OW_ASSOCDB2_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_Array.hpp"
#include "OW_String.hpp"
#include "OW_File.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_SortedVectorSet.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_dbDatabase.hpp"

#include <db.h>

namespace OpenWBEM
{

/**
 * The AssocDbEntry represents an entry in the association database.
 */
class AssocDbEntry2
{
public:
	AssocDbEntry2(std::istream& istrm);
	AssocDbEntry2()
		: m_objectName(CIMNULL)
		, m_offset(-1L)
	{}
	AssocDbEntry2(const CIMObjectPath& objectName,
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
std::ostream& operator << (std::ostream& ostrm, const AssocDbEntry2& arg);
typedef Array<AssocDbEntry2> AssocDbEntry2Array;
bool operator==(const AssocDbEntry2::entry& lhs, const AssocDbEntry2::entry& rhs);
//////////////////////////////////////////////////////////////////////////////
class AssocDb2;
//////////////////////////////////////////////////////////////////////////////
typedef ResultHandlerIFC<AssocDbEntry2::entry> AssocDbEntry2ResultHandlerIFC;
	
class AssocDb2
{
public:
	/**
	 * Create a new AssocDb object.
	 */
	AssocDb2(ServiceEnvironmentIFCRef env);
	/**
	 * Destroy this AssocDb object.
	 */
	~AssocDb2();
	/**
	 * Open this AssocDb object up for business.
	 */
	void open(const String& fileName, ::DB_ENV* env, ::DB_TXN* txn);
	/**
	 * Close this AssocDb object
	 */
	void close();
	/**
	 * @return true if this AssocDb is currently opened.
	 */
	/**
	 * @return true if there are association entries in the association
	 * database for the given target object.
	 */
	bool hasAssocEntries(const String& ns, const CIMObjectPath& instanceName);
	/**
	 * Add an AssocDbEntry& to the database.
	 * @param newEntry	The AssocDbEntry to add to the database.
	 */
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
	void deleteEntry(const CIMObjectPath& objectName,
		const String& assocClassName, const String& resultClass,
		const String& role, const String& resultRole,
		const CIMObjectPath& associatedObject,
		const CIMObjectPath& assocClassPath);
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
		AssocDbEntry2ResultHandlerIFC& result);
private:
	AssocDbEntry2 findEntry(const String& objectKey);
	void deleteEntry(const AssocDbEntry2& entry);
	void addEntry(const AssocDbEntry2& entry);
	void addOrDeleteEntries(const String& ns, const CIMInstance& assocInstance, bool add);
	void addOrDeleteEntries(const String& ns, const CIMClass& assocClass, bool add);
	ServiceEnvironmentIFCRef m_env;
	dbDatabase m_db;
};

} // end namespace OpenWBEM

#endif
