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

#include "OW_config.h"
#include "DebugVisitor.h"
#include "Grammar.h"
#include <iostream>


DebugVisitor::DebugVisitor(){
}
DebugVisitor::~DebugVisitor(){
}


void DebugVisitor::VisitMOFSpecification( const MOFSpecification *pMOFSpecification )
{
	cout << "/* mofSpecification */" << endl;
	for( list<MOFProduction *>::const_iterator i = pMOFSpecification->pMOFProduction->begin();
	     i != pMOFSpecification->pMOFProduction->end();
	     ++i )
	     (*i)->Accept( this );
}


void DebugVisitor::VisitMOFProductionCompilerDirective( const MOFProductionCompilerDirective *pMOFProductionCompilerDirective )
{
	cout << "/* mofProduction = compilerDirective */" << endl;

	pMOFProductionCompilerDirective->pCompilerDirective->Accept( this );
}


void DebugVisitor::VisitMOFProductionClassDeclaration( const MOFProductionClassDeclaration *pMOFProductionClassDeclaration )
{
	cout << "/* mofProduction = classDeclaration */" << endl;

	pMOFProductionClassDeclaration->pClassDeclaration->Accept( this );
}


void DebugVisitor::VisitMOFProductionAssocDeclaration( const MOFProductionAssocDeclaration *pMOFProductionAssocDeclaration )
{
	cout << "/* mofProduction = assocDeclaration */" << endl;

	pMOFProductionAssocDeclaration->pAssocDeclaration->Accept( this );
}


void DebugVisitor::VisitMOFProductionIndicDeclaration( const MOFProductionIndicDeclaration *pMOFProductionIndicDeclaration )
{
	cout << "/* mofProduction = indicDeclaration */" << endl;

	pMOFProductionIndicDeclaration->pIndicDeclaration->Accept( this );
}


void DebugVisitor::VisitMOFProductionQualifierDeclaration( const MOFProductionQualifierDeclaration *pMOFProductionQualifierDeclaration )
{
	cout << "/* mofProduction = qualifierDeclaration */" << endl;

	pMOFProductionQualifierDeclaration->pQualifierDeclaration->Accept( this );
}


void DebugVisitor::VisitMOFProductionInstanceDeclaration( const MOFProductionInstanceDeclaration *pMOFProductionInstanceDeclaration )
{
	cout << "/* mofProduction = instanceDeclaration */" << endl;

	pMOFProductionInstanceDeclaration->pInstanceDeclaration->Accept( this );
}


void DebugVisitor::VisitCompilerDirective( const CompilerDirective *pCompilerDirective )
{
	cout << "/* compilerDirective */" << endl;

	cout << "#pragma ";
	pCompilerDirective->pPragmaName->Accept( this );
	cout << "( ";
	pCompilerDirective->pPragmaParameter->Accept( this );
	cout << " )" << endl;
}


void DebugVisitor::VisitPragmaName( const PragmaName *pPragmaName )
{
	cout << "/* pragmaName */ ";
	
	cout << *pPragmaName->pPragmaName;
}


void DebugVisitor::VisitPragmaParameter( const PragmaParameter *pPragmaParameter )
{
	cout << "/* pragmaParameter */ ";

	cout << *pPragmaParameter->pPragmaParameter;
}


void DebugVisitor::VisitClassDeclaration( const ClassDeclaration *pClassDeclaration )
{
	cout << "/* classDeclaration */" << endl;

	if( pClassDeclaration->pQualifier.get() != 0 )
	{
		cout << "[";
		list<Qualifier *>::const_iterator i = pClassDeclaration->pQualifier->begin();
		(*i)->Accept( this );
		for( ++i; i != pClassDeclaration->pQualifier->end(); ++i )
		{
			cout << ",";
			(*i)->Accept( this );
		}
		cout << "]" << endl;
	}
	cout << "class ";
	pClassDeclaration->pClassName->Accept( this );
	if( pClassDeclaration->pAlias.get() != 0 )
	{
		cout << " ";
		pClassDeclaration->pAlias->Accept( this );
	}
	if( pClassDeclaration->pSuperClass.get() != 0 )
	{
		cout << " ";
		pClassDeclaration->pSuperClass->Accept( this );
	}
	cout << endl << "{" << endl;
	if( pClassDeclaration->pClassFeature.get() != 0 )
	{
		for( list<ClassFeature *>::const_iterator i = pClassDeclaration->pClassFeature->begin();
			i != pClassDeclaration->pClassFeature->end();
			++i )
		{
			cout << "\t";
			(*i)->Accept( this );
		}
	}
	cout << "};" << endl;
}


