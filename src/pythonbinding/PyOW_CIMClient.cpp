/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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
#include <OW_CIMClient.hpp>
#include <OW_ResultHandlerIFC.hpp>
#include <OW_CIMClassEnumeration.hpp>
#include <OW_CIMParamValue.hpp>
#include <OW_CIMQualifierType.hpp>
#include <OW_CIMValue.hpp>
#include <OW_CIMInstance.hpp>
#include <OW_CIMObjectPath.hpp>
// note this comes *after* the OpenWBEM headers, because it has a
// #define ANY void
// which really screws up OpenWBEM
#include <boost/python.hpp>
#ifdef ANY
#undef ANY
#endif

namespace OpenWBEM
{

using namespace boost::python;
 
namespace {
// CIMClient
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_enumNameSpaceE_overloads, CIMClient::enumNameSpaceE, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_enumNameSpace_overloads, CIMClient::enumNameSpace, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_enumClass_overloads, CIMClient::enumClass, 2, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_enumClassE_overloads, CIMClient::enumClassE, 1, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_enumClassNames_overloads, CIMClient::enumClassNames, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_enumClassNamesE_overloads, CIMClient::enumClassNamesE, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_enumInstances_overloads, CIMClient::enumInstances, 2, 7)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_enumInstancesE_overloads, CIMClient::enumInstancesE, 1, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_getClass_overloads, CIMClient::getClass, 1, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_getInstance_overloads, CIMClient::getInstance, 1, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_modifyInstance_overloads, CIMClient::modifyInstance, 1, 3)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_associatorNames_overloads, CIMClient::associatorNames, 2, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_associatorNamesE_overloads, CIMClient::associatorNamesE, 1, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_associators_overloads, CIMClient::associators, 2, 9)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_associatorsE_overloads, CIMClient::associatorsE, 1, 8)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_associatorsClasses_overloads, CIMClient::associatorsClasses, 2, 9)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_associatorsClassesE_overloads, CIMClient::associatorsClassesE, 1, 8)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_referenceNames_overloads, CIMClient::referenceNames, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_referenceNamesE_overloads, CIMClient::referenceNamesE, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_references_overloads, CIMClient::references, 2, 7)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_referencesE_overloads, CIMClient::referencesE, 1, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_referencesClasses_overloads, CIMClient::referencesClasses, 2, 7)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CIMClient_referencesClassesE_overloads, CIMClient::referencesClassesE, 1, 6)
#endif
}
void registerCIMClient()
{
	//    CIMClient
	class_<CIMClient>("CIMClient", init<const String&, const String&, optional<const ClientAuthCBIFCRef&> >())
		.def("setNameSpace", &CIMClient::setNameSpace)
		.def("getNameSpace", &CIMClient::getNameSpace)
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
		.def("createNameSpace", &CIMClient::createNameSpace)
		.def("deleteNameSpace", &CIMClient::deleteNameSpace)
#endif
		.def("enumNameSpaceE", &CIMClient::enumNameSpaceE, CIMClient_enumNameSpaceE_overloads(args("deep")))
		.def("enumNameSpace", &CIMClient::enumNameSpace, CIMClient_enumNameSpace_overloads(args("deep")))
		.def("enumClass", &CIMClient::enumClass, CIMClient_enumClass_overloads(args("deep", "localOnly", "includeQualifiers", "includeClassOrigin")))
		.def("enumClassE", &CIMClient::enumClassE, CIMClient_enumClassE_overloads(args("deep", "localOnly", "includeQualifiers", "includeClassOrigin")))
		.def("enumClassNames", &CIMClient::enumClassNames, CIMClient_enumClassNames_overloads(args("deep")))
		.def("enumClassNamesE", &CIMClient::enumClassNamesE, CIMClient_enumClassNamesE_overloads(args("deep")))
		.def("enumInstances", &CIMClient::enumInstances, CIMClient_enumInstances_overloads(args("deep", "localOnly", "includeQualifiers", "includeClassOrigin", "propertyList")))
		.def("enumInstancesE", &CIMClient::enumInstancesE, CIMClient_enumInstancesE_overloads(args("deep", "localOnly", "includeQualifiers", "includeClassOrigin", "propertyList")))
		.def("enumInstanceNames", &CIMClient::enumInstanceNames)
		.def("enumInstanceNamesE", &CIMClient::enumInstanceNamesE)
		.def("getClass", &CIMClient::getClass, CIMClient_getClass_overloads(args("localOnly", "includeQualifiers", "includeClassOrigin", "propertyList")))
		.def("getInstance", &CIMClient::getInstance, CIMClient_getInstance_overloads(args("localOnly", "includeQualifiers", "includeClassOrigin", "propertyList")))
		.def("invokeMethod", &CIMClient::invokeMethod)
		.def("getQualifierType", &CIMClient::getQualifierType)
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
		.def("setQualifierType", &CIMClient::setQualifierType)
		.def("deleteQualifierType", &CIMClient::deleteQualifierType)
		.def("enumQualifierTypes", &CIMClient::enumQualifierTypes)
		.def("enumQualifierTypesE", &CIMClient::enumQualifierTypesE)
#endif
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
		.def("modifyClass", &CIMClient::modifyClass)
		.def("createClass", &CIMClient::createClass)
		.def("deleteClass", &CIMClient::deleteClass)
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
		.def("modifyInstance", &CIMClient::modifyInstance, CIMClient_modifyInstance_overloads(args("includeQualifiers", "propertyList")))
		.def("createInstance", &CIMClient::createInstance)
		.def("deleteInstance", &CIMClient::deleteInstance)
#endif
		.def("getProperty", &CIMClient::getProperty)
		.def("setProperty", &CIMClient::setProperty)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		.def("associatorNames", &CIMClient::associatorNames, CIMClient_associatorNames_overloads(args("assocClass", "resultClass", "role", "resultRole")))
		.def("associatorNamesE", &CIMClient::associatorNamesE, CIMClient_associatorNamesE_overloads(args("assocClass", "resultClass", "role", "resultRole")))
		.def("associators", &CIMClient::associators, CIMClient_associators_overloads(args("assocClass", "resultClass", "role", "resultRole", "includeQualifiers", "includeClassOrigin", "propertyList")))
		.def("associatorsE", &CIMClient::associatorsE, CIMClient_associatorsE_overloads(args("assocClass", "resultClass", "role", "resultRole", "includeQualifiers", "includeClassOrigin", "propertyList")))
		.def("associatorsClasses", &CIMClient::associatorsClasses, CIMClient_associatorsClasses_overloads(args("assocClass", "resultClass", "role", "resultRole", "includeQualifiers", "includeClassOrigin", "propertyList")))
		.def("associatorsClassesE", &CIMClient::associatorsClassesE, CIMClient_associatorsClassesE_overloads(args("assocClass", "resultClass", "role", "resultRole", "includeQualifiers", "includeClassOrigin", "propertyList")))
		.def("referenceNames", &CIMClient::referenceNames, CIMClient_referenceNames_overloads(args("resultClass", "role")))
		.def("referenceNamesE", &CIMClient::referenceNamesE, CIMClient_referenceNamesE_overloads(args("resultClass", "role")))
		.def("references", &CIMClient::references, CIMClient_references_overloads(args("resultClass", "role", "includeQualifiers", "includeClassOrigin", "propertyList")))
		.def("referencesE", &CIMClient::referencesE, CIMClient_referencesE_overloads(args("resultClass", "role", "includeQualifiers", "includeClassOrigin", "propertyList")))
		.def("referencesClasses", &CIMClient::referencesClasses, CIMClient_referencesClasses_overloads(args("resultClass", "role", "includeQualifiers", "includeClassOrigin", "propertyList")))
		.def("referencesClassesE", &CIMClient::referencesClassesE, CIMClient_referencesClassesE_overloads(args("resultClass", "role", "includeQualifiers", "includeClassOrigin", "propertyList")))
#endif
		.def("execQuery", &CIMClient::execQuery)
		.def("execQueryE", &CIMClient::execQueryE)
		;
}

} // end namespace OpenWBEM

