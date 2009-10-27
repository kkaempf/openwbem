/*******************************************************************************
* Copyright (C) 2007 Quest Software All rights reserved.
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
*  - Neither the name of Quest Software nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software OR THE CONTRIBUTORS
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

#ifndef OW_CPP_QUERY_PROVIDERIFC_HPP_INCLUDE_GUARD_
#define OW_CPP_QUERY_PROVIDERIFC_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CppProviderBaseIFC.hpp"
#include "OW_ProviderFwd.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_IfcsFwd.hpp"

namespace OW_NAMESPACE
{

/**
 * Classes wishing to implement a query provider may derive from this
 * class.
 */
class OW_PROVIDER_API CppQueryProviderIFC: public virtual CppProviderBaseIFC
{
public:
	CppQueryProviderIFC();
	virtual ~CppQueryProviderIFC();

	/**
	 * A provider should override this method to report which classes in
	 * which namespaces it instruments.
	 * It should insert an entry for each class it is responsible for.
	 *  The entry consists of the class name and an optional list of namespaces.
	 *  If the namespace list is empty, all namespaces are implied.
	 * If the method does nothing, then the provider's class must have a
	 * provider qualifier that identifies the provider.  This old behavior is
	 * deprecated and will be removed sometime in the future.  This method
	 * has a default implementation that does nothing It is not necessary to
	 * implement it for an out of process provider.
	 */
	virtual void getQueryProviderInfoWithEnv(
		const ProviderRegistrationEnvironmentIFCRef& env,
		QueryProviderInfo& info);

	/**
	 * A provider should override this method to report which classes in
	 * which namespaces it instruments.
	 * It should insert an entry for each class it is responsible for.
	 *  The entry consists of the class name and an optional list of namespaces.
	 *  If the namespace list is empty, all namespaces are implied.
	 * If the method does nothing, then the provider's class must have a
	 * provider qualifier that identifies the provider.  This old behavior is
	 * deprecated and will be removed sometime in the future.  This method
	 * has a default implementation that does nothing. It is not necessary to
	 * implement it for an out of process provider.
	 */
	virtual void getQueryProviderInfo(QueryProviderInfo& info);

	virtual void queryInstances(
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const WQLSelectStatement& query,
		const WQLCompile& compiledWhereClause,
		CIMInstanceResultHandlerIFC& result,
		const CIMClass& cimClass ) = 0;

	virtual CppQueryProviderIFC* getQueryProvider();
};

typedef blocxx::SharedLibraryReference< blocxx::IntrusiveReference<CppQueryProviderIFC> > CppQueryProviderIFCRef;


} // end namespace OW_NAMESPACE

#endif
