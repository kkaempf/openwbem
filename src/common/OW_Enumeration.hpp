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
#ifndef OW_ENUMERATION_HPP_
#define OW_ENUMERATION_HPP_
#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_Reference.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_File.hpp"
#include "OW_FileSystem.hpp"
#include <fstream>
#include <cstdlib>
#include <iterator>
#include <cstdio> // for SEEK_END

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(Enumeration);
template <class T>
class TempFileEnumerationImpl
{
public:
	TempFileEnumerationImpl()
	: m_size(0), m_Data()
	{
		m_Data.write(reinterpret_cast<const char*>(&enumSig), sizeof(enumSig));
		if (!m_Data.good())
		{
			OW_THROW(EnumerationException, "Failed to write signature to "
				"enumeration tempfile.");
		}
		// now we have to read the sig so that the temp file stream is
		// positioned correctly
		UInt32 tmpSig;
		m_Data.read(reinterpret_cast<char*>(&tmpSig), sizeof(tmpSig));
		if (!m_Data.good())
		{
			OW_THROW(EnumerationException, "Failed to read signature from "
				"enumeration tempfile.");
		}
	}
	TempFileEnumerationImpl(String const& filename)
	: m_size(readSize(filename)), m_Data(filename)
	{
		// now we have to read the sig so that the temp file stream is
		// positioned correctly
		UInt32 tmpSig;
		m_Data.read(reinterpret_cast<char*>(&tmpSig), sizeof(tmpSig));
		if (!m_Data.good())
		{
			OW_THROW(EnumerationException, "Failed to write signature to "
				"enumeration tempfile.");
		}
	}
	~TempFileEnumerationImpl()
	{
	}
	bool hasMoreElements() const
	{
		
		return m_size > 0;
	}
	void nextElement(T& out)
	{
		if (hasMoreElements())
		{
			--m_size;
			out.readObject(m_Data);
		}
		else
			OW_THROW (EnumerationException, "Attempt to Extract from empty Enum");
	}
	T nextElement()
	{
		if (hasMoreElements())
		{
			--m_size;
			T retval;
			retval.readObject(m_Data);
			return retval;
		}
		else
			OW_THROW (EnumerationException, "Attempt to Extract from empty Enum");
	}
	size_t numberOfElements() const
	{
		return m_size;
	}
	void addElement( const T& arg )
	{
		++m_size;
		arg.writeObject(m_Data);
	}
	void clear()
	{
		m_size = 0;
		m_Data.reset();
	}
	String releaseFile()
	{
		// Append the size onto the end of the stream so we can recover it
		// when constructing from the file
		m_Data.write(reinterpret_cast<char*>(&m_size), sizeof(m_size));
		if (!m_Data.good())
		{
			OW_THROW(EnumerationException, "Failed to write size to "
				"enumeration tempfile.");
		}
		String rval = m_Data.releaseFile();
		clear();
		return rval;
	}
	
