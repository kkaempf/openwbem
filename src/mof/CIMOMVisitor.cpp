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
#include "Grammar.h"
#include "CIMOMVisitor.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_Format.hpp"
#include "MofCompiler.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMScope.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"

#include <assert.h>

CIMOMVisitor::CIMOMVisitor(OW_Reference<OW_CIMOMHandleIFC> handle, OW_String& ns)
: m_curClass(OW_Bool(true))
, m_curInstance(OW_Bool(true))
, m_curValue()
, m_curProperty(OW_Bool(true))
, m_curMethod(OW_Bool(true))
, m_hdl(handle)
, m_namespace(ns)
{
}

CIMOMVisitor::~CIMOMVisitor()
{
}


void CIMOMVisitor::VisitMOFSpecification( const MOFSpecification *pMOFSpecification )
{
	for ( OW_List<MOFProduction *>::const_iterator i = pMOFSpecification->pMOFProduction->begin();
		 i != pMOFSpecification->pMOFProduction->end();
		 ++i )
		(*i)->Accept( this );
}


void CIMOMVisitor::VisitMOFProductionCompilerDirective( const MOFProductionCompilerDirective *pMOFProductionCompilerDirective )
{
	pMOFProductionCompilerDirective->pCompilerDirective->Accept( this );
}


void CIMOMVisitor::VisitMOFProductionClassDeclaration( const MOFProductionClassDeclaration *pMOFProductionClassDeclaration )
{
	pMOFProductionClassDeclaration->pClassDeclaration->Accept( this );
}


void CIMOMVisitor::VisitMOFProductionAssocDeclaration( const MOFProductionAssocDeclaration *pMOFProductionAssocDeclaration )
{
	pMOFProductionAssocDeclaration->pAssocDeclaration->Accept( this );
}


void CIMOMVisitor::VisitMOFProductionIndicDeclaration( const MOFProductionIndicDeclaration *pMOFProductionIndicDeclaration )
{
	pMOFProductionIndicDeclaration->pIndicDeclaration->Accept( this );
}


void CIMOMVisitor::VisitMOFProductionQualifierDeclaration( const MOFProductionQualifierDeclaration *pMOFProductionQualifierDeclaration )
{
	pMOFProductionQualifierDeclaration->pQualifierDeclaration->Accept( this );
}


void CIMOMVisitor::VisitMOFProductionInstanceDeclaration( const MOFProductionInstanceDeclaration *pMOFProductionInstanceDeclaration )
{
	pMOFProductionInstanceDeclaration->pInstanceDeclaration->Accept( this );
}


void CIMOMVisitor::VisitCompilerDirective( const CompilerDirective *pCompilerDirective )
{
	//pCompilerDirective->pPragmaName->Accept( this );
	//pCompilerDirective->pPragmaParameter->Accept( this );
	if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("include"))
	{
		// this should be taken care of in the parser, so just ignore it.
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("instancelocale"))
	{
		m_instanceLocale = MofCompiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("locale"))
	{
		m_locale = MofCompiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("namespace"))
	{
		m_namespace = MofCompiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("nonlocal"))
	{
		if (!m_nonLocalType.empty())
		{
			// report an error, both nonlocal and nonlocaltype cannot be set.
			MofCompiler::theErrorHandler->recoverableError("nonlocal and nonlocaltype pragmas can't both be set, pragma nonlocal ignored",
				pCompilerDirective->theLineInfo);
		}
		m_nonLocal = MofCompiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("nonlocaltype"))
	{
		if (!m_nonLocal.empty())
		{
			// report an error, both nonlocal and nonlocaltype cannot be set.
			MofCompiler::theErrorHandler->recoverableError("nonlocal and nonlocaltype pragmas can't both be set, pragma nonlocaltype ignored",
				pCompilerDirective->theLineInfo);
		}
		m_nonLocalType = MofCompiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("source"))
	{
		if (!m_sourceType.empty())
		{
			// report an error, both source and sourcetype cannot be set
			MofCompiler::theErrorHandler->recoverableError("source and sourcetype pragmas can't both be set, pragma source ignored",
				pCompilerDirective->theLineInfo);
		}
		m_source = MofCompiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("sourcetype"))
	{
		if (!m_source.empty())
		{
			// report an error, both source and sourcetype cannot be set
			MofCompiler::theErrorHandler->recoverableError("source and sourcetype pragmas can't both be set, pragma sourcetype ignored",
				pCompilerDirective->theLineInfo);
		}
		m_sourceType = MofCompiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else
	{
		MofCompiler::theErrorHandler->recoverableError(
			format("Ignoring unknown pragma: %1",
				*pCompilerDirective->pPragmaName->pPragmaName).c_str(),
			pCompilerDirective->theLineInfo);
	}
}


void CIMOMVisitor::VisitPragmaName( const PragmaName * )
{
	assert(0);
}


void CIMOMVisitor::VisitPragmaParameter( const PragmaParameter * )
{
	assert(0);
}


