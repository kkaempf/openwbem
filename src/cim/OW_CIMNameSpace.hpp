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

#ifndef __OW_CIMNAMESPACE_HPP__
#define __OW_CIMNAMESPACE_HPP__


#include "OW_config.h"

#include "OW_CIMBase.hpp"
#include "OW_Reference.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_String.hpp"

/**
 * The OW_CIMNameSpace class represents a CIM namespace. The CIM namespace
 * is a component of a CIM object name (namespace + model path) that provides
 * a scope within which all objects are unique. OW_CIMNameSpace objects are
 * ref counted and copy on write. It is possible to have a NULL OW_CIMNameSpace
 * object.
 */
class OW_CIMNameSpace : public OW_CIMBase
{
private:

	struct NSData;

public:

	/**
	 * Create a new OW_CIMNameSpace object.
	 * @param notNull If false, this object will not have any data or
	 * 	implementation associated with it (NULL Object).
	 */
	explicit OW_CIMNameSpace(OW_Bool notNull=OW_Bool(false));

	/**
	 * Create a new OW_CIMNameSpace object.
	 * @param hostUrl The url component of this OW_CIMNameSpace object.
	 * @param nameSpace The namespace (e.g. "root/cimv2") component of this
	 *		OW_CIMNameSpace object.
	 */
	OW_CIMNameSpace(const OW_CIMUrl& hostUrl,
		const OW_String& nameSpace);

	/**
	 * Create a new OW_CIMNameSpace object.
	 * @param hostUrl The url component of this OW_CIMNameSpace object.
	 * @param nameSpace The namespace (e.g. "root/cimv2") component of this
	 *		OW_CIMNameSpace object.
	 */
	explicit OW_CIMNameSpace(const OW_String& nameSpace);

	/**
	 * Create a new OW_CIMNameSpace object.
	 * @param hostUrl The url component of this OW_CIMNameSpace object.
	 * @param nameSpace The namespace (e.g. "root/cimv2") component of this
	 *		OW_CIMNameSpace object.
	 */
	explicit OW_CIMNameSpace(const char* nameSpace);

	/**
	 * Create a new OW_CIMNameSpace object that is a copy of another.
	 * @param arg The OW_CIMNameSpace to make this object a copy of.
	 */
	OW_CIMNameSpace(const OW_CIMNameSpace& arg);

	/**
	 * Destroy this OW_CIMNameSpace object.
	 */
	~OW_CIMNameSpace();

	/**
	 * Set this to a null object.
	 */
	virtual void setNull();

	/**
	 * Assignment operator
	 * @param arg The OW_CIMNameSpace object to assign to this one.
	 * @return A reference to this object after the assignment has been made.
	 */
	OW_CIMNameSpace& operator= (const OW_CIMNameSpace& arg);

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
	 * @return The namespace component of this OW_CIMNameSpace object.
	 */
	OW_String getNameSpace() const;

	/**
	 * @return The host component of this OW_CIMNameSpace object.
	 */
	OW_String getHost() const;

	/**
	 * @return The host URL component of this OW_CIMNameSpace object.
	 */
	OW_CIMUrl getHostUrl() const;

	/**
	 * @return The port number from the URL component of this OW_CIMNameSpace.
	 */
	OW_Int32 getPortNumber() const;

	/**
	 * @return The protocol from the URL component of this OW_CIMNameSpace.
	 */
	OW_String getProtocol() const;

	/**
	 * @return The file name from the URL component of this OW_CIMNameSpace.
	 */
	OW_String getFileName() const;


	/**
	 * @return true if this namespace refers to a namespace hosted by the local
	 * CIMOM.
	 */
	OW_Bool isLocal() const;

	/**
	 * Set the namespace component of this OW_CIMNameSpace object.
	 * @param nameSpace The namespace for this object as an OW_String
	 */
	void setNameSpace(const OW_String& nameSpace);

	/**
	 * Set the host url component of this OW_CIMNameSpace object.
	 * @param hostUrl The new host url for this OW_CIMNameSpace.
	 */
	void setHostUrl(const OW_CIMUrl& hostUrl);

	/**
	 * Set the host of the url component for this OW_CIMNameSpace object.
	 * @param host The new host for the url component of this object.
	 */
	void setHost(const OW_String& host);

	/**
	 * Set the protocol
	 * @param protocol The protocol component of the url for the name space
	 */
	void setProtocol(const OW_String& protocol);

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
	 * @return The string representation of this OW_CIMNameSpace object.
	 */
	virtual OW_String toString() const;

	/**
	 * @return The MOF representation of this OW_CIMNameSpace object.
	 * Currently unimplemented.
	 */
	virtual OW_String toMOF() const {  return "UMIMPLEMENTED"; }

private:

	OW_Reference<NSData> m_pdata;
};

#endif	// __OW_CIMNAMESPACE_HPP__