void DebugVisitor::VisitAssocDeclaration( const AssocDeclaration *pAssocDeclaration )
{
	cout << "/* assocDeclaration */" << endl;

	cout << "[association";
	if( pAssocDeclaration->pQualifier.get() != 0 )
	{
		for( list<Qualifier *>::const_iterator i = pAssocDeclaration->pQualifier->begin();
			i != pAssocDeclaration->pQualifier->end();
			++i )
		{
			cout << ",";	
			(*i)->Accept( this );
		}
	}
	cout << "]" << endl << "class ";
	pAssocDeclaration->pClassName->Accept( this );
	if( pAssocDeclaration->pAlias.get() != 0 )
	{
		cout << " ";
		pAssocDeclaration->pAlias->Accept( this );
	}
	if( pAssocDeclaration->pSuperClass.get() != 0 )
	{
		cout << " ";
		pAssocDeclaration->pSuperClass->Accept( this );
	}
	cout << endl << "{" << endl;
	if( pAssocDeclaration->pAssociationFeature.get() != 0 )
	{
		for( list<AssociationFeature *>::const_iterator i = pAssocDeclaration->pAssociationFeature->begin();
			i != pAssocDeclaration->pAssociationFeature->end();
			++i )
		{
			cout << "\t";
			(*i)->Accept( this );
		}
	}
	cout << "};" << endl;
}


void DebugVisitor::VisitIndicDeclaration( const IndicDeclaration *pIndicDeclaration )
{
	cout << "/* indicDeclaration */" << endl;

	cout << "[indication";
	if( pIndicDeclaration->pQualifier.get() != 0 )
	{	
		for( list<Qualifier *>::const_iterator i = pIndicDeclaration->pQualifier->begin();
			i != pIndicDeclaration->pQualifier->end();
			++i )
		{
			cout << ",";
			(*i)->Accept( this );
		}
	}
	cout << "]" << endl << "class ";
	pIndicDeclaration->pClassName->Accept( this );
	if( pIndicDeclaration->pAlias.get() != 0 )
	{
		cout << " ";
		pIndicDeclaration->pAlias->Accept( this );
	}
	if( pIndicDeclaration->pSuperClass.get() != 0 )
	{
		cout << " ";
		pIndicDeclaration->pSuperClass->Accept( this );
	}
	cout << endl << "{" << endl;
	if( pIndicDeclaration->pClassFeature.get() != 0 )
	{
		for( list<ClassFeature *>::const_iterator i = pIndicDeclaration->pClassFeature->begin();
			i != pIndicDeclaration->pClassFeature->end();
			++i )
		{
			cout << "\t";
			(*i)->Accept( this );
		}
	}
	cout << "};" << endl;
}


void DebugVisitor::VisitClassName( const ClassName *pClassName )
{
	cout << "/* className */ ";

	cout << *pClassName->pClassName;
}


void DebugVisitor::VisitAlias( const Alias *pAlias )
{
	cout << "/* alias */ ";

	cout << "as ";
	pAlias->pAliasIdentifier->Accept( this );
}


void DebugVisitor::VisitAliasIdentifier( const AliasIdentifier *pAliasIdentifier )
{
	cout << "/* aliasIdentifier */ ";

	cout << "$" << *pAliasIdentifier->pAliasIdentifier;
}