void CIMOMVisitor::VisitClassDeclaration( const ClassDeclaration *pClassDeclaration )
{
	m_curClass = OW_CIMClass(*pClassDeclaration->pClassName->pClassName);

	if ( pClassDeclaration->pQualifier.get() != 0 )
	{
		for ( OW_List<Qualifier *>::const_iterator i = pClassDeclaration->pQualifier->begin();
			 i != pClassDeclaration->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			m_curClass.addQualifier(m_curQualifier);
		}
	}

	if ( pClassDeclaration->pAlias.get() != 0 )
	{
		m_aliasMap[*(pClassDeclaration->pAlias->pAliasIdentifier->pAliasIdentifier)] =
			*(pClassDeclaration->pClassName->pClassName);
	}

	if ( pClassDeclaration->pSuperClass.get() != 0 )
	{
		m_curClass.setSuperClass(*(pClassDeclaration->pSuperClass->pClassName->pClassName));
	}

	if ( pClassDeclaration->pClassFeature.get() != 0 )
	{
		for ( OW_List<ClassFeature *>::const_iterator i = pClassDeclaration->pClassFeature->begin();
			 i != pClassDeclaration->pClassFeature->end();
			 ++i )
		{
			(*i)->Accept( this );
		}
	}
	CIMOMcreateClass(pClassDeclaration->theLineInfo);
}

void CIMOMVisitor::VisitAssocDeclaration( const AssocDeclaration *pAssocDeclaration )
{
	m_curClass = OW_CIMClass(*pAssocDeclaration->pClassName->pClassName);

	OW_CIMQualifierType qt = getQualifierType(OW_CIMQualifier::CIM_QUAL_ASSOCIATION, pAssocDeclaration->theLineInfo);
	OW_CIMQualifier q(qt);
	q.setValue(OW_CIMValue(OW_Bool(true)));

	m_curClass.addQualifier(q);
	if ( pAssocDeclaration->pQualifier.get() != 0 )
	{
		for ( OW_List<Qualifier *>::const_iterator i = pAssocDeclaration->pQualifier->begin();
			 i != pAssocDeclaration->pQualifier->end();
			 ++i )
		{
			(*i)->Accept( this );
			m_curClass.addQualifier(m_curQualifier);
		}
	}

	if ( pAssocDeclaration->pAlias.get() != 0 )
	{
		m_aliasMap[*(pAssocDeclaration->pAlias->pAliasIdentifier->pAliasIdentifier)] =
			*(pAssocDeclaration->pClassName->pClassName);
	}
	if ( pAssocDeclaration->pSuperClass.get() != 0 )
	{
		m_curClass.setSuperClass(*(pAssocDeclaration->pSuperClass->pClassName->pClassName));
	}

	if ( pAssocDeclaration->pAssociationFeature.get() != 0 )
	{
		for ( OW_List<AssociationFeature *>::const_iterator i = pAssocDeclaration->pAssociationFeature->begin();
			 i != pAssocDeclaration->pAssociationFeature->end();
			 ++i )
		{
			(*i)->Accept( this );
		}
	}
	CIMOMcreateClass(pAssocDeclaration->theLineInfo);
}


void CIMOMVisitor::VisitIndicDeclaration( const IndicDeclaration *pIndicDeclaration )
{
	m_curClass = OW_CIMClass(*pIndicDeclaration->pClassName->pClassName);
	
	OW_CIMQualifierType qt = getQualifierType(OW_CIMQualifier::CIM_QUAL_INDICATION, pIndicDeclaration->theLineInfo);
	OW_CIMQualifier q(qt);
	q.setValue(OW_CIMValue(OW_Bool(true)));
	m_curClass.addQualifier(q);
	if ( pIndicDeclaration->pQualifier.get() != 0 )
	{
		for ( OW_List<Qualifier *>::const_iterator i = pIndicDeclaration->pQualifier->begin();
			 i != pIndicDeclaration->pQualifier->end();
			 ++i )
		{
			(*i)->Accept( this );
			m_curClass.addQualifier(m_curQualifier);
		}
	}

	if ( pIndicDeclaration->pAlias.get() != 0 )
	{
		m_aliasMap[*(pIndicDeclaration->pAlias->pAliasIdentifier->pAliasIdentifier)] =
			*(pIndicDeclaration->pClassName->pClassName);
	}
	if ( pIndicDeclaration->pSuperClass.get() != 0 )
	{
		m_curClass.setSuperClass(*(pIndicDeclaration->pSuperClass->pClassName->pClassName));
	}

	if ( pIndicDeclaration->pClassFeature.get() != 0 )
	{
		for ( OW_List<ClassFeature *>::const_iterator i = pIndicDeclaration->pClassFeature->begin();
			 i != pIndicDeclaration->pClassFeature->end();
			 ++i )
		{
			(*i)->Accept( this );
		}
	}
	CIMOMcreateClass(pIndicDeclaration->theLineInfo);
}


void CIMOMVisitor::VisitClassName( const ClassName * )
{
	assert(0);
}
void CIMOMVisitor::VisitAlias( const Alias * )
{
	assert(0);
}
void CIMOMVisitor::VisitAliasIdentifier( const AliasIdentifier *pAliasIdentifier )
{
	OW_String alias = m_aliasMap[*pAliasIdentifier->pAliasIdentifier];
	if (alias.empty())
	{
		MofCompiler::theErrorHandler->recoverableError(format("Invalid alias: %1", *pAliasIdentifier->pAliasIdentifier).c_str(),
			pAliasIdentifier->theLineInfo);
	}
	m_curValue = OW_CIMValue(alias);
}
void CIMOMVisitor::VisitSuperClass( const SuperClass * )
{
	assert(0);
}


