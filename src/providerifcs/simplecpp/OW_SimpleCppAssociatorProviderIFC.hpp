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

#ifndef OW_SIMPLECPP_ASSOCIATORPROVIDERIFC_HPP_
#define OW_SIMPLECPP_ASSOCIATORPROVIDERIFC_HPP_

#include "OW_config.h"
#include "OW_SimpleCppProviderBaseIFC.hpp"
#include "OW_SharedLibraryReference.hpp"

/**
 * This interface is implemented by providers of dynamic association
 * classes. The CIMOM invokes these methods when it performs association
 * traversal.
 * All calls to the derived provider will be serialized so that providers need
 * not worry about multi-threading issues.
 */
class OW_SimpleCppAssociatorProviderIFC: public virtual OW_SimpleCppProviderBaseIFC
{
	public:
		virtual ~OW_SimpleCppAssociatorProviderIFC(){}
		
		/**
		 * This method is invoked in order to do the Associators operation as
		 * defined by the CIM operations over HTTP spec at
		 * http://www.dmtf.org/download/spec/xmls/CIM_HTTP_Mapping10.htm#SecAssociators
		 * This operation is used to enumerate CIM
		 * Instances that are associated to a particular source CIM Object.
		 *
		 * @param objectName Defines the source CIM Object whose associated
		 * 	Objects are to be returned.  This is an instance name.
		 * 	Instance name (modelpath).
		 *
		 * @param result Handles the result of the operation
		 *
		 * @throws OW_CIMException. - The following IDs can be expected.
		 * 	CIM_ERR_ACCESS_DENIED
		 * 	CIM_ERR_NOT_SUPPORTED
		 * 	CIM_ERR_INVALID_NAMESPACE
		 * 	CIM_ERR_INVALID_PARAMETER (including missing, duplicate,
		 * 	unrecognized or otherwise incorrect parameters)
		 * 	CIM_ERR_FAILED (some other unspecifed error occurred)
		 */
		virtual void associators(
				const OW_ProviderEnvironmentIFCRef& env,
				const OW_CIMObjectPath& objectName,
				OW_CIMInstanceResultHandlerIFC& result);

		/**
		 * For the definition of this operation, refer to
		 * http://www.dmtf.org/download/spec/xmls/CIM_HTTP_Mapping10.htm#SecAssociatorNames
		 * This operation is used to enumerate the names of CIM
		 * Instances that are associated to a particular source CIM
		 * Instance.
		 *
		 * If a provider does not implement this method, then associators() will
		 * called in lieu of associatorNames(), and the result will be
		 * translated into object paths.
		 *
		 * @param objectName see associator parameter objectName
		 * @param result Handles the result of the operation
		 *
		 * @throws OW_CIMException - as defined in the associator method
		 */
		virtual void associatorNames(
				const OW_ProviderEnvironmentIFCRef& env,
				const OW_CIMObjectPath& objectName,
				OW_CIMObjectPathResultHandlerIFC& result);

		/**
		 * For definition of this operation, refer to
		 * http://www.dmtf.org/download/spec/xmls/CIM_HTTP_Mapping10.htm#SecReferencesMethod
		 * This operation is used to enumerate the association objects
		 * that refer to a particular target CIM Instance.
		 *
		 * @param objectName The objectName input parameter defines the target
		 * 	CIM Object whose referring Objects are to be returned.  This is an
		 * 	Instance name (model path).
		 *
		 * @param result Handles the result of the operation
		 *
		 * @throws OW_CIMException - as defined for the associators method.
		 */
		virtual void references(
				const OW_ProviderEnvironmentIFCRef& env,
				const OW_CIMObjectPath& objectName,
				OW_CIMInstanceResultHandlerIFC& result);

		/**
		 * For definition of this operation, refer to
		 *  ttp://www.dmtf.org/download/spec/xmls/CIM_HTTP_Mapping10.htm#SecReferenceNames
		 * This operation is used to enumerate the association objects
		 * that refer to a particular target CIM Instance.
		 *
		 * If a provider does not implement this method, then references() will
		 * called in lieu of referenceNames(), and the result will be
		 * translated into object paths.
		 *
		 * @param objectName see the reference method.
		 * @param result Handles the result of the operation
		 *
		 * @throws OW_CIMException - as defined for associators method.
		 */
		virtual void referenceNames(
				const OW_ProviderEnvironmentIFCRef& env,
				const OW_CIMObjectPath& objectName,
				OW_CIMObjectPathResultHandlerIFC& result);

		virtual OW_SimpleCppAssociatorProviderIFC* getAssociatorProvider() { return this; }
};

typedef OW_SharedLibraryReference< OW_SimpleCppAssociatorProviderIFC > OW_SimpleCppAssociatorProviderIFCRef;

#endif

