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

#ifndef OW_CIMURL_HPP_INCLUDE_GUARD_
#define OW_CIMURL_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMBase.hpp"
#include "OW_COWIntrusiveReference.hpp"
#include "OW_CIMNULL.hpp"
#include "OW_Types.hpp"

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
class IOException;
/**
 * The CIMUrl is an abstract data type that represents a Uniform resource
 * locator. CIMUrl objects are ref counted and copy on write.
 */
class OW_COMMON_API CIMUrl : public CIMBase
{
private:
	struct URLData;
	friend bool operator<(const CIMUrl::URLData& x,
			const CIMUrl::URLData& y);
public:
	/**
	 * Create a new CIMUrl object.
	 */
	CIMUrl();
	/**
	 * Create a NULL CIMUrl object.
	 */
	explicit CIMUrl(CIMNULL_t);
	/**
	 * Create a new CIMUrl object from a URL string
	 * (i.e. http://localhost:5988/cimom)
	 * @param spec The string that contains the URL
	 */
	explicit CIMUrl(const String& spec);
	/**
	 * Create a new CIMUrl object.
	 * @param protocol The protocol component of the url (i.e. http, https)
	 * @param host The host component of the URL (i.e. calder.com)
	 * @param file The file component of the URL
	 * @param port The port component of the URL
	 */
	CIMUrl(const String& protocol, const String& host,
		const String& file, Int32 port=0);
	/**
	 * Copy constructor
	 * @param arg The CIMUrl object to make this object a copy of.
	 */
	CIMUrl(const CIMUrl& arg);
	
	/**
	 * Create a new CIMUrl object.
	 * Creates an CIMUrl by parsing the specification spec within a specified
	 * context. If the context argument is not null and the spec argument is a
	 * partial URL specification, then any of the strings missing components are
	 * inherited from the context argument.
	 * @param context The CIMUrl that will provide the missing components
	 * @param spec The String representation of the URL
	 */
	CIMUrl(const CIMUrl& context, const String& spec);
	/**
	 * Destroy this CIMUrl object.
	 */
	~CIMUrl();
	/**
	 * Set this to a null object.
	 */
	virtual void setNull();
	/**
	 * Assignment operator
	 * @param arg The CIMUrl object to assign to this one.
	 * @return A reference to this CIMUrl object.
	 */
	CIMUrl& operator= (const CIMUrl& arg);

	typedef COWIntrusiveReference<URLData> CIMUrl::*safe_bool;
	operator safe_bool () const
		{  return m_pdata ? &CIMUrl::m_pdata : 0; }
	bool operator!() const
		{  return !m_pdata; }
	/**
	 * Check this CIMUrl object against another for equality.
	 * @param arg The CIMUrl object to check for equality against.
	 * @return true If arg is equal to this CIMUrl object. Otherwise false.
	 */
	bool equals(const CIMUrl& arg) const;
	/**
	 * Equality operator
	 * @param arg The CIMUrl object to check for equality against.
	 * @return true If arg is equal to this CIMUrl object. Otherwise false.
	 */
	bool operator== (const CIMUrl& arg) const
			{  return equals(arg); }
	/**
	 * Inequality operator
	 * @param arg The CIMUrl object to check for inequality against.
	 * @return true If arg is non equal to this CIMUrl object.
	 * Otherwise false.
	 */
	bool operator!= (const CIMUrl& arg) const
		{  return !equals(arg); }
	/**
	 * @return The String representation of the entire URL
	 */
	String getSpec() const;
	/**
	 * @return The protocol component of the URL
	 */
	String getProtocol() const;
	/**
	 * Set the protocol component of the url
	 * @param protocol The new protocol component for the url
	 * @return a reference to *this
	 */
	CIMUrl& setProtocol(const String& protocol);
	/**
	 * @return The host component of the URL
	 */
	String getHost() const;
	/**
	 * Set the host component of the URL
	 * @param host The new host component for this CIMUrl object.
	 * @return a reference to *this
	 */
	CIMUrl& setHost(const String& host);
	/**
	 * @return The port component of the URL
	 */
	Int32 getPort() const;
	/**
	 * @return The file component of the URL
	 */
	String getFile() const;
	/**
	 * @return The reference data portion of the URL
	 */
	String getRef() const;
	/**
	 * Determine if the file component of this URL is the same as the file
	 * component on another URL
	 *	@param arg The CIMUrl object to compare the file component of.
	 * @return true if this file component are the same. Otherwise false.
	 */
	bool sameFile(const CIMUrl& arg) const;
	/**
	 * @return true if this URL reference a resource on the local machine.
	 */
	bool isLocal() const;
	/**
	 * Read this object from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream &istrm);
	/**
	 * Write this object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;
	/**
	 * @return The string representation of this CIMUrl object.
	 */
	virtual String toString() const;
	/**
	 * @return The MOF representation of this CIMUrl object as an String.
	 */
	virtual String toMOF() const;
private:
	void setLocalHost();
	void setComponents();
	void buildSpec();
	void checkRef();
	void setDefaultValues();

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	COWIntrusiveReference<URLData> m_pdata;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	friend OW_COMMON_API bool operator<(const CIMUrl& lhs, const CIMUrl& rhs);
};

} // end namespace OpenWBEM

#endif
