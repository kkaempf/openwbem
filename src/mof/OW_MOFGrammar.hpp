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

#ifndef OW_MOF_GRAMMAR_HPP_INCLUDE_GUARD_
#define OW_MOF_GRAMMAR_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_List.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_MOFVisitor.hpp"
#include "OW_MOFLineInfo.hpp"

namespace OpenWBEM
{

namespace MOF
{

class OW_MOF_API Initializer
{
public:
	virtual ~Initializer() {}
	virtual void Accept( Visitor * ) const = 0;
};
class OW_MOF_API Flavor
{
public:
	Flavor( const String *pNewFlavor, const lineInfo& li )
	: pFlavor(pNewFlavor)
	, theLineInfo(li)
	{}
	virtual ~Flavor(){}
	
	void Accept( Visitor *pV ) const { pV->VisitFlavor( this ); }
	AutoPtr< const String > pFlavor;
	lineInfo theLineInfo;
};
class OW_MOF_API QualifierParameter
{
public:
	virtual ~QualifierParameter() {}
	virtual void Accept( Visitor * ) const = 0;
};
class OW_MOF_API ConstantValue
{
public:
	virtual ~ConstantValue() {}
	virtual void Accept( Visitor * ) const = 0;
};
class OW_MOF_API ArrayInitializer
{
public:
	ArrayInitializer( List< ConstantValue * >* pNewConstantValue )
		: pConstantValue(pNewConstantValue)
	{}
	virtual ~ArrayInitializer()
	{
		while (pConstantValue.get() && !pConstantValue->empty())
		{
			delete pConstantValue->front();
		 pConstantValue->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitArrayInitializer( this ); }
	AutoPtr< List< ConstantValue * > > pConstantValue;
};
class OW_MOF_API QualifierParameterArrayInitializer : public QualifierParameter
{
public:
	QualifierParameterArrayInitializer(
		const ArrayInitializer* pNewArrayInitializer,
		const lineInfo& li )
		: pArrayInitializer(pNewArrayInitializer)
		, theLineInfo(li)
	{}
	virtual ~QualifierParameterArrayInitializer(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitQualifierParameterArrayInitializer( this );
	}
	
	AutoPtr< const ArrayInitializer > pArrayInitializer;
	lineInfo theLineInfo;
};
class OW_MOF_API QualifierParameterConstantValue : public QualifierParameter
{
public:
	QualifierParameterConstantValue( const ConstantValue* pNewConstantValue,
		const lineInfo& li )
		: pConstantValue(pNewConstantValue)
		, theLineInfo(li)
	{}
	virtual ~QualifierParameterConstantValue(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitQualifierParameterConstantValue( this );
	}
	
