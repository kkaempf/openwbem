/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#include "OW_dbDatabase.hpp"
#include "OW_Exception.hpp"
#include "OW_Format.hpp"
#include "OW_UTF8Utils.hpp"
#include "OW_dbUtils.hpp"

#include <db.h>

extern "C" {
static int recCompare(const DBT* key1, const DBT* key2);
}

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
dbDatabase::dbDatabase()
	: m_db(0)
{
}

//////////////////////////////////////////////////////////////////////////////
dbDatabase::~dbDatabase()
{
	try
	{
		close();
	}
	catch (...)
	{
	}
}

//////////////////////////////////////////////////////////////////////////////
void dbDatabase::open(const char* name, ::DB_ENV* env, ::DB_TXN* txn, EDuplicateKeysFlag allowDuplicates)
{
	// Create the database handle.
	int ret = ::db_create(&m_db, env, 0);
	dbUtils::checkReturn(ret, "db_create");

	if (allowDuplicates == E_DUPLICATES)
	{
		int ret = m_db->set_flags(m_db, DB_DUP);
		dbUtils::checkReturn(ret, "m_db->set_flags: DB_DUP");
	}

	// Open a database in the environment:
	// create if it doesn't exist
	// free-threaded handle
	// read/write owner only
	ret = m_db->open(m_db, txn, name, NULL, DB_BTREE, DB_CREATE | DB_THREAD, 0600);
	dbUtils::checkReturn(ret, "m_db->open");

}

//////////////////////////////////////////////////////////////////////////////
void dbDatabase::close()
{
	if (m_db != 0)
	{
		int ret = m_db->close(m_db, 0);
		dbUtils::checkReturn(ret, "m_db->close");
		m_db = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
bool dbDatabase::get(const String& key, std::vector<unsigned char>& data, ::DB_TXN* txn)
{
	DBT dbtkey = {(void*)key.c_str(), key.length(), key.length(), 0, 0, 0 };
	DBT dbtdata = { 0, 0, 0, 0, 0, DB_DBT_USERMEM };
	int ret = m_db->get(m_db, txn, &dbtkey, &dbtdata, 0);
	if (ret == DB_NOTFOUND)
	{
		return false;
	}
	dbUtils::checkReturn(ret, "m_db->get");
	data.resize(dbtdata.size);
	dbtdata.data = &data[0];
	ret = m_db->get(m_db, txn, &dbtkey, &dbtdata, 0);
	dbUtils::checkReturn(ret, "m_db->get");
	return true;
}

//////////////////////////////////////////////////////////////////////////////
void dbDatabase::put(const String& key, const std::vector<unsigned char>& data, ::DB_TXN* txn)
{
	DBT dbtkey = {(void*)key.c_str(), key.length(), key.length(), 0, 0, 0 };
	DBT dbtdata = { (void*)&data[0], data.size(), 0, 0, 0, 0 };
	int ret = m_db->put(m_db, txn, &dbtkey, &dbtdata, 0);
	dbUtils::checkReturn(ret, "m_db->put");
}

} // end namespace OpenWBEM

//////////////////////////////////////////////////////////////////////////////	
extern "C" {
static int
OW_dbrecCompare(const ::DBT* key1, const ::DBT* key2)
{
	if (key1->data && key2->data)
	{
		return OpenWBEM::UTF8Utils::compareToIgnoreCase(reinterpret_cast<const char*>(key1->data),
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