	bool usingTempFile() const
	{
		return m_Data.usingTempFile();
	}
private:
	// Prevent copying or assignment
	TempFileEnumerationImpl( const TempFileEnumerationImpl<T>& );
	TempFileEnumerationImpl<T>& operator=( const TempFileEnumerationImpl<T>& );
	size_t readSize(String const& filename)
	{
		size_t size;
		// open the file and read the size that is written to the end of it.
		File f = FileSystem::openFile(filename);
		if (!f)
		{
			OW_THROW(EnumerationException, "Failed to open file");
		}
		
		// Check that the correct signature is on the file
		UInt32 fileSig;
		if(f.read(reinterpret_cast<char*>(&fileSig), sizeof(fileSig)) != sizeof(fileSig))
		{
			OW_THROW(EnumerationException, "Failure to read enumeration "
				"signature");
		}
		if (fileSig != enumSig)
		{
			OW_THROW(EnumerationException, "Attempted to construct an "
				"enumeration from a file that does not have the correct "
				"signature");
		}
		
		
		if (f.seek(-sizeof(size), SEEK_END) == -1)
		{
			OW_THROW(EnumerationException, "Failure to seek");
		}
		if(f.read(reinterpret_cast<char*>(&size), sizeof(size)) != sizeof(size))
		{
			OW_THROW(EnumerationException, "Failure to read enumeration "
				"size");
		}
		if (f.close() == -1)
		{
			OW_THROW(EnumerationException, "Failure to close enumeration "
				"file");
		}
		return size;
	}
private:
	size_t m_size;
	TempFileStream m_Data;
	static const UInt32 enumSig;
};
template <class T>
const UInt32 TempFileEnumerationImpl<T>::enumSig = 0x4f57454e; // "OWEN"
template <class T>
class Enumeration
{
public:
	Enumeration()
	: m_impl( new TempFileEnumerationImpl<T> )
	{
	}
	// Build an enumeration out of the file referenced by filename
	Enumeration(String const& filename)
	: m_impl( new TempFileEnumerationImpl<T>(filename) )
	{
	}
	Enumeration(const Enumeration<T>& arg): m_impl(arg.m_impl)
	{
	}
	Enumeration<T>& operator=(const Enumeration<T>& arg)
	{
		m_impl = arg.m_impl;
		return *this;
	}
	bool hasMoreElements() const
	{
		return m_impl->hasMoreElements();
	}
	void nextElement(T& arg)
	{
		m_impl->nextElement(arg);
	}
	T nextElement()
	{
		return m_impl->nextElement();
	}
	size_t numberOfElements() const
	{
		return m_impl->numberOfElements();
	}
	void addElement(const T& arg)
	{
		m_impl->addElement(arg);
	}
	void clear()
	{
		m_impl->clear();
	}
	// Returns the filename of the file that contains the enumeration data.
	// After this call, the enumeration will not contain any items.
	String releaseFile()
	{
		return m_impl->releaseFile();
	}
	bool usingTempFile() const
	{
		return m_impl->usingTempFile();
	}
	~Enumeration()
	{
	}
private:
	Reference< TempFileEnumerationImpl<T> > m_impl;
};
template <class T>
class Enumeration_input_iterator
{
public:
	typedef Enumeration<T> enumeration_type;
	typedef std::input_iterator_tag iterator_category;
	typedef T value_type;
	typedef const T* pointer;
	typedef const T& reference;
	typedef ptrdiff_t difference_type;
	Enumeration_input_iterator() : m_enumeration(0), m_ok(false)
	{
	}
	Enumeration_input_iterator(enumeration_type& e) : m_enumeration(&e)
	{
		m_read();
	}
	reference operator*() const
	{
		return m_value;
	}
	pointer operator->() const
	{
		return &(operator*());
	}
	Enumeration_input_iterator& operator++()
	{
		m_read();
		return *this;
	}
	Enumeration_input_iterator operator++(int)
	{
		Enumeration_input_iterator tmp = *this;
		m_read();
		return tmp;
	}
	bool m_equal(const Enumeration_input_iterator& x) const
	{
		return(m_ok == x.m_ok) && (!m_ok || m_enumeration == x.m_enumeration);
	}
private:
	enumeration_type* m_enumeration;
	T m_value;
	bool m_ok;
	void m_read()
	{
		m_ok = (m_enumeration && m_enumeration->hasMoreElements()) ? true : false;
		if (m_ok)
		{
			m_enumeration->nextElement(m_value);
		}
	}
};
template <class T>
inline bool
	operator==(const Enumeration_input_iterator<T>& x,
	const Enumeration_input_iterator<T>& y)
{
	return x.m_equal(y);
}
template <class T>
inline bool
	operator!=(const Enumeration_input_iterator<T>& x,
	const Enumeration_input_iterator<T>& y)
{
	return !x.m_equal(y);
}
template <class T>
class Enumeration_insert_iterator
{
public:
	typedef Enumeration<T> enumeration_type;
	typedef std::output_iterator_tag            iterator_category;
	typedef void                           value_type;
	typedef void                           difference_type;
	typedef void                           pointer;
	typedef void                           reference;
	Enumeration_insert_iterator(enumeration_type& e) : m_enumeration(&e)
	{
	}
	Enumeration_insert_iterator<T>& operator=(const T& value)
	{
		m_enumeration->addElement(value);
		return *this;
	}
	Enumeration_insert_iterator<T>& operator*()
	{
		return *this;
	}
	Enumeration_insert_iterator<T>& operator++()
	{
		return *this;
	}
	Enumeration_insert_iterator<T>& operator++(int)
	{
		return *this;
	}
private:
	enumeration_type* m_enumeration;
};
template <class Container>
inline Enumeration_insert_iterator<Container> Enumeration_inserter(Enumeration<Container>& x)
{
	return Enumeration_insert_iterator<Container>(x);
}

} // end namespace OpenWBEM

#endif