	AutoPtr< const ConstantValue > pConstantValue;
	lineInfo theLineInfo;
};
class OW_MOF_API QualifierName
{
public:
	QualifierName( const String* pNewQualifierName )
		: pQualifierName(pNewQualifierName)
	{}
	virtual ~QualifierName() {}
	void Accept( Visitor *pV ) const { pV->VisitQualifierName( this ); }
	AutoPtr< const String > pQualifierName;
};
class OW_MOF_API Qualifier
{
public:
	Qualifier( const QualifierName* pNewQualifierName,
		const QualifierParameter* pNewQualifierParameter,
		List< Flavor * >* pNewFlavor,
		const lineInfo& li )
		: pQualifierName(pNewQualifierName)
		, pQualifierParameter(pNewQualifierParameter)
		, pFlavor(pNewFlavor)
		, theLineInfo(li)
	{}
	virtual ~Qualifier()
	{
		while (pFlavor.get() && !pFlavor->empty())
		{
			delete pFlavor->front();
		 pFlavor->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitQualifier( this ); }
	
	AutoPtr< const QualifierName > pQualifierName;
	AutoPtr< const QualifierParameter > pQualifierParameter;
	AutoPtr< List< Flavor * > > pFlavor;
	lineInfo theLineInfo;
};
class OW_MOF_API DefaultValue
{
public:
	DefaultValue( const Initializer* pNewInitializer)
		: pInitializer(pNewInitializer)
	{}
	virtual ~DefaultValue(){}
	void Accept( Visitor *pV ) const { pV->VisitDefaultValue( this ); }
	AutoPtr< const Initializer > pInitializer;
};
class OW_MOF_API ValueInitializer
{
public:
	ValueInitializer( List< Qualifier * >* pNewQualifier,
		const String* pNewValueInitializer,
		const DefaultValue* pNewDefaultValue )
		: pQualifier(pNewQualifier)
		, pValueInitializer(pNewValueInitializer)
		, pDefaultValue(pNewDefaultValue)
	{}
	virtual ~ValueInitializer()
	{
		while (pQualifier.get() && !pQualifier->empty())
		{
			delete pQualifier->front();
			pQualifier->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitValueInitializer( this ); }
	AutoPtr< List< Qualifier * > > pQualifier;
	AutoPtr< const String > pValueInitializer;
	AutoPtr< const DefaultValue > pDefaultValue;
};
class OW_MOF_API PropertyName
{
public:
	PropertyName( const String* pNewPropertyName )
		: pPropertyName(pNewPropertyName)
	{}
	virtual ~PropertyName() {}
	void Accept( Visitor *pV ) const { pV->VisitPropertyName( this ); }
	AutoPtr< const String > pPropertyName;
};
class OW_MOF_API ClassName
{
public:
	ClassName( const String* pNewClassName )
		: pClassName(pNewClassName)
	{}
	virtual ~ClassName(){}
	void Accept( Visitor *pV ) const { pV->VisitClassName( this ); }
	AutoPtr< const String > pClassName;
};
class OW_MOF_API AliasIdentifier
{
public:
	AliasIdentifier( const String* pNewAliasIdentifier, lineInfo li )
		: pAliasIdentifier(pNewAliasIdentifier)
		, theLineInfo(li)
	{}
	virtual ~AliasIdentifier(){}
	void Accept( Visitor *pV ) const { pV->VisitAliasIdentifier( this ); }
	AutoPtr< const String > pAliasIdentifier;
	lineInfo theLineInfo;
};
class OW_MOF_API Alias
{
public:
	Alias( const AliasIdentifier* pNewAliasIdentifier )
		: pAliasIdentifier(pNewAliasIdentifier)
	{}
	virtual ~Alias(){}
	void Accept( Visitor *pV ) const { pV->VisitAlias( this ); }
	AutoPtr< const AliasIdentifier > pAliasIdentifier;
};
class OW_MOF_API InstanceDeclaration
{
public:
	InstanceDeclaration(
		List< Qualifier * >* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		List< ValueInitializer * >* pNewValueInitializer,
		const lineInfo& li)
		: pQualifier(pNewQualifier)
		, pClassName(pNewClassName)
		, pAlias(pNewAlias)
		, pValueInitializer(pNewValueInitializer)
		, theLineInfo(li)
	{}
	virtual ~InstanceDeclaration()
	{
		while (pQualifier.get() && !pQualifier->empty())
		{
			delete pQualifier->front();
			pQualifier->pop_front();
		}
		while (pValueInitializer.get() && !pValueInitializer->empty())
		{
			delete pValueInitializer->front();
			pValueInitializer->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitInstanceDeclaration( this ); }
	
	AutoPtr< List< Qualifier * > > pQualifier;
	AutoPtr< const ClassName > pClassName;
	AutoPtr< const Alias > pAlias;
	AutoPtr< List< ValueInitializer * > > pValueInitializer;
	 lineInfo theLineInfo;
};
class OW_MOF_API DefaultFlavor
{
public:
	DefaultFlavor( List< Flavor * >* pNewFlavor )
		: pFlavor(pNewFlavor)
	{}
	virtual ~DefaultFlavor()
	{
		while (pFlavor.get() && !pFlavor->empty())
		{
			delete pFlavor->front();
			pFlavor->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitDefaultFlavor( this ); }
	
	AutoPtr< List< Flavor * > > pFlavor;
};
class OW_MOF_API MetaElement
{
public:
	MetaElement( const String* pNewMetaElement, const lineInfo& li )
		: pMetaElement(pNewMetaElement)
		, theLineInfo(li)
	{}
	virtual ~MetaElement(){}
	void Accept( Visitor *pV ) const { pV->VisitMetaElement( this ); }
	AutoPtr< const String > pMetaElement;
	lineInfo theLineInfo;
};
class OW_MOF_API Scope
{
public:
	Scope( List< MetaElement * >* pNewMetaElement )
		: pMetaElement(pNewMetaElement)
	{}
	virtual ~Scope()
	{
		while (pMetaElement.get() && !pMetaElement->empty())
		{
			delete pMetaElement->front();
			pMetaElement->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitScope( this ); }
	
	AutoPtr< List< MetaElement * > > pMetaElement;
};
class OW_MOF_API DataType
{
public:
	DataType( const String* pNewDataType )
		: pDataType(pNewDataType)
	{}
	virtual ~DataType(){}
	void Accept( Visitor *pV ) const { pV->VisitDataType( this ); }
	AutoPtr< const String > pDataType;
};
class OW_MOF_API IntegerValue
{
public:
	virtual ~IntegerValue() {}
	virtual void Accept( Visitor * ) const = 0;
};
class OW_MOF_API Array
{
public:
	Array( const IntegerValue* pNewArray )
		: pArray(pNewArray)
	{}
	virtual ~Array(){}
	void Accept( Visitor *pV ) const { pV->VisitArray( this ); }
	AutoPtr< const IntegerValue > pArray;
};
class OW_MOF_API QualifierType
{
public:
	QualifierType( const DataType* pNewDataType,
		const Array* pNewArray,
		const DefaultValue* pNewDefaultValue )
		: pDataType(pNewDataType)
		, pArray(pNewArray)
		, pDefaultValue(pNewDefaultValue)
	{}
	virtual ~QualifierType(){}
	void Accept( Visitor *pV ) const { pV->VisitQualifierType( this ); }
	