void CIMOMVisitor::VisitClassFeaturePropertyDeclaration( const ClassFeaturePropertyDeclaration *pClassFeaturePropertyDeclaration )
{
	pClassFeaturePropertyDeclaration->pPropertyDeclaration->Accept( this );
	m_curClass.addProperty(m_curProperty);
}


void CIMOMVisitor::VisitClassFeatureMethodDeclaration( const ClassFeatureMethodDeclaration *pClassFeatureMethodDeclaration )
{
	pClassFeatureMethodDeclaration->pMethodDeclaration->Accept( this );
	m_curClass.addMethod(m_curMethod);
}


void CIMOMVisitor::VisitClassFeatureReferenceDeclaration( const ClassFeatureReferenceDeclaration *pClassFeatureReferenceDeclaration )
{
	pClassFeatureReferenceDeclaration->pReferenceDeclaration->Accept( this );
	m_curClass.addProperty(m_curProperty);
}


void CIMOMVisitor::VisitAssociationFeatureClassFeature( const AssociationFeatureClassFeature *pAssociationFeatureClassFeature )
{
	pAssociationFeatureClassFeature->pClassFeature->Accept( this );
}


void CIMOMVisitor::VisitQualifier( const Qualifier *pQualifier )
{
	m_curQualifier = OW_CIMQualifier(*pQualifier->pQualifierName->pQualifierName);
	OW_CIMQualifierType qt = getQualifierType(m_curQualifier.getName(), pQualifier->theLineInfo);
	m_curQualifier.setDefaults(qt);

	if ( pQualifier->pQualifierParameter.get() != 0 )
	{
		pQualifier->pQualifierParameter->Accept( this );
		m_curQualifier.setValue(m_curValue);
	}
	else
	{
		if (qt.getDataType() == OW_CIMDataType(OW_CIMDataType::BOOLEAN))
		{
			m_curQualifier.setValue(OW_CIMValue(OW_Bool(true)));
		}
		else
		{
			MofCompiler::theErrorHandler->fatalError(
				"Missing value for non-boolean qualifier.",
				pQualifier->theLineInfo);
		}
	}

	if ( pQualifier->pFlavor.get() != 0 )
	{
		for ( OW_List<Flavor *>::const_iterator i = pQualifier->pFlavor->begin();
			 i != pQualifier->pFlavor->end(); ++i )
		{
			if ( (*i)->pFlavor->equalsIgnoreCase( "ENABLEOVERRIDE" ) )
			{
				m_curQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::ENABLEOVERRIDE));
			}
			else if ( (*i)->pFlavor->equalsIgnoreCase( "DISABLEOVERRIDE" ) )
			{
				m_curQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::DISABLEOVERRIDE));
			}
			else if ( (*i)->pFlavor->equalsIgnoreCase( "RESTRICTED" ) )
			{
				m_curQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::RESTRICTED));
			}
			else if ( (*i)->pFlavor->equalsIgnoreCase( "TOSUBCLASS" ) )
			{
				m_curQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TOSUBCLASS));
			}
			else if ( (*i)->pFlavor->equalsIgnoreCase( "TRANSLATABLE" ) )
			{
				m_curQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TRANSLATE));
			}
			else
			{
				MofCompiler::theErrorHandler->fatalError(
					format("Internal Compiler Error. Invalid flavor: %1", *(*i)->pFlavor).c_str(),
					(*i)->theLineInfo);
			}
		}
	}
}


void CIMOMVisitor::VisitQualifierParameterConstantValue( const QualifierParameterConstantValue *pQualifierParameterConstantValue )
{
	pQualifierParameterConstantValue->pConstantValue->Accept( this );
	// Need to get the type from the CIMOM
	OW_CIMDataType dt = getQualifierDataType(m_curQualifier.getName(), pQualifierParameterConstantValue->theLineInfo);
	// now cast the value into the correct type
	m_curValue = OW_CIMValueCast::castValueToDataType(m_curValue, dt);
}


void CIMOMVisitor::VisitQualifierParameterArrayInitializer( const QualifierParameterArrayInitializer *pQualifierParameterArrayInitializer )
{
	pQualifierParameterArrayInitializer->pArrayInitializer->Accept( this );

	// Need to get the type from the CIMOM
	OW_CIMDataType dt = getQualifierDataType(m_curQualifier.getName(), pQualifierParameterArrayInitializer->theLineInfo);
	// now cast the value into the correct type
	m_curValue = OW_CIMValueCast::castValueToDataType(m_curValue, dt);
}


void CIMOMVisitor::VisitFlavor( const Flavor * )
{
	assert(0);
}


