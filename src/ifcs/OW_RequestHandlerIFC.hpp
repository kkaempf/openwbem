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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#ifndef OW_REQUESTHANDLERIFC_HPP_
#define OW_REQUESTHANDLERIFC_HPP_
#include "OW_config.h"
#include "OW_Types.h"
#include "OW_String.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_SortedVectorMap.hpp"
#include <iosfwd>

namespace OpenWBEM
{

class CIMFeatures;
class ServiceEnvironmentIFC;
typedef Reference<ServiceEnvironmentIFC> ServiceEnvironmentIFCRef;
class OperationContext;

/**
 * This is an abstract base class for a CIM product requiring a HTTP Server.
 * (a CIM Server or a CIM Listener).  The HTTPServer contains a reference
 * to one of these.  When a [M-]POST or OPTIONS is done, the process() and
 * options() functions for this class are called, respectively.
 */
class RequestHandlerIFC;
typedef SharedLibraryReference< Reference<RequestHandlerIFC> > RequestHandlerIFCRef;
class RequestHandlerIFC
{
public:
	RequestHandlerIFC();
	virtual ~RequestHandlerIFC();
	/**
	 * The HTTP server calls this once all HTTP headers have been processed
	 * and removed from the input stream.  Also, the http server takes care
	 * of any necessary content coding (SSL, chunking, compression). 
	 * @param istr The input for the request to be processed.
	 * @param ostrEntity Non-error output gets written here.
	 * @param ostrError Error output gets written here.
	 * @context the name of the user accessing the CIM Product.
	 */
	void process(std::istream* istr, std::ostream* ostrEntity,
		std::ostream* ostrError, OperationContext& context);
	/**
	 * Did an error occur during process()?  (should ostrEntity
	 * be sent back, or osrtError?)
	 * @return true if an error occurred.
	 */
	bool hasError(Int32& errCode, String& errDescr);
	bool hasError() { return m_hasError; }
	/**
	 * What options are available for a particular path?
	 * @param cf a CIMFeatures object to fill out.
	 */
	void options(CIMFeatures& cf, OperationContext& context)
		{  doOptions(cf, context); }
	virtual RequestHandlerIFC* clone() const = 0;
	ServiceEnvironmentIFCRef getEnvironment() const;
	virtual void setEnvironment(ServiceEnvironmentIFCRef env);
	virtual StringArray getSupportedContentTypes() const = 0;
	virtual String getContentType() const = 0;
	String getCIMError() const { return m_cimError; }
	void setError(Int32 errorCode, const String& errorDescription)
	{
		m_hasError = true;
		m_errorCode = errorCode;
		m_errorDescription = errorDescription;
	}
	void clearError()
	{
		m_hasError = false;
		m_errorCode = 0;
		m_errorDescription.erase();
	}
protected:
	/**
	 * The HTTP server calls this once all HTTP headers have been processed
	 * and removed from the input stream.  Also, the http server takes care
	 * of any necessary content coding (SSL, chunking, compression). 
	 * @param istr The input for the request to be processed.
	 * @param ostrEntity Non-error output gets written here.
	 * @param ostrError Error output gets written here.
	 * @path the path portion of the HTTP URL used to access the cimom
	 * @context the operation context.
	 */
	virtual void doProcess(std::istream* istr, std::ostream* ostrEntity,
		std::ostream* ostrError, OperationContext& context) = 0;
	/**
	 * Fill out the Features that the request handler at the path supports.
	 * @param cf The features to fill out.
	 * @param path The requested path
	 */
	virtual void doOptions(CIMFeatures& cf, OperationContext& context) = 0;
	String m_cimError;
	String getHost();
	String m_cachedHost;
private:
	// set these through setError() and clearError()
	bool m_hasError;
	Int32 m_errorCode;
	String m_errorDescription;
	ServiceEnvironmentIFCRef m_env;
};

} // end namespace OpenWBEM


#if !defined(OW_STATIC_SERVICES)
#define OW_REQUEST_HANDLER_FACTORY(derived, handlerName) \
extern "C" OpenWBEM::RequestHandlerIFC* \
createRequestHandler() \
{ \
	return new derived; \
} \
extern "C" const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
}
#else
#define OW_REQUEST_HANDLER_FACTORY(derived, handlerName) \
extern "C" OpenWBEM::RequestHandlerIFC* \
createRequestHandler_##handlerName() \
{ \
	return new derived; \
}
#endif /* !defined(OW_STATIC_SERVICES) */

#endif
