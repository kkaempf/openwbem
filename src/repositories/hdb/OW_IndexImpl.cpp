/*******************************************************************************
* Copyright (C) 2003 Vintela, Inc. All rights reserved.
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
#include "OW_config.h"
#include "OW_Index.hpp"
#include "OW_FileSystem.hpp"
#include "OW_Exception.hpp"
#include "OW_Format.hpp"
extern "C"
{
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "db.h"
}

extern "C" {
static int recCompare(const DBT* key1, const DBT* key2);
}

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(Index);
OW_DEFINE_EXCEPTION(Index);

/////////////////////////////////////////////////////////////////////////////
class IndexImpl : public Index
{
public:
	IndexImpl();
	virtual ~IndexImpl();
	virtual void open(const char* fileName, EDuplicateKeysFlag allowDuplicates = E_NO_DUPLICATES);
	virtual void close();
	virtual IndexEntry findFirst(const char* key=NULL);
	virtual IndexEntry findNext();
	virtual IndexEntry findPrev();
	virtual IndexEntry find(const char* key);
	virtual bool add(const char* key, Int32 offset);
	virtual bool remove(const char* key, Int32 offset=-1L);
	virtual bool update(const char* key, Int32 newOffset);
	virtual void flush();
	void reopen();
	void openIfClosed();
private:
	DB* m_pDB;
	String m_dbFileName;
};
//////////////////////////////////////////////////////////////////////////////	
// STATIC
Reference<Index>
Index::createIndexObject()
{
	return Reference<Index>(new IndexImpl);
}
//////////////////////////////////////////////////////////////////////////////	
IndexImpl::IndexImpl() :
	m_pDB(NULL), m_dbFileName()
{
}
//////////////////////////////////////////////////////////////////////////////	
IndexImpl::~IndexImpl()
{
	try
	{
		close();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndexImpl::open(const char* fileName, EDuplicateKeysFlag allowDuplicates)
{
	close();
	BTREEINFO dbinfo;
	m_dbFileName = fileName;
	m_dbFileName += ".ndx";
	::memset(&dbinfo, 0, sizeof(dbinfo));
	dbinfo.flags = (allowDuplicates == E_ALLDUPLICATES) ? R_DUP : 0;
	dbinfo.compare = recCompare;
	if(FileSystem::canRead(m_dbFileName)
		&& FileSystem::canWrite(m_dbFileName))
	{
		m_pDB = dbopen(m_dbFileName.c_str(), O_RDWR, S_IRUSR | S_IWUSR,
			DB_BTREE, &dbinfo);
		if(m_pDB == NULL)
		{
			OW_THROW(IndexException, Format("Failed to open index file: %1, errno =%2(%3)", m_dbFileName, errno, strerror(errno)).c_str());
		}
	}
	else
	{
		m_pDB = dbopen(m_dbFileName.c_str(), O_TRUNC | O_RDWR | O_CREAT,
			S_IRUSR | S_IWUSR,  DB_BTREE, &dbinfo);
		if(m_pDB == NULL)
		{
			OW_THROW(IndexException, Format("Failed to create index file: %1, errno =%2(%3)", m_dbFileName, errno, strerror(errno)).c_str());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndexImpl::reopen()
{
	BTREEINFO dbinfo;
	close();
	::memset(&dbinfo, 0, sizeof(dbinfo));
	dbinfo.compare = recCompare;
	m_pDB = dbopen(m_dbFileName.c_str(), O_RDWR, S_IRUSR | S_IWUSR,
		DB_BTREE, &dbinfo);
	if(m_pDB == NULL)
	{
		String msg = "Failed to re-open index file: ";
		msg += m_dbFileName;
		OW_THROW(IndexException, msg.c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndexImpl::openIfClosed()
{
	if (!m_pDB)
	{
		BTREEINFO dbinfo;
		::memset(&dbinfo, 0, sizeof(dbinfo));
		dbinfo.compare = recCompare;
		m_pDB = dbopen(m_dbFileName.c_str(), O_RDWR, S_IRUSR | S_IWUSR,
			DB_BTREE, &dbinfo);
		if(m_pDB == NULL)
		{
			String msg = "Failed to re-open index file: ";
			msg += m_dbFileName;
			OW_THROW(IndexException, msg.c_str());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
IndexImpl::close()
{
	if(m_pDB != NULL)
	{
		m_pDB->close(m_pDB);
		m_pDB = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////	
void
IndexImpl::flush()
{
	if(m_pDB != NULL)
	{
		reopen();
		//m_pDB->sync(m_pDB, 0);
	}
}
namespace
{
class OpenCloser
{
public:
	OpenCloser(IndexImpl* pIndex) : m_pIndex(pIndex) 
	{
		m_pIndex->openIfClosed();
	}
	~OpenCloser()
	{
		m_pIndex->close();
	}
private:
	IndexImpl* m_pIndex;
};
}
//////////////////////////////////////////////////////////////////////////////	
// Find exact
IndexEntry
IndexImpl::find(const char* key)
{
	openIfClosed();
	if(m_pDB == NULL)
	{
		OW_THROW(IndexException, "Index file hasn't been opened");
	}
	DBT theKey, theRec;
	theKey.data = const_cast<void*>(static_cast<const void*>(key));
	theKey.size = ::strlen(key+1);
	if(m_pDB->seq(m_pDB, &theKey, &theRec, R_CURSOR) == 0)
	{
		if(!::strcmp(reinterpret_cast<const char*>(theKey.data), key))
		{
			Int32 tmp;
			memcpy(&tmp, theRec.data, sizeof(tmp));
			return IndexEntry(reinterpret_cast<const char*>(theKey.data), tmp);
		}
	}
	return IndexEntry();
}
//////////////////////////////////////////////////////////////////////////////	
bool
IndexImpl::add(const char* key, Int32 offset)
{
	OpenCloser oc(this);
	if(m_pDB == NULL)
	{
		OW_THROW(IndexException, "Index file hasn't been opened");
	}
	DBT theRec, theKey;
	theRec.data = &offset;
	theRec.size = sizeof(offset);
	theKey.data = const_cast<void*>(static_cast<const void*>(key));
	theKey.size = ::strlen(key)+1;
	return (m_pDB->put(m_pDB, &theKey, &theRec, 0) == 0);
}
//////////////////////////////////////////////////////////////////////////////	
bool
IndexImpl::remove(const char* key, Int32 offset)
{
	OpenCloser oc(this);
	if(m_pDB == NULL)
	{
		OW_THROW(IndexException, "Index file hasn't been opened");
	}
	DBT theKey;
	theKey.data = const_cast<void*>(static_cast<const void*>(key));
	theKey.size = ::strlen(key)+1;
	IndexEntry ientry = findFirst(key);
	while(ientry)
	{
		if(!ientry.key.equals(key))
		{
			break;
		}
		if(offset == -1L || ientry.offset == offset)
		{
			return (m_pDB->del(m_pDB, &theKey, R_CURSOR) == 0);
		}
		ientry = findNext();
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////	
bool
IndexImpl::update(const char* key, Int32 newOffset)
{
	OpenCloser oc(this);
	if(m_pDB == NULL)
	{
		OW_THROW(IndexException, "Index file hasn't been opened");
	}
	if(!find(key))
	{
		return false;
	}
	DBT theRec, theKey;
	theRec.data = &newOffset;
	theRec.size = sizeof(newOffset);
	theKey.data = const_cast<void*>(static_cast<const void*>(key));
	theKey.size = ::strlen(key)+1;
	return (m_pDB->put(m_pDB, &theKey, &theRec, R_CURSOR) == 0);
}
//////////////////////////////////////////////////////////////////////////////	
IndexEntry
IndexImpl::findFirst(const char* key)
{
	openIfClosed();
	if(m_pDB == NULL)
	{
		OW_THROW(IndexException, "Index file hasn't been opened");
	}
	DBT theRec, theKey;
	memset(&theKey, 0, sizeof(theKey));
	memset(&theRec, 0, sizeof(theRec));
	int op = R_FIRST;
	if(key != NULL)
	{
		op = R_CURSOR;
		theKey.data = const_cast<void*>(static_cast<const void*>(key));
		theKey.size = ::strlen(key)+1;
	}
	int cc = m_pDB->seq(m_pDB, &theKey, &theRec, op);
	if(cc == 0)
	{
		Int32 tmp;
		memcpy(&tmp, theRec.data, sizeof(tmp));
		return IndexEntry(reinterpret_cast<const char*>(theKey.data), tmp);
	}
	return IndexEntry();
}
//////////////////////////////////////////////////////////////////////////////	
IndexEntry
IndexImpl::findNext()
{
	openIfClosed();
	if(m_pDB == NULL)
	{
		OW_THROW(IndexException, "Index file hasn't been opened");
	}
	DBT theRec, theKey;
	if(m_pDB->seq(m_pDB, &theKey, &theRec, R_NEXT) == 0)
	{
		Int32 tmp;
		memcpy(&tmp, theRec.data, sizeof(tmp));
		return IndexEntry(reinterpret_cast<const char*>(theKey.data), tmp);
	}
	return IndexEntry();
}
//////////////////////////////////////////////////////////////////////////////	
IndexEntry
IndexImpl::findPrev()
{
	openIfClosed();
	if(m_pDB == NULL)
	{
		OW_THROW(IndexException, "Index file hasn't been opened");
	}
	DBT theRec, theKey;
	if(m_pDB->seq(m_pDB, &theKey, &theRec, R_PREV) == 0)
	{
		Int32 tmp;
		memcpy(&tmp, theRec.data, sizeof(tmp));
		return IndexEntry(reinterpret_cast<const char*>(theKey.data), tmp);
	}
	return IndexEntry();
}

} // end namespace OpenWBEM

//////////////////////////////////////////////////////////////////////////////	
extern "C" {
static int
recCompare(const DBT* key1, const DBT* key2)
{
	if (key1->data && key2->data)
	{
		return strcmp(reinterpret_cast<const char*>(key1->data), 
			reinterpret_cast<const char*>(key2->data));
	}
	else if (key1->data && !key2->data)
	{
		return 1;
	}
	else if (!key1->data && key2->data)
	{
		return -1;
	}
	else // key1->data == 0 && key2->data == 0
	{
		return 0;
	}
}
} // extern "C"