void CIMOMVisitor::VisitPropertyDeclaration( const PropertyDeclaration *pPropertyDeclaration )
{
	m_curProperty = OW_CIMProperty(*pPropertyDeclaration->pPropertyName->pPropertyName);
	if ( pPropertyDeclaration->pQualifier.get() != 0 )
	{
		for ( OW_List<Qualifier *>::const_iterator i = pPropertyDeclaration->pQualifier->begin();
			 i != pPropertyDeclaration->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			m_curProperty.addQualifier(m_curQualifier);
		}

	}

	OW_Int64 arraySize = -1;
	if ( pPropertyDeclaration->pArray.get() != 0 )
	{
		OW_CIMDataType dt = OW_CIMDataType::getDataType(*pPropertyDeclaration->pDataType->pDataType);
		if (pPropertyDeclaration->pArray->pArray.get() != 0)
		{
			pPropertyDeclaration->pArray->pArray->Accept( this );
			m_curValue.get(arraySize);
		}
		dt.setToArrayType(arraySize);
		m_curProperty.setDataType(dt);
	}
	else
	{
		m_curProperty.setDataType(OW_CIMDataType::getDataType(*pPropertyDeclaration->pDataType->pDataType));
	}
	if ( pPropertyDeclaration->pDefaultValue.get() != 0 )
	{
		pPropertyDeclaration->pDefaultValue->Accept( this );
		if ( arraySize != -1 )
		{
			if ( m_curValue.isArray() )
			{
				if ( m_curValue.getArraySize() != arraySize )
				{
					MofCompiler::theErrorHandler->recoverableError(
						format("Array size (%1) doesn't match number of elements (%2)", arraySize, m_curValue.getArraySize()).c_str(),
						pPropertyDeclaration->theLineInfo);
				}
			}
			else
			{
				MofCompiler::theErrorHandler->recoverableError(
					"Property declared as array, but value is not an array",
					pPropertyDeclaration->theLineInfo);
			}
		}
		m_curProperty.setValue(m_curValue);
	}

}


void CIMOMVisitor::VisitReferenceDeclaration( const ReferenceDeclaration *pReferenceDeclaration )
{
	m_curProperty = OW_CIMProperty(*pReferenceDeclaration->pReferenceName->pReferenceName);
	if ( pReferenceDeclaration->pQualifier.get() != 0 )
	{
		for ( OW_List<Qualifier *>::const_iterator i = pReferenceDeclaration->pQualifier->begin();
			 i != pReferenceDeclaration->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			m_curProperty.addQualifier(m_curQualifier);
		}
	}

	OW_CIMDataType dt(*(pReferenceDeclaration->pObjectRef->pClassName->pClassName));
	m_curProperty.setDataType(dt);
	if ( pReferenceDeclaration->pDefaultValue.get() != 0 )
	{
		pReferenceDeclaration->pDefaultValue->Accept( this );
		m_curProperty.setValue(m_curValue);
	}

}


void CIMOMVisitor::VisitMethodDeclaration( const MethodDeclaration *pMethodDeclaration )
{
	m_curMethod = OW_CIMMethod(*pMethodDeclaration->pMethodName->pMethodName);
	if ( pMethodDeclaration->pQualifier.get() != 0 )
	{
		for ( OW_List<Qualifier *>::const_iterator i = pMethodDeclaration->pQualifier->begin();
			 i != pMethodDeclaration->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			m_curMethod.addQualifier(m_curQualifier);
		}

	}
	OW_CIMDataType dt = OW_CIMDataType::getDataType(*pMethodDeclaration->pDataType->pDataType);
	m_curMethod.setReturnType(dt);

	if ( pMethodDeclaration->pParameter.get() != 0 )
	{
		OW_CIMParameterArray params;
		for ( OW_List<Parameter *>::const_iterator j = pMethodDeclaration->pParameter->begin();
			 j != pMethodDeclaration->pParameter->end(); ++j )
		{
			(*j)->Accept( this );
			params.append(m_curParameter);
		}
		m_curMethod.setParameters(params);
	}
}


void CIMOMVisitor::VisitPropertyName( const PropertyName * )
{
	assert(0);
}


void CIMOMVisitor::VisitReferenceName( const ReferenceName * )
{
	assert(0);
}


void CIMOMVisitor::VisitMethodName( const MethodName * )
{
	assert(0);
}


void CIMOMVisitor::VisitDataType( const DataType * )
{
	assert(0);
}


void CIMOMVisitor::VisitObjectRef( const ObjectRef * )
{
	assert(0);
}


void CIMOMVisitor::VisitParameterDataType( const ParameterDataType *pParameterDataType )
{
	m_curParameter = OW_CIMParameter(*pParameterDataType->pParameterName->pParameterName);
	if ( pParameterDataType->pQualifier.get() != 0 )
	{
		OW_CIMQualifierArray quals;
		for ( OW_List<Qualifier *>::const_iterator i = pParameterDataType->pQualifier->begin();
			 i != pParameterDataType->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			quals.append(m_curQualifier);
		}
		m_curParameter.setQualifiers(quals);
	}

	OW_CIMDataType dt = OW_CIMDataType::getDataType(*pParameterDataType->pDataType->pDataType);
	if ( pParameterDataType->pArray.get() != 0 )
	{
		OW_Int64 arraySize = 0;
		if (pParameterDataType->pArray->pArray.get() != 0)
		{
			pParameterDataType->pArray->pArray->Accept( this );
			m_curValue.get(arraySize);
		}
		dt.setToArrayType(arraySize);
	}
	m_curParameter.setDataType(dt);
}


