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

#ifndef CIMOMVISITOR_HPP_
#define CIMOMVISITOR_HPP_

#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_Map.hpp"
#include "Visitor.h"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_MofParserErrorHandlerIFC.hpp"

struct lineInfo;

class CIMOMVisitor : public Visitor
{
public:
	CIMOMVisitor( OW_CIMOMHandleIFCRef hdl, OW_String& ns,
		OW_Reference<OW_MofParserErrorHandlerIFC> _theErrorHandler);
	~CIMOMVisitor();

	void VisitMOFSpecification( const MOFSpecification * );
	
	void VisitMOFProductionCompilerDirective( const MOFProductionCompilerDirective * );
	void VisitMOFProductionClassDeclaration( const MOFProductionClassDeclaration * );
	void VisitMOFProductionAssocDeclaration( const MOFProductionAssocDeclaration * );
	void VisitMOFProductionIndicDeclaration( const MOFProductionIndicDeclaration * );
	void VisitMOFProductionQualifierDeclaration( const MOFProductionQualifierDeclaration * );
	void VisitMOFProductionInstanceDeclaration( const MOFProductionInstanceDeclaration * );
	
	void VisitCompilerDirective( const CompilerDirective * );
	void VisitPragmaName( const PragmaName * );
	void VisitPragmaParameter( const PragmaParameter * );
	void VisitClassDeclaration( const ClassDeclaration * );
	void VisitAssocDeclaration( const AssocDeclaration * );
	void VisitIndicDeclaration( const IndicDeclaration * );
	void VisitAlias( const Alias * );
	void VisitAliasIdentifier( const AliasIdentifier * );
	void VisitSuperClass( const SuperClass * );
	void VisitClassName( const ClassName * );
	
	void VisitClassFeaturePropertyDeclaration( const ClassFeaturePropertyDeclaration * );	
	void VisitClassFeatureMethodDeclaration( const ClassFeatureMethodDeclaration * );	
	void VisitClassFeatureReferenceDeclaration( const ClassFeatureReferenceDeclaration * );	
	
	void VisitAssociationFeatureClassFeature( const AssociationFeatureClassFeature * );	
	
	void VisitPropertyDeclaration( const PropertyDeclaration * );
	void VisitReferenceDeclaration( const ReferenceDeclaration * );
	void VisitMethodDeclaration( const MethodDeclaration * );
	void VisitQualifier( const Qualifier * );
	
	void VisitQualifierParameterConstantValue( const QualifierParameterConstantValue * );
	void VisitQualifierParameterArrayInitializer( const QualifierParameterArrayInitializer * );

	void VisitFlavor( const Flavor * );
	void VisitPropertyName( const PropertyName * );
	void VisitReferenceName( const ReferenceName * );
	void VisitMethodName( const MethodName * );
	void VisitDataType( const DataType * );
	void VisitObjectRef( const ObjectRef * );

	void VisitIntegerValueBinaryValue( const IntegerValueBinaryValue * );
	void VisitIntegerValueOctalValue( const IntegerValueOctalValue * );
	void VisitIntegerValueDecimalValue( const IntegerValueDecimalValue * );
	void VisitIntegerValueHexValue( const IntegerValueHexValue * );

	void VisitConstantValueIntegerValue( const ConstantValueIntegerValue * );
	void VisitConstantValueFloatValue( const ConstantValueFloatValue * );
	void VisitConstantValueStringValue( const ConstantValueStringValue * );
	void VisitConstantValueCharValue( const ConstantValueCharValue * );
	void VisitConstantValueBooleanValue( const ConstantValueBooleanValue * );
	void VisitConstantValueNullValue( const ConstantValueNullValue * );
	
	void VisitParameterDataType( const ParameterDataType * );
	void VisitParameterObjectRef( const ParameterObjectRef * );
	
	void VisitParameterName( const ParameterName * );
	void VisitArray( const Array * );
	void VisitDefaultValue( const DefaultValue * );
	
	void VisitInitializerReferenceInitializer( const InitializerReferenceInitializer * );
	void VisitInitializerArrayInitializer( const InitializerArrayInitializer * );
	void VisitInitializerConstantValue( const InitializerConstantValue * );

	void VisitArrayInitializer( const ArrayInitializer * );
	
	void VisitReferenceInitializerAliasIdentifier( const ReferenceInitializerAliasIdentifier * );
	void VisitReferenceInitializerObjectHandle( const ReferenceInitializerObjectHandle * );
	void VisitObjectHandle( const ObjectHandle * );

	void VisitQualifierDeclaration( const QualifierDeclaration * );
	void VisitQualifierName( const QualifierName * );
	void VisitQualifierType( const QualifierType * );
	void VisitScope( const Scope * );
	void VisitMetaElement( const MetaElement * );
	void VisitDefaultFlavor( const DefaultFlavor * );
	void VisitInstanceDeclaration( const InstanceDeclaration * );
	void VisitValueInitializer( const ValueInitializer * );

private:
	OW_CIMClass m_curClass;
	OW_CIMInstance m_curInstance;
	OW_CIMQualifier m_curQualifier;
	OW_CIMQualifierType m_curQualifierType;
	OW_CIMValue m_curValue;
	OW_CIMProperty m_curProperty;
	OW_CIMMethod m_curMethod;
	OW_CIMParameter m_curParameter;
	OW_Reference<OW_CIMOMHandleIFC> m_hdl;
	OW_Map<OW_String, OW_String> m_aliasMap;
	OW_String m_namespace;
	
	OW_String m_instanceLocale;
	OW_String m_locale;
	
	OW_String m_nonLocal;
	OW_String m_nonLocalType;
	OW_String m_source;
	OW_String m_sourceType;
	
	OW_Map<OW_String, OW_CIMQualifierType> m_dataTypeCache;
	OW_CIMDataType getQualifierDataType(const OW_String& qualName, const lineInfo& li);
	OW_CIMQualifierType getQualifierType(const OW_String& qualName, const lineInfo& li);
	
	OW_CIMValue convertValuesIntoValueArray( const OW_CIMValueArray& values );
	
	// Functions that call into the remote cimom handle
	void CIMOMcreateClass(const lineInfo& li);
	void CIMOMsetQualifierType(const lineInfo& li);
	void CIMOMcreateInstance(const lineInfo& li);
	OW_CIMQualifierType CIMOMgetQualifierType(const OW_String& qualName, const lineInfo& li);

	OW_Reference<OW_MofParserErrorHandlerIFC> theErrorHandler;
};

#endif