	AutoPtr< const DataType > pDataType;
	AutoPtr< const Array > pArray;
	AutoPtr< const DefaultValue > pDefaultValue;
};
	
class OW_MOF_API QualifierDeclaration
{
public:
	QualifierDeclaration(
		const QualifierName* pNewQualifierName,
		const QualifierType* pNewQualifierType,
		const Scope* pNewScope,
		const DefaultFlavor* pNewDefaultFlavor,
		const lineInfo& li)
		: pQualifierName(pNewQualifierName)
		, pQualifierType(pNewQualifierType)
		, pScope(pNewScope)
		, pDefaultFlavor(pNewDefaultFlavor)
		, theLineInfo(li)
	{}
	virtual ~QualifierDeclaration(){}
	void Accept( Visitor *pV ) const { pV->VisitQualifierDeclaration( this ); }
	AutoPtr< const QualifierName > pQualifierName;
	AutoPtr< const QualifierType > pQualifierType;
	AutoPtr< const Scope > pScope;
	AutoPtr< const DefaultFlavor > pDefaultFlavor;
	lineInfo theLineInfo;
};
class OW_MOF_API ReferenceName
{
public:
	ReferenceName( const String* pNewReferenceName )
		: pReferenceName(pNewReferenceName)
	{}
	virtual ~ReferenceName(){}
	void Accept( Visitor *pV ) const { pV->VisitReferenceName( this ); }
	AutoPtr< const String > pReferenceName;
};
class OW_MOF_API IntegerValueBinaryValue : public IntegerValue
{
public:
	IntegerValueBinaryValue( const String* pNewBinaryValue )
		: pBinaryValue(pNewBinaryValue)
	{}
	virtual ~IntegerValueBinaryValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueBinaryValue( this );
	}
	AutoPtr< const String > pBinaryValue;
};
class OW_MOF_API IntegerValueOctalValue : public IntegerValue
{
public:
	IntegerValueOctalValue( const String* pNewOctalValue )
		: pOctalValue(pNewOctalValue)
	{}
	virtual ~IntegerValueOctalValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueOctalValue( this );
	}
	AutoPtr< const String > pOctalValue;
};
class OW_MOF_API IntegerValueDecimalValue : public IntegerValue
{
public:
	IntegerValueDecimalValue( const String* pNewDecimalValue )
		: pDecimalValue(pNewDecimalValue)
	{}
	virtual ~IntegerValueDecimalValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueDecimalValue( this );
	}
	AutoPtr< const String > pDecimalValue;
};
class OW_MOF_API IntegerValueHexValue : public IntegerValue
{
public:
	IntegerValueHexValue( const String* pNewHexValue )
		: pHexValue(pNewHexValue)
	{}
	virtual ~IntegerValueHexValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueHexValue( this );
	}
	AutoPtr< const String > pHexValue;
};
class OW_MOF_API ConstantValueIntegerValue : public ConstantValue
{
public:
	ConstantValueIntegerValue( const IntegerValue* pNewIntegerValue )
		: pIntegerValue(pNewIntegerValue)
	{}
	virtual ~ConstantValueIntegerValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueIntegerValue( this );
	}
	AutoPtr< const IntegerValue > pIntegerValue;
};
class OW_MOF_API ConstantValueFloatValue : public ConstantValue
{
public:
	ConstantValueFloatValue( const String* pNewFloatValue )
		: pFloatValue(pNewFloatValue)
	{}
	virtual ~ConstantValueFloatValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueFloatValue( this );
	}
	AutoPtr< const String > pFloatValue;
};
class OW_MOF_API ConstantValueCharValue : public ConstantValue
{
public:
	ConstantValueCharValue( const String* pNewCharValue )
		: pCharValue(pNewCharValue)
	{}
	virtual ~ConstantValueCharValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueCharValue( this );
	}
	AutoPtr< const String > pCharValue;
};
class OW_MOF_API ConstantValueStringValue : public ConstantValue
{
public:
	ConstantValueStringValue( const String* pNewStringValue )
		: pStringValue(pNewStringValue)
	{}
	virtual ~ConstantValueStringValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueStringValue( this );
	}
	AutoPtr< const String > pStringValue;
};
class OW_MOF_API ConstantValueBooleanValue : public ConstantValue
{
public:
	ConstantValueBooleanValue( const String* pNewBooleanValue )
		: pBooleanValue(pNewBooleanValue)
	{}
	virtual ~ConstantValueBooleanValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueBooleanValue( this );
	}
	AutoPtr< const String > pBooleanValue;
};
class OW_MOF_API ConstantValueNullValue : public ConstantValue
{
public:
	ConstantValueNullValue( const String* pNewNullValue )
		: pNullValue(pNewNullValue)
	{}
	virtual ~ConstantValueNullValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueNullValue( this );
	}
	AutoPtr< const String > pNullValue;
};
class OW_MOF_API ObjectHandle
{
public:
	ObjectHandle( const String* pNewObjectHandle )
		: pObjectHandle(pNewObjectHandle)
	{}
	virtual ~ObjectHandle(){}
	void Accept( Visitor *pV ) const { pV->VisitObjectHandle( this ); }
	AutoPtr< const String > pObjectHandle;
};
class OW_MOF_API ReferenceInitializer
{
public:
	virtual ~ReferenceInitializer() {}
	virtual void Accept( Visitor * ) const = 0;
};
class OW_MOF_API ReferenceInitializerAliasIdentifier : public ReferenceInitializer
{
public:
	ReferenceInitializerAliasIdentifier(
		const AliasIdentifier * pNewAliasIdentifier)
		: pAliasIdentifier(pNewAliasIdentifier)
	{}
	virtual ~ReferenceInitializerAliasIdentifier(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitReferenceInitializerAliasIdentifier( this );
	}
	