void CIMOMVisitor::VisitParameterObjectRef( const ParameterObjectRef *pParameterObjectRef )
{
	m_curParameter = OW_CIMParameter(*pParameterObjectRef->pParameterName->pParameterName);
	if ( pParameterObjectRef->pQualifier.get() != 0 )
	{
		OW_CIMQualifierArray quals;
		
		for ( OW_List<Qualifier *>::const_iterator i = pParameterObjectRef->pQualifier->begin();
			  i != pParameterObjectRef->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			quals.append(m_curQualifier);
		}
		m_curParameter.setQualifiers(quals);
	}

	OW_CIMDataType dt(*pParameterObjectRef->pObjectRef->pClassName->pClassName);

	if ( pParameterObjectRef->pArray.get() != 0 )
	{
		OW_Int64 arraySize = 0;
		if (pParameterObjectRef->pArray->pArray.get() != 0)
		{
			pParameterObjectRef->pArray->pArray->Accept( this );
			m_curValue.get(arraySize);
		}
		dt.setToArrayType(arraySize);
	}
	m_curParameter.setDataType(dt);
}


void CIMOMVisitor::VisitParameterName( const ParameterName * )
{
	assert(0);
}


void CIMOMVisitor::VisitArray( const Array * )
{
	assert(0);
}


void CIMOMVisitor::VisitDefaultValue( const DefaultValue *pDefaultValue )
{
	pDefaultValue->pInitializer->Accept( this );
}


void CIMOMVisitor::VisitInitializerReferenceInitializer( const InitializerReferenceInitializer *pInitializerReferenceInitializer )
{
	pInitializerReferenceInitializer->pReferenceInitializer->Accept( this );
}


void CIMOMVisitor::VisitInitializerArrayInitializer( const InitializerArrayInitializer *pInitializerArrayInitializer )
{
	pInitializerArrayInitializer->pArrayInitializer->Accept( this );
}


void CIMOMVisitor::VisitInitializerConstantValue( const InitializerConstantValue *pInitializerConstantValue )
{
	pInitializerConstantValue->pConstantValue->Accept( this );
}


void CIMOMVisitor::VisitArrayInitializer( const ArrayInitializer *pArrayInitializer )
{
	OW_CIMValueArray values;
	for( OW_List<ConstantValue *>::const_iterator i = pArrayInitializer->pConstantValue->begin();
		 i != pArrayInitializer->pConstantValue->end(); ++i )
	{
		(*i)->Accept( this );
		values.push_back(m_curValue);
	}
	m_curValue = convertValuesIntoValueArray(values);
}

template <class T>
OW_CIMValue doArrayConversion( T& tempArray, const OW_CIMValueArray& values )
{
	for (size_t i = 0; i < values.size(); ++i)
	{
		typename T::value_type element;
		values[i].get(element);
		tempArray.push_back(element);
	}
	return OW_CIMValue(tempArray);
}

OW_CIMValue CIMOMVisitor::convertValuesIntoValueArray( const OW_CIMValueArray& values )
{
	if (values.size())
	{
		if (values[0].getType() == OW_CIMDataType::BOOLEAN)
		{
			OW_BoolArray temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::CHAR16)
		{
			OW_Char16Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::UINT8)
		{
			OW_UInt8Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::SINT8)
		{
			OW_Int8Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::UINT16)
		{
			OW_UInt16Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::SINT16)
		{
			OW_Int16Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::UINT32)
		{
			OW_UInt32Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::SINT32)
		{
			OW_Int32Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::UINT64)
		{
			OW_UInt64Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::SINT64)
		{
			OW_Int64Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::REAL64)
		{
			OW_Real64Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::REAL32)
		{
			OW_Real32Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == OW_CIMDataType::STRING)
		{
			OW_StringArray temp;
			return doArrayConversion(temp, values);
		}
	}
	return OW_CIMValue();
}




void CIMOMVisitor::VisitReferenceInitializerObjectHandle( const ReferenceInitializerObjectHandle *pReferenceInitializerObjectHandle )
{
	pReferenceInitializerObjectHandle->pObjectHandle->Accept( this );
}


void CIMOMVisitor::VisitReferenceInitializerAliasIdentifier( const ReferenceInitializerAliasIdentifier *pReferenceInitializerAliasIdentifier )
{
	pReferenceInitializerAliasIdentifier->pAliasIdentifier->Accept( this );
}


void CIMOMVisitor::VisitObjectHandle( const ObjectHandle *pObjectHandle )
{
	m_curValue = OW_CIMValue(*pObjectHandle->pObjectHandle);
}


void CIMOMVisitor::VisitQualifierDeclaration( const QualifierDeclaration *pQualifierDeclaration )
{
	m_curQualifierType = OW_CIMQualifierType(*pQualifierDeclaration->pQualifierName->pQualifierName);
	
	pQualifierDeclaration->pQualifierType->Accept( this );

	pQualifierDeclaration->pScope->Accept( this );
	if ( pQualifierDeclaration->pDefaultFlavor.get() != 0 )
	{
		pQualifierDeclaration->pDefaultFlavor->Accept( this );
	}
	CIMOMsetQualifierType(pQualifierDeclaration->theLineInfo);
}


