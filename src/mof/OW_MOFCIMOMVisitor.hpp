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
 * @author Dan Nuffer
 */

#ifndef OW_CIMOMVISITOR_HPP_INCLUDE_GUARD_
#define OW_CIMOMVISITOR_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_Map.hpp"
#include "OW_MOFVisitor.hpp"
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
#include "OW_MOFParserErrorHandlerIFC.hpp"
#include "OW_Cache.hpp"
#include "OW_RepositoryCIMOMHandle.hpp"
#include "OW_MOFCompiler.hpp"

namespace OpenWBEM
{

namespace MOF
{

struct lineInfo;
class CIMOMVisitor : public Visitor
{
public:
	CIMOMVisitor(const CIMOMHandleIFCRef& hdl, const Compiler::Options& opts,
		const Reference<ParserErrorHandlerIFC>& _theErrorHandler);
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

	// state vars used while processing the tree
	CIMClass m_curClass;
	CIMInstance m_curInstance;
	CIMQualifier m_curQualifier;
	CIMQualifierType m_curQualifierType;
	CIMValue m_curValue;
	CIMProperty m_curProperty;
	CIMMethod m_curMethod;
	CIMParameter m_curParameter;

	// connection to the rest of the world
	CIMOMHandleIFCRef m_hdl;
	RepositoryCIMOMHandleRef m_rephdl;
	Reference<ParserErrorHandlerIFC> theErrorHandler;

	// instance aliases
	Map<String, String> m_aliasMap;

	// config
	Compiler::Options m_opts;
	
	// used for current #pragma values
	String m_namespace;
	String m_instanceLocale;
	String m_locale;
	String m_nonLocal;
	String m_nonLocalType;
	String m_source;
	String m_sourceType;
	
	Cache<CIMQualifierType> m_dataTypeCache;
	CIMDataType getQualifierDataType(const String& qualName, const lineInfo& li);
	CIMQualifierType getQualifierType(const String& qualName, const lineInfo& li);

	Cache<CIMClass> m_classCache;
	CIMClass getClass(const String& className, const lineInfo& li);
	
	CIMValue convertValuesIntoValueArray( const CIMValueArray& values );
	
	// Functions that call into the remote cimom handle
	CIMClass CIMOMgetClass(const String& className, const lineInfo& li);
	void CIMOMcreateClass(const lineInfo& li);
	void CIMOMsetQualifierType(const lineInfo& li);
	void CIMOMcreateInstance(const lineInfo& li);
	CIMQualifierType CIMOMgetQualifierType(const String& qualName, const lineInfo& li);
	void CIMOMcreateNamespace(const lineInfo& li);
	
};

} // end namespace MOF
} // end namespace OpenWBEM

#endif