void DebugVisitor::VisitSuperClass( const SuperClass *pSuperClass )
{
	cout << "/* superClass */ ";

	cout << ":";
	pSuperClass->pClassName->Accept( this );
}


void DebugVisitor::VisitClassFeaturePropertyDeclaration( const ClassFeaturePropertyDeclaration *pClassFeaturePropertyDeclaration )
{
	cout << "/* classFeature = propertyDeclaration */" << endl;

	pClassFeaturePropertyDeclaration->pPropertyDeclaration->Accept( this );
}


void DebugVisitor::VisitClassFeatureMethodDeclaration( const ClassFeatureMethodDeclaration *pClassFeatureMethodDeclaration )
{
	cout << "/* classFeature = methodDeclaration */" << endl;

	pClassFeatureMethodDeclaration->pMethodDeclaration->Accept( this );
}


void DebugVisitor::VisitClassFeatureReferenceDeclaration( const ClassFeatureReferenceDeclaration *pClassFeatureReferenceDeclaration )
{
	cout << "/* classFeature = referenceDeclaration */" << endl;

	pClassFeatureReferenceDeclaration->pReferenceDeclaration->Accept( this );
}


void DebugVisitor::VisitAssociationFeatureClassFeature( const AssociationFeatureClassFeature *pAssociationFeatureClassFeature )
{
	cout << "/* associationFeature = classFeature */" << endl;

	pAssociationFeatureClassFeature->pClassFeature->Accept( this );
}


void DebugVisitor::VisitQualifier( const Qualifier *pQualifier )
{
	cout << "/* qualifier */ ";

	pQualifier->pQualifierName->Accept( this );
	if( pQualifier->pQualifierParameter.get() != 0 )
	{
		cout << " ";
		pQualifier->pQualifierParameter->Accept( this );
	}
	if( pQualifier->pFlavor.get() != 0)
	{
		cout << ":";
		list<Flavor *>::const_iterator i = pQualifier->pFlavor->begin();
		(*i)->Accept( this );
		for( ++i; i != pQualifier->pFlavor->end(); ++i )
		{
			cout << " ";
			(*i)->Accept( this );
		}
	}
}


void DebugVisitor::VisitQualifierParameterConstantValue( const QualifierParameterConstantValue *pQualifierParameterConstantValue )
{
	cout << "/* qualifierParameter = constantValue */ ";

	cout << "( " << *pQualifierParameterConstantValue->pConstantValue << " )";
}


void DebugVisitor::VisitQualifierParameterArrayInitializer( const QualifierParameterArrayInitializer *pQualifierParameterArrayInitializer )
{
	cout << "/* qualifierParameter = arrayInitializer */ ";

	pQualifierParameterArrayInitializer->pArrayInitializer->Accept( this );
}


void DebugVisitor::VisitFlavor( const Flavor *pFlavor )
{
	cout << "/* flavor */ ";

	cout << *pFlavor->pFlavor;
}


void DebugVisitor::VisitPropertyDeclaration( const PropertyDeclaration *pPropertyDeclaration )
{
	cout << "/* propertyDeclaration */" << endl;

	if( pPropertyDeclaration->pQualifier.get() != 0 )
	{
		cout << "[";
		list<Qualifier *>::const_iterator i = pPropertyDeclaration->pQualifier->begin();
		(*i)->Accept( this );
		for( ++i; i != pPropertyDeclaration->pQualifier->end(); ++i )
		{
			cout << ",";
			(*i)->Accept( this );
		}
		cout << "] ";
	}
	pPropertyDeclaration->pDataType->Accept( this );
	cout << " ";
	pPropertyDeclaration->pPropertyName->Accept( this );
	if( pPropertyDeclaration->pArray.get() != 0 )
	{
		cout << " ";
		pPropertyDeclaration->pArray->Accept( this );
	}
	if( pPropertyDeclaration->pDefaultValue.get() != 0 )
	{
		cout << " ";
		pPropertyDeclaration->pDefaultValue->Accept( this );
	}
	cout << ";" << endl;
}


