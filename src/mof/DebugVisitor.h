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

/***************************************************************************
                          DebugVisitor.h  -  description
                             -------------------
    begin                : Thu Feb 1 2001
    copyright            : (C) 2001 by Shane Smit
    email                : ssmit@caldera.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OW_DEBUGVISITOR_H
#define OW_DEBUGVISITOR_H

#include "OW_config.h"
#include "Visitor.h"

/**
  *@author Shane Smit
  */

class DebugVisitor : public Visitor  {
public: 
	DebugVisitor();
	~DebugVisitor();

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
};

#endif
