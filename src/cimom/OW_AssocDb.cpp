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
#include "OW_AssocDb.hpp"
#include "OW_RepositoryStreams.hpp"
#include "OW_IOException.hpp"
#include "OW_Format.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"

using std::istream;
using std::ostream;
using std::endl;

//////////////////////////////////////////////////////////////////////////////
// Local functions
static OW_UInt32 calcCheckSum(unsigned char* src, OW_Int32 len);
static void writeRecHeader(AssocDbRecHeader& rh, OW_Int32 offset, OW_File file);
static void readRecHeader(AssocDbRecHeader& rh, OW_Int32 offset, OW_File file);

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbEntry::OW_AssocDbEntry(istream& istrm) :
	m_offset(-1L)
{
	readObject(istrm);
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbEntry::OW_AssocDbEntry(const OW_CIMObjectPath& objectName,
		const OW_String& assocClass,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_String& resultRole,
		const OW_CIMObjectPath& associatedObject,
		const OW_CIMObjectPath& associationPath) :
	m_objectName(objectName), m_assocClass(assocClass),
	m_resultClass(resultClass), m_role(role), m_resultRole(resultRole),
	m_associatedObject(associatedObject), m_associationPath(associationPath),
	m_offset(-1L)
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbEntry::writeObject(ostream& ostrm) const
{
	m_objectName.writeObject(ostrm);
	m_assocClass.writeObject(ostrm);
	m_resultClass.writeObject(ostrm);
	m_role.writeObject(ostrm);
	m_resultRole.writeObject(ostrm);
	m_associatedObject.writeObject(ostrm);
	m_associationPath.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbEntry::readObject(istream& istrm)
{
	m_objectName.readObject(istrm);
	m_assocClass.readObject(istrm);
	m_resultClass.readObject(istrm);
	m_role.readObject(istrm);
	m_resultRole.readObject(istrm);
	m_associatedObject.readObject(istrm);
	m_associationPath.readObject(istrm);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_AssocDbEntry::operator == (const OW_AssocDbEntry& arg) const
{
	return (m_objectName.toString().equals(arg.m_objectName.toString())
		&& m_assocClass.equalsIgnoreCase(arg.m_assocClass)
		&& m_resultClass.equalsIgnoreCase(arg.m_resultClass)
		&& m_role.equalsIgnoreCase(arg.m_role)
		&& m_resultRole.equalsIgnoreCase(arg.m_resultRole)
		&& m_associatedObject.toString().equals(arg.m_associatedObject.toString())
		&& m_associationPath.toString().equals(arg.m_associationPath.toString()));
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_AssocDbEntry::makeKey(const OW_CIMObjectPath& objectName, const OW_String& role,
	const OW_String& resultRole)
{
	// use # as the separator, because that's not a valid character in an
	// object path or any CIM identifier
	OW_String lowerRole = role;
	lowerRole.toLowerCase();
	OW_String lowerResultRole = resultRole;
	lowerResultRole.toLowerCase();
	return objectName.toString() + "#" + lowerRole + "#" + lowerResultRole;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_AssocDbEntry::makeKey() const
{
	// use # as the separator, because that's not a valid character in an
	// object path or any CIM identifier
	return makeKey(getObjectName(), getRole(), getResultRole());
}

//////////////////////////////////////////////////////////////////////////////
ostream&
operator << (ostream& ostrm, const OW_AssocDbEntry& arg)
{
	ostrm
		<< "\tobjectName: " << arg.getObjectName().toString() << endl
		<< "\tassocClass: " << arg.getAssocClass() << endl
		<< "\tresultClass: " << arg.getResultClass() << endl
		<< "\trole: " << arg.getRole() << endl
		<< "\tresultRole: " << arg.getResultRole() << endl
		<< "\tassociatedObject: " << arg.getAssociatedObject().toString() << endl
		<< "\tassociationPath: " << arg.getAssociationPath().toString() << endl
		<< "\tkey: " << arg.makeKey() << endl;

	return ostrm;
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbHandle::AssocDbHandleData::AssocDbHandleData() :
	m_pdb(NULL), m_file()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbHandle::AssocDbHandleData::AssocDbHandleData(
	const AssocDbHandleData& arg) :
	m_pdb(arg.m_pdb), m_file(arg.m_file)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbHandle::AssocDbHandleData::AssocDbHandleData(OW_AssocDb* pdb,
	OW_File file) :
	m_pdb(pdb), m_file(file)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbHandle::AssocDbHandleData::~AssocDbHandleData()
{
	if(m_pdb)
	{
		m_pdb->decHandleCount();
		m_file.close();
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbHandle::AssocDbHandleData&
OW_AssocDbHandle::AssocDbHandleData::operator= (const AssocDbHandleData& arg)
{
	m_pdb = arg.m_pdb;
	m_file = arg.m_file;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbHandle::addEntry(const OW_AssocDbEntry& newEntry)
{
	m_pdata->m_pdb->addEntry(newEntry, *this);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_AssocDbHandle::hasAssocEntries(const OW_String& ns, const OW_CIMObjectPath& instanceName)
{
	OW_CIMObjectPath pathWithNS(instanceName);
	pathWithNS.setNameSpace(ns);
	OW_String targetObject = pathWithNS.toString();
	return (m_pdata->m_pdb->findEntry(targetObject, *this)) ? true : false;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbHandle::addEntries(const OW_String& ns, const OW_CIMInstance& assocInstance)
{
	addOrDeleteEntries(ns, assocInstance, true);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbHandle::deleteEntries(const OW_String& ns, const OW_CIMInstance& assocInstance)
{
	addOrDeleteEntries(ns, assocInstance, false);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbHandle::addOrDeleteEntries(const OW_String& ns, const OW_CIMInstance& assocInstance, bool add)
{
	OW_String assocClass = assocInstance.getClassName();
	OW_CIMObjectPath assocPath(assocClass, ns);
	assocPath.setKeys(assocInstance);

	// search for references
	OW_CIMPropertyArray propRa = assocInstance.getProperties();
	for(size_t i = 0; i < propRa.size(); i++)
	{
		OW_CIMValue propValue1 = propRa[i].getValue();
		if(propValue1 && propValue1.getType() == OW_CIMDataType::REFERENCE)
		{
			// found first reference, search for second
			for (size_t j = 0; j < propRa.size(); ++j)
			{
				if (j == i)
				{
					continue; // don't bother with same ones.
				}
				OW_CIMValue propValue2 = propRa[j].getValue();
				if(propValue2 && propValue2.getType() == OW_CIMDataType::REFERENCE)
				{
					// found a second reference, now set up the vars we need
					// and create index entries.
					OW_CIMObjectPath objectName;
					propValue1.get(objectName);
					if (objectName.getNameSpace().empty())
					{
						objectName.setNameSpace(ns);
					}
					OW_CIMObjectPath associatedObject;
					propValue2.get(associatedObject);
					if (associatedObject.getNameSpace().empty())
					{
						objectName.setNameSpace(ns);
					}
					OW_String resultClass = associatedObject.getObjectName();
					OW_String role = propRa[i].getName();
					OW_String resultRole = propRa[j].getName();
					OW_AssocDbEntry entry1(objectName, assocClass, resultClass,
						role, resultRole, associatedObject, assocPath);
					OW_AssocDbEntry entry2(objectName, assocClass, resultClass,
						OW_String(), resultRole, associatedObject, assocPath);
					OW_AssocDbEntry entry3(objectName, assocClass, resultClass,
						role, OW_String(), associatedObject, assocPath);
					OW_AssocDbEntry entry4(objectName, assocClass, resultClass,
						OW_String(), OW_String(), associatedObject, assocPath);
					if (add)
					{
						addEntry(entry1);
						addEntry(entry2);
						addEntry(entry3);
						addEntry(entry4);
					}
					else
					{
						deleteEntry(entry1);
						deleteEntry(entry2);
						deleteEntry(entry3);
						deleteEntry(entry4);
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbHandle::addEntries(const OW_String& ns, const OW_CIMClass& assocClass)
{
	addOrDeleteEntries(ns, assocClass, true);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbHandle::deleteEntries(const OW_String& ns, const OW_CIMClass& assocClass)
{
	addOrDeleteEntries(ns, assocClass, false);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbHandle::addOrDeleteEntries(const OW_String& ns, const OW_CIMClass& assocClass, bool add)
{
	OW_String assocClassName = assocClass.getName();
	OW_CIMObjectPath assocClassPath(assocClassName, ns);

	// search for references
	OW_CIMPropertyArray propRa = assocClass.getProperties();
	for(size_t i = 0; i < propRa.size(); i++)
	{
		OW_CIMProperty p1 = propRa[i];
		if(p1.getDataType() == OW_CIMDataType(OW_CIMDataType::REFERENCE))
		{
			// found first reference, search for others
			for (size_t j = 0; j < propRa.size(); ++j)
			{
				if (j == i)
				{
					continue; // don't bother with same ones.
				}
				OW_CIMProperty p2 = propRa[j];
				if(p2.getDataType() == OW_CIMDataType(OW_CIMDataType::REFERENCE))
				{
					// found another reference, now set up the vars we need
					// and create index entries.
					OW_CIMObjectPath objectName(p1.getDataType().getRefClassName(), ns);
					OW_String resultClass = p2.getDataType().getRefClassName();
					OW_String role = p1.getName();
					OW_String resultRole = p2.getName();
					OW_CIMObjectPath associatedObject(resultClass, ns);
					OW_AssocDbEntry entry1(objectName, assocClassName, resultClass,
						role, resultRole, associatedObject, assocClassPath);
					OW_AssocDbEntry entry2(objectName, assocClassName, resultClass,
						OW_String(), resultRole, associatedObject, assocClassPath);
					OW_AssocDbEntry entry3(objectName, assocClassName, resultClass,
						role, OW_String(), associatedObject, assocClassPath);
					OW_AssocDbEntry entry4(objectName, assocClassName, resultClass,
						OW_String(), OW_String(), associatedObject, assocClassPath);
					if (add)
					{
						addEntry(entry1);
						addEntry(entry2);
						addEntry(entry3);
						addEntry(entry4);
					}
					else
					{
						deleteEntry(entry1);
						deleteEntry(entry2);
						deleteEntry(entry3);
						deleteEntry(entry4);
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbHandle::deleteEntry(const OW_AssocDbEntry& entryToDelete)
{
	m_pdata->m_pdb->deleteEntry(entryToDelete, *this);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbHandle::getAllEntries(const OW_CIMObjectPath& objectName,
		const OW_SortedVectorSet<OW_String>* passocClasses,
		const OW_SortedVectorSet<OW_String>* presultClasses,
		const OW_String& role,
		const OW_String& resultRole,
		OW_AssocDbEntryResultHandlerIFC& result)
{
	if ((passocClasses && passocClasses->size() == 0)
		|| presultClasses && presultClasses->size() == 0)
	{
		return; // one of the filter will reject everything, so don't even bother
	}
	OW_String key = OW_AssocDbEntry::makeKey(objectName, role, resultRole);

	OW_MutexLock l = m_pdata->m_pdb->getDbLock();

	OW_AssocDbEntry dbentry = m_pdata->m_pdb->findEntry(key, *this);
	while(dbentry && dbentry.makeKey() == key)
	{
		if(((passocClasses == 0) || (passocClasses->count(dbentry.getAssocClass()) > 0))
		   && ((presultClasses == 0) || (presultClasses->count(dbentry.getResultClass()) > 0)))
		{
			result.handle(dbentry);
		}

		dbentry = m_pdata->m_pdb->nextEntry(*this);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDb::OW_AssocDb(OW_CIMOMEnvironmentRef env)
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
OW_AssocDb::~OW_AssocDb()
{
	if(m_hdlCount > 0)
	{
		m_env->logDebug("*** OW_AssocDb::~OW_AssocDb - STILL OUTSTANDING"
			" HANDLES ***");
	}
	close();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDb::open(const OW_String& fileName)
{
	OW_MutexLock l = getDbLock();
	if(m_opened)
	{
		return;
	}

	m_hdlCount = 0;
	m_fileName = fileName;

	OW_String fname = m_fileName + ".dat";

	if(!OW_FileSystem::canWrite(fname.c_str()))
	{
		if(!createFile())
		{
			OW_THROW(OW_IOException,
				format("Failed to create file: %1", fname).c_str());
		}
	}
	else
	{
		if(!checkFile())
		{
			OW_THROW(OW_IOException,
				format("Failed to open file: %1", fname).c_str());
		}
	}

	m_opened = true;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_AssocDb::createFile()
{
	OW_AssocDbHeader b = { OW_ASSOCSIGNATURE, -1L };
	m_hdrBlock = b;

	OW_File f = OW_FileSystem::createFile(m_fileName + ".dat");
	if(!f)
	{
		return false;
	}

	if(f.write(&m_hdrBlock, sizeof(m_hdrBlock), 0) != sizeof(m_hdrBlock))
	{
		f.close();
		OW_THROW(OW_IOException, "Failed to write header of HDB");
	}

	f.close();
	m_pIndex = OW_Index::createIndexObject();
	m_pIndex->open(m_fileName.c_str(), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_AssocDb::checkFile()
{
	OW_File f = OW_FileSystem::openFile(m_fileName + ".dat");
	if(!f)
	{
		OW_THROW(OW_IOException,
			format("Failed to open file: %1", m_fileName).c_str());
	}

	size_t sizeRead = f.read(&m_hdrBlock, sizeof(m_hdrBlock), 0);
	f.close();

	if(sizeRead != sizeof(m_hdrBlock))
	{
		OW_THROW(OW_IOException,
			format("Failed to read Assoc DB header from file: %1",
				m_fileName).c_str());
	}

	if(::strncmp(m_hdrBlock.signature, OW_ASSOCSIGNATURE, OW_ASSOCSIGLEN))
	{
		OW_THROW(OW_IOException,
			format("Invalid format for Assoc db file: %1", m_fileName).c_str());
	}

	m_pIndex = OW_Index::createIndexObject();
	m_pIndex->open(m_fileName.c_str(), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDb::close()
{
	OW_MutexLock l = getDbLock();
	if(m_opened)
	{
		m_pIndex->close();
		m_pIndex = NULL;
		m_opened = false;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbHandle
OW_AssocDb::getHandle()
{
	OW_MutexLock l = getDbLock();
	OW_File file = OW_FileSystem::openFile(m_fileName + ".dat");
	if(!file)
	{
		OW_THROW(OW_IOException,
			format("Failed to open file while creating handle: %1",
				m_fileName).c_str());
	}

	m_hdlCount++;
	return OW_AssocDbHandle(this, file);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDb::decHandleCount()
{
	OW_MutexLock l = getDbLock();
	m_hdlCount--;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE - OW_AssocDbHandle uses
OW_AssocDbEntry
OW_AssocDb::findEntry(const OW_String& objectKey, OW_AssocDbHandle& hdl)
{
	OW_MutexLock l = getDbLock();

	OW_AssocDbEntry dbentry;

	OW_IndexEntry ie = m_pIndex->findFirst(objectKey.c_str());
	if(ie && ie.key.equals(objectKey))
	{
		dbentry = readEntry(ie.offset, hdl);
	}

	return dbentry;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE - OW_AssocDbHandle uses
OW_AssocDbEntry
OW_AssocDb::nextEntry(OW_AssocDbHandle& hdl)
{
	OW_MutexLock l = getDbLock();

	OW_AssocDbEntry dbentry;
	OW_IndexEntry ie = m_pIndex->findNext();
	if(!ie)
	{
		return dbentry;
	}

	return readEntry(ie.offset, hdl);
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
OW_AssocDbEntry
OW_AssocDb::readEntry(OW_Int32 offset, OW_AssocDbHandle& hdl)
{
	OW_AssocDbEntry dbentry;
	AssocDbRecHeader rh;

	readRecHeader(rh, offset, hdl.getFile());

	OW_AutoPtrVec<unsigned char> bfr(new unsigned char[rh.dataSize]);

	if(hdl.getFile().read(bfr.get(), rh.dataSize) != rh.dataSize)
	{
		OW_THROW(OW_IOException, "Failed to read data for rec on assoc db");
	}

	OW_RepositoryIStream istrm(rh.dataSize, bfr.get());
	dbentry.readObject(istrm);
	dbentry.setOffset(offset);

	return dbentry;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE - OW_AssocDbHandle uses
void
OW_AssocDb::deleteEntry(const OW_AssocDbEntry& entry, OW_AssocDbHandle& hdl)
{
	OW_MutexLock l = getDbLock();
	OW_String key = entry.makeKey();
	OW_AssocDbEntry dbentry;
	OW_IndexEntry ie = m_pIndex->findFirst(key.c_str());
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
// PRIVATE - OW_AssocDbHandle uses
void
OW_AssocDb::addEntry(const OW_AssocDbEntry& nentry, OW_AssocDbHandle& hdl)
{
	OW_MutexLock l = getDbLock();
	OW_RepositoryOStream ostrm;
	nentry.writeObject(ostrm);
	OW_UInt32 blkSize = ostrm.length() + sizeof(AssocDbRecHeader);
	OW_Int32 offset;
	AssocDbRecHeader rh = getNewBlock(offset, blkSize, hdl);
	rh.dataSize = ostrm.length();
	writeRecHeader(rh, offset, hdl.getFile());

	if(hdl.getFile().write(ostrm.getData(), ostrm.length()) !=
		size_t(ostrm.length()))
	{
		OW_THROW(OW_IOException, "Failed to write data assoc db");
	}

	if(!m_pIndex->add(nentry.makeKey().c_str(), offset))
	{
		m_env->logError(format("OW_AssocDb::addEntry failed to add entry to"
			" association index: ", nentry.makeKey()));
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_AssocDb::addToFreeList(OW_Int32 offset, OW_AssocDbHandle& hdl)
{
	AssocDbRecHeader rh;

	readRecHeader(rh, offset, hdl.getFile());
	rh.nextFree = m_hdrBlock.firstFree;
	writeRecHeader(rh, offset, hdl.getFile());

	m_hdrBlock.firstFree = offset;
	if(hdl.getFile().write(&m_hdrBlock, sizeof(m_hdrBlock), 0L) !=
		sizeof(m_hdrBlock))
	{
		OW_THROW(OW_IOException, "Failed write file header on deletion");
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
AssocDbRecHeader
OW_AssocDb::getNewBlock(OW_Int32& offset, OW_UInt32 blkSize,
	OW_AssocDbHandle& hdl)
{
	AssocDbRecHeader rh;
	AssocDbRecHeader lh;
	OW_Int32 lastOffset = -1L;

	OW_Int32 coffset = m_hdrBlock.firstFree;
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
					OW_THROW(OW_IOException,
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
writeRecHeader(AssocDbRecHeader& rh, OW_Int32 offset, OW_File file)
{
	rh.chkSum = calcCheckSum((unsigned char*)&rh.nextFree,
		sizeof(rh) - sizeof(rh.chkSum));

	if(file.write(&rh, sizeof(rh), offset) != sizeof(rh))
	{
		OW_THROW(OW_IOException, "Failed to write record to assoc db");
	}
}

//////////////////////////////////////////////////////////////////////////////
static void
readRecHeader(AssocDbRecHeader& rh, OW_Int32 offset, OW_File file)
{
	if(file.read(&rh, sizeof(rh), offset) != sizeof(rh))
	{
		OW_THROW(OW_IOException, "Failed to read record from assoc db");
	}

	OW_UInt32 chkSum = calcCheckSum((unsigned char*)&rh.nextFree,
		 sizeof(rh) - sizeof(rh.chkSum));

	if(chkSum != rh.chkSum)
	{
		OW_THROW(OW_IOException, "Check sum failed reading rec from assoc db");
	}
}

//////////////////////////////////////////////////////////////////////////////
static OW_UInt32
calcCheckSum(unsigned char* src, OW_Int32 len)
{
	register OW_UInt32 cksum = 0;
	register OW_Int32 i;

	for(i = 0; i < len; i++)
	{
		cksum += (OW_UInt32) src[i];
	}

	return cksum;
}

