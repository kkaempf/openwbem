/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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

using namespace boost::python;

 
namespace {

// OW_CIMClient
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_enumNameSpaceE_overloads, OW_CIMClient::enumNameSpaceE, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_enumNameSpace_overloads, OW_CIMClient::enumNameSpace, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_enumClass_overloads, OW_CIMClient::enumClass, 2, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_enumClassE_overloads, OW_CIMClient::enumClassE, 1, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_enumClassNames_overloads, OW_CIMClient::enumClassNames, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_enumClassNamesE_overloads, OW_CIMClient::enumClassNamesE, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_enumInstances_overloads, OW_CIMClient::enumInstances, 2, 7)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_enumInstancesE_overloads, OW_CIMClient::enumInstancesE, 1, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_getClass_overloads, OW_CIMClient::getClass, 1, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_getInstance_overloads, OW_CIMClient::getInstance, 1, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_modifyInstance_overloads, OW_CIMClient::modifyInstance, 1, 3)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_associatorNames_overloads, OW_CIMClient::associatorNames, 2, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_associatorNamesE_overloads, OW_CIMClient::associatorNamesE, 1, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_associators_overloads, OW_CIMClient::associators, 2, 9)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_associatorsE_overloads, OW_CIMClient::associatorsE, 1, 8)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_associatorsClasses_overloads, OW_CIMClient::associatorsClasses, 2, 9)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_associatorsClassesE_overloads, OW_CIMClient::associatorsClassesE, 1, 8)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_referenceNames_overloads, OW_CIMClient::referenceNames, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_referenceNamesE_overloads, OW_CIMClient::referenceNamesE, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_references_overloads, OW_CIMClient::references, 2, 7)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_referencesE_overloads, OW_CIMClient::referencesE, 1, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_referencesClasses_overloads, OW_CIMClient::referencesClasses, 2, 7)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_CIMClient_referencesClassesE_overloads, OW_CIMClient::referencesClassesE, 1, 6)
#endif

}


