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

#ifndef OW_SERVICEIFC_HPP_INCLUDE_GUARD_
#define OW_SERVICEIFC_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"

namespace OpenWBEM
{

// This class is a base interface for any services for Openwbem.
class ServiceIFC : public IntrusiveCountableBase
{
public:
	virtual ~ServiceIFC();
	virtual void setServiceEnvironment(const ServiceEnvironmentIFCRef& env) = 0;
	virtual void startService() = 0;
	virtual void shutdown() = 0;
};
typedef SharedLibraryReference< IntrusiveReference<ServiceIFC> > ServiceIFCRef;

} // end namespace OpenWBEM

#if !defined(OW_STATIC_SERVICES)
#define OW_SERVICE_FACTORY(derived, serviceName) \
extern "C" OpenWBEM::ServiceIFC* \
createService() \
{ \
	return new derived; \
} \
extern "C" const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
}
#else
#define OW_SERVICE_FACTORY(derived, serviceName) \
extern "C" OpenWBEM::ServiceIFC* \
createService_##serviceName() \
{ \
	return new derived; \
}
#endif /* !defined(OW_STATIC_SERVICES) */

#endif /* OW_SERVICEIFC_HPP_INCLUDE_GUARD_ */
