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

#ifndef OW_CIMNAMESPACE_HPP_INCLUDE_GUARD_
#define OW_CIMNAMESPACE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMBase.hpp"
#include "OW_COWReference.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_String.hpp"
#include "OW_CIMNULL.hpp"

namespace OpenWBEM
{

/**
 * The CIMNameSpace class represents a CIM namespace. The CIM namespace
 * is a component of a CIM object name (namespace + model path) that provides
 * a scope within which all objects are unique. CIMNameSpace objects are
 * ref counted and copy on write. It is possible to have a NULL CIMNameSpace
 * object.
 */
class CIMNameSpace : public CIMBase
{
private:
	struct NSData;
	friend bool operator<(const CIMNameSpace::NSData& x, 
			const CIMNameSpace::NSData& y); 
public:
	/**
	 * Create a new CIMNameSpace object.
	 */
	CIMNameSpace();
	/**
	 * Create a NULL CIMNameSpace object.
	 */
	explicit CIMNameSpace(CIMNULL_t);
	/**
	 * Create a new CIMNameSpace object.
	 * @param hostUrl The url component of this CIMNameSpace object.
	 * @param nameSpace The namespace (e.g. "root/cimv2") component of this
	 *		CIMNameSpace object.
	 */
	CIMNameSpace(const CIMUrl& hostUrl,
		const String& nameSpace);
	/**
	 * Create a new CIMNameSpace object.
	 * @param hostUrl The url component of this CIMNameSpace object.
	 * @param nameSpace The namespace (e.g. "root/cimv2") component of this
	 *		CIMNameSpace object.
	 */
	explicit CIMNameSpace(const String& nameSpace);
	/**
	 * Create a new CIMNameSpace object.
	 * @param hostUrl The url component of this CIMNameSpace object.
	 * @param nameSpace The namespace (e.g. "root/cimv2") component of this
	 *		CIMNameSpace object.
	 */
	explicit CIMNameSpace(const char* nameSpace);
	/**
	 * Create a new CIMNameSpace object that is a copy of another.
	 * @param arg The CIMNameSpace to make this object a copy of.
	 */
	CIMNameSpace(const CIMNameSpace& arg);
	/**
	 * Destroy this CIMNameSpace object.
	 */
	~CIMNameSpace();
	/**
	 * Set this to a null object.
	 */
	virtual void setNull();
	/**
	 * Assignment operator
	 * @param arg The CIMNameSpace object to assign to this one.
	 * @return A reference to this object after the assignment has been made.
	 */
	CIMNameSpace& operator= (const CIMNameSpace& arg);
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
	 * @return The namespace component of this CIMNameSpace object.
	 */
	String getNameSpace() const;
	/**
	 * @return The host component of this CIMNameSpace object.
	 */
	String getHost() const;
	/**
	 * @return The host URL component of this CIMNameSpace object.
	 */
	CIMUrl getHostUrl() const;
	/**
	 * @return The port number from the URL component of this CIMNameSpace.
	 */
	Int32 getPortNumber() const;
	/**
	 * @return The protocol from the URL component of this CIMNameSpace.
	 */
	String getProtocol() const;
	/**
	 * @return The file name from the URL component of this CIMNameSpace.
	 */
	String getFileName() const;
	/**
	 * @return true if this namespace refers to a namespace hosted by the local
	 * CIMOM.
	 */
	bool isLocal() const;
	/**
	 * Set the namespace component of this CIMNameSpace object.
	 * @param nameSpace The namespace for this object as an String
	 * @return a reference to *this
	 */
	CIMNameSpace& setNameSpace(const String& nameSpace);
	/**
	 * Set the host url component of this CIMNameSpace object.
	 * @param hostUrl The new host url for this CIMNameSpace.
	 * @return a reference to *this
	 */
	CIMNameSpace& setHostUrl(const CIMUrl& hostUrl);
	/**
	 * Set the host of the url component for this CIMNameSpace object.
	 * @param host The new host for the url component of this object.
	 * @return a reference to *this
	 */
	CIMNameSpace& setHost(const String& host);
	/**
	 * Set the protocol
	 * @param protocol The protocol component of the url for the name space
	 * @return a reference to *this
	 */
	CIMNameSpace& setProtocol(const String& protocol);
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
	 * @return The string representation of this CIMNameSpace object.
	 */
	virtual String toString() const;
	/**
	 * @return The MOF representation of this CIMNameSpace object.
	 * Currently unimplemented.
	 */
	virtual String toMOF() const {  return "UMIMPLEMENTED"; }
private:
	COWReference<NSData> m_pdata;
	friend bool operator<(const CIMNameSpace& lhs, const CIMNameSpace& rhs);
};

} // end namespace OpenWBEM

typedef OpenWBEM::CIMNameSpace OW_CIMNameSpace OW_DEPRECATED;

#endif