void registerOW_CIMClient()
{
    //    OW_CIMClient
    class_<OW_CIMClient>("OW_CIMClient", init<const OW_String&, const OW_String&, optional<const OW_ClientAuthCBIFCRef&> >())
        .def("setNameSpace", &OW_CIMClient::setNameSpace)
        .def("getNameSpace", &OW_CIMClient::getNameSpace)
        .def("createNameSpace", &OW_CIMClient::createNameSpace)
        .def("deleteNameSpace", &OW_CIMClient::deleteNameSpace)
        .def("enumNameSpaceE", &OW_CIMClient::enumNameSpaceE, OW_CIMClient_enumNameSpaceE_overloads(args("deep")))
        .def("enumNameSpace", &OW_CIMClient::enumNameSpace, OW_CIMClient_enumNameSpace_overloads(args("deep")))
        .def("deleteClass", &OW_CIMClient::deleteClass)
        .def("deleteInstance", &OW_CIMClient::deleteInstance)
        .def("deleteQualifierType", &OW_CIMClient::deleteQualifierType)
        .def("enumClass", &OW_CIMClient::enumClass, OW_CIMClient_enumClass_overloads(args("deep", "localOnly", "includeQualifiers", "includeClassOrigin")))
        .def("enumClassE", &OW_CIMClient::enumClassE, OW_CIMClient_enumClassE_overloads(args("deep", "localOnly", "includeQualifiers", "includeClassOrigin")))
        .def("enumClassNames", &OW_CIMClient::enumClassNames, OW_CIMClient_enumClassNames_overloads(args("deep")))
        .def("enumClassNamesE", &OW_CIMClient::enumClassNamesE, OW_CIMClient_enumClassNamesE_overloads(args("deep")))
        .def("enumInstances", &OW_CIMClient::enumInstances, OW_CIMClient_enumInstances_overloads(args("deep", "localOnly", "includeQualifiers", "includeClassOrigin", "propertyList")))
        .def("enumInstancesE", &OW_CIMClient::enumInstancesE, OW_CIMClient_enumInstancesE_overloads(args("deep", "localOnly", "includeQualifiers", "includeClassOrigin", "propertyList")))
        .def("enumInstanceNames", &OW_CIMClient::enumInstanceNames)
        .def("enumInstanceNamesE", &OW_CIMClient::enumInstanceNamesE)
        .def("enumQualifierTypes", &OW_CIMClient::enumQualifierTypes)
        .def("enumQualifierTypesE", &OW_CIMClient::enumQualifierTypesE)
        .def("getClass", &OW_CIMClient::getClass, OW_CIMClient_getClass_overloads(args("localOnly", "includeQualifiers", "includeClassOrigin", "propertyList")))
        .def("getInstance", &OW_CIMClient::getInstance, OW_CIMClient_getInstance_overloads(args("localOnly", "includeQualifiers", "includeClassOrigin", "propertyList")))
        .def("invokeMethod", &OW_CIMClient::invokeMethod)
        .def("getQualifierType", &OW_CIMClient::getQualifierType)
        .def("setQualifierType", &OW_CIMClient::setQualifierType)
        .def("modifyClass", &OW_CIMClient::modifyClass)
        .def("createClass", &OW_CIMClient::createClass)
        .def("modifyInstance", &OW_CIMClient::modifyInstance, OW_CIMClient_modifyInstance_overloads(args("includeQualifiers", "propertyList")))
        .def("createInstance", &OW_CIMClient::createInstance)
        .def("getProperty", &OW_CIMClient::getProperty)
        .def("setProperty", &OW_CIMClient::setProperty)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
        .def("associatorNames", &OW_CIMClient::associatorNames, OW_CIMClient_associatorNames_overloads(args("assocClass", "resultClass", "role", "resultRole")))
        .def("associatorNamesE", &OW_CIMClient::associatorNamesE, OW_CIMClient_associatorNamesE_overloads(args("assocClass", "resultClass", "role", "resultRole")))
        .def("associators", &OW_CIMClient::associators, OW_CIMClient_associators_overloads(args("assocClass", "resultClass", "role", "resultRole", "includeQualifiers", "includeClassOrigin", "propertyList")))
        .def("associatorsE", &OW_CIMClient::associatorsE, OW_CIMClient_associatorsE_overloads(args("assocClass", "resultClass", "role", "resultRole", "includeQualifiers", "includeClassOrigin", "propertyList")))
        .def("associatorsClasses", &OW_CIMClient::associatorsClasses, OW_CIMClient_associatorsClasses_overloads(args("assocClass", "resultClass", "role", "resultRole", "includeQualifiers", "includeClassOrigin", "propertyList")))
        .def("associatorsClassesE", &OW_CIMClient::associatorsClassesE, OW_CIMClient_associatorsClassesE_overloads(args("assocClass", "resultClass", "role", "resultRole", "includeQualifiers", "includeClassOrigin", "propertyList")))
        .def("referenceNames", &OW_CIMClient::referenceNames, OW_CIMClient_referenceNames_overloads(args("resultClass", "role")))
        .def("referenceNamesE", &OW_CIMClient::referenceNamesE, OW_CIMClient_referenceNamesE_overloads(args("resultClass", "role")))
        .def("references", &OW_CIMClient::references, OW_CIMClient_references_overloads(args("resultClass", "role", "includeQualifiers", "includeClassOrigin", "propertyList")))
        .def("referencesE", &OW_CIMClient::referencesE, OW_CIMClient_referencesE_overloads(args("resultClass", "role", "includeQualifiers", "includeClassOrigin", "propertyList")))
        .def("referencesClasses", &OW_CIMClient::referencesClasses, OW_CIMClient_referencesClasses_overloads(args("resultClass", "role", "includeQualifiers", "includeClassOrigin", "propertyList")))
        .def("referencesClassesE", &OW_CIMClient::referencesClassesE, OW_CIMClient_referencesClassesE_overloads(args("resultClass", "role", "includeQualifiers", "includeClassOrigin", "propertyList")))
#endif
        .def("execQuery", &OW_CIMClient::execQuery)
        .def("execQueryE", &OW_CIMClient::execQueryE)

        ;

}

