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
#include <cstdio> // for SEEK_END

namespace OpenWBEM
{

using std::istream;
using std::ostream;
using std::endl;
//////////////////////////////////////////////////////////////////////////////
// Local functions
static UInt32 calcCheckSum(unsigned char* src, Int32 len);
static void writeRecHeader(AssocDbRecHeader& rh, Int32 offset, File file);
static void readRecHeader(AssocDbRecHeader& rh, Int32 offset, File file);
//////////////////////////////////////////////////////////////////////////////
AssocDbEntry::AssocDbEntry(istream& istrm)
	: m_objectName(CIMNULL)
	, m_offset(-1L)
{
	readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////
AssocDbEntry::AssocDbEntry(const CIMObjectPath& objectName,
		const String& role,
		const String& resultRole) :
	m_objectName(objectName), m_role(role), m_resultRole(resultRole),
	m_offset(-1L)
{
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry::writeObject(ostream& ostrm) const
{
	m_objectName.writeObject(ostrm);
	m_role.writeObject(ostrm);
	m_resultRole.writeObject(ostrm);
	BinarySerialization::writeArray(ostrm, m_entries);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry::entry::writeObject(ostream& ostrm) const
{
	m_assocClass.writeObject(ostrm);
	m_resultClass.writeObject(ostrm);
	m_associatedObject.writeObject(ostrm);
	m_associationPath.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry::readObject(istream& istrm)
{
	m_objectName.readObject(istrm);
	m_role.readObject(istrm);
	m_resultRole.readObject(istrm);
	BinarySerialization::readArray(istrm, m_entries);
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDbEntry::entry::readObject(istream& istrm)
{
	m_assocClass.readObject(istrm);
	m_resultClass.readObject(istrm);
	m_associatedObject.readObject(istrm);
	m_associationPath.readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////
String
AssocDbEntry::makeKey(const CIMObjectPath& objectName, const String& role,
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
	m_pdb(arg.m_pdb), m_file(arg.m_file)
{
}
//////////////////////////////////////////////////////////////////////////////
AssocDbHandle::AssocDbHandleData::AssocDbHandleData(AssocDb* pdb,
	File file) :
	m_pdb(pdb), m_file(file)
{
}
//////////////////////////////////////////////////////////////////////////////
AssocDbHandle::AssocDbHandleData::~AssocDbHandleData()
{
	try
	{
		if(m_pdb)
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
		const String& assocClassName, const String& resultClass,
		const String& role, const String& resultRole,
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
	String targetObject = pathWithNS.toString();
	return (m_pdata->m_pdb->findEntry(targetObject, *this)) ? true : false;
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
	String assocClass = assocInstance.getClassName();
	CIMObjectPath assocPath(assocClass, ns);
	assocPath.setKeys(assocInstance);
	// search for references
	CIMPropertyArray propRa = assocInstance.getProperties();
	for(size_t i = 0; i < propRa.size(); i++)
	{
		CIMValue propValue1 = propRa[i].getValue();
		if(propValue1 && propValue1.getType() == CIMDataType::REFERENCE)
		{
			// found first reference, search for second
			for (size_t j = 0; j < propRa.size(); ++j)
			{
				if (j == i)
				{
					continue; // don't bother with same ones.
				}
				CIMValue propValue2 = propRa[j].getValue();
				if(propValue2 && propValue2.getType() == CIMDataType::REFERENCE)
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
					String resultClass = associatedObject.getObjectName();
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
	String assocClassName = assocClass.getName();
	CIMObjectPath assocClassPath(assocClassName, ns);
	// search for references
	CIMPropertyArray propRa = assocClass.getAllProperties();
	for(size_t i = 0; i < propRa.size(); i++)
	{
		CIMProperty p1 = propRa[i];
		if(p1.getDataType().getType() == CIMDataType::REFERENCE)
		{
			// found first reference, search for others
			for (size_t j = 0; j < propRa.size(); ++j)
			{
				if (j == i)
				{
					continue; // don't bother with same ones.
				}
				CIMProperty p2 = propRa[j];
				if(p2.getDataType().getType() == CIMDataType::REFERENCE)
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
AssocDbHandle::deleteEntry(const CIMObjectPath& objectName,
		const String& assocClassName, const String& resultClass,
		const String& role, const String& resultRole,
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
		const SortedVectorSet<String>* passocClasses,
		const SortedVectorSet<String>* presultClasses,
		const String& role,
		const String& resultRole,
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
			if(((passocClasses == 0) || (passocClasses->count(e.m_assocClass) > 0))
			   && ((presultClasses == 0) || (presultClasses->count(e.m_resultClass) > 0)))
			{
				result.handle(e);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
AssocDb::AssocDb(ServiceEnvironmentIFCRef env)
	: m_hdrBlock()
	, m_pIndex(NULL)
	, m_fileName()
	, m_hdlCount(0)
	, m_opened(false)
	, m_guard()
	, m_env(env)
{
}
//////////////////////////////////////////////////////////////////////////////
AssocDb::~AssocDb()
{
	try
	{
		if(m_hdlCount > 0)
		{
			m_env->getLogger()->logDebug("*** AssocDb::~AssocDb - STILL OUTSTANDING"
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
AssocDb::open(const String& fileName)
{
	MutexLock l = getDbLock();
	if(m_opened)
	{
		return;
	}
	m_hdlCount = 0;
	m_fileName = fileName;
	String fname = m_fileName + ".dat";
	createFile();
	if(!checkFile())
	{
		OW_THROW(IOException,
			format("Failed to open file: %1", fname).c_str());
	}
	m_opened = true;
}
//////////////////////////////////////////////////////////////////////////////
bool
AssocDb::createFile()
{
	AssocDbHeader b = { OW_ASSOCSIGNATURE, -1L };
	m_hdrBlock = b;
	File f = FileSystem::createFile(m_fileName + ".dat");
	if(!f)
	{
		return false;
	}
	if(f.write(&m_hdrBlock, sizeof(m_hdrBlock), 0) != sizeof(m_hdrBlock))
	{
		f.close();
		OW_THROW(IOException, "Failed to write header of HDB");
	}
	f.close();
	m_pIndex = Index::createIndexObject();
	m_pIndex->open(m_fileName.c_str(), Index::E_ALLDUPLICATES);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
bool
AssocDb::checkFile()
{
	File f = FileSystem::openFile(m_fileName + ".dat");
	if(!f)
	{
		OW_THROW(IOException,
			format("Failed to open file: %1", m_fileName).c_str());
	}
	size_t sizeRead = f.read(&m_hdrBlock, sizeof(m_hdrBlock), 0);
	f.close();
	if(sizeRead != sizeof(m_hdrBlock))
	{
		OW_THROW(IOException,
			format("Failed to read Assoc DB header from file: %1",
				m_fileName).c_str());
	}
	if(::strncmp(m_hdrBlock.signature, OW_ASSOCSIGNATURE, OW_ASSOCSIGLEN))
	{
		OW_THROW(IOException,
			format("Invalid format for Assoc db file: %1", m_fileName).c_str());
	}
	m_pIndex = Index::createIndexObject();
	m_pIndex->open(m_fileName.c_str(), Index::E_ALLDUPLICATES);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
void
AssocDb::close()
{
	MutexLock l = getDbLock();
	if(m_opened)
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
	if(!file)
	{
		OW_THROW(IOException,
			format("Failed to open file while creating handle: %1",
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
	if(ie && ie.key.equals(objectKey))
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
	if(!ie)
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
	AutoPtrVec<unsigned char> bfr(new unsigned char[rh.dataSize]);
	if(hdl.getFile().read(bfr.get(), rh.dataSize) != rh.dataSize)
	{
		OW_THROW(IOException, "Failed to read data for rec on assoc db");
	}
	DataIStream istrm(rh.dataSize, bfr.get());
	dbentry.readObject(istrm);
	dbentry.setOffset(offset);
	return dbentry;
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDbHandle uses
void
AssocDb::deleteEntry(const CIMObjectPath& objectName,
	const String& assocClassName, const String& resultClass,
	const String& role, const String& resultRole,
	const CIMObjectPath& associatedObject,
	const CIMObjectPath& assocClassPath, AssocDbHandle& hdl)
{
	String key = AssocDbEntry::makeKey(objectName, role, resultRole);
	AssocDbEntry dbentry;
	MutexLock l = getDbLock();
	IndexEntry ie = m_pIndex->findFirst(key.c_str());
	if(ie)
	{
		dbentry = readEntry(ie.offset, hdl);
		
		OW_ASSERT(dbentry.makeKey().equals(key));
		AssocDbEntry::entry e;
		e.m_assocClass = assocClassName;
		e.m_resultClass = resultClass;
		e.m_associatedObject = associatedObject;
		e.m_associationPath = assocClassPath;
		Array<AssocDbEntry::entry>::iterator iter = find(dbentry.m_entries.begin(), dbentry.m_entries.end(), e);
		OW_ASSERT(iter != dbentry.m_entries.end());
		if (iter != dbentry.m_entries.end())
		{
			dbentry.m_entries.erase(iter);
		}
		
		if(dbentry.m_entries.size() == 0)
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
		// TODO: Log this OW_ASSERT(0 == "AssocDb::deleteEntry failed to find key.  Database may be corrupt");
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
	while(ie)
	{
		dbentry = readEntry(ie.offset, hdl);
		if(!dbentry.makeKey().equals(key))
		{
			break;
		}
		if(dbentry == entry)
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
	DataOStream ostrm;
	nentry.writeObject(ostrm);
	UInt32 blkSize = ostrm.length() + sizeof(AssocDbRecHeader);
	Int32 offset;
	AssocDbRecHeader rh = getNewBlock(offset, blkSize, hdl);
	rh.dataSize = ostrm.length();
	writeRecHeader(rh, offset, hdl.getFile());
	
	if(hdl.getFile().write(ostrm.getData(), ostrm.length()) !=
		size_t(ostrm.length()))
	{
		OW_THROW(IOException, "Failed to write data assoc db");
	}
	
	if(!m_pIndex->add(nentry.makeKey().c_str(), offset))
	{
		m_env->getLogger()->logError(format("AssocDb::addEntry failed to add entry to"
			" association index: ", nentry.makeKey()));
	}
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE - AssocDbHandle uses
void
AssocDb::addEntry(const CIMObjectPath& objectName,
		const String& assocClassName, const String& resultClass,
		const String& role, const String& resultRole,
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
void
AssocDb::addToFreeList(Int32 offset, AssocDbHandle& hdl)
{
	AssocDbRecHeader rh;
	readRecHeader(rh, offset, hdl.getFile());
	rh.nextFree = m_hdrBlock.firstFree;
	writeRecHeader(rh, offset, hdl.getFile());
	m_hdrBlock.firstFree = offset;
	if(hdl.getFile().write(&m_hdrBlock, sizeof(m_hdrBlock), 0L) !=
		sizeof(m_hdrBlock))
	{
		OW_THROW(IOException, "Failed write file header on deletion");
	}
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE
AssocDbRecHeader
AssocDb::getNewBlock(Int32& offset, UInt32 blkSize,
	AssocDbHandle& hdl)
{
	AssocDbRecHeader rh;
	AssocDbRecHeader lh;
	Int32 lastOffset = -1L;
	Int32 coffset = m_hdrBlock.firstFree;
	while(coffset != -1)
	{
		readRecHeader(rh, coffset, hdl.getFile());
		if(rh.blkSize >= blkSize)
		{
			if(lastOffset != -1L)
			{
				lh.nextFree = rh.nextFree;
				writeRecHeader(lh, lastOffset, hdl.getFile());
			}
			if(m_hdrBlock.firstFree == coffset)
			{
				m_hdrBlock.firstFree = rh.nextFree;
				if(hdl.getFile().write(&m_hdrBlock, sizeof(m_hdrBlock), 0L) !=
					sizeof(m_hdrBlock))
				{
					OW_THROW(IOException,
						"failed to write file header while updating free list");
				}
			}
			rh.nextFree = 0L;
			writeRecHeader(rh, coffset, hdl.getFile());
			offset = coffset;
			return rh;
		}
		lastOffset = coffset;
		lh = rh;
		coffset = rh.nextFree;
	}
	hdl.getFile().seek(0L, SEEK_END);
	offset = hdl.getFile().tell();
	memset(&rh, 0, sizeof(rh));
	rh.blkSize = blkSize;
	return rh;
}
//////////////////////////////////////////////////////////////////////////////
static void
writeRecHeader(AssocDbRecHeader& rh, Int32 offset, File file)
{
	rh.chkSum = calcCheckSum(reinterpret_cast<unsigned char*>(&rh.nextFree),
		sizeof(rh) - sizeof(rh.chkSum));
	if(file.write(&rh, sizeof(rh), offset) != sizeof(rh))
	{
		OW_THROW(IOException, "Failed to write record to assoc db");
	}
}
//////////////////////////////////////////////////////////////////////////////
static void
readRecHeader(AssocDbRecHeader& rh, Int32 offset, File file)
{
	if(file.read(&rh, sizeof(rh), offset) != sizeof(rh))
	{
		OW_THROW(IOException, "Failed to read record from assoc db");
	}
	UInt32 chkSum = calcCheckSum(reinterpret_cast<unsigned char*>(&rh.nextFree),
		 sizeof(rh) - sizeof(rh.chkSum));
	if(chkSum != rh.chkSum)
	{
		OW_THROW(IOException, "Check sum failed reading rec from assoc db");
	}
}
//////////////////////////////////////////////////////////////////////////////
static UInt32
calcCheckSum(unsigned char* src, Int32 len)
{
	register UInt32 cksum = 0;
	register Int32 i;
	for(i = 0; i < len; i++)
	{
		cksum += static_cast<UInt32>(src[i]);
	}
	return cksum;
}
//////////////////////////////////////////////////////////////////////////////
bool operator==(const AssocDbEntry::entry& lhs, const AssocDbEntry::entry& rhs)
{
	return lhs.m_assocClass.equalsIgnoreCase(rhs.m_assocClass) &&
		lhs.m_resultClass.equalsIgnoreCase(rhs.m_resultClass) &&
		lhs.m_associatedObject.equals(rhs.m_associatedObject) &&
		lhs.m_associationPath.equals(rhs.m_associationPath);
}

} // end namespace OpenWBEM