void DebugVisitor::VisitReferenceDeclaration( const ReferenceDeclaration *pReferenceDeclaration )
{
	cout << "/* referenceDeclaration */" << endl;
	
	if( pReferenceDeclaration->pQualifier.get() != 0 )
	{
		cout << "[";
		list<Qualifier *>::const_iterator i = pReferenceDeclaration->pQualifier->begin();
		(*i)->Accept( this );
		for( ++i; i != pReferenceDeclaration->pQualifier->end(); ++i )
		{
			cout << ",";
			(*i)->Accept( this );
		}
		cout << "] ";
	}
	pReferenceDeclaration->pObjectRef->Accept( this );
	cout << " ";
	pReferenceDeclaration->pReferenceName->Accept( this );
	if( pReferenceDeclaration->pDefaultValue.get() != 0 )
	{
		cout << " ";
		pReferenceDeclaration->pDefaultValue->Accept( this );
	}
	cout << ";" << endl;
}


void DebugVisitor::VisitMethodDeclaration( const MethodDeclaration *pMethodDeclaration )
{
	cout << "/* methodDeclaration */" << endl;

	if( pMethodDeclaration->pQualifier.get() != 0 )
	{
		cout << "[";
		list<Qualifier *>::const_iterator i = pMethodDeclaration->pQualifier->begin();
		(*i)->Accept( this );
		for( ++i; i != pMethodDeclaration->pQualifier->end(); ++i )
		{
			cout << ",";
			(*i)->Accept( this );
		}
		cout << "] ";
	}
	pMethodDeclaration->pDataType->Accept( this );
	cout << " ";
	pMethodDeclaration->pMethodName->Accept( this );
	cout << "( ";
	if( pMethodDeclaration->pParameter.get() != 0 )
	{
		list<Parameter *>::const_iterator j = pMethodDeclaration->pParameter->begin();
		(*j)->Accept( this );
		for( ++j; j != pMethodDeclaration->pParameter->end(); ++j )
		{
			cout << ",";
			(*j)->Accept( this );
		}
	}
	cout << " );" << endl;
}


void DebugVisitor::VisitPropertyName( const PropertyName *pPropertyName )
{
	cout << "/* propertyName */ ";

	cout << *pPropertyName->pPropertyName;
}


void DebugVisitor::VisitReferenceName( const ReferenceName *pReferenceName )
{
	cout << "/* referenceName */ ";

	cout << *pReferenceName->pReferenceName;
}


void DebugVisitor::VisitMethodName( const MethodName *pMethodName )
{
	cout << "/* methodName */ ";
	
	cout << *pMethodName->pMethodName;
}


void DebugVisitor::VisitDataType( const DataType *pDataType )
{
	cout << "/* dataType */ ";

	cout << *pDataType->pDataType;
}


void DebugVisitor::VisitObjectRef( const ObjectRef *pObjectRef )
{
	cout << "/* objectRef */ ";

	pObjectRef->pClassName->Accept( this );
	cout << " ref";
}


void DebugVisitor::VisitParameterDataType( const ParameterDataType *pParameterDataType )
{
	cout << "/* parameter = dataType */ ";

	if( pParameterDataType->pQualifier.get() != 0 )
	{
		cout << "[";
		list<Qualifier *>::const_iterator i = pParameterDataType->pQualifier->begin();
		(*i)->Accept( this );
		for( ++i; i != pParameterDataType->pQualifier->end(); ++i )
		{
			cout << ",";
			(*i)->Accept( this );
		}
		cout << "] ";
	}
	pParameterDataType->pDataType->Accept( this );
	cout << " ";
	pParameterDataType->pParameterName->Accept( this );
	if( pParameterDataType->pArray.get() != 0 )
	{
		cout << " ";
		pParameterDataType->pArray->Accept( this );
	}
}


