/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
* Copyright (C) 2005-2006 Novell, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Dan Nuffer
 */

#ifndef OW_CPP_METHODPROVIDERIFC_HPP_
#define OW_CPP_METHODPROVIDERIFC_HPP_
#include "OW_config.h"
#include "OW_CppProviderBaseIFC.hpp"
#include "blocxx/SharedLibraryReference.hpp"
#include "OW_MethodProviderInfo.hpp"

namespace OW_NAMESPACE
{

/**
 * This is the interface implemented by method providers.  These providers are
 * used to provide implementation for all methods of CIM classes.
 * All calls to the derived provider will be serialized so that providers need
 * not worry about multi-threading issues.
 */
class OW_CPPPROVIFC_API CppMethodProviderIFC: public virtual CppProviderBaseIFC
{
public:
	virtual ~CppMethodProviderIFC();
	/**
	 * A provider should override this method to report which classes in
	 * which namespaces it instruments.
	 * It should insert an entry for each class it is responsible for.
	 *  The entry consists of the class name and an optional list of namespaces.
	 *  If the namespace list is empty, all namespaces are implied.
	 * If the method does nothing, then the provider's class must have a
	 * provider qualifier that identifies the provider.  This old behavior is
	 * deprecated and will be removed sometime in the future.  This method
	 * has a default implementation that does nothing, to allow old providers
	 * to be migrated forward with little or no change, but once the old
	 * provider location method is removed, this member function will be pure
	 * virtual.
	 */
	virtual void getMethodProviderInfoWithEnv(
		const ProviderRegistrationEnvironmentIFCRef& env,
		MethodProviderInfo&);

	/**
	 * A provider should override this method to report which classes in
	 * which namespaces it instruments.
	 * It should insert an entry for each class it is responsible for.
	 *  The entry consists of the class name and an optional list of namespaces.
	 *  If the namespace list is empty, all namespaces are implied.
	 * If the method does nothing, then the provider's class must have a
	 * provider qualifier that identifies the provider.  This old behavior is
	 * deprecated and will be removed sometime in the future.  This method
	 * has a default implementation that does nothing, to allow old providers
	 * to be migrated forward with little or no change, but once the old
	 * provider location method is removed, this member function will be pure
	 * virtual.
	 */
	virtual void getMethodProviderInfo(MethodProviderInfo&);

	/**
	 * The CIMOM calls this method when the method specified in the parameters
	 * is to be invoked.
	 *
	 * @param ns The namespace of the target object.
	 * @param path Contains the path to the instance whose method must be
	 * 	invoked.
	 * @param methodName The name of the method.
	 * @param inParams An array of CIMValues which are the input parameters
	 * 	for this method.
	 * @param outParams An array of CIMValues which are the output
	 * 	parameters for this method.
	 *
	 * @return CIMValue - The return value of the method.  Must be a
	 *    valid CIMValue.
	 *
	 * @throws CIMException
	 */
	virtual CIMValue invokeMethod(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMObjectPath& path,
			const String& methodName,
			const CIMParamValueArray& in,
			CIMParamValueArray& out ) = 0;

	enum ELockType
	{
		E_NO_LOCK,
		E_READ_LOCK,
		E_WRITE_LOCK
	};

	/**
	 * Return the type of lock necessary for the method invocation. If this method is not overridden, then the default
	 * implementation will return E_WRITE_LOCK.
	 *
	 * If a method does callbacks via a CIMOMHandleRef obtained from env, you as a developer must carefully consider how
	 * your method provider is written and what guarantees the method provider needs from the cimom.
	 *
	 * If getLockTypeForMethod() returns E_NO_LOCK, then the read-write mutex will not be locked by the provider's
	 * calling thread. This has the following implications for the method provider:
	 *  - It must be fully thread safe.
	 *  - No isolation guarantees about the CIMOM can be made. Other threads may be simultaneously modifying the
	 *    repository or other providers. Any callback operations will acquire the necessary lock, but it is only held
	 *    for one operation. Multiple operations are not atomic.
	 *
	 * If getLockTypeForMethod() returns E_READ_LOCK, then the read-write mutex will be read locked by the provider's
	 * calling thread. This has the following implications for the method provider:
	 *  - It must be fully thread safe.
	 *  - No other write operations will occur in the CIMOM, so the provider can assume that the repository and other
	 *    providers will not change state during the execution of invokeMethod().
	 *
	 * If getLockTypeForMethod() returns E_WRITE_LOCK, then the read-write mutex will be write locked by the provider's
	 * calling thread. This has the following implications for the method provider:
	 *  - It does not have to be thread safe, only one thread will execute invokeMethod() at a time.
	 *  - No other write or read operations will occur in the CIMOM, so the provider can assume that the repository and
	 *     other providers will not change state during the execution of invokeMethod().
	 *
	 * @param ns The namespace of the target object.
	 * @param path Contains the path to the instance whose method must be
	 * 	invoked.
	 * @param methodName The name of the method.
	 * @param inParams An array of CIMValues which are the input parameters
	 *  for this method.
	 *
	 * @return The necessary locking type.
	 *
	 * @throws CIMException
	 */
	virtual ELockType getLockTypeForMethod(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName,
		const CIMParamValueArray& in);

	virtual CppMethodProviderIFC* getMethodProvider();
};
typedef SharedLibraryReference< IntrusiveReference<CppMethodProviderIFC> > CppMethodProviderIFCRef;

} // end namespace OW_NAMESPACE

#endif

