/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_URL_HPP_
#define OW_URL_HPP_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Exception.hpp"

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(MalformedURL)

/**
 * This class represents URLs, and is used to easily parse and manage them.
 * See DMTF DSP0207 WBEM URI Mapping Specification 1.0.0
 */
struct URL
{
	/**
	 * Creates an URL with empty member variables.  This is mean to be filled
	 * out before being used.
	 */
	URL();

	/**
	 * Allocate a new URL, based on a string.  This ctor parses the string
	 * and fills out the member variables.
	 * An Acceptable URL for this class is defined as follows:
	 * [scheme"://"][[<principal>][":"<credential>]"@"]<host>[":"<port>]["/"<namespace name>["/:"<model path>]]
	 * The only required element is <host>
	 * 
	 * Standard WBEM schemes are: cimxml.wbem, cimxml.wbems, http, https.
	 * OW specific WBEM schemes are: owbinary.wbem, owbinary.wbems
	 * 
	 * A port may be a number to indicate a TCP port, or it may be the special
	 *  value owipc which indicates the Unix Domain Socket for the system.
	 * 
	 * @param sUrl The URL such as "https://jdd:test@myhost.com:5989/interop/:CIM_Namespace.Name=unknown,CreationClassName=CIM_ComputerSystem"
	 * @throws MalformedURLException
	 */
	URL(const String& sUrl);
	String scheme;
	String principal;
	String credential;
	String host;
	String port;
	String namespaceName;
	String modelPath;
	/**
	 * Return a string based on the data in the member variables.
	 */
	String toString() const;

	// known schemes
	static const char* const CIMXML_WBEM;
	static const char* const CIMXML_WBEMS;
	static const char* const HTTP;
	static const char* const HTTPS;
	static const char* const OWBINARY_WBEM;
	static const char* const OWBINARY_WBEMS;

	// used to test for owbinary.wbem or owbinary.wbems, also used to be used
	// to specify the owbinary encoding when part of the path (now 
	// namespaceName)
	static const char* const OWBINARY;

	// special port
	static const char* const OWIPC;

};

} // end namespace OpenWBEM

#endif