void DebugVisitor::VisitParameterObjectRef( const ParameterObjectRef *pParameterObjectRef )
{
	cout << "/* parameter = objectRef */ ";

	if( pParameterObjectRef->pQualifier.get() != 0 )
	{
		cout << "[";
		list<Qualifier *>::const_iterator i = pParameterObjectRef->pQualifier->begin();
		(*i)->Accept( this );
		for( ++i; i != pParameterObjectRef->pQualifier->end(); ++i )
		{
			cout << ",";
			(*i)->Accept( this );
		}
		cout << "] ";
	}
	pParameterObjectRef->pObjectRef->Accept( this );
	cout << " ";
	pParameterObjectRef->pParameterName->Accept( this );
	if( pParameterObjectRef->pArray.get() != 0 )
	{
		cout << " ";
		pParameterObjectRef->pArray->Accept( this );
	}
}


void DebugVisitor::VisitParameterName( const ParameterName *pParameterName )
{
	cout << "/* parameterName */ ";

	cout << *pParameterName->pParameterName;
}


void DebugVisitor::VisitArray( const Array *pArray )
{
	cout << "/* array */ ";

	cout << "[";
	if( pArray != 0 )
		cout << *pArray->pArray;
	cout << "]";
}


void DebugVisitor::VisitDefaultValue( const DefaultValue *pDefaultValue )
{
	cout << "/* defaultValue */ ";
	
	cout << "=";
	pDefaultValue->pInitializer->Accept( this );
}


void DebugVisitor::VisitInitializerReferenceInitializer( const InitializerReferenceInitializer *pInitializerReferenceInitializer )
{
	cout << "/* initializer = referenceInitializer */ ";

	pInitializerReferenceInitializer->pReferenceInitializer->Accept( this );
}


void DebugVisitor::VisitInitializerArrayInitializer( const InitializerArrayInitializer *pInitializerArrayInitializer )
{
	cout << "/* initializer = arrayInitializer */ ";

	pInitializerArrayInitializer->pArrayInitializer->Accept( this );
}


void DebugVisitor::VisitInitializerConstantValue( const InitializerConstantValue *pInitializerConstantValue )
{
	cout << "/* initializer = constantValue */ ";
	
	cout << *pInitializerConstantValue->pConstantValue;
}


void DebugVisitor::VisitArrayInitializer( const ArrayInitializer *pArrayInitializer )
{
	cout << "/* arrayInitializer */ ";

	cout << "{";
	if( pArrayInitializer->pConstantValue.get() != 0 )
	{
		list<OW_String *>::const_iterator i = pArrayInitializer->pConstantValue->begin();
		cout << **i;
		for( ++i; i != pArrayInitializer->pConstantValue->end(); ++i )
			cout << "," << **i;
	}
	cout << "}";
}


void DebugVisitor::VisitReferenceInitializerObjectHandle( const ReferenceInitializerObjectHandle *pReferenceInitializerObjectHandle )
{
	cout << "/* referenceInitializer = objectHandle */ ";

	pReferenceInitializerObjectHandle->pObjectHandle->Accept( this );
}


void DebugVisitor::VisitReferenceInitializerAliasIdentifier( const ReferenceInitializerAliasIdentifier *pReferenceInitializerAliasIdentifier )
{
	cout << "/* referenceInitializer = aliasIdentifier */ ";

	pReferenceInitializerAliasIdentifier->pAliasIdentifier->Accept( this );
}


void DebugVisitor::VisitObjectHandle( const ObjectHandle *pObjectHandle )
{
	cout << "/* objectHandle */ ";
	
	cout << *pObjectHandle->pObjectHandle;
}


void DebugVisitor::VisitQualifierDeclaration( const QualifierDeclaration *pQualifierDeclaration )
{
	cout << "/* qualifierDeclaration */ ";

	cout << "qualifier ";
	pQualifierDeclaration->pQualifierName->Accept( this );
	cout << " ";
	pQualifierDeclaration->pQualifierType->Accept( this );
	cout << " ";
	pQualifierDeclaration->pScope->Accept( this );
	if( pQualifierDeclaration->pDefaultFlavor.get() != 0 )
	{	
		cout << " ";
		pQualifierDeclaration->pDefaultFlavor->Accept( this );
	}
}


