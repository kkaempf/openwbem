/*******************************************************************************
* Copyright (C) 2004 Novell, Inc All rights reserved.
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
*  - Neither the name of Novell nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_CPPSIMPLEASSOCIATORPROVIDER_INCLUDE_GUARD_
#define OW_CPPSIMPLEASSOCIATORPROVIDER_INCLUDE_GUARD_

#include "OW_CppAssociatorProviderIFC.hpp"


namespace OpenWBEM
{
/**
 * class CppSimpleAssociatorProviderIFC
 * The intent of this class is to simplify the writing of Associator 
 * Providers.  Instead of implementing four functions (referneceNames, 
 * references, associators, associatorNames), you just have to implement
 * one: doReferences().  doReferences() is a modified version of references. 
 * This base class does the work of providing the CIMOM with the four 
 * original functions based on what is returned from doReferences. 
 */ 
    class CppSimpleAssociatorProviderIFC : public CppAssociatorProviderIFC
    {
    protected:

/**
 * This method is invoked in order to do the Associators, AssociatorNames, 
 * References, and ReferenceNames operation as
 * defined by the CIM operations over HTTP spec at
 * http://www.dmtf.org/download/spec/xmls/CIM_HTTP_Mapping10.htm#SecAssociators
 * This operation is used to enumerate CIM Instances of a particular 
 * Association class
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
 * 	returned set of Objects by mandating that the resultRole of each 
 *      returned Object MUST be an ObjectPath to either an Instance of this 
 *      Class (or one of its subclasses) or be this Class (or one of its 
 *      subclasses).
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
 * 	set of Objects by mandating that each returned Object MUST contain
 *      a reference (ObjectPath) to an Instance or Class which is 
 * 	associated to the source Object via an Association in which the
 * 	Associated Object plays the specified role (i.e. the name of the
 * 	Property in the Association Class that refers to the Associated
 * 	Object MUST match the value of this parameter).
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
 * 
 * 
 * 
 *  +------------------+                               +-----------------+
 *  | ObjectNameClass  |                               |  ResultClass    |
 *  | ~~~~~~~~~~~~~~~  |                               |  ~~~~~~~~~~~    |
 *  +------------------+                               +-----------------+
 *     |                +----------------------------+                 |
 *     |                |  [Association] AssocClass  |                 |
 *     |   ObjectName   |  ~~~~~~~~~~~~~~~~~~~~~~~~  |                 |
 *     +----------------+ ObjectNameClass REF Role   |                 |
 *      (CIMObjectPath) | ResultClass REF ResultRole +-----------------+
 *                      |                            |  (CIMObjectPath)
 *                      +----------------------------+
 */
        virtual void doReferences(const ProviderEnvironmentIFCRef &env, 
                                  CIMInstanceResultHandlerIFC &result, 
                                  const String &ns, 
                                  const CIMObjectPath &objectName, 
                                  const CIMClass &assocClass, 
                                  const String &resultClass, 
                                  const String &role, 
                                  const String &resultRole) = 0; 

    public:
        virtual void associators(const ProviderEnvironmentIFCRef &env, 
                                 CIMInstanceResultHandlerIFC &result, 
                                 const String &ns, 
                                 const CIMObjectPath &objectName, 
                                 const String &assocClass, 
                                 const String &resultClass, 
                                 const String &role, 
                                 const String &resultRole, 
                                 WBEMFlags:: EIncludeQualifiersFlag includeQualifiers, 
                                 WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin, 
                                 const StringArray *propertyList); 
        virtual void associatorNames(const ProviderEnvironmentIFCRef &env, 
                                     CIMObjectPathResultHandlerIFC &result, 
                                     const String &ns, 
                                     const CIMObjectPath &objectName, 
                                     const String &assocClass, 
                                     const String &resultClass, 
                                     const String &role, 
                                     const String &resultRole); 
        virtual void references(const ProviderEnvironmentIFCRef 
                                &env, CIMInstanceResultHandlerIFC &result, 
                                const String &ns, 
                                const CIMObjectPath &objectName, 
                                const String &resultClass, 
                                const String &role, 
                                WBEMFlags:: EIncludeQualifiersFlag includeQualifiers, 
                                WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin, 
                                const StringArray *propertyList); 
        virtual void referenceNames(const ProviderEnvironmentIFCRef &env, 
                                    CIMObjectPathResultHandlerIFC &result, 
                                    const String &ns, 
                                    const CIMObjectPath &objectName, 
                                    const String &resultClass, 
                                    const String &role); 
    };
} // end namespace OpenWBEM. 

#endif // #ifndef OW_CPPSIMPLEASSOCIATORPROVIDER_INCLUDE_GUARD_