void CIMOMVisitor::VisitQualifierName( const QualifierName * )
{
	assert(0);
}


void CIMOMVisitor::VisitQualifierType( const QualifierType *pQualifierType )
{
	OW_CIMDataType dt = OW_CIMDataType::getDataType(*pQualifierType->pDataType->pDataType);
	if ( pQualifierType->pArray.get() != 0 )
	{
		OW_Int64 arraySize = 0;
		if (pQualifierType->pArray->pArray.get() != 0)
		{
			pQualifierType->pArray->pArray->Accept( this );
			m_curValue.get(arraySize);
		}
		dt.setToArrayType(arraySize);
	}
	m_curQualifierType.setDataType(dt);

	if ( pQualifierType->pDefaultValue.get() != 0 )
	{
		pQualifierType->pDefaultValue->Accept( this );
		m_curQualifierType.setDefaultValue(m_curValue);
	}
}


void CIMOMVisitor::VisitScope( const Scope *pScope )
{
	for ( OW_List<MetaElement *>::const_iterator i = pScope->pMetaElement->begin();
		 i != pScope->pMetaElement->end();
		 ++i )
	{
		OW_CIMScope scope;
		/* Removed by CIM Specification 2.2 Addenda Sheet: 01
		http://www.dmtf.org/spec/release/CIM_Errata/CIM_Spec_Addenda221.htm
		Our grammar still parses this, because it's used in CIM_Schema23.mof,
		So the user will just get a warning.
		if ((*i)->pMetaElement->equalsIgnoreCase("SCHEMA"))
		{
			scope = OW_CIMScope(OW_CIMScope::SCHEMA);
		}
		else*/
		if ((*i)->pMetaElement->equalsIgnoreCase("CLASS"))
		{
			scope = OW_CIMScope(OW_CIMScope::CLASS);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("ASSOCIATION"))
		{
			scope = OW_CIMScope(OW_CIMScope::ASSOCIATION);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("INDICATION"))
		{
			scope = OW_CIMScope(OW_CIMScope::INDICATION);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("QUALIFIER"))
		{
			scope = OW_CIMScope(OW_CIMScope::QUALIFIER);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("PROPERTY"))
		{
			scope = OW_CIMScope(OW_CIMScope::PROPERTY);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("REFERENCE"))
		{
			scope = OW_CIMScope(OW_CIMScope::REFERENCE);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("METHOD"))
		{
			scope = OW_CIMScope(OW_CIMScope::METHOD);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("PARAMETER"))
		{
			scope = OW_CIMScope(OW_CIMScope::PARAMETER);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("ANY"))
		{
			scope = OW_CIMScope(OW_CIMScope::ANY);
		}
		else
		{
			MofCompiler::theErrorHandler->recoverableError(
				format("Invalid scope: %1", *(*i)->pMetaElement).c_str(),
				(*i)->theLineInfo );
		}
		
		m_curQualifierType.addScope(scope);
	}

}


void CIMOMVisitor::VisitMetaElement( const MetaElement * )
{
	assert(0);
}


void CIMOMVisitor::VisitDefaultFlavor( const DefaultFlavor *pDefaultFlavor )
{
	for ( OW_List<Flavor *>::const_iterator i = pDefaultFlavor->pFlavor->begin();
		 i != pDefaultFlavor->pFlavor->end();
		 ++i )
	{
		if ((*i)->pFlavor->equalsIgnoreCase("ENABLEOVERRIDE"))
		{
			m_curQualifierType.addFlavor(OW_CIMFlavor(OW_CIMFlavor::ENABLEOVERRIDE));
		}
		else if ((*i)->pFlavor->equalsIgnoreCase("DISABLEOVERRIDE"))
		{
			m_curQualifierType.addFlavor(OW_CIMFlavor(OW_CIMFlavor::DISABLEOVERRIDE));
		}
		else if ((*i)->pFlavor->equalsIgnoreCase("RESTRICTED"))
		{
			m_curQualifierType.addFlavor(OW_CIMFlavor(OW_CIMFlavor::RESTRICTED));
		}
		else if ((*i)->pFlavor->equalsIgnoreCase("TOSUBCLASS"))
		{
			m_curQualifierType.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TOSUBCLASS));
		}
		else if ((*i)->pFlavor->equalsIgnoreCase("TRANSLATABLE"))
		{
			m_curQualifierType.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TRANSLATE));
		}
		else
		{
			MofCompiler::theErrorHandler->fatalError(
				format("Internal Compiler Error. Invalid flavor: %1", *(*i)->pFlavor).c_str(),
				(*i)->theLineInfo);
		}
	}
}