	AutoPtr< const AliasIdentifier > pAliasIdentifier;
};
class OW_MOF_API ReferenceInitializerObjectHandle : public ReferenceInitializer
{
public:
	ReferenceInitializerObjectHandle( const ObjectHandle* pNewObjectHandle )
		: pObjectHandle(pNewObjectHandle)
	{}
	virtual ~ReferenceInitializerObjectHandle(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitReferenceInitializerObjectHandle( this );
	}
	AutoPtr< const ObjectHandle > pObjectHandle;
};
class OW_MOF_API InitializerReferenceInitializer : public Initializer
{
public:
	InitializerReferenceInitializer(
		const ReferenceInitializer* pNewReferenceInitializer)
		: pReferenceInitializer(pNewReferenceInitializer)
	{}
	virtual ~InitializerReferenceInitializer(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitInitializerReferenceInitializer( this );
	}
	AutoPtr< const ReferenceInitializer > pReferenceInitializer;
};		
class OW_MOF_API InitializerArrayInitializer : public Initializer
{
public:
	InitializerArrayInitializer( const ArrayInitializer* pNewArrayInitializer)
		: pArrayInitializer(pNewArrayInitializer)
	{}
	virtual ~InitializerArrayInitializer(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitInitializerArrayInitializer( this );
	}
	AutoPtr< const ArrayInitializer > pArrayInitializer;
};		
class OW_MOF_API InitializerConstantValue : public Initializer
{
public:
	InitializerConstantValue( const ConstantValue *pNewConstantValue )
		: pConstantValue(pNewConstantValue)
	{}
	virtual ~InitializerConstantValue(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitInitializerConstantValue( this );
	}
	AutoPtr< const ConstantValue > pConstantValue;
};		
class OW_MOF_API ParameterName
{
public:
	ParameterName( const String* pNewParameterName )
		: pParameterName(pNewParameterName)
	{}
	virtual ~ParameterName(){}
	void Accept( Visitor *pV ) const { pV->VisitParameterName( this ); }
	AutoPtr< const String > pParameterName;
};
class OW_MOF_API Parameter
{
public:
	virtual ~Parameter() {}
	virtual void Accept( Visitor * ) const = 0;
};
class OW_MOF_API ObjectRef
{
public:
	ObjectRef( const ClassName* pNewClassName )
		: pClassName(pNewClassName)
	{}
	virtual ~ObjectRef(){}
	void Accept( Visitor *pV ) const { pV->VisitObjectRef( this ); }
	AutoPtr< const ClassName > pClassName;
};
class OW_MOF_API ParameterObjectRef : public Parameter
{
public:
	ParameterObjectRef(
		List< Qualifier * >* pNewQualifier,
		const ObjectRef* pNewObjectRef,
		const ParameterName* pNewParameterName,
		const Array* pNewArray )
		: pQualifier(pNewQualifier)
		, pObjectRef(pNewObjectRef)
		, pParameterName(pNewParameterName)
		, pArray(pNewArray)
	{}
	virtual ~ParameterObjectRef()
	{
		while (pQualifier.get() && !pQualifier->empty())
		{
			delete pQualifier->front();
			pQualifier->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitParameterObjectRef( this ); }
	AutoPtr< List< Qualifier * > > pQualifier;
	AutoPtr< const ObjectRef > pObjectRef;
	AutoPtr< const ParameterName > pParameterName;
	AutoPtr< const Array > pArray;	
};
class OW_MOF_API ParameterDataType : public Parameter
{
public:
	ParameterDataType(
		List< Qualifier * >* pNewQualifier,
		const DataType* pNewDataType,
		const ParameterName* pNewParameterName,
		const Array* pNewArray )
		: pQualifier(pNewQualifier)
		, pDataType(pNewDataType)
		, pParameterName(pNewParameterName)
		, pArray(pNewArray)
	{}
	virtual ~ParameterDataType()
	{
		while (pQualifier.get() && !pQualifier->empty())
		{
			delete pQualifier->front();
			pQualifier->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitParameterDataType( this ); }
	AutoPtr< List< Qualifier * > > pQualifier;
	AutoPtr< const DataType > pDataType;
	AutoPtr< const ParameterName > pParameterName;
	AutoPtr< const Array > pArray;	
};
class OW_MOF_API MethodName
{
public:
	MethodName( const String* pNewMethodName )
		: pMethodName(pNewMethodName)
	{}
	virtual ~MethodName(){}
	void Accept( Visitor *pV ) const { pV->VisitMethodName( this ); }
	AutoPtr< const String > pMethodName;
};
class OW_MOF_API MethodDeclaration
{
public:
	MethodDeclaration(
		List< Qualifier * >* pNewQualifier,
		const DataType* pNewDataType,
		const MethodName* pNewMethodName,
		List< Parameter * >* pNewParameter )
		: pQualifier(pNewQualifier)
		, pDataType(pNewDataType)
		, pMethodName(pNewMethodName)
		, pParameter(pNewParameter)
	{}
	virtual ~MethodDeclaration()
	{
		while (pQualifier.get() && !pQualifier->empty())
		{
			delete pQualifier->front();
			pQualifier->pop_front();
		}
		while (pParameter.get() && !pParameter->empty())
		{
			delete pParameter->front();
			pParameter->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitMethodDeclaration( this ); }
	
	AutoPtr< List< Qualifier * > > pQualifier;
	AutoPtr< const DataType > pDataType;
	AutoPtr< const MethodName > pMethodName;
	AutoPtr< List< Parameter * > > pParameter;
};
class OW_MOF_API ReferenceDeclaration
{
public:
	ReferenceDeclaration(
		List< Qualifier * >* pNewQualifier,
		const ObjectRef* pNewObjectRef,
		const ReferenceName* pNewReferenceName,
		const DefaultValue* pNewDefaultValue )
		: pQualifier(pNewQualifier)
		, pObjectRef(pNewObjectRef)
		, pReferenceName(pNewReferenceName)
		, pDefaultValue(pNewDefaultValue)
	{}
	virtual ~ReferenceDeclaration()
	{
		while (pQualifier.get() && !pQualifier->empty())
		{
			delete pQualifier->front();
			pQualifier->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitReferenceDeclaration( this ); }
	
	AutoPtr< List< Qualifier * > > pQualifier;
	AutoPtr< const ObjectRef > pObjectRef;
	AutoPtr< const ReferenceName > pReferenceName;
	AutoPtr< const DefaultValue > pDefaultValue;
};
class OW_MOF_API PropertyDeclaration
{
public:
	PropertyDeclaration(
		List< Qualifier * >* pNewQualifier,
		const DataType* pNewDataType,
		const PropertyName* pNewPropertyName,
		const Array* pNewArray,
		const DefaultValue* pNewDefaultValue,
		const lineInfo& li)
		: pQualifier(pNewQualifier)
		, pDataType(pNewDataType)
		, pPropertyName(pNewPropertyName)
		, pArray(pNewArray)
		, pDefaultValue(pNewDefaultValue)
		, theLineInfo(li)
	{}
	virtual ~PropertyDeclaration()
	{
		while (pQualifier.get() && !pQualifier->empty())
		{
			delete pQualifier->front();
			pQualifier->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitPropertyDeclaration( this ); }
	AutoPtr< List< Qualifier * > > pQualifier;
	AutoPtr< const DataType > pDataType;
	AutoPtr< const PropertyName > pPropertyName;
	AutoPtr< const Array > pArray;
	AutoPtr< const DefaultValue > pDefaultValue;
	lineInfo theLineInfo;
};
class OW_MOF_API AssociationFeature
{
public:
	virtual ~AssociationFeature() {}
	virtual void Accept( Visitor * ) const = 0;
};
class OW_MOF_API ClassFeature
{
public:
	virtual ~ClassFeature() {}
	virtual void Accept( Visitor * ) const = 0;
};
class OW_MOF_API AssociationFeatureClassFeature : public AssociationFeature
{
public:
	AssociationFeatureClassFeature( const ClassFeature* pNewClassFeature )
		: pClassFeature(pNewClassFeature)
	{}
	virtual ~AssociationFeatureClassFeature(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitAssociationFeatureClassFeature( this );
	}
	
