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

#ifndef OW_INDEX_HPP_INCLUDE_GUARD_
#define OW_INDEX_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_SafeBool.hpp"

namespace OW_NAMESPACE
{

struct IndexEntry;
class Index;
typedef IntrusiveReference<Index> IndexRef;

/**
 * The Index class is simply an interface class that helps facilitate a
 * pluggable architecture for an indexing scheme on file. All Index
 * objects will be used to maintain an index on some file. The only data ever
 * associated with the key of the index will be a long value that will be
 * used as an offset into a file. The implementor of the Index class must
 * implement the createIndexObject class. This class will be called to get
 * new Index objects that will later be deleted by the caller.
 * The intention is  for the implementor to create a sub-class of Index and
 * have createIndexObject be the factory for those object.
 */
class OW_HDB_API Index : public IntrusiveCountableBase
{
public:
	/**
	 * Destroy this Index object. This will close the index file if it
	 * is open.
	 */
	virtual ~Index();
	enum EDuplicateKeysFlag
	{
		E_NO_DUPLICATES,
		E_ALLDUPLICATES
	};
	/**
	 * Open the given index file.
	 * @param fileName	The file name for the index file. If the file name
	 * has an extension, it will be removed. The name of the index file will
	 * be the base of fileName with the extension ".ndx".
	 * @exception IndexException If the file cannot be created or opened.
	 */
	virtual void open(const char* fileName, EDuplicateKeysFlag allowDuplicates = E_NO_DUPLICATES) = 0;
	/**
	 * Close the underlying index file if it is open.
	 */
	virtual void close() = 0;
	/**
	 * Position the cursor on the 1st entry in the index or the first record
	 * with a given key.
	 *
	 * @param key	The key to find the first occurence of. If NULL is given,
	 * the 1st entry of the index is returned, otherwise the first entry that
	 * has a key that is greater than or equal to the key parameter.
	 *
	 * @return An IndexEntry that contains information about the 1st entry
	 * in the index on success. On failure the returned IndexEntry will be
	 * null.
	 *
	 * @exception IndexException If the index file hasn't been opened.
	 */
	virtual IndexEntry findFirst(const char* key=0) = 0;
	/**
	 * Position the cursor on the next entry in the index.
	 * @return An IndexEntry that contains information about the current
	 * entry in the index on success. On failure the returned IndexEntry
	 * will be null.
	 * @exception IndexException If the index file hasn't been opened.
	 */
	virtual IndexEntry findNext() = 0;
	/**
	 * Position the cursor on the previous entry in the index.
	 * @return An IndexEntry that contains information about the current
	 * entry in the index on success. On failure the returned IndexEntry
	 * will be null.
	 * @exception IndexException If the index file hasn't been opened.
	 */
	virtual IndexEntry findPrev() = 0;
	/**
	 * Position the cursor on the index entry associated a specified key.
	 * @return An IndexEntry that contains information about the current
	 * entry in the index on success. On failure the returned IndexEntry
	 * will be null.
	 * @exception IndexException If the index file hasn't been opened.
	 */
	virtual IndexEntry find(const char* key) = 0;
	/**
	 * Add an entry to the index.
	 * @param key		The key for the index entry to add.
	 * @param offset	The offset value for the index entry to add.
	 * @return true if an index entry was created in the index file. false if
	 * the index entry was already on file.
	 * @exception IndexException If the index file hasn't been opened.
	 */
	virtual bool add(const char* key, Int32 offset) = 0;
	/**
	 * Remove an entry from the index.
	 * @param key	The key of the index entry to remove.
	 * @param offset The value for the offset field of the index entry to remove.
	 *					  This parm is meaningless unless the index allows duplicate
	 *					  keys.
	 * @return true if the index entry was removed from the index file. false if
	 * the index entry could not be found.
	 * @exception IndexException If the index file hasn't been opened.
	 */
	virtual bool remove(const char* key, Int32 offset=-1L) = 0;
	/**
	 * Update an index entry.
	 * @param key			The key of the index entry to update.
	 * @param newOffset	The new value for the index entry.
	 * @return true if the record was updated. false if the record was not
	 * found.
	 * @exception IndexException If the index file hasn't been opened.
	 */
	virtual bool update(const char* key, Int32 newOffset) = 0;
	/**
	 * Flush all changes to the underlying index file.
	 */
	virtual void flush();
	/**
	 * Create dynamically allocated instances of a sub-class of Indexer.
	 * This method must be implemented by whomever desires to implement an
	 * Indexer sub-class.
	 * @return An instance of a sub-class of Indexer.
	 */
	static IndexRef createIndexObject();
};
//////////////////////////////////////////////////////////////////////////////
struct OW_HDB_API IndexEntry
{
	/**
	 * Create a null IndexEntry
	 */
	IndexEntry() : key(), offset(-1) {}
	/**
	 * Create an IndexEntry with the given key and offset
	 * @param k	The key for this index entry.
	 * @param o	The offset associated with the given key.
	 */
	IndexEntry(const String& k, Int32 o) :
		key(k), offset(o) {}

	/**
	 * @return true if this IndexEntry contains a value.
	 */
	OW_SAFE_BOOL_IMPL(IndexEntry, Int32, IndexEntry::offset, (offset != -1 && !key.empty()))

	/**
	 * The key associated with this index entry. If it has a zero length, this
	 * should be considered an invalid IndexEntry.
	 */
	String key;
	/**
	 * The offset associated with this index entry. If this is -1L, this should
	 * be considered an invalid IndexEntry.
	 */
	Int32 offset;
};

} // end namespace OW_NAMESPACE

#endif