void CIMOMVisitor::VisitInstanceDeclaration( const InstanceDeclaration *pInstanceDeclaration )
{
	OW_CIMClass cc = m_hdl->getClass(m_namespace,
		*pInstanceDeclaration->pClassName->pClassName, false);
	m_curInstance = cc.newInstance();
	if ( pInstanceDeclaration->pQualifier.get() != 0 )
	{
		for ( OW_List<Qualifier *>::const_iterator i = pInstanceDeclaration->pQualifier->begin();
			  i != pInstanceDeclaration->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			m_curInstance.setQualifier(m_curQualifier);
		}
	}

	if ( pInstanceDeclaration->pValueInitializer.get() != 0 )
	{
		for ( OW_List<ValueInitializer *>::const_iterator i = pInstanceDeclaration->pValueInitializer->begin();
			  i != pInstanceDeclaration->pValueInitializer->end(); ++i )
		{
			(*i)->Accept( this );
			OW_CIMProperty tempProp = m_curInstance.getProperty(m_curProperty.getName());
			if (tempProp)
			{
				OW_CIMQualifierArray newQuals = m_curProperty.getQualifiers();
				for (size_t i = 0; i < newQuals.size(); ++i)
				{
					tempProp.setQualifier(newQuals[i]);
				}

				OW_CIMValue castValue = OW_CIMValueCast::castValueToDataType(
						m_curProperty.getValue(),
						OW_CIMDataType(tempProp.getDataType()));

				if (castValue.getType() == OW_CIMDataType::REFERENCE)
				{
					OW_CIMObjectPath cop;
					castValue.get(cop);
					if (cop)
					{
						// If the object path doesn't have a : character, then we need to set the namespace on it.
						if (m_curProperty.getValue().toString().indexOf(':') == -1)
						{
							cop.setNameSpace(m_namespace);
							castValue = OW_CIMValue(cop);
						}
					}
				}

				if (!castValue)
				{
					MofCompiler::theErrorHandler->recoverableError(
							format("Value is not the correct type: %1.  The type should be: %2", m_curProperty.getValue().toString(), tempProp.getDataType().toString()).c_str(), pInstanceDeclaration->theLineInfo);
				}

				tempProp.setValue(castValue);
				m_curInstance.setProperty(tempProp);
			}
			else
			{
				m_curInstance.setProperty(m_curProperty);
			}
		}
	}
	
	if ( pInstanceDeclaration->pAlias.get() != 0 )
	{
		OW_CIMObjectPath cop(m_curInstance.getClassName(), m_curInstance.getKeyValuePairs());
		
		m_aliasMap[*(pInstanceDeclaration->pAlias->pAliasIdentifier->pAliasIdentifier)] =
			cop.modelPath();
	}

	CIMOMcreateInstance(pInstanceDeclaration->theLineInfo);
}


void CIMOMVisitor::VisitValueInitializer( const ValueInitializer *pValueInitializer )
{
	m_curProperty = OW_CIMProperty(*pValueInitializer->pValueInitializer);
	if ( pValueInitializer->pQualifier.get() != 0 )
	{
		for ( OW_List<Qualifier *>::const_iterator i = pValueInitializer->pQualifier->begin();
			  i != pValueInitializer->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			m_curProperty.setQualifier(m_curQualifier);
		}
	}
	pValueInitializer->pDefaultValue->Accept( this );
	m_curProperty.setValue(m_curValue);
}

void CIMOMVisitor::VisitIntegerValueBinaryValue( const IntegerValueBinaryValue *pIntegerValueBinaryValue )
{
	OW_Int64 val = pIntegerValueBinaryValue->pBinaryValue->toInt64(2);
	m_curValue = OW_CIMValue(val);
}

void CIMOMVisitor::VisitIntegerValueOctalValue( const IntegerValueOctalValue *pIntegerValueOctalValue )
{
	OW_Int64 val = pIntegerValueOctalValue->pOctalValue->toInt64(8);
	m_curValue = OW_CIMValue(val);
}

void CIMOMVisitor::VisitIntegerValueDecimalValue( const IntegerValueDecimalValue *pIntegerValueDecimalValue )
{
	OW_Int64 val = pIntegerValueDecimalValue->pDecimalValue->toInt64(10);
	m_curValue = OW_CIMValue(val);
}

void CIMOMVisitor::VisitIntegerValueHexValue( const IntegerValueHexValue *pIntegerValueHexValue )
{
	OW_Int64 val = pIntegerValueHexValue->pHexValue->toInt64(16);
	m_curValue = OW_CIMValue(val);
}

void CIMOMVisitor::VisitConstantValueIntegerValue( const ConstantValueIntegerValue *pConstantValueIntegerValue )
{
	pConstantValueIntegerValue->pIntegerValue->Accept( this );
}

void CIMOMVisitor::VisitConstantValueFloatValue( const ConstantValueFloatValue *pConstantValueFloatValue )
{
	OW_Real64 val = pConstantValueFloatValue->pFloatValue->toReal64();
	m_curValue = OW_CIMValue(val);
}

void CIMOMVisitor::VisitConstantValueStringValue( const ConstantValueStringValue *pConstantValueStringValue )
{
	m_curValue = OW_CIMValue(MofCompiler::fixParsedString(*pConstantValueStringValue->pStringValue));
}

void CIMOMVisitor::VisitConstantValueCharValue( const ConstantValueCharValue *pConstantValueCharValue )
{
	char c = pConstantValueCharValue->pCharValue->charAt(1);
	m_curValue = OW_CIMValue(c);
}