	AutoPtr< const ClassFeature > pClassFeature;
};
class OW_MOF_API ClassFeatureMethodDeclaration : public ClassFeature
{
public:
	ClassFeatureMethodDeclaration(
		const MethodDeclaration* pNewMethodDeclaration )
		: pMethodDeclaration(pNewMethodDeclaration)
	{}
	virtual ~ClassFeatureMethodDeclaration(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitClassFeatureMethodDeclaration( this );
	}
	
	AutoPtr< const MethodDeclaration > pMethodDeclaration;
};
class OW_MOF_API ClassFeaturePropertyDeclaration : public ClassFeature
{
public:
	ClassFeaturePropertyDeclaration(
		const PropertyDeclaration* pNewPropertyDeclaration )
		: pPropertyDeclaration(pNewPropertyDeclaration)
	{}
	virtual ~ClassFeaturePropertyDeclaration(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitClassFeaturePropertyDeclaration( this );
	}
	
	AutoPtr< const PropertyDeclaration > pPropertyDeclaration;
};
/* Note: This should be in AssociationFeature, but I found some MOF files
	that use this in ClassFeature */
class OW_MOF_API ClassFeatureReferenceDeclaration : public ClassFeature
{
public:
	ClassFeatureReferenceDeclaration(
		const ReferenceDeclaration* pNewReferenceDeclaration )
		: pReferenceDeclaration(pNewReferenceDeclaration)
	{}
	virtual ~ClassFeatureReferenceDeclaration(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitClassFeatureReferenceDeclaration( this );
	}
	
