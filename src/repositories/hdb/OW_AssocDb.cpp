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

#include "OW_config.h"
#include "OW_AssocDb.hpp"
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
#include "OW_HDBCommon.hpp"

#include <cstdio> // for SEEK_END
#include <algorithm> // for std::find and sort
#include <climits> // for CHAR_BIT
#include <map>
#include <set>

namespace OW_NAMESPACE
{

namespace
{
	const String COMPONENT_NAME("ow.repository.hdb");
}

using std::streambuf;
using std::ostream;
using std::endl;
//////////////////////////////////////////////////////////////////////////////
// Local functions
static UInt32 calcCheckSum(unsigned char* src, Int32 len); // throw()
static void writeRecHeader(AssocDbRecHeader& rh, Int32 offset, File& file); // throw(IOException)
static void readRecHeader(AssocDbRecHeader& rh, Int32 offset, const File& file);
//////////////////////////////////////////////////////////////////////////////
AssocDbEntry::AssocDbEntry(streambuf & istrm)
	: m_objectName(CIMNULL)
	, m_offset(-1L)
{
	readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////
AssocDbEntry::AssocDbEntry(const CIMObjectPath& objectName,
		const CIMName& role,
		const CIMName& resultRole) :
	m_objectName(objectName), m_role(role), m_resultRole(resultRole),
	m_offset(-1L)
{
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry::writeObject(streambuf & ostrm) const
{
	m_objectName.writeObject(ostrm);
	m_role.writeObject(ostrm);
	m_resultRole.writeObject(ostrm);
	BinarySerialization::writeArray(ostrm, m_entries);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry::entry::writeObject(streambuf & ostrm) const
{
	m_assocClass.writeObject(ostrm);
	m_resultClass.writeObject(ostrm);
	m_associatedObject.writeObject(ostrm);
	m_associationPath.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry::readObject(streambuf & istrm)
{
	m_objectName.readObject(istrm);
	m_role.readObject(istrm);
	m_resultRole.readObject(istrm);
	BinarySerialization::readArray(istrm, m_entries);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry::entry::readObject(streambuf & istrm)
{
	m_assocClass.readObject(istrm);
	m_resultClass.readObject(istrm);
	m_associatedObject.readObject(istrm);
	m_associationPath.readObject(istrm);
}
namespace
{

const char NS_SEPARATOR_C(':');

//////////////////////////////////////////////////////////////////////////////
void
makeClassKey(const String& ns, const String& className, StringBuffer& out)
{
	out += ns;
	out += NS_SEPARATOR_C;
	String lowerClassName(className);
	lowerClassName.toLowerCase();
	out += lowerClassName;
}
//////////////////////////////////////////////////////////////////////////////
void
makeInstanceKey(const CIMObjectPath& cop, StringBuffer& out)
{
	makeClassKey(cop.getNameSpace(), cop.getClassName(), out);
	// Get keys from object path
	HDBUtilKeyArray kra(cop.getKeys());
	kra.toString(out);
}
} // end unnamed namespace
//////////////////////////////////////////////////////////////////////////////
String
AssocDbEntry::makeKey(const CIMObjectPath& objectName, const CIMName& role,
	const CIMName& resultRole)
{
	StringBuffer key;
	if (0)
	{
		// This is broken, but we need to handle old versions. Unfortunately there is no version field in the assoc db, so we have no way of really knowing
		// without going to a lot of work :(
		String lowerName = objectName.toString();
		lowerName.toLowerCase();
		key += lowerName;
	}
	else
	{
		if (objectName.isClassPath())
		{
			makeClassKey(objectName.getNameSpace(), objectName.getClassName(), key);
		}
		else
		{
			makeInstanceKey(objectName, key);
		}
	}

	String lowerRole = role.toString();
	lowerRole.toLowerCase();
	String lowerResultRole = resultRole.toString();
	lowerResultRole.toLowerCase();

	// use # as the separator, because that's not a valid character in an
	// object path or any CIM identifier
	key += '#';
	key += lowerRole;
	key += '#';
	key += lowerResultRole;
	return key.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
String
AssocDbEntry::makeKey() const
{
	// use # as the separator, because that's not a valid character in an
	// object path or any CIM identifier
	return makeKey(m_objectName, m_role, m_resultRole);
}
//////////////////////////////////////////////////////////////////////////////
ostream&
operator << (ostream& ostrm, const AssocDbEntry& arg)
{
	ostrm
		<< "\tobjectName: " << arg.m_objectName.toString() << endl
		<< "\trole: " << arg.m_role << endl
		<< "\tresultRole: " << arg.m_resultRole << endl
		<< "\tkey: " << arg.makeKey() << endl;
	return ostrm;
}
//////////////////////////////////////////////////////////////////////////////
AssocDbHandle::AssocDbHandleData::AssocDbHandleData() :
	m_pdb(NULL), m_file()
{
}
//////////////////////////////////////////////////////////////////////////////
AssocDbHandle::AssocDbHandleData::AssocDbHandleData(
	const AssocDbHandleData& arg) :
	IntrusiveCountableBase(arg), m_pdb(arg.m_pdb), m_file(arg.m_file)
{
}
//////////////////////////////////////////////////////////////////////////////
AssocDbHandle::AssocDbHandleData::AssocDbHandleData(AssocDb* pdb,
	const File& file) :
	m_pdb(pdb), m_file(file)
{
}
//////////////////////////////////////////////////////////////////////////////
AssocDbHandle::AssocDbHandleData::~AssocDbHandleData()
{
	try
	{
		if (m_pdb)
		{
			m_pdb->decHandleCount();
			m_file.close();
		}
	}
	catch (...)
	{
		// if decHandleCount throws, just ignore it.
	}
}
//////////////////////////////////////////////////////////////////////////////
AssocDbHandle::AssocDbHandleData&
AssocDbHandle::AssocDbHandleData::operator= (const AssocDbHandleData& arg)
{
	m_pdb = arg.m_pdb;
	m_file = arg.m_file;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbHandle::addEntry(const CIMObjectPath& objectName,
		const CIMName& assocClassName, const CIMName& resultClass,
		const CIMName& role, const CIMName& resultRole,
		const CIMObjectPath& associatedObject,
		const CIMObjectPath& assocClassPath)
{
	m_pdata->m_pdb->addEntry(objectName,
		assocClassName, resultClass,
		role, resultRole,
		associatedObject,
		assocClassPath, *this);
}
//////////////////////////////////////////////////////////////////////////////
bool
AssocDbHandle::hasAssocEntries(const String& ns, const CIMObjectPath& instanceName)
{
	CIMObjectPath pathWithNS(instanceName);
	pathWithNS.setNameSpace(ns);
	String key = AssocDbEntry::makeKey(pathWithNS, String(), String());
	return (m_pdata->m_pdb->findEntry(key, *this)) ? true : false;
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbHandle::addEntries(const String& ns, const CIMInstance& assocInstance)
{
	addOrDeleteEntries(ns, assocInstance, true);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbHandle::deleteEntries(const String& ns, const CIMInstance& assocInstance)
{
	addOrDeleteEntries(ns, assocInstance, false);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbHandle::addOrDeleteEntries(const String& ns, const CIMInstance& assocInstance, bool add)
{
	CIMName assocClass = assocInstance.getClassName();
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
					CIMName resultClass = associatedObject.getClassName();
					CIMName role = propRa[i].getName();
					CIMName resultRole = propRa[j].getName();
					if (add)
					{
						addEntry(objectName, assocClass, resultClass,
							role, resultRole, associatedObject, assocPath);
						addEntry(objectName, assocClass, resultClass,
							CIMName(), resultRole, associatedObject, assocPath);
						addEntry(objectName, assocClass, resultClass,
							role, CIMName(), associatedObject, assocPath);
						addEntry(objectName, assocClass, resultClass,
							CIMName(), CIMName(), associatedObject, assocPath);
					}
					else
					{
						deleteEntry(objectName, assocClass, resultClass,
							role, resultRole, associatedObject, assocPath);
						deleteEntry(objectName, assocClass, resultClass,
							CIMName(), resultRole, associatedObject, assocPath);
						deleteEntry(objectName, assocClass, resultClass,
							role, CIMName(), associatedObject, assocPath);
						deleteEntry(objectName, assocClass, resultClass,
							CIMName(), CIMName(), associatedObject, assocPath);
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbHandle::addEntries(const String& ns, const CIMClass& assocClass)
{
	addOrDeleteEntries(ns, assocClass, true);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbHandle::deleteEntries(const String& ns, const CIMClass& assocClass)
{
	addOrDeleteEntries(ns, assocClass, false);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbHandle::addOrDeleteEntries(const String& ns, const CIMClass& assocClass, bool add)
{
	CIMName assocClassName = assocClass.getName();
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
					CIMName resultClass = p2.getDataType().getRefClassName();
					CIMName role = p1.getName();
					CIMName resultRole = p2.getName();
					CIMObjectPath associatedObject(resultClass, ns);
					if (add)
					{
						addEntry(objectName, assocClassName, resultClass,
							role, resultRole, associatedObject, assocClassPath);
						addEntry(objectName, assocClassName, resultClass,
							CIMName(), resultRole, associatedObject, assocClassPath);
						addEntry(objectName, assocClassName, resultClass,
							role, CIMName(), associatedObject, assocClassPath);
						addEntry(objectName, assocClassName, resultClass,
							CIMName(), CIMName(), associatedObject, assocClassPath);
					}
					else
					{
						deleteEntry(objectName, assocClassName, resultClass,
							role, resultRole, associatedObject, assocClassPath);
						deleteEntry(objectName, assocClassName, resultClass,
							CIMName(), resultRole, associatedObject, assocClassPath);
						deleteEntry(objectName, assocClassName, resultClass,
							role, CIMName(), associatedObject, assocClassPath);
						deleteEntry(objectName, assocClassName, resultClass,
							CIMName(), CIMName(), associatedObject, assocClassPath);
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbHandle::deleteEntry(const CIMObjectPath& objectName,
		const CIMName& assocClassName, const CIMName& resultClass,
		const CIMName& role, const CIMName& resultRole,
		const CIMObjectPath& associatedObject,
		const CIMObjectPath& assocClassPath)
{
	m_pdata->m_pdb->deleteEntry(objectName,
		assocClassName, resultClass,
		role, resultRole,
		associatedObject,
		assocClassPath, *this);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbHandle::getAllEntries(const CIMObjectPath& objectName,
		const SortedVectorSet<CIMName>* passocClasses,
		const SortedVectorSet<CIMName>* presultClasses,
		const CIMName& role,
		const CIMName& resultRole,
		AssocDbEntryResultHandlerIFC& result)
{
	if ((passocClasses && passocClasses->size() == 0)
		|| presultClasses && presultClasses->size() == 0)
	{
		return; // one of the filters will reject everything, so don't even bother
	}
	String key = AssocDbEntry::makeKey(objectName, role, resultRole);
	MutexLock l = m_pdata->m_pdb->getDbLock();
	AssocDbEntry dbentry = m_pdata->m_pdb->findEntry(key, *this);
	if (dbentry)
	{
		for (size_t i = 0; i < dbentry.m_entries.size(); ++i)
		{
			AssocDbEntry::entry& e = dbentry.m_entries[i];
			if (((passocClasses == 0) || (passocClasses->count(e.m_assocClass) > 0))
			   && ((presultClasses == 0) || (presultClasses->count(e.m_resultClass) > 0)))
			{
				result.handle(e);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
AssocDb::AssocDb()
	: m_pIndex(NULL)
	, m_hdlCount(0)
	, m_opened(false)
{
}

//////////////////////////////////////////////////////////////////////////////
AssocDb::~AssocDb()
{
	OW_ASSERT(m_hdlCount == 0);
	try
	{
		if (m_hdlCount > 0)
		{
			Logger logger(COMPONENT_NAME);
			OW_LOG_DEBUG(logger, "*** AssocDb::~AssocDb - STILL OUTSTANDING"
				" HANDLES ***");
		}
		close();
	}
	catch (...)
	{
		// logDebug or close could throw.
	}
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb::init(const ServiceEnvironmentIFCRef& env)
{
	m_env = env;
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb::open(const String& fileName)
{
	MutexLock l = getDbLock();
	if (m_opened)
	{
		return;
	}
	m_hdlCount = 0;
	m_fileName = fileName;
	String fname = m_fileName + ".dat";
	createFile();
	checkFile();
	if (!check())
	{
		OW_THROW(HDBException, Format("Repository \"%1\" is corrupt.", fileName).c_str());
	}
	m_opened = true;
}
//////////////////////////////////////////////////////////////////////////////
bool
AssocDb::createFile()
{
	AssocDbHeader b = { OW_ASSOCSIGNATURE, -1L, HDBVERSION };
	m_hdrBlock = b;
	File f = FileSystem::createFile(m_fileName + ".dat");
	if (!f)
	{
		return false;
	}
	if (f.write(&m_hdrBlock, sizeof(m_hdrBlock), 0) != sizeof(m_hdrBlock))
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to write header of HDB");
	}
	f.close();
	m_pIndex = Index::createIndexObject();
	m_pIndex->open(m_fileName.c_str(), Index::E_NO_DUPLICATES);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb::checkFile()
{
	File f = FileSystem::openFile(m_fileName + ".dat");
	if (!f)
	{
		OW_THROW_ERRNO_MSG(IOException,
			Format("Failed to open file: %1", m_fileName).c_str());
	}
	size_t sizeRead = f.read(&m_hdrBlock, sizeof(m_hdrBlock), 0);
	f.close();
	if (sizeRead != sizeof(m_hdrBlock))
	{
		OW_THROW_ERRNO_MSG(IOException,
			Format("Failed to read Assoc DB header from file: %1",
				m_fileName).c_str());
	}
	if (::strncmp(m_hdrBlock.signature, OW_ASSOCSIGNATURE, OW_ASSOCSIGLEN))
	{
		OW_THROW(IOException,
			Format("Invalid Format for Assoc db file: %1", m_fileName).c_str());
	}
	if (m_hdrBlock.version < MinHDBVERSION || m_hdrBlock.version > HDBVERSION)
	{
		OW_THROW(HDBException, Format("Invalid version (%1) for file (%2). Expected (%3)", m_hdrBlock.version, m_fileName, HDBVERSION).c_str());
	}
	m_pIndex = Index::createIndexObject();
	m_pIndex->open(m_fileName.c_str(), Index::E_ALLDUPLICATES);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb::close()
{
	MutexLock l = getDbLock();
	if (m_opened)
	{
		m_pIndex->close();
		m_pIndex = NULL;
		m_opened = false;
	}
}
//////////////////////////////////////////////////////////////////////////////
AssocDbHandle
AssocDb::getHandle()
{
	MutexLock l = getDbLock();
	File file = FileSystem::openFile(m_fileName + ".dat");
	if (!file)
	{
		OW_THROW_ERRNO_MSG(IOException,
			Format("Failed to open file while creating handle: %1",
				m_fileName).c_str());
	}
	m_hdlCount++;
	return AssocDbHandle(this, file);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb::decHandleCount()
{
	MutexLock l = getDbLock();
	m_hdlCount--;
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDbHandle uses
AssocDbEntry
AssocDb::findEntry(const String& objectKey, AssocDbHandle& hdl)
{
	MutexLock l = getDbLock();
	AssocDbEntry dbentry;
	IndexEntry ie = m_pIndex->findFirst(objectKey.c_str());
	if (ie && ie.key.equals(objectKey))
	{
		dbentry = readEntry(ie.offset, hdl);
	}
	return dbentry;
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDbHandle uses
AssocDbEntry
AssocDb::nextEntry(AssocDbHandle& hdl)
{
	MutexLock l = getDbLock();
	AssocDbEntry dbentry;
	IndexEntry ie = m_pIndex->findNext();
	if (!ie)
	{
		return dbentry;
	}
	return readEntry(ie.offset, hdl);
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
AssocDbEntry
AssocDb::readEntry(Int32 offset, AssocDbHandle& hdl)
{
	AssocDbEntry dbentry;
	AssocDbRecHeader rh;
	readRecHeader(rh, offset, hdl.getFile());
	if (((rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) != AssocDbRecHeader::E_BLK_ALLOCATED) &&
		((rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) != AssocDbRecHeader::E_BLK_ALLOC_UNKNOWN))
	{
		OW_THROW(HDBException, "Internal Error! Attempting to read an entry from an unallocated block!");
	}

	AutoPtrVec<unsigned char> bfr(new unsigned char[rh.dataSize]);
	if (hdl.getFile().read(bfr.get(), rh.dataSize) != rh.dataSize)
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to read data for rec on assoc db");
	}

	DataIStreamBuf istrm(rh.dataSize, bfr.get());
	dbentry.readObject(istrm);
	dbentry.setOffset(offset);
	return dbentry;
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDbHandle uses
void
AssocDb::deleteEntry(const CIMObjectPath& objectName,
	const CIMName& assocClassName, const CIMName& resultClass,
	const CIMName& role, const CIMName& resultRole,
	const CIMObjectPath& associatedObject,
	const CIMObjectPath& assocClassPath, AssocDbHandle& hdl)
{
	String key = AssocDbEntry::makeKey(objectName, role, resultRole);
	AssocDbEntry dbentry;
	MutexLock l = getDbLock();
	IndexEntry ie = m_pIndex->findFirst(key.c_str());
	if (ie)
	{
		dbentry = readEntry(ie.offset, hdl);

		if (!dbentry.makeKey().equals(key))
		{
			Logger lgr(COMPONENT_NAME);
			String msg = Format("entry key = %1, db entry key = %2", key, dbentry.makeKey());
			OW_LOG_ERROR(lgr, msg);
			OW_THROW(HDBException, msg.c_str());
		}

		AssocDbEntry::entry e;
		e.m_assocClass = assocClassName;
		e.m_resultClass = resultClass;
		e.m_associatedObject = associatedObject;
		e.m_associationPath = assocClassPath;
		Array<AssocDbEntry::entry>::iterator iter = std::find(dbentry.m_entries.begin(), dbentry.m_entries.end(), e);
		if (iter != dbentry.m_entries.end())
		{
			dbentry.m_entries.erase(iter);
		}
		else
		{
			OW_THROW(HDBException, "not found");
		}
		
		if (dbentry.m_entries.size() == 0)
		{
			m_pIndex->remove(key.c_str(), dbentry.getOffset());
			addToFreeList(dbentry.getOffset(), hdl);
		}
		else
		{
			deleteEntry(dbentry, hdl);
			addEntry(dbentry, hdl);
		}
	}
	else
	{
		Logger lgr(COMPONENT_NAME);
		OW_LOG_ERROR(lgr, "AssocDb::deleteEntry failed to find key.  Database may be corrupt");
	}
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDbHandle uses
void
AssocDb::deleteEntry(const AssocDbEntry& entry, AssocDbHandle& hdl)
{
	MutexLock l = getDbLock();
	String key = entry.makeKey();
	AssocDbEntry dbentry;
	IndexEntry ie = m_pIndex->findFirst(key.c_str());
	while (ie)
	{
		dbentry = readEntry(ie.offset, hdl);
		if (!dbentry.makeKey().equals(key))
		{
			break;
		}
		if (dbentry == entry)
		{
			m_pIndex->remove(key.c_str(), dbentry.getOffset());
			addToFreeList(dbentry.getOffset(), hdl);
			break;
		}
		ie = m_pIndex->findNext();
	}
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDbHandle uses
void
AssocDb::addEntry(const AssocDbEntry& nentry, AssocDbHandle& hdl)
{
	MutexLock l = getDbLock();
	DataOStreamBuf ostrm;
	nentry.writeObject(ostrm);
	UInt32 headerAndDataLen = ostrm.length() + sizeof(AssocDbRecHeader);
	Int32 offset;
	AssocDbRecHeader rh = getNewBlock(offset, headerAndDataLen, hdl);
	rh.dataSize = ostrm.length();
	File f = hdl.getFile();
	writeRecHeader(rh, offset, f);

	if (f.write(ostrm.getData(), rh.dataSize) != size_t(rh.dataSize))
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to write data assoc db");
	}

	UInt32 recordBlkSize = rh.blkSize;
	if (recordBlkSize > headerAndDataLen)
	{
		// need to write a byte at the end of the block, in case this block is at the end of the file, it needs to expand.
		if (f.seek(static_cast<Int32>(recordBlkSize - headerAndDataLen) - 1, SEEK_CUR) == -1)
		{
			OW_THROW_ERRNO_MSG(IOException, Format("Failed to seek (SEEK_CUR) data assoc db to offset: %1", recordBlkSize - headerAndDataLen - 1).c_str());
		}

		char zero = '\0';
		if (f.write(&zero, sizeof(zero)) != sizeof(zero))
		{
			OW_THROW_ERRNO_MSG(IOException, "Failed to write data assoc db");
		}

	}

	if (!m_pIndex->add(nentry.makeKey().c_str(), offset))
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_ERROR(logger, Format("AssocDb::addEntry failed to add entry to"
			" association index: ", nentry.makeKey()));
		OW_THROW_ERRNO_MSG(IOException, "Failed to add entry to association index");
	}
	OW_ASSERT(checkFreeList());
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDbHandle uses
void
AssocDb::addEntry(const CIMObjectPath& objectName,
		const CIMName& assocClassName, const CIMName& resultClass,
		const CIMName& role, const CIMName& resultRole,
		const CIMObjectPath& associatedObject,
		const CIMObjectPath& assocClassPath, AssocDbHandle& hdl)
{
	String key = AssocDbEntry::makeKey(objectName, role, resultRole);
	MutexLock l = getDbLock();
	AssocDbEntry dbentry = findEntry(key, hdl);
	if (dbentry)
	{
		deleteEntry(dbentry, hdl);
	}
	else
	{
		dbentry = AssocDbEntry(objectName, role, resultRole);
	}
	AssocDbEntry::entry e;
	e.m_assocClass = assocClassName;
	e.m_resultClass = resultClass;
	e.m_associatedObject = associatedObject;
	e.m_associationPath = assocClassPath;
	dbentry.m_entries.push_back(e);
	addEntry(dbentry, hdl);
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
// precondition: db mutex locked by this thread.
void
AssocDb::addToFreeList(Int32 offset, AssocDbHandle& hdl)
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG3(lgr, Format("AssocDb::addToFreeList offset = %1", offset));
	AssocDbRecHeader rh;
	File f = hdl.getFile();
	readRecHeader(rh, offset, f);
	rh.nextFree = m_hdrBlock.firstFree;
	if (!((rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) == AssocDbRecHeader::E_BLK_ALLOCATED || 
		  (rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) == AssocDbRecHeader::E_BLK_ALLOC_UNKNOWN))
	{
		OW_THROW(HDBException, "Internal error!. !((rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) == AssocDbRecHeader::E_BLK_ALLOCATED || (rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) == AssocDbRecHeader::E_BLK_ALLOC_UNKNOWN)");
	}
	rh.flags &= ~AssocDbRecHeader::E_BLK_ALLOC_MASK;
	rh.flags |= AssocDbRecHeader::E_BLK_FREE;
	if (rh.nextFree == offset)
	{
		OW_THROW(HDBException, "Internal error!. rh.nextFree == offset");
	}
	writeRecHeader(rh, offset, f);
	m_hdrBlock.firstFree = offset;
	if (f.write(&m_hdrBlock, sizeof(m_hdrBlock), 0L) !=
		sizeof(m_hdrBlock))
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed write file header on deletion");
	}
	OW_ASSERT(checkFreeList());
}
//////////////////////////////////////////////////////////////////////////////
bool
AssocDb::check()
{
	MutexLock l = getDbLock();
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, "AssocDb::check");
	bool rv = true;
	std::set<Int32> freeBlocks;
	rv &= checkFreeList(freeBlocks);
	std::set<Int32> offsets;
	rv &= checkIndex(offsets);
	rv &= checkDb(freeBlocks, offsets);
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
bool AssocDb::checkFreeList()
{
	std::set<Int32> freeBlocks;
	return checkFreeList(freeBlocks);
}

//////////////////////////////////////////////////////////////////////////////
bool AssocDb::checkFreeList(std::set<Int32>& freeBlocks)
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG3(lgr, "AssocDb::checkFreeList");
	AssocDbHandle hdl = this->getHandle();
	AssocDbRecHeader rh;
	Int32 coffset = m_hdrBlock.firstFree;
	File f = hdl.getFile();
	while (coffset != -1)
	{
		readRecHeader(rh, coffset, f);
		if (rh.nextFree == coffset)
		{
			OW_LOG_ERROR(lgr, Format("AssocDb::checkFreeList failed because nextFree==coffset!, rh.nextFree = %1", rh.nextFree));
			return false;
		}
		if ((rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) != AssocDbRecHeader::E_BLK_FREE)
		{
			if ((rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) == AssocDbRecHeader::E_BLK_ALLOC_UNKNOWN)
			{
				OW_LOG_DEBUG3(lgr, Format("AssocDb::checkFreeList block in the free list with unknown status, coffset = %1", coffset));
			}
			else
			{
				OW_LOG_ERROR(lgr, Format("AssocDb::checkFreeList failed because the block flags are marked as allocated, coffset = %1", coffset));
				return false;
			}
		}
		freeBlocks.insert(coffset);
		coffset = rh.nextFree;
	}
	OW_LOG_DEBUG(lgr, "AssocDb::checkFreeList successful");
	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool AssocDb::checkIndex(std::set<Int32>& offsets)
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, "AssocDb::checkIndex");
	std::set<String> keys;
	IndexEntry curEntry = m_pIndex->findFirst();
	AssocDbHandle hdl = getHandle();
	while (curEntry)
	{
		AssocDbEntry dbentry = readEntry(curEntry.offset, hdl);

		if (!dbentry.makeKey().equals(curEntry.key))
		{
			OW_LOG_ERROR(lgr, Format("AssocDb::checkIndex() found invalid index entry index key = %1, db entry key = %2", curEntry.key, dbentry.makeKey()));
			return false;
		}

		if (keys.count(curEntry.key) != 0)
		{
			OW_LOG_ERROR(lgr, Format("AssocDb::checkIndex() found duplicate key in index: %1", curEntry.key));
			return false;
		}
		if (offsets.count(curEntry.offset) != 0)
		{
			OW_LOG_ERROR(lgr, Format("AssocDb::checkIndex() found duplicate offset in index: %1", curEntry.offset));
			return false;
		}
		keys.insert(curEntry.key);
		offsets.insert(curEntry.offset);
		curEntry = m_pIndex->findNext();
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool AssocDb::checkDb(std::set<Int32>& freeList, std::set<Int32>& offsets)
{
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, "AssocDb::checkDb");
	AssocDbHandle hdl = getHandle();
	File f = hdl.getFile();
	Int32 curOffset = 0;

	// first thing is the header
	AssocDbHeader testHeader;
	size_t sizeRead = f.read(&testHeader, sizeof(testHeader), 0);
	if (testHeader != m_hdrBlock)
	{
		OW_LOG_ERROR(lgr, "AssocDb::checkDb() found header block does not match");
		return false;
	}

	curOffset += sizeof(testHeader);

	// next are blocks. The blocks either have to be free or pointed to by an offset in the index (the offsets parameter)
	Int64 fileSize = f.size();
	while (curOffset < fileSize)
	{
		AssocDbRecHeader curRecHeader;
		readRecHeader(curRecHeader, curOffset, f);
		if (curRecHeader.blkSize < (curRecHeader.dataSize + sizeof(AssocDbRecHeader)))
		{
			OW_LOG_ERROR(lgr, Format("AssocDb::checkDb() curRecHeader.blkSize(%1) < (curRecHeader.dataSize(%2) + sizeof(AssocDbRecHeader))(%3)", curRecHeader.blkSize, curRecHeader.dataSize, sizeof(AssocDbRecHeader)));
			return false;
		}

		if ((curRecHeader.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) == AssocDbRecHeader::E_BLK_FREE)
		{
			if (freeList.count(curOffset) != 1)
			{
				OW_LOG_ERROR(lgr, Format("AssocDb::checkDb() free block is not in free list: %1", curOffset));
				return false;
			}
		}
		else if ((curRecHeader.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) == AssocDbRecHeader::E_BLK_ALLOCATED)
		{
			if (offsets.count(curOffset) != 1)
			{
				OW_LOG_ERROR(lgr, Format("AssocDb::checkDb() allocated block is not pointed to by an index entry: %1", curOffset));
				return false;
			}
		}
		else if ((curRecHeader.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) == AssocDbRecHeader::E_BLK_ALLOC_UNKNOWN)
		{
			if ((freeList.count(curOffset) != 1) && (offsets.count(curOffset) != 1))
			{
				OW_LOG_ERROR(lgr, Format("AssocDb::checkDb() free block is not in free list or pointed to by an index entry: %1", curOffset));
				return false;
			}
		}
		else
		{
			OW_LOG_ERROR(lgr, Format("AssocDb::checkDb() allocated block is not marked as unknown, allocated or freed: %1", curOffset));
			return false;
		}

		curOffset += curRecHeader.blkSize;
	}

	return true;
}

namespace
{
	UInt32 nextHighestPowerOfTwo(UInt32 n)
	{
		// http://en.wikipedia.org/wiki/Power_of_two
		--n;
		for (size_t i = 1; i <= sizeof(n) * CHAR_BIT; ++i)
		{
			n |= (n >> i);
		}
		++n;
		return n;
	}
} // end unnamed namespace

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
AssocDbRecHeader
AssocDb::getNewBlock(Int32& offset, UInt32 blkSize_,
	AssocDbHandle& hdl)
{
start:
	OW_ASSERT(checkFreeList());
	Logger lgr(COMPONENT_NAME);
	OW_LOG_DEBUG2(lgr, Format("AssocDb::getNewBlock blkSize_ = %1", blkSize_));
	AssocDbRecHeader rh;
	AssocDbRecHeader lh;
	Int32 lastOffset = -1L;
	Int32 coffset = m_hdrBlock.firstFree;
	File f = hdl.getFile();
	// round the block size up to the nearest power of 2 to help prevent fragmentation.
	UInt32 blkSize = nextHighestPowerOfTwo(blkSize_);
	OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock blkSize = %1", blkSize));
	typedef std::map<Int32, UInt32> AdjacentHeaderMap;
	AdjacentHeaderMap adjacentHeaderMap;

	typedef std::map<Int32, Int32> PrecedingBlockMap;
	PrecedingBlockMap precedingBlockMap;

	while (coffset != -1)
	{
		OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock reading block %1", coffset));
		readRecHeader(rh, coffset, f);
		if (rh.nextFree == coffset)
		{
			OW_THROW(HDBException, "Internal error!. rh.nextFree == offset");
		}

		if (((rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) != AssocDbRecHeader::E_BLK_FREE) &&
			((rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) != AssocDbRecHeader::E_BLK_ALLOC_UNKNOWN))
		{
			OW_THROW(HDBException, "Internal error!. ((rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) != AssocDbRecHeader::E_BLK_FREE) && ((rh.flags & AssocDbRecHeader::E_BLK_ALLOC_MASK) != AssocDbRecHeader::E_BLK_ALLOC_UNKNOWN)");
		}

		OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock inserting into precedingBlockMap %1, %2", coffset, lastOffset));
		precedingBlockMap.insert(std::make_pair(coffset, lastOffset));

		// consolidate
		bool foundAdjacentBlock = false;
		do
		{
			foundAdjacentBlock = false;
			OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock inserting into adjacentHeaderMap %1, %2", coffset, rh.blkSize));
			std::pair<AdjacentHeaderMap::iterator, bool> irv = adjacentHeaderMap.insert(std::make_pair(coffset, rh.blkSize));
			if (!irv.second)
			{
				// a previous entry was merged, so we need to update the size.
				adjacentHeaderMap[coffset] = rh.blkSize;
			}

			// look at the previous block
			if (irv.first != adjacentHeaderMap.begin())
			{
				AdjacentHeaderMap::iterator prevBlockIter = irv.first;
				--prevBlockIter;
				OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock previous free block: %1, length: %2", prevBlockIter->first, prevBlockIter->second));
				if (prevBlockIter->first + prevBlockIter->second == UInt32(coffset))
				{
					// it's an adjacent block, so now combine it.
					// add the current block's size to the previous block
					AssocDbRecHeader prevHeader;
					readRecHeader(prevHeader, prevBlockIter->first, f);
					prevHeader.blkSize += rh.blkSize;
					adjacentHeaderMap[prevBlockIter->first] = prevHeader.blkSize;
					prevBlockIter->second = prevHeader.blkSize;
					OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock setting previous free block: %1, length: %2", prevBlockIter->first, prevBlockIter->second));
					writeRecHeader(prevHeader, prevBlockIter->first, f);
					// splice the current block out of the list by updating the block at lastOffset to point to the next block.
					if (lastOffset != -1L)
					{
						readRecHeader(lh, lastOffset, f); // lh may have been modified if it was at the same offset as prevHeader, so reload it.
						lh.nextFree = rh.nextFree;
						precedingBlockMap[lh.nextFree] = lastOffset;
						OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock removing free block from list. Updating predecessor free block: %1, nextFree: %2", lastOffset, lh.nextFree));
						writeRecHeader(lh, lastOffset, f);
					}
					if (m_hdrBlock.firstFree == coffset)
					{
						m_hdrBlock.firstFree = rh.nextFree;
						precedingBlockMap[rh.nextFree] = -1;
						if (f.write(&m_hdrBlock, sizeof(m_hdrBlock), 0L) != sizeof(m_hdrBlock))
						{
							OW_THROW_ERRNO_MSG(IOException, "failed to write file header while updating free list");
						}
						OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock removing free block from list. Updating main header block firstFree: %1", m_hdrBlock.firstFree));
					}
					// make the current block the new larger block.
					coffset = prevBlockIter->first;
					PrecedingBlockMap::iterator lastIter = precedingBlockMap.find(coffset);
					if (lastIter == precedingBlockMap.end())
					{
						OW_THROW(HDBException, "Internal error. Did not expect to get a failure to find coffset in precedingBlockMap");
					}
					lastOffset = lastIter->second;
					readRecHeader(rh, coffset, f);
					// this statement invalidates all iterators to adjacentHeaderMap
					OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock removing %1 (current block) from adjacentHeaderMap", irv.first->first));
					adjacentHeaderMap.erase(irv.first);
					foundAdjacentBlock = true;
					goto start;
				}
				else
				{
					OW_LOG_DEBUG3(lgr, Format("Previous block is not adjacent %1 != %2", prevBlockIter->first + prevBlockIter->second, UInt32(coffset)));
				}

			}

			// look at the next block
			OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock looking for free block at: %1", coffset + rh.blkSize));
			AdjacentHeaderMap::iterator nextBlockIter = adjacentHeaderMap.find(coffset + rh.blkSize);
			if (nextBlockIter != adjacentHeaderMap.end())
			{
				OW_LOG_DEBUG3(lgr, Format("Found a following free block: %1, size: %2", nextBlockIter->first, nextBlockIter->second));

				// remove the next block from the list
				PrecedingBlockMap::iterator precedingBlockIter = precedingBlockMap.find(nextBlockIter->first);
				if (precedingBlockIter == precedingBlockMap.end())
				{
					OW_THROW_ERRNO_MSG(HDBException, Format("failed to find predecessor for %1", nextBlockIter->first).c_str());
				}
				AssocDbRecHeader nextBlockHeader;
				readRecHeader(nextBlockHeader, nextBlockIter->first, f);

				Int32 precedingBlock = precedingBlockIter->second;
				if (precedingBlock == -1)
				{
					m_hdrBlock.firstFree = nextBlockHeader.nextFree;
					if (m_hdrBlock.firstFree == coffset)
					{
						lastOffset = -1;
					}
					precedingBlockMap[m_hdrBlock.firstFree] = -1;
					OW_LOG_DEBUG3(lgr, Format("Setting m_hdrBlock.firstFree to: %1", m_hdrBlock.firstFree));
					if (f.write(&m_hdrBlock, sizeof(m_hdrBlock), 0L) != sizeof(m_hdrBlock))
					{
						OW_THROW_ERRNO_MSG(IOException, "failed to write file header while updating free list");
					}
				}
				else
				{
					AssocDbRecHeader precedingBlockHeader;
					readRecHeader(precedingBlockHeader, precedingBlock, f);
					precedingBlockHeader.nextFree = nextBlockHeader.nextFree;
					if (precedingBlock == coffset)
					{
						//precedingBlockHeader.nextFree = rh.nextFree;
						rh.nextFree = precedingBlockHeader.nextFree;
					}
					precedingBlockMap[precedingBlockHeader.nextFree] = precedingBlock;
					OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock removing free block from list. Updating predecessor free block: %1, nextFree: %2", precedingBlock, precedingBlockHeader.nextFree));
					writeRecHeader(precedingBlockHeader, precedingBlock, f);
				}

				// add the next block's size to the current block
				rh.blkSize += nextBlockIter->second;
				adjacentHeaderMap[coffset] = rh.blkSize;
				OW_LOG_DEBUG3(lgr, "AssocDb::getNewBlock adding next block's size to the current block");
				writeRecHeader(rh, coffset, f);

				if (lastOffset == nextBlockIter->first)
				{
					lastOffset = precedingBlock;
					readRecHeader(lh, precedingBlock, f);
				}

				// this statement invalidates all iterators to adjacentHeaderMap
				OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock removing %1 (next block) from adjacentHeaderMap", nextBlockIter->first));
				adjacentHeaderMap.erase(nextBlockIter);

				foundAdjacentBlock = true;
				goto start;
			}
			else
			{
				OW_LOG_DEBUG3(lgr, "Did not find an free block after the current block");
			}

		} while (foundAdjacentBlock);

		// If the current block is larger than necessary, break it into two.
		if (rh.blkSize > blkSize)
		{
			OW_LOG_DEBUG3(lgr, Format("Current block size (%1) is larger than necessary (%2)", rh.blkSize, blkSize));
			Int32 prevBlockSize = rh.blkSize;
			AssocDbRecHeader newFreeBlock;
			newFreeBlock.blkSize = prevBlockSize - blkSize;
			newFreeBlock.nextFree = rh.nextFree;
			newFreeBlock.flags |= AssocDbRecHeader::E_BLK_FREE;
			Int32 newOffset = coffset + blkSize;
			OW_LOG_DEBUG3(lgr, Format("newOffset = %1, newFreeBlock.blkSize = %2, newFreeBlock.nextFree = %3", newOffset, newFreeBlock.blkSize, newFreeBlock.nextFree));
			writeRecHeader(newFreeBlock, newOffset, f);
			rh.blkSize = blkSize;
			rh.nextFree = newOffset;
			OW_LOG_DEBUG3(lgr, "AddocDb::getNewBlock setting current block to point to new free block");
			writeRecHeader(rh, coffset, f);
		}


		if (rh.blkSize == blkSize)
		{
			OW_LOG_DEBUG3(lgr, Format("Current block size (%1) is sufficient, coffset = %2, lastOffset = %3, m_hdrBlock.firstFree = %4", rh.blkSize, coffset, lastOffset, m_hdrBlock.firstFree));
			if (lastOffset != -1L)
			{
				lh.nextFree = rh.nextFree;
				OW_LOG_DEBUG3(lgr, Format("updating lh to point to next free block. lh.nextFree = %1", lh.nextFree));
				writeRecHeader(lh, lastOffset, f);
			}
			else if (m_hdrBlock.firstFree == coffset)
			{
				m_hdrBlock.firstFree = rh.nextFree;
				OW_LOG_DEBUG3(lgr, Format("updating m_hdrBlock to point to next free block. m_hdrBlock.firstFree = %1", m_hdrBlock.firstFree));
				if (f.write(&m_hdrBlock, sizeof(m_hdrBlock), 0L) != sizeof(m_hdrBlock))
				{
					OW_THROW_ERRNO_MSG(IOException,
						"failed to write file header while updating free list");
				}
			}
			else
			{
				OW_ASSERTMSG(0, "Failed to update the free list even though a node was removed!");
			}
			rh.nextFree = 0L;
			rh.flags &= ~AssocDbRecHeader::E_BLK_ALLOC_MASK;
			rh.flags |= AssocDbRecHeader::E_BLK_ALLOCATED;
			OW_LOG_DEBUG3(lgr, "Found a block, marking nextFree to 0");
			writeRecHeader(rh, coffset, f);
			offset = coffset;
			OW_LOG_DEBUG(lgr, "AssocDb::getNewBlock finished with a re-used block.");
			OW_ASSERT(checkFreeList());
			return rh;
		}
		else
		{
			OW_LOG_DEBUG3(lgr, Format("Current block size (%1) is not sufficient (%2)", rh.blkSize, blkSize));
		}
		lastOffset = coffset;
		lh = rh;
		coffset = rh.nextFree;
	}
	OW_LOG_DEBUG(lgr, "AssocDb::getNewBlock() Did not find a free block. Expanding and returning a block at the end.");
	f.seek(0L, SEEK_END);
	Int64 offset64 = f.tell();
	offset = static_cast<Int32>(offset64);
	if (Int64(offset) != offset64)
	{
		OW_THROW(HDBException, "file size > 2GB. Overflow.");
	}
	OW_LOG_DEBUG3(lgr, Format("AssocDb::getNewBlock() new block offset: %1", offset));
	memset(&rh, 0, sizeof(rh));
	rh.blkSize = blkSize;
	rh.flags &= ~AssocDbRecHeader::E_BLK_ALLOC_MASK;
	rh.flags |= AssocDbRecHeader::E_BLK_ALLOCATED;
	OW_ASSERT(checkFreeList());
	return rh;
}
//////////////////////////////////////////////////////////////////////////////
static void
writeRecHeader(AssocDbRecHeader& rh, Int32 offset, File& file)
{
	Logger lgr(COMPONENT_NAME);
	rh.chkSum = calcCheckSum(reinterpret_cast<unsigned char*>(&rh.nextFree),
		sizeof(rh) - sizeof(rh.chkSum));
	OW_LOG_DEBUG3(lgr, Format("writeRecHeader writing header to offset %1, chksum: %2, nextFree: %3, blkSize: %4, flags: %5", offset, rh.chkSum, rh.nextFree, rh.blkSize, rh.flags));
	if (rh.nextFree == offset)
	{
		OW_THROW(HDBException, "Internal error!. rh.nextFree == offset");
	}
	if (file.write(&rh, sizeof(rh), offset) != sizeof(rh))
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to write record to assoc db");
	}
}
//////////////////////////////////////////////////////////////////////////////
static void
readRecHeader(AssocDbRecHeader& rh, Int32 offset, const File& file)
{
	Logger lgr(COMPONENT_NAME);
	if (file.read(&rh, sizeof(rh), offset) != sizeof(rh))
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to read record from assoc db");
	}
	UInt32 chkSum = calcCheckSum(reinterpret_cast<unsigned char*>(&rh.nextFree),
		 sizeof(rh) - sizeof(rh.chkSum));
	if (chkSum != rh.chkSum)
	{
		OW_THROW(HDBException, Format("Check sum failed reading rec from assoc db. calculated = %1, expected = %2, offset = %3", chkSum, rh.chkSum, offset).c_str());
	}
	OW_LOG_DEBUG3(lgr, Format("readRecHeader read header from offset %1, chksum: %2, nextFree: %3, blkSize: %4, flags: %5", offset, rh.chkSum, rh.nextFree, rh.blkSize, rh.flags));
}
//////////////////////////////////////////////////////////////////////////////
static UInt32
calcCheckSum(unsigned char* src, Int32 len)
{
	register UInt32 cksum = 0;
	register Int32 i;
	for (i = 0; i < len; i++)
	{
		cksum += static_cast<UInt32>(src[i]);
	}
	return cksum;
}
//////////////////////////////////////////////////////////////////////////////
bool operator==(const AssocDbEntry::entry& lhs, const AssocDbEntry::entry& rhs)
{
	return lhs.m_assocClass == rhs.m_assocClass &&
		lhs.m_resultClass == rhs.m_resultClass &&
		lhs.m_associatedObject.equals(rhs.m_associatedObject) &&
		lhs.m_associationPath.equals(rhs.m_associationPath);
}

} // end namespace OW_NAMESPACE