void CIMOMVisitor::VisitConstantValueBooleanValue( const ConstantValueBooleanValue *pConstantValueBooleanValue )
{
	OW_Bool b;
	if (pConstantValueBooleanValue->pBooleanValue->equalsIgnoreCase("TRUE"))
	{
		b = true;
	}
	else
	{
		b = false;
	}
	m_curValue = OW_CIMValue(b);
}

void CIMOMVisitor::VisitConstantValueNullValue( const ConstantValueNullValue * )
{
	m_curValue.setNull();
}

OW_CIMDataType CIMOMVisitor::getQualifierDataType(const OW_String& qualName, const lineInfo& li)
{
	return getQualifierType(qualName, li).getDataType();
}

OW_CIMQualifierType CIMOMVisitor::getQualifierType(const OW_String& qualName, const lineInfo& li)
{
	OW_String lcqualName = qualName;
	lcqualName.toLowerCase();
	OW_Map<OW_String, OW_CIMQualifierType>::const_iterator i = m_dataTypeCache.find(lcqualName);
	if (i == m_dataTypeCache.end())
	{
		m_dataTypeCache[lcqualName] = CIMOMgetQualifierType(qualName, li);
	}
	
	return m_dataTypeCache[lcqualName];
}


void CIMOMVisitor::CIMOMcreateClass(const lineInfo& li)
{
	try
	{
		MofCompiler::theErrorHandler->progressMessage(format("Processing class: %1", m_curClass.getName()).c_str(), li);
		m_hdl->createClass(OW_CIMObjectPath(m_curClass.getName(), m_namespace), m_curClass);
		MofCompiler::theErrorHandler->progressMessage(format("Created class: %1", m_curClass.getName()).c_str(), li);
	}
	catch (const OW_CIMException& ce)
	{
		if (ce.getErrNo() == OW_CIMException::ALREADY_EXISTS)
		{
			try
			{
				m_hdl->modifyClass(OW_CIMObjectPath(m_curClass.getName(), m_namespace), m_curClass);
				MofCompiler::theErrorHandler->progressMessage(format("Updated class: %1", m_curClass.getName()).c_str(), li);
			}
			catch (const OW_CIMException& ce)
			{
				MofCompiler::theErrorHandler->recoverableError(format("Received error from CIMOM: %1", ce.getMessage()).c_str(), li);
			}
		}
		else
		{
			MofCompiler::theErrorHandler->recoverableError(format("Received error from CIMOM: %1", ce.getMessage()).c_str(), li);
		}
	}
}

void CIMOMVisitor::CIMOMsetQualifierType(const lineInfo& li)
{
	try
	{
		MofCompiler::theErrorHandler->progressMessage(format("Setting QualifierType: %1", m_curQualifierType.getName()).c_str(), li);
		m_hdl->setQualifierType(OW_CIMObjectPath(m_curQualifierType.getName(), m_namespace), m_curQualifierType);
		// save it in the cache
		OW_String lcqualName = m_curQualifierType.getName();
		lcqualName.toLowerCase();
		m_dataTypeCache[lcqualName] = m_curQualifierType;
	}
	catch (const OW_CIMException& ce)
	{
		MofCompiler::theErrorHandler->recoverableError(format("Received error from CIMOM: %1", ce.getMessage()).c_str(), li);
	}
}

void CIMOMVisitor::CIMOMcreateInstance(const lineInfo& li)
{
	OW_CIMObjectPath cop(m_curInstance.getClassName(), m_curInstance.getKeyValuePairs());
	cop.setNameSpace(m_namespace);
	MofCompiler::theErrorHandler->progressMessage(format("Processing Instance: %1", cop.modelPath()).c_str(), li);
	try
	{

		m_hdl->createInstance(cop,
			m_curInstance);
		MofCompiler::theErrorHandler->progressMessage(format("Created Instance: %1", cop.modelPath()).c_str(), li);
	}
	catch (const OW_CIMException& ce)
	{
		if (ce.getErrNo() == OW_CIMException::ALREADY_EXISTS)
		{
			try
			{
				m_hdl->modifyInstance(cop, m_curInstance);
				MofCompiler::theErrorHandler->progressMessage(format("Updated Instance: %1", cop.modelPath()).c_str(), li);
			}
			catch (const OW_CIMException& ce)
			{
				MofCompiler::theErrorHandler->recoverableError(format("Received error from CIMOM: %1", ce.getMessage()).c_str(), li);
			}
		}
		else
		{
			MofCompiler::theErrorHandler->recoverableError(format("Received error from CIMOM: %1", ce.getMessage()).c_str(), li);
		}
	}
}

OW_CIMQualifierType CIMOMVisitor::CIMOMgetQualifierType(const OW_String& qualName, const lineInfo& li)
{
	try
	{
		return m_hdl->getQualifierType(m_namespace, qualName);
	}
	catch (const OW_CIMException& ce)
	{
		MofCompiler::theErrorHandler->fatalError(format("Received error from CIMOM: %1", ce.getMessage()).c_str(), li);
	}
	return OW_CIMQualifierType(true);
}

