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
#include "OW_config.h"
#include "OW_AssocDb2.hpp"
#include "OW_DataStreams.hpp"
#include "OW_IOException.hpp"
#include "OW_Format.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_Assertion.hpp"
#include "OW_Logger.hpp"

#include <cstdio> // for SEEK_END
#include <db.h>

namespace OpenWBEM
{

using std::istream;
using std::ostream;
using std::endl;
//////////////////////////////////////////////////////////////////////////////
AssocDbEntry2::AssocDbEntry2(istream& istrm)
	: m_objectName(CIMNULL)
	, m_offset(-1L)
{
	readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////
AssocDbEntry2::AssocDbEntry2(const CIMObjectPath& objectName,
		const String& role,
		const String& resultRole) :
	m_objectName(objectName), m_role(role), m_resultRole(resultRole),
	m_offset(-1L)
{
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry2::writeObject(ostream& ostrm) const
{
	m_objectName.writeObject(ostrm);
	m_role.writeObject(ostrm);
	m_resultRole.writeObject(ostrm);
	BinarySerialization::writeArray(ostrm, m_entries);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry2::entry::writeObject(ostream& ostrm) const
{
	m_assocClass.writeObject(ostrm);
	m_resultClass.writeObject(ostrm);
	m_associatedObject.writeObject(ostrm);
	m_associationPath.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry2::readObject(istream& istrm)
{
	m_objectName.readObject(istrm);
	m_role.readObject(istrm);
	m_resultRole.readObject(istrm);
	BinarySerialization::readArray(istrm, m_entries);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry2::entry::readObject(istream& istrm)
{
	m_assocClass.readObject(istrm);
	m_resultClass.readObject(istrm);
	m_associatedObject.readObject(istrm);
	m_associationPath.readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////
String
AssocDbEntry2::makeKey(const CIMObjectPath& objectName, const String& role,
	const String& resultRole)
{
	// use # as the separator, because that's not a valid character in an
	// object path or any CIM identifier
	String lowerName = objectName.toString();
	lowerName.toLowerCase();
	String lowerRole = role;
	lowerRole.toLowerCase();
	String lowerResultRole = resultRole;
	lowerResultRole.toLowerCase();
	return lowerName + "#" + lowerRole + "#" + lowerResultRole;
}
//////////////////////////////////////////////////////////////////////////////
String
AssocDbEntry2::makeKey() const
{
	// use # as the separator, because that's not a valid character in an
	// object path or any CIM identifier
	return makeKey(m_objectName, m_role, m_resultRole);
}
//////////////////////////////////////////////////////////////////////////////
ostream&
operator << (ostream& ostrm, const AssocDbEntry2& arg)
{
	ostrm
		<< "\tobjectName: " << arg.m_objectName.toString() << endl
		<< "\trole: " << arg.m_role << endl
		<< "\tresultRole: " << arg.m_resultRole << endl
		<< "\tkey: " << arg.makeKey() << endl;
	return ostrm;
}
//////////////////////////////////////////////////////////////////////////////
bool
AssocDb2::hasAssocEntries(const String& ns, const CIMObjectPath& instanceName)
{
	CIMObjectPath pathWithNS(instanceName);
	pathWithNS.setNameSpace(ns);
	String targetObject = pathWithNS.toString();
	return (findEntry(targetObject)) ? true : false;
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb2::addEntries(const String& ns, const CIMInstance& assocInstance)
{
	addOrDeleteEntries(ns, assocInstance, true);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb2::deleteEntries(const String& ns, const CIMInstance& assocInstance)
{
	addOrDeleteEntries(ns, assocInstance, false);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb2::addOrDeleteEntries(const String& ns, const CIMInstance& assocInstance, bool add)
{
	String assocClass = assocInstance.getClassName();
	CIMObjectPath assocPath(assocClass, ns);
	assocPath.setKeys(assocInstance);
	// search for references
	CIMPropertyArray propRa = assocInstance.getProperties();
	for (size_t i = 0; i < propRa.size(); i++)
	{
		CIMValue propValue1 = propRa[i].getValue();
		if (propValue1 && propValue1.getType() == CIMDataType::REFERENCE)
		{
			// found first reference, search for second
			for (size_t j = 0; j < propRa.size(); ++j)
			{
				if (j == i)
				{
					continue; // don't bother with same ones.
				}
				CIMValue propValue2 = propRa[j].getValue();
				if (propValue2 && propValue2.getType() == CIMDataType::REFERENCE)
				{
					// found a second reference, now set up the vars we need
					// and create index entries.
					CIMObjectPath objectName(CIMNULL);
					propValue1.get(objectName);
					if (objectName.getNameSpace().empty())
					{
						objectName.setNameSpace(ns);
					}
					CIMObjectPath associatedObject(CIMNULL);
					propValue2.get(associatedObject);
					if (associatedObject.getNameSpace().empty())
					{
						objectName.setNameSpace(ns);
					}
					String resultClass = associatedObject.getClassName();
					String role = propRa[i].getName();
					String resultRole = propRa[j].getName();
					if (add)
					{
						addEntry(objectName, assocClass, resultClass,
							role, resultRole, associatedObject, assocPath);
						addEntry(objectName, assocClass, resultClass,
							String(), resultRole, associatedObject, assocPath);
						addEntry(objectName, assocClass, resultClass,
							role, String(), associatedObject, assocPath);
						addEntry(objectName, assocClass, resultClass,
							String(), String(), associatedObject, assocPath);
					}
					else
					{
						deleteEntry(objectName, assocClass, resultClass,
							role, resultRole, associatedObject, assocPath);
						deleteEntry(objectName, assocClass, resultClass,
							String(), resultRole, associatedObject, assocPath);
						deleteEntry(objectName, assocClass, resultClass,
							role, String(), associatedObject, assocPath);
						deleteEntry(objectName, assocClass, resultClass,
							String(), String(), associatedObject, assocPath);
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb2::addEntries(const String& ns, const CIMClass& assocClass)
{
	addOrDeleteEntries(ns, assocClass, true);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb2::deleteEntries(const String& ns, const CIMClass& assocClass)
{
	addOrDeleteEntries(ns, assocClass, false);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb2::addOrDeleteEntries(const String& ns, const CIMClass& assocClass, bool add)
{
	String assocClassName = assocClass.getName();
	CIMObjectPath assocClassPath(assocClassName, ns);
	// search for references
	CIMPropertyArray propRa = assocClass.getAllProperties();
	for (size_t i = 0; i < propRa.size(); i++)
	{
		CIMProperty p1 = propRa[i];
		if (p1.getDataType().getType() == CIMDataType::REFERENCE)
		{
			// found first reference, search for others
			for (size_t j = 0; j < propRa.size(); ++j)
			{
				if (j == i)
				{
					continue; // don't bother with same ones.
				}
				CIMProperty p2 = propRa[j];
				if (p2.getDataType().getType() == CIMDataType::REFERENCE)
				{
					// found another reference, now set up the vars we need
					// and create index entries.
					CIMObjectPath objectName(p1.getDataType().getRefClassName(), ns);
					String resultClass = p2.getDataType().getRefClassName();
					String role = p1.getName();
					String resultRole = p2.getName();
					CIMObjectPath associatedObject(resultClass, ns);
					if (add)
					{
						addEntry(objectName, assocClassName, resultClass,
							role, resultRole, associatedObject, assocClassPath);
						addEntry(objectName, assocClassName, resultClass,
							String(), resultRole, associatedObject, assocClassPath);
						addEntry(objectName, assocClassName, resultClass,
							role, String(), associatedObject, assocClassPath);
						addEntry(objectName, assocClassName, resultClass,
							String(), String(), associatedObject, assocClassPath);
					}
					else
					{
						deleteEntry(objectName, assocClassName, resultClass,
							role, resultRole, associatedObject, assocClassPath);
						deleteEntry(objectName, assocClassName, resultClass,
							String(), resultRole, associatedObject, assocClassPath);
						deleteEntry(objectName, assocClassName, resultClass,
							role, String(), associatedObject, assocClassPath);
						deleteEntry(objectName, assocClassName, resultClass,
							String(), String(), associatedObject, assocClassPath);
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb2::getAllEntries(const CIMObjectPath& objectName,
		const SortedVectorSet<String>* passocClasses,
		const SortedVectorSet<String>* presultClasses,
		const String& role,
		const String& resultRole,
		AssocDbEntry2ResultHandlerIFC& result)
{
	if ((passocClasses && passocClasses->size() == 0)
		|| presultClasses && presultClasses->size() == 0)
	{
		return; // one of the filters will reject everything, so don't even bother
	}
	String key = AssocDbEntry2::makeKey(objectName, role, resultRole);
	AssocDbEntry2 dbentry = findEntry(key);
	if (dbentry)
	{
		for (size_t i = 0; i < dbentry.m_entries.size(); ++i)
		{
			AssocDbEntry2::entry& e = dbentry.m_entries[i];
			if (((passocClasses == 0) || (passocClasses->count(e.m_assocClass) > 0))
			   && ((presultClasses == 0) || (presultClasses->count(e.m_resultClass) > 0)))
			{
				result.handle(e);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
AssocDb2::AssocDb2(ServiceEnvironmentIFCRef env)
	: m_env(env)
{
}
//////////////////////////////////////////////////////////////////////////////
AssocDb2::~AssocDb2()
{
	try
	{
		close();
	}
	catch (...)
	{
		// logDebug or close could throw.
	}
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb2::open(const String& name, ::DB_ENV* env, ::DB_TXN* txn)
{
	m_db.open(name.c_str(), env, txn);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb2::close()
{
	m_db.close();
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDb2 uses
AssocDbEntry2
AssocDb2::findEntry(const String& objectKey)
{
	AssocDbEntry2 dbentry;
//     IndexEntry ie = m_pIndex->findFirst(objectKey.c_str());
//     if (ie && ie.key.equals(objectKey))
//     {
//         dbentry = readEntry(ie.offset, hdl);
//     }
	return dbentry;
}
// PRIVATE - AssocDb2 uses
void
AssocDb2::deleteEntry(const CIMObjectPath& objectName,
	const String& assocClassName, const String& resultClass,
	const String& role, const String& resultRole,
	const CIMObjectPath& associatedObject,
	const CIMObjectPath& assocClassPath)
{
//     String key = AssocDbEntry2::makeKey(objectName, role, resultRole);
//     AssocDbEntry2 dbentry;
//     MutexLock l = getDbLock();
//     IndexEntry ie = m_pIndex->findFirst(key.c_str());
//     if (ie)
//     {
//         dbentry = readEntry(ie.offset, hdl);
//
//         OW_ASSERT(dbentry.makeKey().equals(key));
//         AssocDbEntry2::entry e;
//         e.m_assocClass = assocClassName;
//         e.m_resultClass = resultClass;
//         e.m_associatedObject = associatedObject;
//         e.m_associationPath = assocClassPath;
//         Array<AssocDbEntry2::entry>::iterator iter = find(dbentry.m_entries.begin(), dbentry.m_entries.end(), e);
//         OW_ASSERT(iter != dbentry.m_entries.end());
//         if (iter != dbentry.m_entries.end())
//         {
//             dbentry.m_entries.erase(iter);
//         }
//
//         if (dbentry.m_entries.size() == 0)
//         {
//             m_pIndex->remove(key.c_str(), dbentry.getOffset());
//             addToFreeList(dbentry.getOffset(), hdl);
//         }
//         else
//         {
//             deleteEntry(dbentry, hdl);
//             addEntry(dbentry, hdl);
//         }
//     }
//     else
//     {
//         // TODO: Log this OW_ASSERT(0 == "AssocDb2::deleteEntry failed to find key.  Database may be corrupt");
//     }
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDb2 uses
void
AssocDb2::deleteEntry(const AssocDbEntry2& entry)
{
//     MutexLock l = getDbLock();
//     String key = entry.makeKey();
//     AssocDbEntry2 dbentry;
//     IndexEntry ie = m_pIndex->findFirst(key.c_str());
//     while (ie)
//     {
//         dbentry = readEntry(ie.offset, hdl);
//         if (!dbentry.makeKey().equals(key))
//         {
//             break;
//         }
//         if (dbentry == entry)
//         {
//             m_pIndex->remove(key.c_str(), dbentry.getOffset());
//             addToFreeList(dbentry.getOffset(), hdl);
//             break;
//         }
//         ie = m_pIndex->findNext();
//     }
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDb2 uses
void
AssocDb2::addEntry(const AssocDbEntry2& nentry)
{
//     MutexLock l = getDbLock();
//     DataOStream ostrm;
//     nentry.writeObject(ostrm);
//     UInt32 blkSize = ostrm.length() + sizeof(AssocDbRecHeader2);
//     Int32 offset;
//     AssocDbRecHeader2 rh = getNewBlock(offset, blkSize, hdl);
//     rh.dataSize = ostrm.length();
//     writeRecHeader(rh, offset, hdl.getFile());
//
//     if (hdl.getFile().write(ostrm.getData(), ostrm.length()) !=
//         size_t(ostrm.length()))
//     {
//         OW_THROW(IOException, "Failed to write data assoc db");
//     }
//
//     if (!m_pIndex->add(nentry.makeKey().c_str(), offset))
//     {
//         OW_LOG_ERROR(m_env->getLogger(COMPONENT_NAME), format("AssocDb2::addEntry failed to add entry to"
//             " association index: ", nentry.makeKey()));
//         OW_THROW(IOException, "Failed to add entry to association index");
//     }
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDb2 uses
void
AssocDb2::addEntry(const CIMObjectPath& objectName,
		const String& assocClassName, const String& resultClass,
		const String& role, const String& resultRole,
		const CIMObjectPath& associatedObject,
		const CIMObjectPath& assocClassPath)
{
	String key = AssocDbEntry2::makeKey(objectName, role, resultRole);
	AssocDbEntry2 dbentry = findEntry(key);
	if (dbentry)
	{
		deleteEntry(dbentry);
	}
	else
	{
		dbentry = AssocDbEntry2(objectName, role, resultRole);
	}
	AssocDbEntry2::entry e;
	e.m_assocClass = assocClassName;
	e.m_resultClass = resultClass;
	e.m_associatedObject = associatedObject;
	e.m_associationPath = assocClassPath;
	dbentry.m_entries.push_back(e);
	addEntry(dbentry);
}
//////////////////////////////////////////////////////////////////////////////
bool operator==(const AssocDbEntry2::entry& lhs, const AssocDbEntry2::entry& rhs)
{
	return lhs.m_assocClass.equalsIgnoreCase(rhs.m_assocClass) &&
		lhs.m_resultClass.equalsIgnoreCase(rhs.m_resultClass) &&
		lhs.m_associatedObject.equals(rhs.m_associatedObject) &&
		lhs.m_associationPath.equals(rhs.m_associationPath);
}

} // end namespace OpenWBEM

