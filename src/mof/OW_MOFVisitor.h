/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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

#ifndef VISITOR_H
#define VISITOR_H

#include "OW_config.h"

// forward declarations
class MOFSpecification;

class MOFProductionCompilerDirective;
class MOFProductionClassDeclaration;
class MOFProductionAssocDeclaration;
class MOFProductionIndicDeclaration;
class MOFProductionQualifierDeclaration;
class MOFProductionInstanceDeclaration;

class CompilerDirective;
class PragmaName;
class PragmaParameter;
class ClassDeclaration;
class AssocDeclaration;
class IndicDeclaration;
class Alias;
class AliasIdentifier;
class SuperClass;
class ClassName;
	
class ClassFeaturePropertyDeclaration;	
class ClassFeatureMethodDeclaration;	
class ClassFeatureReferenceDeclaration;	
	
class AssociationFeatureClassFeature;	

class PropertyDeclaration;
class ReferenceDeclaration;
class MethodDeclaration;
class Qualifier;
	
class QualifierParameterConstantValue;
class QualifierParameterArrayInitializer;

class Flavor;
class PropertyName;
class ReferenceName;
class MethodName;
class DataType;
class ObjectRef;
class ParameterDataType;
class ParameterObjectRef;
class ParameterName;
class Array;
class DefaultValue;

class IntegerValueBinaryValue;
class IntegerValueOctalValue;
class IntegerValueDecimalValue;
class IntegerValueHexValue;

class FloatValue;
class CharValue;
class StringValue;
class BooleanValue;
class NullValue;

class ConstantValueIntegerValue;
class ConstantValueFloatValue;
class ConstantValueStringValue;
class ConstantValueCharValue;
class ConstantValueBooleanValue;
class ConstantValueNullValue;

class InitializerReferenceInitializer;
class InitializerArrayInitializer;
class InitializerConstantValue;

class ArrayInitializer;

class ReferenceInitializerAliasIdentifier;
class ReferenceInitializerObjectHandle;
class ObjectHandle;

class QualifierDeclaration;
class QualifierName;
class QualifierType;
class Scope;
class MetaElement;
class DefaultFlavor;
class InstanceDeclaration;
class ValueInitializer;
//class Identifier;


class Visitor {
public:
	Visitor();
	virtual ~Visitor();
	
	virtual void VisitMOFSpecification( const MOFSpecification * ) = 0;
	
	virtual void VisitMOFProductionCompilerDirective( const MOFProductionCompilerDirective * ) = 0;
	virtual void VisitMOFProductionClassDeclaration( const MOFProductionClassDeclaration * ) = 0;
	virtual void VisitMOFProductionAssocDeclaration( const MOFProductionAssocDeclaration * ) = 0;
	virtual void VisitMOFProductionIndicDeclaration( const MOFProductionIndicDeclaration * ) = 0;
	virtual void VisitMOFProductionQualifierDeclaration( const MOFProductionQualifierDeclaration * ) = 0;
	virtual void VisitMOFProductionInstanceDeclaration( const MOFProductionInstanceDeclaration * ) = 0;
	
	virtual void VisitCompilerDirective( const CompilerDirective * ) = 0;
	virtual void VisitPragmaName( const PragmaName * ) = 0;
	virtual void VisitPragmaParameter( const PragmaParameter * ) = 0;
	virtual void VisitClassDeclaration( const ClassDeclaration * ) = 0;
	virtual void VisitAssocDeclaration( const AssocDeclaration * ) = 0;
	virtual void VisitIndicDeclaration( const IndicDeclaration * ) = 0;
	virtual void VisitAlias( const Alias * ) = 0;
	virtual void VisitAliasIdentifier( const AliasIdentifier * ) = 0;
	virtual void VisitSuperClass( const SuperClass * ) = 0;
	virtual void VisitClassName( const ClassName * ) = 0;
	
