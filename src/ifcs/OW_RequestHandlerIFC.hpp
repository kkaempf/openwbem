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


#ifndef OW_REQUESTHANDLERIFC_HPP_
#define OW_REQUESTHANDLERIFC_HPP_

#include "OW_config.h"
#include "OW_Bool.hpp"
#include "OW_String.hpp"
#include <iosfwd>

class OW_CIMFeatures;

class OW_ServiceEnvironmentIFC;
typedef OW_Reference<OW_ServiceEnvironmentIFC> OW_ServiceEnvironmentIFCRef;

/**
 * This is an abstract base class for a CIM product requiring a HTTP Server.
 * (a CIM Server or a CIM Listener).  The OW_HTTPServer contains a reference
 * to one of these.  When a [M-]POST or OPTIONS is done, the process() and
 * options() functions for this class are called, respectively.
 */

class OW_RequestHandlerIFC;
typedef OW_Reference<OW_RequestHandlerIFC> OW_RequestHandlerIFCRef;

class OW_RequestHandlerIFC
{
public:
	OW_RequestHandlerIFC();

	virtual ~OW_RequestHandlerIFC();

	/**
	 * The HTTP server calls this once all HTTP headers have been
	 * processed and removed from the input stream.  Also, the http server
	 * takes care of any necessary content coding (SSL, chunking, compression).
	 * @param istr The input for the request to be processed.
	 * @param ostrEntity Non-error output gets written here.
	 * @param ostrError Error output gets written here.
	 * @path the path portion of the HTTP URL used to access the cimom
	 * @userName the name of the user accessing the CIM Product.
	 */
	void process(std::istream* istr, std::ostream* ostrEntity,
		std::ostream* ostrError, const OW_String& userName);

	/**
	 * Did an error occur during process()?  (should ostrEntity
	 * be sent back, or osrtError?)
	 * @return true if an error occurred.
	 */

	OW_Bool hasError();

	/**
	 * What options are available for a particular path?
	 * @param cf a OW_CIMFeatures object to fill out.
	 */
	void options(OW_CIMFeatures& cf)
		{  doOptions(cf); }

	/**
	 * Get the identifier of this request handler
	 * @return the path.
	 */
	OW_String getId() const;

	virtual OW_RequestHandlerIFCRef clone() const = 0;

	OW_ServiceEnvironmentIFCRef getEnvironment() const;

	virtual void setEnvironment(OW_ServiceEnvironmentIFCRef env);

protected:
	/**
	 * The HTTP server calls this once all HTTP headers have been
	 * processed and removed from the input stream.  Also, the http server
	 * takes care of any necessary content coding (SSL, chunking, compression).
	 * @param istr The input for the request to be processed.
	 * @param ostrEntity Non-error output gets written here.
	 * @param ostrError Error output gets written here.
	 * @path the path portion of the HTTP URL used to access the cimom
	 * @userName the name of the user accessing the CIM Product.
	 */
	virtual void doProcess(std::istream* istr, std::ostream* ostrEntity,
		std::ostream* ostrError, const OW_String& userName) = 0;

	/**
	 * Did an error occur during process()?  (should ostrEntity
	 * be sent back, or osrtError?)
	 * @return true if an error occurred.
	 */
	virtual OW_Bool doHasError() = 0;

	/**
	 * Fill out the Features that the request handler at the path supports.
	 * @param cf The features to fill out.
	 * @param path The requested path
	 */
	virtual void doOptions(OW_CIMFeatures& cf) = 0;

	virtual OW_String doGetId() const = 0;

private:
	OW_ServiceEnvironmentIFCRef m_env;
};

#define OW_CIMXML_ID "CIM/XML"
#define OW_BINARY_ID "OWBINARY"
#define OW_BINARY_USER_NAME_OPT "OW_BINARY_USER_NAME_OPT"

#define OW_REQUEST_HANDLER_FACTORY(derived) \
extern "C" OW_RequestHandlerIFC* \
createRequestHandler() \
{ \
	return new derived; \
} \
extern "C" const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
}


#endif