	AutoPtr< const ReferenceDeclaration > pReferenceDeclaration;
};
class OW_MOF_API SuperClass
{
public:
	SuperClass( const ClassName* pNewClassName )
		: pClassName(pNewClassName)
	{}
	virtual ~SuperClass(){}
	void Accept( Visitor *pV ) const { pV->VisitSuperClass( this ); }
	
	AutoPtr< const ClassName > pClassName;
};
class OW_MOF_API IndicDeclaration
{
public:
	IndicDeclaration(
		List< Qualifier * >* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		const SuperClass* pNewSuperClass,
		List< ClassFeature * >* pNewClassFeature,
		const lineInfo& li)
		: pQualifier(pNewQualifier)
		, pClassName(pNewClassName)
		, pAlias(pNewAlias)
		, pSuperClass(pNewSuperClass)
		, pClassFeature(pNewClassFeature)
		, theLineInfo(li)
	{}
	virtual ~IndicDeclaration()
	{
		while (pQualifier.get() && !pQualifier->empty())
		{
			delete pQualifier->front();
			pQualifier->pop_front();
		}
		while (pClassFeature.get() && !pClassFeature->empty())
		{
			delete pClassFeature->front();
			pClassFeature->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitIndicDeclaration( this ); }
	
	AutoPtr< List< Qualifier * > > pQualifier;
	AutoPtr< const ClassName > pClassName;
	AutoPtr< const Alias > pAlias;
	AutoPtr< const SuperClass > pSuperClass;
	AutoPtr< List< ClassFeature * > > pClassFeature;
	lineInfo theLineInfo;
};
class OW_MOF_API AssocDeclaration
{
public:
	AssocDeclaration(
		List< Qualifier * >* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		const SuperClass* pNewSuperClass,
		List< AssociationFeature * >* pNewAssociationFeature,
		const lineInfo& li )
		: pQualifier(pNewQualifier)
		, pClassName(pNewClassName)
		, pAlias(pNewAlias)
		, pSuperClass(pNewSuperClass)
		, pAssociationFeature(pNewAssociationFeature)
		, theLineInfo(li)
	{}
	virtual ~AssocDeclaration()
	{
		while (pQualifier.get() && !pQualifier->empty())
		{
			delete pQualifier->front();
			pQualifier->pop_front();
		}
		while (pAssociationFeature.get() && !pAssociationFeature->empty())
		{
			delete pAssociationFeature->front();
			pAssociationFeature->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitAssocDeclaration( this ); }
	
	AutoPtr< List< Qualifier * > > pQualifier;
	AutoPtr< const ClassName > pClassName;
	AutoPtr< const Alias > pAlias;
	AutoPtr< const SuperClass > pSuperClass;
	AutoPtr< List< AssociationFeature * > > pAssociationFeature;
	lineInfo theLineInfo;
};
class OW_MOF_API ClassDeclaration
{
public:
	ClassDeclaration(
		List< Qualifier * >* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		const SuperClass* pNewSuperClass,
		List< ClassFeature * >* pNewClassFeature,
		const lineInfo& li )
		: pQualifier(pNewQualifier)
		, pClassName(pNewClassName)
		, pAlias(pNewAlias)
		, pSuperClass(pNewSuperClass)
		, pClassFeature(pNewClassFeature)
		, theLineInfo(li)
	{}
	virtual ~ClassDeclaration()
	{
		while (pQualifier.get() && !pQualifier->empty())
		{
			delete pQualifier->front();
			pQualifier->pop_front();
		}
		while (pClassFeature.get() && !pClassFeature->empty())
		{
			delete pClassFeature->front();
			pClassFeature->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitClassDeclaration( this ); }
	
	AutoPtr< List< Qualifier * > > pQualifier;
	AutoPtr< const ClassName > pClassName;
	AutoPtr< const Alias > pAlias;
	AutoPtr< const SuperClass > pSuperClass;
	AutoPtr< List< ClassFeature * > > pClassFeature;
	lineInfo theLineInfo;
};
class OW_MOF_API PragmaParameter
{
public:
	PragmaParameter( const String* pNewPragmaParameter )
		: pPragmaParameter(pNewPragmaParameter)
	{}
	virtual ~PragmaParameter(){}
	void Accept( Visitor *pV ) const { pV->VisitPragmaParameter( this ); }
	AutoPtr< const String > pPragmaParameter;
};
class OW_MOF_API PragmaName
{
public:
	PragmaName( const String* pNewPragmaName )
		: pPragmaName(pNewPragmaName)
	{}
	virtual ~PragmaName(){}
	void Accept( Visitor *pV ) const { pV->VisitPragmaName( this ); }
	AutoPtr< const String > pPragmaName;
};
class OW_MOF_API CompilerDirective
{
public:
	CompilerDirective(
		const PragmaName* pNewPragmaName,
		const PragmaParameter* pNewPragmaParameter,
		const lineInfo& li)
		: pPragmaName(pNewPragmaName)
		, pPragmaParameter(pNewPragmaParameter)
		, theLineInfo(li)
	{}
	virtual ~CompilerDirective(){}
	void Accept( Visitor *pV ) const { pV->VisitCompilerDirective( this ); }
	
