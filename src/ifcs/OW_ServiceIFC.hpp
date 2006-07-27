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

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

// This class is a base interface for any services for Openwbem.
class OW_COMMON_API ServiceIFC : virtual public IntrusiveCountableBase
{
public:
	virtual ~ServiceIFC();
	/**
	 * Every service may have a name.  The name is used to calculate dependencies, other services can return
	 * the name from getDependencies(), to instruct the cimom about which order to start and shutdown
	 * all the services.
	 * The default is to have no name, in that case, no other service can depend on it.
	 */
	virtual String getName() const;

	/**
	 * Get the list of services this service depends on. The returned services will be started before and
	 * shutdown after this service. It is an error to return the name of a non-existent service, and the
	 * cimom startup will fail.
	 * The default is to have no dependencies.
	 */
	virtual StringArray getDependencies() const;

	/**
	 * Get the list of services that depend on this service which don't return this service's name from
	 * getDependencies().  It is an error to return a service's name if that service returns this service's
	 * name from getDependencies().
	 * The returned services will be started after and shutdown before this service.  It is an error to
	 * return the name of a non-existent service, and the cimom startup will fail.
	 * This mechanism allows a service to be initialized before another service, even if the other service didn't
	 * specify the name of this service as a dependency.
	 * The default is to have no dependent services.
	 */
	virtual StringArray getDependentServices() const;

	/**
	 * init() will be called to give the derived class an opportunity to initialize itself.
	 * Do not create threads which interact with the environment until start() is called.
	 * During the loading/initializing phase, the environment is single-threaded.
	 * @param env The service's interface to it's environment. A copy of this may be saved and re-used.
	 *  All copies of env or objects obtained by calling member functions of env should be set to 0 in
	 *  shutdown() to prevent circular reference counts.
	 */
	virtual void init(const ServiceEnvironmentIFCRef& env) = 0;
	/**
	 * initialized() will be called after init() has been sucessfully called on all services.
	 * This gives a service the chance to communicate with another service if necessary.
	 * The default implementation does nothing.
	 */
	virtual void initialized();

	/**
	 * In start(), a service should start doing whatever it does, such as starting a new thread or adding
	 * selectables to the environment.
	 * A service can't rely on the order of initalization, so if it needs to communicate with another one,
	 * that works hould be done in started()
	 * start() should not return until the service is actually started, but it must return.
	 */
	virtual void start();

	/**
	 * started() will be called on all services after start() has been called on all services.
	 * The default implementation does nothing.
	 */
	virtual void started();

	/**
	 * shuttingdown() will be called before shutdown() is called on all services. This gives a service the
	 * chance to communicate with any other services before they are shutdown.  After shuttingDown() is called
	 * the services will begin to be shutdown.
	 * The default implementation does nothing.
	 */
	virtual void shuttingDown();

	/**
	 * The service must shutdown completely before returning from this function.
	 * All copies of the service environment or objects obtained from it should be set to 0 to avoid
	 * circular reference counts.
	 */
	virtual void shutdown() = 0;
};

} // end namespace OW_NAMESPACE

#if !defined(OW_STATIC_SERVICES)
#define OW_SERVICE_FACTORY(derived, serviceName) \
extern "C" OW_EXPORT OW_NAMESPACE::ServiceIFC* \
createService() \
{ \
	return new derived; \
} \
extern "C" OW_EXPORT const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
}
#else
#define OW_SERVICE_FACTORY(derived, serviceName) \
extern "C" OW_NAMESPACE::ServiceIFC* \
createService_##serviceName() \
{ \
	return new derived; \
}
#endif /* !defined(OW_STATIC_SERVICES) */

#endif /* OW_SERVICEIFC_HPP_INCLUDE_GUARD_ */
