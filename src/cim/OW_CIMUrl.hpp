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

#ifndef OW_CIMURL_HPP_INCLUDE_GUARD_
#define OW_CIMURL_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_CIMException.hpp"
#include "OW_CIMBase.hpp"
#include "OW_COWReference.hpp"
#include "OW_Bool.hpp"
#include "OW_CIMNULL.hpp"

//////////////////////////////////////////////////////////////////////////////
class OW_IOException;

/**
 * The OW_CIMUrl is an abstract data type that represents a Uniform resource
 * locator. OW_CIMUrl objects are ref counted and copy on write.
 */
class OW_CIMUrl : public OW_CIMBase
{
private:

	struct URLData;
	friend bool operator<(const OW_CIMUrl::URLData& x, 
			const OW_CIMUrl::URLData& y); 

public:

	/**
	 * Create a new OW_CIMUrl object.
	 */
	OW_CIMUrl();

	/**
	 * Create a NULL OW_CIMUrl object.
	 */
	explicit OW_CIMUrl(OW_CIMNULL_t);

	/**
	 * Create a new OW_CIMUrl object from a URL string
	 * (i.e. http://localhost:5988/cimom)
	 * @param spec The string that contains the URL
	 */
	explicit OW_CIMUrl(const OW_String& spec);

	/**
	 * Create a new OW_CIMUrl object.
	 * @param protocol The protocol component of the url (i.e. http, https)
	 * @param host The host component of the URL (i.e. calder.com)
	 * @param file The file component of the URL
	 * @param port The port component of the URL
	 */
	OW_CIMUrl(const OW_String& protocol, const OW_String& host,
		const OW_String& file, OW_Int32 port=0);

	/**
	 * Copy constructor
	 * @param arg The OW_CIMUrl object to make this object a copy of.
	 */
	OW_CIMUrl(const OW_CIMUrl& arg);
	
	/**
	 * Create a new OW_CIMUrl object.
	 * Creates an OW_CIMUrl by parsing the specification spec within a specified
	 * context. If the context argument is not null and the spec argument is a
	 * partial URL specification, then any of the strings missing components are
	 * inherited from the context argument.
	 * @param context The OW_CIMUrl that will provide the missing components
	 * @param spec The OW_String representation of the URL
	 */
	OW_CIMUrl(const OW_CIMUrl& context, const OW_String& spec);

	/**
	 * Destroy this OW_CIMUrl object.
	 */
	~OW_CIMUrl();

	/**
	 * Set this to a null object.
	 */
	virtual void setNull();

	/**
	 * Assignment operator
	 * @param arg The OW_CIMUrl object to assign to this one.
	 * @return A reference to this OW_CIMUrl object.
	 */
	OW_CIMUrl& operator= (const OW_CIMUrl& arg);

private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (!m_pdata.isNull()) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (!m_pdata.isNull()) ? 0: &dummy::nonnull; }

	/**
	 * Check this OW_CIMUrl object against another for equality.
	 * @param arg The OW_CIMUrl object to check for equality against.
	 * @return true If arg is equal to this OW_CIMUrl object. Otherwise false.
	 */
	OW_Bool equals(const OW_CIMUrl& arg) const;

	/**
	 * Equality operator
	 * @param arg The OW_CIMUrl object to check for equality against.
	 * @return true If arg is equal to this OW_CIMUrl object. Otherwise false.
	 */
	OW_Bool operator== (const OW_CIMUrl& arg) const
			{  return equals(arg); }

	/**
	 * Inequality operator
	 * @param arg The OW_CIMUrl object to check for inequality against.
	 * @return true If arg is non equal to this OW_CIMUrl object.
	 * Otherwise false.
	 */
	OW_Bool operator!= (const OW_CIMUrl& arg) const
		{  return !equals(arg); }

	/**
	 * @return The OW_String representation of the entire URL
	 */
	OW_String getSpec() const;

	/**
	 * @return The protocol component of the URL
	 */
	OW_String getProtocol() const;

	/**
	 * Set the protocol component of the url
	 * @param protocol The new protocol component for the url
	 * @return a reference to *this
	 */
	OW_CIMUrl& setProtocol(const OW_String& protocol);


	/**
	 * @return The host component of the URL
	 */
	OW_String getHost() const;

	/**
	 * Set the host component of the URL
	 * @param host The new host component for this OW_CIMUrl object.
	 * @return a reference to *this
	 */
	OW_CIMUrl& setHost(const OW_String& host);

	/**
	 * @return The port component of the URL
	 */
	OW_Int32 getPort() const;

	/**
	 * @return The file component of the URL
	 */
	OW_String getFile() const;

	/**
	 * @return The reference data portion of the URL
	 */
	OW_String getRef() const;

	/**
	 * Determine if the file component of this URL is the same as the file
	 * component on another URL
	 *	@param arg The OW_CIMUrl object to compare the file component of.
	 * @return true if this file component are the same. Otherwise false.
	 */
	OW_Bool sameFile(const OW_CIMUrl& arg) const;

	/**
	 * @return true if this URL reference a resource on the local machine.
	 */
	OW_Bool isLocal() const;

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
	 * @return The string representation of this OW_CIMUrl object.
	 */
	virtual OW_String toString() const;

	/**
	 * @return The MOF representation of this OW_CIMUrl object as an OW_String.
	 */
	virtual OW_String toMOF() const;

private:

	void setLocalHost();

	void setComponents();
	void buildSpec();
	void checkRef();
	void setDefaultValues();

	OW_COWReference<URLData> m_pdata;

	friend bool operator<(const OW_CIMUrl& lhs, const OW_CIMUrl& rhs);
};

#endif