	AutoPtr< const PragmaName > pPragmaName;
	AutoPtr< const PragmaParameter > pPragmaParameter;
	lineInfo theLineInfo;
};
class OW_MOF_API MOFProduction {
public:
	virtual ~MOFProduction() {}
	virtual void Accept( Visitor * ) const = 0;
};
class OW_MOF_API MOFProductionInstanceDeclaration : public MOFProduction
{
public:
	MOFProductionInstanceDeclaration(
		const InstanceDeclaration* pNewInstanceDeclaration )
		: pInstanceDeclaration(pNewInstanceDeclaration)
	{}
	virtual ~MOFProductionInstanceDeclaration(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitMOFProductionInstanceDeclaration( this );
	}
	
	AutoPtr< const InstanceDeclaration > pInstanceDeclaration;
};
class OW_MOF_API MOFProductionQualifierDeclaration : public MOFProduction
{
public:
	MOFProductionQualifierDeclaration(
		const QualifierDeclaration* pNewQualifierDeclaration )
		: pQualifierDeclaration(pNewQualifierDeclaration)
	{}
	virtual ~MOFProductionQualifierDeclaration(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitMOFProductionQualifierDeclaration( this );
	}
	
	AutoPtr< const QualifierDeclaration > pQualifierDeclaration;
};
class OW_MOF_API MOFProductionIndicDeclaration : public MOFProduction
{
public:
	MOFProductionIndicDeclaration(
		const IndicDeclaration* pNewIndicDeclaration )
		: pIndicDeclaration(pNewIndicDeclaration)
	{}
	virtual ~MOFProductionIndicDeclaration() {}
	void Accept( Visitor *pV ) const
	{
		pV->VisitMOFProductionIndicDeclaration( this );
	}
	
