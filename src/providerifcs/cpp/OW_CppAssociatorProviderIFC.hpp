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
#ifndef OW_CPP_ASSOCIATORPROVIDERIFC_HPP_
#define OW_CPP_ASSOCIATORPROVIDERIFC_HPP_
#include "OW_config.h"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_AssociatorProviderInfo.hpp"

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
namespace OpenWBEM
{

/**
 * This interface is implemented by providers of dynamic association
 * classes. The CIMOM invokes these methods when it performs association
 * traversal.
 * All calls to the derived provider will be serialized so that providers need
 * not worry about multi-threading issues.
 */
class CppAssociatorProviderIFC: public virtual CppInstanceProviderIFC
{
	public:
		virtual ~CppAssociatorProviderIFC();
		
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
		virtual void getAssociatorProviderInfo(AssociatorProviderInfo&)
		{
		}
		/**
		 * This method is invoked in order to do the Associators operation as
		 * defined by the CIM operations over HTTP spec at
		 * http://www.dmtf.org/download/spec/xmls/CIM_HTTP_Mapping10.htm#SecAssociators
		 * This operation is used to enumerate CIM
		 * Instances that are associated to a particular source CIM Object.
		 *
		 * @param assocClass Defines the association that the objectName
		 * 	object should be associated to. The provider uses this
		 * 	information to identify which association must be
		 * 	traversed in case it supports more than one
		 * 	association.
		 * 	
		 * @param objectName Defines the source CIM Object whose associated
		 * 	Objects are to be returned.  This is an instance name.
		 * 	Instance name (modelpath).
		 *
		 * @param resultClass The resultClass input parameter, if not empty (""),
		 * 	MUST be a valid CIM Class name.  It acts as a filter on the
		 * 	returned set of Objects by mandating that each returned Object
		 * 	MUST be either an Instance of this Class (or one of its subclasses)
		 * 	or be this Class (or one of its subclasses).
		 *
		 * @param role The role input parameter, if not empty (""), MUST be a
		 * 	valid Property name.  It acts as a filter on the returned set of
		 * 	Objects by mandating that each returned Object MUST be associated
		 * 	to the source Object via an Association in which the source Object
		 * 	plays the specified role (i.e. the name of the Property in the
		 * 	Association Class that refers to the source Object MUST match the
		 * 	value of this parameter).
		 * 	
		 * @param resultRole The resultRole input parameter if not empty (""),
		 * 	MUST be a valid Property name.  It acts as a filter on the returned
		 * 	set of Objects by mandating that each returned Object MUST be
		 * 	associated to the source Object via an Association in which the
		 * 	returned Object plays the specified role (i.e. the name of the
		 * 	Property in the Association Class that refers to the returned
		 * 	Object MUST match the value of this parameter).
		 *
		 * @param includeQualifiers If the includeQualifiers input parameter
		 * 	is true this specifies that all Qualifiers for each Object
		 * 	(including Qualifiers on the Object and on any returned Properties)
		 * 	MUST be included as elements in the response.   If false no
		 * 	elements are present in each returned Object.
		 *
		 * @param includeClassOrigin If the includeClassOrigin input parameter
		 * 	is true, this specifies that the CLASSORIGIN attribute MUST be
		 * 	present on all appropriate elements in each returned Object. If
		 * 	false, no CLASSORIGIN attributes are present in each returned
		 * 	Object.
		 *
		 * @param propertyList If the propertyList input parameter is not NULL
		 * 	the members of the array define one or more Property names.
		 * 	Each returned Object MUST NOT include elements for any Properties
		 * 	missing from this list.  Note that if localOnly is specified as
		 * 	true (or deepInheritance is specified as false) this acts as an
		 * 	additional filter on the set or Properties returned (for example,
		 * 	if Property A is included in the propertyList but localOnly is set
		 * 	to true and A is not local to a returned Instance, then it will
		 * 	not be included in that Instance).  If the propertyList input
		 * 	parameter is an empty array this signifies that no Properties are
		 * 	included in each returned Object.  If the propertyList parameter
		 * 	is omitted this specifies that all Properties (subject to the
		 * 	conditions expressed by the other parameters) are included in each
		 * 	returned Object.  If the propertyList contains duplicate elements,
		 * 	the Server ignores the duplicates but otherwise processes the
		 * 	request normally.  If the propertyList contains elements which are
		 * 	invalid Property names for any target Object, the Server ignores
		 * 	such entries but should otherwise process the request normally.
		 * 	Clients SHOULD NOT explicitly specify properties in the propertyList
		 * 	parameter unless they have specified a non-empty value for the
		 * 	resultClass parameter.
		 *
		 * @returns If successful, the method returns zero or more CIM Instances
		 * 	meeting the requested criteria.
		 *
		 * @throws CIMException. - The following IDs can be expected.
		 * 	CIM_ERR_ACCESS_DENIED
		 * 	CIM_ERR_NOT_SUPPORTED
		 * 	CIM_ERR_INVALID_NAMESPACE
		 * 	CIM_ERR_INVALID_PARAMETER (including missing, duplicate,
		 * 	unrecognized or otherwise incorrect parameters)
		 * 	CIM_ERR_FAILED (some other unspecifed error occurred)
		 */
		virtual void associators(
				const ProviderEnvironmentIFCRef& env,
				CIMInstanceResultHandlerIFC& result,
				const String& ns,
				const CIMObjectPath& objectName,
				const String& assocClass,
				const String& resultClass,
				const String& role,
				const String& resultRole,
				WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
				WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
				const StringArray* propertyList) = 0;
		/**
		 * For the definition of this operation, refer to
		 * http://www.dmtf.org/download/spec/xmls/CIM_HTTP_Mapping10.htm#SecAssociatorNames
		 * This operation is used to enumerate the names of CIM
		 * Instances that are associated to a particular source CIM
		 * Instance.
		 *
		 * @param assocClass see associator parameter assocClass
		 * @param objectName see associator parameter objectName
		 * @param resultClass see associator parameter resultClass
		 * @param role see associator parameter role
		 * @param resultRole see associator parameter resultRole
		 * @returns If successful, the method returns zero or more full CIM
		 * 	Instance of Objects meeting the requested criteria.
		 * @throws CIMException - as defined in the associator method
		 */
		virtual void associatorNames(
				const ProviderEnvironmentIFCRef& env,
				CIMObjectPathResultHandlerIFC& result,
				const String& ns,
				const CIMObjectPath& objectName,
				const String& assocClass,
				const String& resultClass,
				const String& role,
				const String& resultRole ) = 0;
		/**
		 * For definition of this operation, refer to
		 * http://www.dmtf.org/download/spec/xmls/CIM_HTTP_Mapping10.htm#SecReferencesMethod
		 * This operation is used to enumerate the association objects
		 * that refer to a particular target CIM Instance.
		 *
		 * @param resultClass Defines the association that the objectName object
		 * 	should be associated to.  The provider uses this information to
		 * 	identify which association must be traversed in case it supports
		 * 	more than one association.
		 *
		 * @param objectName The objectName input parameter defines the target
		 * 	CIM Object whose referring Objects are to be returned.  This is an
		 * 	Instance name (model path).
		 *
		 * @param role The role input parameter, if not empty (""), MUST be a
		 * 	valid Property name.  It acts as a filter on the returned set of
		 * 	Objects by mandating that each returned Object MUST refer to the
		 * 	target Object via a Property whose name matches the value of this
		 * 	parameter.
		 *
		 * @param includeQualifiers see the includeQualifiers for associators.
		 * @param includeClassOrigin see the includeClassOrigin for associators.
		 * @param propertyList see the propertyList for associators
		 *
		 * @returns If successful, the method returns zero or more CIM Instances
		 * 	meeting the requested criteria.
		 *
		 * @throws CIMException - as defined for the associators method.
		 */
		virtual void references(
				const ProviderEnvironmentIFCRef& env,
				CIMInstanceResultHandlerIFC& result,
				const String& ns,
				const CIMObjectPath& objectName,
				const String& resultClass,
				const String& role,
				WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
				WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
				const StringArray* propertyList) = 0;
		/**
		 * For definition of this operation, refer to
		 *  ttp://www.dmtf.org/download/spec/xmls/CIM_HTTP_Mapping10.htm#SecReferenceNames
		 * This operation is used to enumerate the association objects
		 * that refer to a particular target CIM Instance.
		 *
		 * @param resultClass see the reference method.
		 * @param objectName see the reference method.
		 * @param role see the reference method
		 *
		 * @returns If successful, the method returns the names of zero or more
		 * full CIM Instance paths of Objects meeting the requested criteria.
		 * @throws CIMException - as defined for associators method.
		 */
		virtual void referenceNames(
				const ProviderEnvironmentIFCRef& env,
				CIMObjectPathResultHandlerIFC& result,
				const String& ns,
				const CIMObjectPath& objectName,
				const String& resultClass,
				const String& role ) = 0;
		virtual CppAssociatorProviderIFC* getAssociatorProvider() { return this; }
};
typedef SharedLibraryReference< Reference<CppAssociatorProviderIFC> > CppAssociatorProviderIFCRef;

} // end namespace OpenWBEM

typedef OpenWBEM::CppAssociatorProviderIFC OW_CppAssociatorProviderIFC OW_DEPRECATED;

#endif //#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

#endif
