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
#ifndef OW_DB_DATABASE_HPP_INCLUDE_GUARD_
#define OW_DB_DATABASE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_String.hpp"

#include <db.h>

#include <vector>

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(dbDatabase);

class dbDatabase
{
public:
	
	dbDatabase();

	/**
	 * Destructor. This will close the database if it is open. It is
	 * recommended that close be called separately so that exceptions may
	 * be caught and reported.  The destructor will not throw
	 * exceptions.
	 */
	~dbDatabase();

	enum EDuplicateKeysFlag
	{
		E_NO_DUPLICATES,
		E_DUPLICATES
	};

	/**
	 * Open the given db file.
	 * @param name The name of the database.
	 * @exception dbDatabase If the file cannot be created or opened.
	 */
	void open(const char* name, ::DB_ENV* env, ::DB_TXN* txn, EDuplicateKeysFlag allowDuplicates = E_NO_DUPLICATES);
	
	/**
	 * Close the underlying database if it is open.
	 */
	void close();

	/**
	 * Get the data associated with key.
	 * @return true if the get was successful, false if the key doesn't exist.
	 * @throws dbDatabaseException on error.
	 */
	bool get(const String& key, std::vector<unsigned char>& data, ::DB_TXN* txn);

	/**
	 * Put data into the database
	 * @throws dbDatabaseException on error.
	 */
	void put(const String& key, const std::vector<unsigned char>& data, ::DB_TXN* txn);

private:

	::DB* m_db;
};


} // end namespace OW_NAMESPACE

#endif