	AutoPtr< const IndicDeclaration > pIndicDeclaration;
};
class OW_MOF_API MOFProductionAssocDeclaration : public MOFProduction
{
public:
	MOFProductionAssocDeclaration(
		const AssocDeclaration* pNewAssocDeclaration )
		: pAssocDeclaration(pNewAssocDeclaration)
	{}
	virtual ~MOFProductionAssocDeclaration(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitMOFProductionAssocDeclaration( this );
	}
	
	AutoPtr< const AssocDeclaration > pAssocDeclaration;
};
class OW_MOF_API MOFProductionClassDeclaration : public MOFProduction
{
public:
	MOFProductionClassDeclaration(
		const ClassDeclaration* pNewClassDeclaration )
		: pClassDeclaration(pNewClassDeclaration)
	{}
	virtual ~MOFProductionClassDeclaration() {}
	void Accept( Visitor *pV ) const
	{
		pV->VisitMOFProductionClassDeclaration( this );
	}
	
	AutoPtr< const ClassDeclaration > pClassDeclaration;
};
class OW_MOF_API MOFProductionCompilerDirective : public MOFProduction
{
public:
	MOFProductionCompilerDirective(
		const CompilerDirective* pNewCompilerDirective )
		: pCompilerDirective(pNewCompilerDirective)
	{}
	virtual ~MOFProductionCompilerDirective() {}
	void Accept( Visitor *pV ) const
	{
		pV->VisitMOFProductionCompilerDirective( this );
	}
	AutoPtr< const CompilerDirective > pCompilerDirective;
};
class OW_MOF_API MOFSpecification {
public:
	MOFSpecification( List< MOFProduction * >* pNewMOFProduction )
		: pMOFProduction(pNewMOFProduction)
	{}
	virtual ~MOFSpecification()
	{
		while (pMOFProduction.get() && !pMOFProduction->empty())
		{
			delete pMOFProduction->front();
			pMOFProduction->pop_front();
		}
	}
	void Accept( Visitor *pV ) const { pV->VisitMOFSpecification( this ); }
	
	AutoPtr< List< MOFProduction * > > pMOFProduction;
};

} // end namespace MOF
} // end namespace OpenWBEM

#endif