	virtual void VisitClassFeaturePropertyDeclaration( const ClassFeaturePropertyDeclaration * ) = 0;	
	virtual void VisitClassFeatureMethodDeclaration( const ClassFeatureMethodDeclaration * ) = 0;	
	virtual void VisitClassFeatureReferenceDeclaration( const ClassFeatureReferenceDeclaration * ) = 0;	
	
	virtual void VisitAssociationFeatureClassFeature( const AssociationFeatureClassFeature * ) = 0;	
	
	virtual void VisitPropertyDeclaration( const PropertyDeclaration * ) = 0;
	virtual void VisitReferenceDeclaration( const ReferenceDeclaration * ) = 0;
	virtual void VisitMethodDeclaration( const MethodDeclaration * ) = 0;
	virtual void VisitQualifier( const Qualifier * ) = 0;
	
	virtual void VisitQualifierParameterConstantValue( const QualifierParameterConstantValue * ) = 0;
	virtual void VisitQualifierParameterArrayInitializer( const QualifierParameterArrayInitializer * ) = 0;

	virtual void VisitFlavor( const Flavor * ) = 0;
	virtual void VisitPropertyName( const PropertyName * ) = 0;
	virtual void VisitReferenceName( const ReferenceName * ) = 0;
	virtual void VisitMethodName( const MethodName * ) = 0;
	virtual void VisitDataType( const DataType * ) = 0;
	virtual void VisitObjectRef( const ObjectRef * ) = 0;

	virtual void VisitIntegerValueBinaryValue( const IntegerValueBinaryValue * ) = 0;
	virtual void VisitIntegerValueOctalValue( const IntegerValueOctalValue * ) = 0;
	virtual void VisitIntegerValueDecimalValue( const IntegerValueDecimalValue * ) = 0;
	virtual void VisitIntegerValueHexValue( const IntegerValueHexValue * ) = 0;

	virtual void VisitConstantValueIntegerValue( const ConstantValueIntegerValue * ) = 0;
	virtual void VisitConstantValueFloatValue( const ConstantValueFloatValue * ) = 0;
	virtual void VisitConstantValueStringValue( const ConstantValueStringValue * ) = 0;
	virtual void VisitConstantValueCharValue( const ConstantValueCharValue * ) = 0;
	virtual void VisitConstantValueBooleanValue( const ConstantValueBooleanValue * ) = 0;
	virtual void VisitConstantValueNullValue( const ConstantValueNullValue * ) = 0;
	
	virtual void VisitParameterDataType( const ParameterDataType * ) = 0;
	virtual void VisitParameterObjectRef( const ParameterObjectRef * ) = 0;
	
	virtual void VisitParameterName( const ParameterName * ) = 0;
	virtual void VisitArray( const Array * ) = 0;
	virtual void VisitDefaultValue( const DefaultValue * ) = 0;
	
	virtual void VisitInitializerReferenceInitializer( const InitializerReferenceInitializer * ) = 0;
	virtual void VisitInitializerArrayInitializer( const InitializerArrayInitializer * ) = 0;
	virtual void VisitInitializerConstantValue( const InitializerConstantValue * ) = 0;

	virtual void VisitArrayInitializer( const ArrayInitializer * ) = 0;
	
	virtual void VisitReferenceInitializerAliasIdentifier( const ReferenceInitializerAliasIdentifier * ) = 0;
	virtual void VisitReferenceInitializerObjectHandle( const ReferenceInitializerObjectHandle * ) = 0;
    virtual void VisitObjectHandle( const ObjectHandle * ) = 0;
	
	virtual void VisitQualifierDeclaration( const QualifierDeclaration * ) = 0;
	virtual void VisitQualifierName( const QualifierName * ) = 0;
	virtual void VisitQualifierType( const QualifierType * ) = 0;
	virtual void VisitScope( const Scope * ) = 0;
	virtual void VisitMetaElement( const MetaElement * ) = 0;
	virtual void VisitDefaultFlavor( const DefaultFlavor * ) = 0;
	virtual void VisitInstanceDeclaration( const InstanceDeclaration * ) = 0;
	virtual void VisitValueInitializer( const ValueInitializer * ) = 0;
};


#endif