void DebugVisitor::VisitQualifierName( const QualifierName *pQualifierName )
{
	cout << "/* qualifierName */ ";

	cout << *pQualifierName->pQualifierName;
}


void DebugVisitor::VisitQualifierType( const QualifierType *pQualifierType )
{
	cout << "/* qualifierType */ " << endl;

	cout << ":";
	pQualifierType->pDataType->Accept( this );
	if( pQualifierType->pArray.get() != 0 )
	{
		cout << " ";
		pQualifierType->pArray->Accept( this );
	}
	if( pQualifierType->pDefaultValue.get() != 0 )
	{
		cout << " ";
		pQualifierType->pDefaultValue->Accept( this );
	}
}


void DebugVisitor::VisitScope( const Scope *pScope )
{
	cout << "/* scope */ ";
	
	cout << ",scope(";
	for( list<MetaElement *>::const_iterator i = pScope->pMetaElement->begin();
		i != pScope->pMetaElement->end();
		++i )
	{
		cout << ",";
		(*i)->Accept( this );
	}
	cout << ")";
}


void DebugVisitor::VisitMetaElement( const MetaElement *pMetaElement )
{
	cout << "/* metaElement */ ";

	cout << *pMetaElement->pMetaElement;
}


void DebugVisitor::VisitDefaultFlavor( const DefaultFlavor *pDefaultFlavor )
{
	cout << "/* defaultFlavor */ " << endl;

	cout << ",flavor(";
	for( list<Flavor *>::const_iterator i = pDefaultFlavor->pFlavor->begin();
		i != pDefaultFlavor->pFlavor->end();
		++i )
	{
		cout << ",";
		(*i)->Accept( this );
	}
	cout << ")";
}


void DebugVisitor::VisitInstanceDeclaration( const InstanceDeclaration *pInstanceDeclaration )
{
	cout << "/* instanceDeclaration */" << endl;

	if( pInstanceDeclaration->pQualifier.get() != 0 )
	{
		cout << "[";
		list<Qualifier *>::const_iterator i = pInstanceDeclaration->pQualifier->begin();
		(*i)->Accept( this );
		for( ++i; i != pInstanceDeclaration->pQualifier->end(); ++i )
		{
			cout << ",";
			(*i)->Accept( this );
		}
		cout << "] ";
	}
	cout << "instance of ";
	pInstanceDeclaration->pClassName->Accept( this );
	if( pInstanceDeclaration->pAlias.get() != 0 )
	{
		cout << " ";
		pInstanceDeclaration->pAlias->Accept( this );
	}
	cout << "{";
	if( pInstanceDeclaration->pValueInitializer.get() != 0 )
	{
		list<ValueInitializer *>::const_iterator i = pInstanceDeclaration->pValueInitializer->begin();
		(*i)->Accept( this );
		for( ++i; i != pInstanceDeclaration->pValueInitializer->end(); ++i )
		{
			cout << ",";
			(*i)->Accept( this );
		}
	}
	cout << "};";
}


void DebugVisitor::VisitValueInitializer( const ValueInitializer *pValueInitializer )
{
	cout << "/* valueInitializer */ ";
	
	if( pValueInitializer->pQualifier.get() != 0 )
	{
		cout << "[";
		list<Qualifier *>::const_iterator i = pValueInitializer->pQualifier->begin();
		(*i)->Accept( this );
		for( ++i; i != pValueInitializer->pQualifier->end(); ++i )
		{
			cout << ",";
			(*i)->Accept( this );
		}
		cout << "]" << endl;
	}
	cout << *pValueInitializer->pValueInitializer;
	pValueInitializer->pDefaultValue->Accept( this );
	cout << ";";
}

// vim: ts=4
