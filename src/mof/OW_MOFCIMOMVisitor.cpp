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
#include "OW_config.h"
#include "OW_MOFGrammar.hpp"
#include "OW_MOFCIMOMVisitor.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_Format.hpp"
#include "OW_MOFCompiler.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMScope.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMNameSpaceUtils.hpp"
#include <assert.h>

namespace OpenWBEM
{

namespace MOF
{

using namespace WBEMFlags;
CIMOMVisitor::CIMOMVisitor(const CIMOMHandleIFCRef& handle, 
	const Compiler::Options& opts,
	const Reference<ParserErrorHandlerIFC>& _theErrorHandler)
: m_curValue(CIMNULL)
, m_hdl(handle)
, m_rephdl(handle.cast_to<RepositoryCIMOMHandle>())
, theErrorHandler(_theErrorHandler)
, m_opts(opts)
, m_namespace(opts.m_namespace)
{
}
CIMOMVisitor::~CIMOMVisitor()
{
}
void CIMOMVisitor::VisitMOFSpecification( const MOFSpecification *pMOFSpecification )
{
	for ( List<MOFProduction *>::const_iterator i = pMOFSpecification->pMOFProduction->begin();
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
		m_instanceLocale = Compiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("locale"))
	{
		m_locale = Compiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("namespace"))
	{
		m_namespace = Compiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("nonlocal"))
	{
		if (!m_nonLocalType.empty())
		{
			// report an error, both nonlocal and nonlocaltype cannot be set.
			theErrorHandler->recoverableError("nonlocal and nonlocaltype pragmas can't both be set, pragma nonlocal ignored",
				pCompilerDirective->theLineInfo);
		}
		m_nonLocal = Compiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("nonlocaltype"))
	{
		if (!m_nonLocal.empty())
		{
			// report an error, both nonlocal and nonlocaltype cannot be set.
			theErrorHandler->recoverableError("nonlocal and nonlocaltype pragmas can't both be set, pragma nonlocaltype ignored",
				pCompilerDirective->theLineInfo);
		}
		m_nonLocalType = Compiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("source"))
	{
		if (!m_sourceType.empty())
		{
			// report an error, both source and sourcetype cannot be set
			theErrorHandler->recoverableError("source and sourcetype pragmas can't both be set, pragma source ignored",
				pCompilerDirective->theLineInfo);
		}
		m_source = Compiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else if (pCompilerDirective->pPragmaName->pPragmaName->equalsIgnoreCase("sourcetype"))
	{
		if (!m_source.empty())
		{
			// report an error, both source and sourcetype cannot be set
			theErrorHandler->recoverableError("source and sourcetype pragmas can't both be set, pragma sourcetype ignored",
				pCompilerDirective->theLineInfo);
		}
		m_sourceType = Compiler::fixParsedString(*pCompilerDirective->pPragmaParameter->pPragmaParameter);
	}
	else
	{
		theErrorHandler->recoverableError(
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
	m_curClass = CIMClass(*pClassDeclaration->pClassName->pClassName);
	if ( pClassDeclaration->pQualifier.get() != 0 )
	{
		for ( List<Qualifier *>::const_iterator i = pClassDeclaration->pQualifier->begin();
			 i != pClassDeclaration->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			m_curClass.addQualifier(m_curQualifier);
		}
	}
	if ( pClassDeclaration->pAlias.get() != 0 )
	{
		theErrorHandler->recoverableError("Class aliases are deprecated per DMTF CR817", pClassDeclaration->theLineInfo);
		m_aliasMap[*(pClassDeclaration->pAlias->pAliasIdentifier->pAliasIdentifier)] =
			*(pClassDeclaration->pClassName->pClassName);
	}
	if ( pClassDeclaration->pSuperClass.get() != 0 )
	{
		m_curClass.setSuperClass(*(pClassDeclaration->pSuperClass->pClassName->pClassName));
	}
	if ( pClassDeclaration->pClassFeature.get() != 0 )
	{
		for ( List<ClassFeature *>::const_iterator i = pClassDeclaration->pClassFeature->begin();
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
	m_curClass = CIMClass(*pAssocDeclaration->pClassName->pClassName);
	CIMQualifierType qt = getQualifierType(CIMQualifier::CIM_QUAL_ASSOCIATION, pAssocDeclaration->theLineInfo);
	CIMQualifier q(qt);
	q.setValue(CIMValue(Bool(true)));
	m_curClass.addQualifier(q);
	if ( pAssocDeclaration->pQualifier.get() != 0 )
	{
		for ( List<Qualifier *>::const_iterator i = pAssocDeclaration->pQualifier->begin();
			 i != pAssocDeclaration->pQualifier->end();
			 ++i )
		{
			(*i)->Accept( this );
			m_curClass.addQualifier(m_curQualifier);
		}
	}
	if ( pAssocDeclaration->pAlias.get() != 0 )
	{
		theErrorHandler->recoverableError("Class aliases are deprecated per DMTF CR817", pAssocDeclaration->theLineInfo);
		m_aliasMap[*(pAssocDeclaration->pAlias->pAliasIdentifier->pAliasIdentifier)] =
			*(pAssocDeclaration->pClassName->pClassName);
	}
	if ( pAssocDeclaration->pSuperClass.get() != 0 )
	{
		m_curClass.setSuperClass(*(pAssocDeclaration->pSuperClass->pClassName->pClassName));
	}
	if ( pAssocDeclaration->pAssociationFeature.get() != 0 )
	{
		for ( List<AssociationFeature *>::const_iterator i = pAssocDeclaration->pAssociationFeature->begin();
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
	m_curClass = CIMClass(*pIndicDeclaration->pClassName->pClassName);
	
	CIMQualifierType qt = getQualifierType(CIMQualifier::CIM_QUAL_INDICATION, pIndicDeclaration->theLineInfo);
	CIMQualifier q(qt);
	q.setValue(CIMValue(Bool(true)));
	m_curClass.addQualifier(q);
	if ( pIndicDeclaration->pQualifier.get() != 0 )
	{
		for ( List<Qualifier *>::const_iterator i = pIndicDeclaration->pQualifier->begin();
			 i != pIndicDeclaration->pQualifier->end();
			 ++i )
		{
			(*i)->Accept( this );
			m_curClass.addQualifier(m_curQualifier);
		}
	}
	if ( pIndicDeclaration->pAlias.get() != 0 )
	{
		theErrorHandler->recoverableError("Class aliases are deprecated per DMTF CR817", pIndicDeclaration->theLineInfo);
		m_aliasMap[*(pIndicDeclaration->pAlias->pAliasIdentifier->pAliasIdentifier)] =
			*(pIndicDeclaration->pClassName->pClassName);
	}
	if ( pIndicDeclaration->pSuperClass.get() != 0 )
	{
		m_curClass.setSuperClass(*(pIndicDeclaration->pSuperClass->pClassName->pClassName));
	}
	if ( pIndicDeclaration->pClassFeature.get() != 0 )
	{
		for ( List<ClassFeature *>::const_iterator i = pIndicDeclaration->pClassFeature->begin();
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
	String alias = m_aliasMap[*pAliasIdentifier->pAliasIdentifier];
	if (alias.empty())
	{
		theErrorHandler->recoverableError(format("Invalid alias: %1", *pAliasIdentifier->pAliasIdentifier).c_str(),
			pAliasIdentifier->theLineInfo);
	}
	m_curValue = CIMValue(alias);
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
	m_curQualifier = CIMQualifier(*pQualifier->pQualifierName->pQualifierName);
	CIMQualifierType qt = getQualifierType(m_curQualifier.getName(), pQualifier->theLineInfo);
	m_curQualifier.setDefaults(qt);
	if ( pQualifier->pQualifierParameter.get() != 0 )
	{
		pQualifier->pQualifierParameter->Accept( this );
		m_curQualifier.setValue(m_curValue);
	}
	else
	{
		// CIM spec 4.7: The MOF syntax allows specifying a qualifier without
		// an explicit value. In this case, the assumed value depends on the
		// qualifier type: booleans are true, numeric types are null, strings
		// are null and arrays are empty.
		if (qt.getDataType().isArrayType())
		{
			// create an empty array of the right type.  An empty String array
			// will cast to anything.
			m_curQualifier.setValue(CIMValueCast::castValueToDataType(CIMValue(StringArray()),qt.getDataType()));
		}
		else if (qt.getDataType().getType() == CIMDataType::BOOLEAN)
		{
			m_curQualifier.setValue(CIMValue(Bool(true)));
		}
		else
		{
			m_curQualifier.setValue(CIMValue(CIMNULL));
		}
	}
	if ( pQualifier->pFlavor.get() != 0 )
	{
		for ( List<Flavor *>::const_iterator i = pQualifier->pFlavor->begin();
			 i != pQualifier->pFlavor->end(); ++i )
		{
			if ( (*i)->pFlavor->equalsIgnoreCase( "ENABLEOVERRIDE" ) )
			{
				m_curQualifier.addFlavor(CIMFlavor(CIMFlavor::ENABLEOVERRIDE));
			}
			else if ( (*i)->pFlavor->equalsIgnoreCase( "DISABLEOVERRIDE" ) )
			{
				m_curQualifier.addFlavor(CIMFlavor(CIMFlavor::DISABLEOVERRIDE));
			}
			else if ( (*i)->pFlavor->equalsIgnoreCase( "RESTRICTED" ) )
			{
				m_curQualifier.addFlavor(CIMFlavor(CIMFlavor::RESTRICTED));
			}
			else if ( (*i)->pFlavor->equalsIgnoreCase( "TOSUBCLASS" ) )
			{
				m_curQualifier.addFlavor(CIMFlavor(CIMFlavor::TOSUBCLASS));
			}
			else if ( (*i)->pFlavor->equalsIgnoreCase( "TRANSLATABLE" ) )
			{
				m_curQualifier.addFlavor(CIMFlavor(CIMFlavor::TRANSLATE));
			}
			else
			{
				theErrorHandler->fatalError(
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
	CIMDataType dt = getQualifierDataType(m_curQualifier.getName(), pQualifierParameterConstantValue->theLineInfo);
	// now cast the value into the correct type
	m_curValue = CIMValueCast::castValueToDataType(m_curValue, dt);
}
void CIMOMVisitor::VisitQualifierParameterArrayInitializer( const QualifierParameterArrayInitializer *pQualifierParameterArrayInitializer )
{
	pQualifierParameterArrayInitializer->pArrayInitializer->Accept( this );
	// Need to get the type from the CIMOM
	CIMDataType dt = getQualifierDataType(m_curQualifier.getName(), pQualifierParameterArrayInitializer->theLineInfo);
	// now cast the value into the correct type
	m_curValue = CIMValueCast::castValueToDataType(m_curValue, dt);
}
void CIMOMVisitor::VisitFlavor( const Flavor * )
{
	assert(0);
}
void CIMOMVisitor::VisitPropertyDeclaration( const PropertyDeclaration *pPropertyDeclaration )
{
	m_curProperty = CIMProperty(*pPropertyDeclaration->pPropertyName->pPropertyName);
	if ( pPropertyDeclaration->pQualifier.get() != 0 )
	{
		for ( List<Qualifier *>::const_iterator i = pPropertyDeclaration->pQualifier->begin();
			 i != pPropertyDeclaration->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			m_curProperty.addQualifier(m_curQualifier);
		}
	}
	Int64 arraySize = -1;
	if ( pPropertyDeclaration->pArray.get() != 0 )
	{
		CIMDataType dt = CIMDataType::getDataType(*pPropertyDeclaration->pDataType->pDataType);
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
		m_curProperty.setDataType(CIMDataType::getDataType(*pPropertyDeclaration->pDataType->pDataType));
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
					theErrorHandler->recoverableError(
						format("Array size (%1) doesn't match number of elements (%2)", arraySize, m_curValue.getArraySize()).c_str(),
						pPropertyDeclaration->theLineInfo);
				}
			}
			else
			{
				theErrorHandler->recoverableError(
					"Property declared as array, but value is not an array",
					pPropertyDeclaration->theLineInfo);
			}
		}
		m_curProperty.setValue(m_curValue);
	}
}
void CIMOMVisitor::VisitReferenceDeclaration( const ReferenceDeclaration *pReferenceDeclaration )
{
	m_curProperty = CIMProperty(*pReferenceDeclaration->pReferenceName->pReferenceName);
	if ( pReferenceDeclaration->pQualifier.get() != 0 )
	{
		for ( List<Qualifier *>::const_iterator i = pReferenceDeclaration->pQualifier->begin();
			 i != pReferenceDeclaration->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			m_curProperty.addQualifier(m_curQualifier);
		}
	}
	CIMDataType dt(*(pReferenceDeclaration->pObjectRef->pClassName->pClassName));
	m_curProperty.setDataType(dt);
	if ( pReferenceDeclaration->pDefaultValue.get() != 0 )
	{
		pReferenceDeclaration->pDefaultValue->Accept( this );
		m_curProperty.setValue(m_curValue);
	}
}
void CIMOMVisitor::VisitMethodDeclaration( const MethodDeclaration *pMethodDeclaration )
{
	m_curMethod = CIMMethod(*pMethodDeclaration->pMethodName->pMethodName);
	if ( pMethodDeclaration->pQualifier.get() != 0 )
	{
		for ( List<Qualifier *>::const_iterator i = pMethodDeclaration->pQualifier->begin();
			 i != pMethodDeclaration->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			m_curMethod.addQualifier(m_curQualifier);
		}
	}
	CIMDataType dt = CIMDataType::getDataType(*pMethodDeclaration->pDataType->pDataType);
	m_curMethod.setReturnType(dt);
	if ( pMethodDeclaration->pParameter.get() != 0 )
	{
		CIMParameterArray params;
		for ( List<Parameter *>::const_iterator j = pMethodDeclaration->pParameter->begin();
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
	m_curParameter = CIMParameter(*pParameterDataType->pParameterName->pParameterName);
	if ( pParameterDataType->pQualifier.get() != 0 )
	{
		CIMQualifierArray quals;
		for ( List<Qualifier *>::const_iterator i = pParameterDataType->pQualifier->begin();
			 i != pParameterDataType->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			quals.append(m_curQualifier);
		}
		m_curParameter.setQualifiers(quals);
	}
	CIMDataType dt = CIMDataType::getDataType(*pParameterDataType->pDataType->pDataType);
	if ( pParameterDataType->pArray.get() != 0 )
	{
		Int64 arraySize = 0;
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
	m_curParameter = CIMParameter(*pParameterObjectRef->pParameterName->pParameterName);
	if ( pParameterObjectRef->pQualifier.get() != 0 )
	{
		CIMQualifierArray quals;
		
		for ( List<Qualifier *>::const_iterator i = pParameterObjectRef->pQualifier->begin();
			  i != pParameterObjectRef->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			quals.append(m_curQualifier);
		}
		m_curParameter.setQualifiers(quals);
	}
	CIMDataType dt(*pParameterObjectRef->pObjectRef->pClassName->pClassName);
	if ( pParameterObjectRef->pArray.get() != 0 )
	{
		Int64 arraySize = 0;
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
	CIMValueArray values;
	if (pArrayInitializer->pConstantValue.get() != 0)
	{
		for( List<ConstantValue *>::const_iterator i = pArrayInitializer->pConstantValue->begin();
			 i != pArrayInitializer->pConstantValue->end(); ++i )
		{
			(*i)->Accept( this );
			values.push_back(m_curValue);
		}
	}
	m_curValue = convertValuesIntoValueArray(values);
}
template <class T>
CIMValue doArrayConversion( T& tempArray, const CIMValueArray& values )
{
	for (size_t i = 0; i < values.size(); ++i)
	{
		typename T::value_type element;
		values[i].get(element);
		tempArray.push_back(element);
	}
	return CIMValue(tempArray);
}
CIMValue CIMOMVisitor::convertValuesIntoValueArray( const CIMValueArray& values )
{
	if (values.size())
	{
		if (values[0].getType() == CIMDataType::BOOLEAN)
		{
			BoolArray temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::CHAR16)
		{
			Char16Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::UINT8)
		{
			UInt8Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::SINT8)
		{
			Int8Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::UINT16)
		{
			UInt16Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::SINT16)
		{
			Int16Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::UINT32)
		{
			UInt32Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::SINT32)
		{
			Int32Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::UINT64)
		{
			UInt64Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::SINT64)
		{
			Int64Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::REAL64)
		{
			Real64Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::REAL32)
		{
			Real32Array temp;
			return doArrayConversion(temp, values);
		}
		else if (values[0].getType() == CIMDataType::STRING)
		{
			StringArray temp;
			return doArrayConversion(temp, values);
		}
	}
	return CIMValue(StringArray());
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
	m_curValue = CIMValue(*pObjectHandle->pObjectHandle);
}
void CIMOMVisitor::VisitQualifierDeclaration( const QualifierDeclaration *pQualifierDeclaration )
{
	m_curQualifierType = CIMQualifierType(*pQualifierDeclaration->pQualifierName->pQualifierName);
	
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
	CIMDataType dt = CIMDataType::getDataType(*pQualifierType->pDataType->pDataType);
	if ( pQualifierType->pArray.get() != 0 )
	{
		Int64 arraySize = 0;
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
	for ( List<MetaElement *>::const_iterator i = pScope->pMetaElement->begin();
		 i != pScope->pMetaElement->end();
		 ++i )
	{
		CIMScope scope;
		/* Removed by CIM Specification 2.2 Addenda Sheet: 01
		http://www.dmtf.org/spec/release/CIM_Errata/CIM_Spec_Addenda221.htm
		Our grammar still parses this, because it's used in CIM_Schema23.mof,
		So the user will just get a warning.
		if ((*i)->pMetaElement->equalsIgnoreCase("SCHEMA"))
		{
			scope = CIMScope(CIMScope::SCHEMA);
		}
		else*/
		if ((*i)->pMetaElement->equalsIgnoreCase("CLASS"))
		{
			scope = CIMScope(CIMScope::CLASS);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("ASSOCIATION"))
		{
			scope = CIMScope(CIMScope::ASSOCIATION);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("INDICATION"))
		{
			scope = CIMScope(CIMScope::INDICATION);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("QUALIFIER"))
		{
			scope = CIMScope(CIMScope::QUALIFIER);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("PROPERTY"))
		{
			scope = CIMScope(CIMScope::PROPERTY);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("REFERENCE"))
		{
			scope = CIMScope(CIMScope::REFERENCE);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("METHOD"))
		{
			scope = CIMScope(CIMScope::METHOD);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("PARAMETER"))
		{
			scope = CIMScope(CIMScope::PARAMETER);
		}
		else if ((*i)->pMetaElement->equalsIgnoreCase("ANY"))
		{
			scope = CIMScope(CIMScope::ANY);
		}
		else
		{
			theErrorHandler->recoverableError(
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
	for ( List<Flavor *>::const_iterator i = pDefaultFlavor->pFlavor->begin();
		 i != pDefaultFlavor->pFlavor->end();
		 ++i )
	{
		if ((*i)->pFlavor->equalsIgnoreCase("ENABLEOVERRIDE"))
		{
			m_curQualifierType.addFlavor(CIMFlavor(CIMFlavor::ENABLEOVERRIDE));
		}
		else if ((*i)->pFlavor->equalsIgnoreCase("DISABLEOVERRIDE"))
		{
			m_curQualifierType.addFlavor(CIMFlavor(CIMFlavor::DISABLEOVERRIDE));
		}
		else if ((*i)->pFlavor->equalsIgnoreCase("RESTRICTED"))
		{
			m_curQualifierType.addFlavor(CIMFlavor(CIMFlavor::RESTRICTED));
		}
		else if ((*i)->pFlavor->equalsIgnoreCase("TOSUBCLASS"))
		{
			m_curQualifierType.addFlavor(CIMFlavor(CIMFlavor::TOSUBCLASS));
		}
		else if ((*i)->pFlavor->equalsIgnoreCase("TRANSLATABLE"))
		{
			m_curQualifierType.addFlavor(CIMFlavor(CIMFlavor::TRANSLATE));
		}
		else
		{
			theErrorHandler->fatalError(
				format("Internal Compiler Error. Invalid flavor: %1", *(*i)->pFlavor).c_str(),
				(*i)->theLineInfo);
		}
	}
}
void CIMOMVisitor::VisitInstanceDeclaration( const InstanceDeclaration *pInstanceDeclaration )
{
	CIMClass cc = getClass(*pInstanceDeclaration->pClassName->pClassName, pInstanceDeclaration->theLineInfo);
		//m_hdl->getClass(m_namespace,
		//*pInstanceDeclaration->pClassName->pClassName, false);
	m_curInstance = cc.newInstance();
	if ( pInstanceDeclaration->pQualifier.get() != 0 )
	{
		for ( List<Qualifier *>::const_iterator i = pInstanceDeclaration->pQualifier->begin();
			  i != pInstanceDeclaration->pQualifier->end(); ++i )
		{
			(*i)->Accept( this );
			m_curInstance.setQualifier(m_curQualifier);
		}
	}
	if ( pInstanceDeclaration->pValueInitializer.get() != 0 )
	{
		for ( List<ValueInitializer *>::const_iterator i = pInstanceDeclaration->pValueInitializer->begin();
			  i != pInstanceDeclaration->pValueInitializer->end(); ++i )
		{
			(*i)->Accept( this );
			CIMProperty tempProp = m_curInstance.getProperty(m_curProperty.getName());
			if (tempProp)
			{
				CIMQualifierArray newQuals = m_curProperty.getQualifiers();
				for (size_t i = 0; i < newQuals.size(); ++i)
				{
					tempProp.setQualifier(newQuals[i]);
				}
				CIMValue castValue = CIMValueCast::castValueToDataType(
						m_curProperty.getValue(),
						CIMDataType(tempProp.getDataType()));
				if (castValue.getType() == CIMDataType::REFERENCE)
				{
					CIMObjectPath cop(CIMNULL);
					castValue.get(cop);
					if (cop)
					{
						// If the object path doesn't have a : character, then we need to set the namespace on it.
						if (m_curProperty.getValue().toString().indexOf(':') == String::npos)
						{
							cop.setNameSpace(m_namespace);
							castValue = CIMValue(cop);
						}
					}
				}
				if (!castValue)
				{
					theErrorHandler->recoverableError(
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
		CIMObjectPath cop(m_namespace, m_curInstance);
		
		m_aliasMap[*(pInstanceDeclaration->pAlias->pAliasIdentifier->pAliasIdentifier)] =
			cop.modelPath();
	}
	CIMOMcreateInstance(pInstanceDeclaration->theLineInfo);
}
void CIMOMVisitor::VisitValueInitializer( const ValueInitializer *pValueInitializer )
{
	m_curProperty = CIMProperty(*pValueInitializer->pValueInitializer);
	if ( pValueInitializer->pQualifier.get() != 0 )
	{
		for ( List<Qualifier *>::const_iterator i = pValueInitializer->pQualifier->begin();
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
	Int64 val = pIntegerValueBinaryValue->pBinaryValue->toInt64(2);
	m_curValue = CIMValue(val);
}
void CIMOMVisitor::VisitIntegerValueOctalValue( const IntegerValueOctalValue *pIntegerValueOctalValue )
{
	Int64 val = pIntegerValueOctalValue->pOctalValue->toInt64(8);
	m_curValue = CIMValue(val);
}
void CIMOMVisitor::VisitIntegerValueDecimalValue( const IntegerValueDecimalValue *pIntegerValueDecimalValue )
{
	Int64 val = pIntegerValueDecimalValue->pDecimalValue->toInt64(10);
	m_curValue = CIMValue(val);
}
void CIMOMVisitor::VisitIntegerValueHexValue( const IntegerValueHexValue *pIntegerValueHexValue )
{
	Int64 val = pIntegerValueHexValue->pHexValue->toInt64(16);
	m_curValue = CIMValue(val);
}
void CIMOMVisitor::VisitConstantValueIntegerValue( const ConstantValueIntegerValue *pConstantValueIntegerValue )
{
	pConstantValueIntegerValue->pIntegerValue->Accept( this );
}
void CIMOMVisitor::VisitConstantValueFloatValue( const ConstantValueFloatValue *pConstantValueFloatValue )
{
	Real64 val = pConstantValueFloatValue->pFloatValue->toReal64();
	m_curValue = CIMValue(val);
}
void CIMOMVisitor::VisitConstantValueStringValue( const ConstantValueStringValue *pConstantValueStringValue )
{
	m_curValue = CIMValue(Compiler::fixParsedString(*pConstantValueStringValue->pStringValue));
}
void CIMOMVisitor::VisitConstantValueCharValue( const ConstantValueCharValue *pConstantValueCharValue )
{
	char c = pConstantValueCharValue->pCharValue->charAt(1);
	m_curValue = CIMValue(c);
}
void CIMOMVisitor::VisitConstantValueBooleanValue( const ConstantValueBooleanValue *pConstantValueBooleanValue )
{
	Bool b;
	if (pConstantValueBooleanValue->pBooleanValue->equalsIgnoreCase("TRUE"))
	{
		b = true;
	}
	else
	{
		b = false;
	}
	m_curValue = CIMValue(b);
}
void CIMOMVisitor::VisitConstantValueNullValue( const ConstantValueNullValue * )
{
	m_curValue.setNull();
}
CIMDataType CIMOMVisitor::getQualifierDataType(const String& qualName, const lineInfo& li)
{
	return getQualifierType(qualName, li).getDataType();
}
CIMQualifierType CIMOMVisitor::getQualifierType(const String& qualName, const lineInfo& li)
{
	String lcqualName = qualName;
	lcqualName.toLowerCase();
	CIMQualifierType qt = m_dataTypeCache.getFromCache(lcqualName);
	if (!qt)
	{
		qt = CIMOMgetQualifierType(qualName, li);
		m_dataTypeCache.addToCache(qt, lcqualName);
	}
	return qt;
}
CIMClass CIMOMVisitor::getClass(const String& className, const lineInfo& li)
{
	String lcclassName = className;
	lcclassName.toLowerCase();
	CIMClass c = m_classCache.getFromCache(lcclassName);
	if (!c)
	{
		c = CIMOMgetClass(className, li);
		m_classCache.addToCache(c, lcclassName);
	}
	return c;
}
void CIMOMVisitor::CIMOMcreateClass(const lineInfo& li)
{
	try
	{
		theErrorHandler->progressMessage(format("Processing class: %1", m_curClass.getName()).c_str(), li);
		try
		{
			m_hdl->createClass(m_namespace, m_curClass);
		}
		catch (CIMException& e)
		{
			if (e.getErrNo() == CIMException::INVALID_NAMESPACE && m_opts.m_createNamespaces)
			{
				CIMOMcreateNamespace(li);
				m_hdl->createClass(m_namespace, m_curClass);
			}
			else
			{
				throw;
			}
		}
		theErrorHandler->progressMessage(format("Created class: %1", m_curClass.getName()).c_str(), li);
		// Note we won't add the class to the cache, since mof usually is just creating classes, it'll be mostly a waste of time.  getClass will put classes in the cache,
		// in the case that there are lots of instances, each class will only have to be fetched once.
	}
	catch (const CIMException& ce)
	{
		if (ce.getErrNo() == CIMException::ALREADY_EXISTS)
		{
			try
			{
				m_hdl->modifyClass(m_namespace, m_curClass);
				theErrorHandler->progressMessage(format("Updated class: %1", m_curClass.getName()).c_str(), li);
			}
			catch (const CIMException& ce)
			{
				theErrorHandler->recoverableError(format("Error: %1", ce.getMessage()).c_str(), li);
			}
		}
		else
		{
			theErrorHandler->fatalError(format("Error: %1", ce.getMessage()).c_str(), li);
		}
	}
}
void CIMOMVisitor::CIMOMsetQualifierType(const lineInfo& li)
{
	try
	{
		theErrorHandler->progressMessage(format("Setting QualifierType: %1", m_curQualifierType.getName()).c_str(), li);
		try
		{
			m_hdl->setQualifierType(m_namespace, m_curQualifierType);
		}
		catch (CIMException& e)
		{
			if (e.getErrNo() == CIMException::INVALID_NAMESPACE && m_opts.m_createNamespaces)
			{
				CIMOMcreateNamespace(li);
				m_hdl->setQualifierType(m_namespace, m_curQualifierType);
			}
			else
			{
				throw;
			}
		}
		// save it in the cache
		String lcqualName = m_curQualifierType.getName();
		lcqualName.toLowerCase();
		m_dataTypeCache.addToCache(m_curQualifierType, lcqualName);
	}
	catch (const CIMException& ce)
	{
		theErrorHandler->fatalError(format("Error: %1", ce.getMessage()).c_str(), li);
	}
}
void CIMOMVisitor::CIMOMcreateInstance(const lineInfo& li)
{
	CIMObjectPath cop(m_namespace, m_curInstance);
	theErrorHandler->progressMessage(format("Processing Instance: %1", cop.toString()).c_str(), li);
	try
	{
		try
		{
			m_hdl->createInstance(m_namespace, m_curInstance);
		}
		catch (CIMException& e)
		{
			if (e.getErrNo() == CIMException::INVALID_NAMESPACE && m_opts.m_createNamespaces)
			{
				CIMOMcreateNamespace(li);
				m_hdl->createInstance(m_namespace, m_curInstance);
			}
			else
			{
				throw;
			}
		}
		theErrorHandler->progressMessage(format("Created Instance: %1", cop.toString()).c_str(), li);
	}
	catch (const CIMException& ce)
	{
		if (ce.getErrNo() == CIMException::ALREADY_EXISTS)
		{
			try
			{
				m_hdl->modifyInstance(m_namespace, m_curInstance);
				theErrorHandler->progressMessage(format("Updated Instance: %1", cop.toString()).c_str(), li);
			}
			catch (const CIMException& ce)
			{
				theErrorHandler->fatalError(format("Error: %1", ce.getMessage()).c_str(), li);
			}
		}
		else
		{
			theErrorHandler->fatalError(format("Error: %1", ce.getMessage()).c_str(), li);
		}
	}
}
CIMQualifierType CIMOMVisitor::CIMOMgetQualifierType(const String& qualName, const lineInfo& li)
{
	try
	{
		try
		{
			return m_hdl->getQualifierType(m_namespace, qualName);
		}
		catch (CIMException& e)
		{
			if (e.getErrNo() == CIMException::INVALID_NAMESPACE && m_opts.m_createNamespaces)
			{
				CIMOMcreateNamespace(li);
				return m_hdl->getQualifierType(m_namespace, qualName);
			}
			else
			{
				throw;
			}
		}
	}
	catch (const CIMException& ce)
	{
		theErrorHandler->fatalError(format("Error: %1", ce.getMessage()).c_str(), li);
	}
	return CIMQualifierType();
}
CIMClass CIMOMVisitor::CIMOMgetClass(const String& className, const lineInfo& li)
{
	try
	{
		try
		{
			return m_hdl->getClass(m_namespace, className, E_NOT_LOCAL_ONLY);
		}
		catch (CIMException& e)
		{
			if (e.getErrNo() == CIMException::INVALID_NAMESPACE && m_opts.m_createNamespaces)
			{
				CIMOMcreateNamespace(li);
				return m_hdl->getClass(m_namespace, className, E_NOT_LOCAL_ONLY);
			}
			else
			{
				throw;
			}
		}
	}
	catch (const CIMException& ce)
	{
		theErrorHandler->fatalError(format("Error: %1", ce.getMessage()).c_str(), li);
	}
	return CIMClass();
}

void CIMOMVisitor::CIMOMcreateNamespace(const lineInfo& li)
{
	// don't bother catching exceptions here.  This function is only called if
	// the namespace needs to exist.  If we can't create the namespace, that's
	// a fatal error.
	theErrorHandler->progressMessage(format("Creating namespace: %1", m_namespace).c_str(), li);
	if (m_rephdl)
	{
		m_rephdl->createNameSpace(m_namespace);
	}
	else
	{
		try
		{
			CIMNameSpaceUtils::createCIM_Namespace(*m_hdl, m_namespace);
		}
		catch (const CIMException& e)
		{
			// server doesn't support CIM_Namespace, try __Namespace
			CIMNameSpaceUtils::create__Namespace(*m_hdl, m_namespace);
		}

	}
	theErrorHandler->progressMessage(format("Created namespace: %1", m_namespace).c_str(), li);
}

} // end namespace MOF
} // end namespace OpenWBEM

