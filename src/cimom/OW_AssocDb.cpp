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

using std::istream;
using std::ostream;
using std::endl;

//////////////////////////////////////////////////////////////////////////////
// Local functions
static unsigned int calcCheckSum(unsigned char* src, int len);
static void writeRecHeader(AssocDbRecHeader& rh, long offset, OW_File file);
static void readRecHeader(AssocDbRecHeader& rh, long offset, OW_File file);

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbEntry::OW_AssocDbEntry(istream& istrm) :
	m_targetObject(), m_assocClassName(), m_propertyName(), m_assocKey(),
	m_offset(-1L)
{
	readObject(istrm);
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbEntry::OW_AssocDbEntry(const OW_String& targetObject,
	const OW_String& assocClassName, const OW_String& propertyName,
	const OW_String& assocKey) :
	m_targetObject(targetObject), m_assocClassName(assocClassName),
	m_propertyName(propertyName), m_assocKey(assocKey), m_offset(-1L)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbEntry::OW_AssocDbEntry(const OW_AssocDbEntry& arg) :
	m_targetObject(arg.m_targetObject), m_assocClassName(arg.m_assocClassName),
	m_propertyName(arg.m_propertyName), m_assocKey(arg.m_assocKey),
	m_offset(arg.m_offset)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbEntry&
OW_AssocDbEntry::operator= (const OW_AssocDbEntry& arg)
{
	m_targetObject = arg.m_targetObject;
	m_assocClassName = arg.m_assocClassName;
	m_propertyName = arg.m_propertyName;
	m_assocKey = arg.m_assocKey;
	m_offset = arg.m_offset;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbEntry::writeObject(ostream& ostrm) const
{
	m_targetObject.writeObject(ostrm);
	m_assocClassName.writeObject(ostrm);
	m_propertyName.writeObject(ostrm);
	m_assocKey.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbEntry::readObject(istream& istrm)
{
	m_targetObject.readObject(istrm);
	m_assocClassName.readObject(istrm);
	m_propertyName.readObject(istrm);
	m_assocKey.readObject(istrm);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_AssocDbEntry::operator == (const OW_AssocDbEntry& arg) const
{
	return (m_targetObject.equalsIgnoreCase(arg.m_targetObject)
		&& m_assocClassName.equalsIgnoreCase(arg.m_assocClassName)
		&& m_propertyName.equalsIgnoreCase(arg.m_propertyName)
		&& m_assocKey.equalsIgnoreCase(arg.m_assocKey));
}

//////////////////////////////////////////////////////////////////////////////
ostream&
operator << (ostream& ostrm, const OW_AssocDbEntry& arg)
{
	ostrm
		<< "\tTarget: " << arg.getTargetObject()
		<< "\tProperty: " << arg.getPropertyName()
		<< "\tAssoc key: " << arg.getAssocKey()
		<< "\tAssoc cls: " << arg.getAssocClassName();

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
void
OW_AssocDbHandle::deleteEntries(const OW_CIMObjectPath& assocKey,
	const OW_CIMInstance& assocInstance)
{
	OW_String strAssocKey = assocKey.toString();
	OW_String assocClass = assocKey.getObjectName();
	OW_CIMPropertyArray propRa = assocInstance.getProperties();
	for(size_t i = 0; i < propRa.size(); i++)
	{
		OW_CIMValue targetValue = propRa[i].getValue();

		if(targetValue && targetValue.getType() == OW_CIMDataType::REFERENCE)
		{
			OW_AssocDbEntryArray era = getAllEntries(targetValue.toString(),
				assocClass, propRa[i].getName());
			for(size_t j = 0; j < era.size(); j++)
			{
				if(era[j].getAssocKey() == strAssocKey)
				{
					deleteEntry(era[j]);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_AssocDbHandle::isEntries(const OW_String& targetObject)
{
	return (m_pdata->m_pdb->findEntry(targetObject, *this)) ? true : false;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbHandle::addEntries(const OW_CIMObjectPath& assocKey,
	const OW_CIMInstance& assocInstance)
{
	OW_String strAssocKey = assocKey.toString();
	OW_String assocClass = assocKey.getObjectName();
	OW_CIMPropertyArray propRa = assocInstance.getProperties();
	for(size_t i = 0; i < propRa.size(); i++)
	{
		OW_CIMValue targetValue = propRa[i].getValue();
		if(targetValue && targetValue.getType() == OW_CIMDataType::REFERENCE)
		{
			OW_AssocDbEntry entry(targetValue.toString(), assocClass,
				propRa[i].getName(), strAssocKey);
			addEntry(entry);
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
OW_AssocDbHandle::deleteEntries(const OW_AssocDbEntryArray& entryra)
{
	for(size_t i = 0; i < entryra.size(); i++)
	{
		deleteEntry(entryra[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssocDbHandle::deleteEntries(const OW_String& targetObjectName)
{
	OW_AssocDbEntryArray era = getAllEntries(targetObjectName, OW_String(),
		OW_String());
	deleteEntries(era);
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbEntryArray
OW_AssocDbHandle::getAllEntries(const OW_String& targetObject,
	const OW_String& assocClass, const OW_String& propertyName)
{
	OW_StringArray assocClasses;
	assocClasses.append(assocClass);
	return getAllEntries(targetObject, assocClasses, propertyName);
}

//////////////////////////////////////////////////////////////////////////////
OW_AssocDbEntryArray
OW_AssocDbHandle::getAllEntries(const OW_String& targetObject,
	const OW_StringArray& assocClasses, const OW_String& propertyName)
{
	OW_MutexLock l = m_pdata->m_pdb->getDbLock();

	OW_AssocDbEntryArray ra;
	OW_AssocDbEntry dbentry = m_pdata->m_pdb->findEntry(targetObject, *this);
	while(dbentry.getTargetObject().equalsIgnoreCase(targetObject))
	{
		bool doAdd = true;
		if(propertyName.length() > 0)
		{
			if(!propertyName.equalsIgnoreCase(dbentry.getPropertyName()))
			{
				doAdd = false;
			}
		}

		if(doAdd)
		{
			if(assocClasses.size() > 0)
			{
				size_t i = 0;
				for(; i < assocClasses.size(); i++)
				{
					if(assocClasses[i].equalsIgnoreCase(dbentry.getAssocClassName()))
					{
						break;
					}
				}

				if(i == assocClasses.size())
				{
					doAdd = false;
				}
			}
		}

		if(doAdd)
		{
			ra.append(dbentry);
		}

		dbentry = m_pdata->m_pdb->nextEntry(*this);
	}

	return ra;
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
OW_AssocDb::findEntry(const OW_String& targetObject, OW_AssocDbHandle& hdl)
{
	OW_MutexLock l = getDbLock();

	OW_AssocDbEntry dbentry;

	OW_IndexEntry ie = m_pIndex->findFirst(targetObject.c_str());
	if(ie && ie.key.equals(targetObject))
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
OW_AssocDb::readEntry(long offset, OW_AssocDbHandle& hdl)
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
	OW_String key = entry.getTargetObject();
	OW_AssocDbEntry dbentry;
	OW_IndexEntry ie = m_pIndex->findFirst(key.c_str());
	while(ie)
	{
		dbentry = readEntry(ie.offset, hdl);
		if(!dbentry.getTargetObject().equalsIgnoreCase(key))
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
	unsigned int blkSize = ostrm.length() + sizeof(AssocDbRecHeader);
	long offset;
	AssocDbRecHeader rh = getNewBlock(offset, blkSize, hdl);
	rh.dataSize = ostrm.length();
	writeRecHeader(rh, offset, hdl.getFile());

	if(hdl.getFile().write(ostrm.getData(), ostrm.length()) !=
		size_t(ostrm.length()))
	{
		OW_THROW(OW_IOException, "Failed to write data assoc db");
	}

	if(!m_pIndex->add(nentry.getTargetObject().c_str(), offset))
	{
		m_env->logError(format("OW_AssocDb::addEntry failed to add entry to"
			" association index: ", nentry.getTargetObject()));
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_AssocDb::addToFreeList(long offset, OW_AssocDbHandle& hdl)
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
OW_AssocDb::getNewBlock(long& offset, unsigned int blkSize,
	OW_AssocDbHandle& hdl)
{
	AssocDbRecHeader rh;
	AssocDbRecHeader lh;
	long lastOffset = -1L;

	long coffset = m_hdrBlock.firstFree;
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
writeRecHeader(AssocDbRecHeader& rh, long offset, OW_File file)
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
readRecHeader(AssocDbRecHeader& rh, long offset, OW_File file)
{
	if(file.read(&rh, sizeof(rh), offset) != sizeof(rh))
	{
		OW_THROW(OW_IOException, "Failed to read record from assoc db");
	}

	unsigned int chkSum = calcCheckSum((unsigned char*)&rh.nextFree,
		 sizeof(rh) - sizeof(rh.chkSum));

	if(chkSum != rh.chkSum)
	{
		OW_THROW(OW_IOException, "Check sum failed reading rec from assoc db");
	}
}

//////////////////////////////////////////////////////////////////////////////
static unsigned int
calcCheckSum(unsigned char* src, int len)
{
	register unsigned int cksum = 0;
	register int i;

	for(i = 0; i < len; i++)
	{
		cksum += (unsigned int) src[i];
	}

	return cksum;
}

