/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
#include "blocxx/String.hpp"
#include "blocxx/List.hpp"
#include "blocxx/AutoPtr.hpp"
#include "OW_MOFVisitor.hpp"
#include "OW_MOFLineInfo.hpp"

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
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
	Flavor( const blocxx::String *pNewFlavor, const LineInfo& li )
	: pFlavor(pNewFlavor)
	, theLineInfo(li)
	{}
	virtual ~Flavor(){}

	void Accept( Visitor *pV ) const { pV->VisitFlavor( this ); }

	blocxx::AutoPtr<const blocxx::String> pFlavor;
	LineInfo theLineInfo;
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
	ArrayInitializer( blocxx::List<ConstantValue*>* pNewConstantValue )
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
	blocxx::AutoPtr< blocxx::List<ConstantValue*> > pConstantValue;
};
class OW_MOF_API QualifierParameterArrayInitializer : public QualifierParameter
{
public:
	QualifierParameterArrayInitializer(
		const ArrayInitializer* pNewArrayInitializer,
		const LineInfo& li )
		: pArrayInitializer(pNewArrayInitializer)
		, theLineInfo(li)
	{}
	virtual ~QualifierParameterArrayInitializer(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitQualifierParameterArrayInitializer( this );
	}

	blocxx::AutoPtr<const ArrayInitializer> pArrayInitializer;
	LineInfo theLineInfo;
};
class OW_MOF_API QualifierParameterConstantValue : public QualifierParameter
{
public:
	QualifierParameterConstantValue( const ConstantValue* pNewConstantValue,
		const LineInfo& li )
		: pConstantValue(pNewConstantValue)
		, theLineInfo(li)
	{}
	virtual ~QualifierParameterConstantValue(){}
	void Accept( Visitor *pV ) const
	{
		pV->VisitQualifierParameterConstantValue( this );
	}

	blocxx::AutoPtr<const ConstantValue> pConstantValue;
	LineInfo theLineInfo;
};
class OW_MOF_API QualifierName
{
public:
	QualifierName( const blocxx::String* pNewQualifierName )
		: pQualifierName(pNewQualifierName)
	{}
	virtual ~QualifierName() {}
	void Accept( Visitor *pV ) const { pV->VisitQualifierName( this ); }
	blocxx::AutoPtr< const blocxx::String > pQualifierName;
};
class OW_MOF_API Qualifier
{
public:
	Qualifier( const QualifierName* pNewQualifierName,
		const QualifierParameter* pNewQualifierParameter,
		blocxx::List<Flavor*>* pNewFlavor,
		const LineInfo& li )
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

	blocxx::AutoPtr<const QualifierName> pQualifierName;
	blocxx::AutoPtr<const QualifierParameter> pQualifierParameter;
	blocxx::AutoPtr< blocxx::List<Flavor*> > pFlavor;
	LineInfo theLineInfo;
};
class OW_MOF_API DefaultValue
{
public:
	DefaultValue( const Initializer* pNewInitializer)
		: pInitializer(pNewInitializer)
	{}
	virtual ~DefaultValue(){}
	void Accept( Visitor *pV ) const { pV->VisitDefaultValue( this ); }
	blocxx::AutoPtr<const Initializer> pInitializer;
};
class OW_MOF_API ValueInitializer
{
public:
	ValueInitializer( blocxx::List<Qualifier*>* pNewQualifier,
		const blocxx::String* pNewValueInitializer,
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
	blocxx::AutoPtr< blocxx::List<Qualifier*> > pQualifier;
	blocxx::AutoPtr<const blocxx::String> pValueInitializer;
	blocxx::AutoPtr<const DefaultValue> pDefaultValue;
};
class OW_MOF_API PropertyName
{
public:
	PropertyName( const blocxx::String* pNewPropertyName )
		: pPropertyName(pNewPropertyName)
	{}
	virtual ~PropertyName() {}
	void Accept( Visitor *pV ) const { pV->VisitPropertyName( this ); }
	blocxx::AutoPtr<const blocxx::String> pPropertyName;
};
class OW_MOF_API ClassName
{
public:
	ClassName( const blocxx::String* pNewClassName )
		: pClassName(pNewClassName)
	{}
	virtual ~ClassName(){}
	void Accept( Visitor *pV ) const { pV->VisitClassName( this ); }
	blocxx::AutoPtr<const blocxx::String> pClassName;
};
class OW_MOF_API AliasIdentifier
{
public:
	AliasIdentifier( const blocxx::String* pNewAliasIdentifier, LineInfo li )
		: pAliasIdentifier(pNewAliasIdentifier)
		, theLineInfo(li)
	{}
	virtual ~AliasIdentifier(){}
	void Accept( Visitor *pV ) const { pV->VisitAliasIdentifier( this ); }
	blocxx::AutoPtr<const blocxx::String> pAliasIdentifier;
	LineInfo theLineInfo;
};
class OW_MOF_API Alias
{
public:
	Alias( const AliasIdentifier* pNewAliasIdentifier )
		: pAliasIdentifier(pNewAliasIdentifier)
	{}
	virtual ~Alias(){}
	void Accept( Visitor *pV ) const { pV->VisitAlias( this ); }
	blocxx::AutoPtr<const AliasIdentifier> pAliasIdentifier;
};
class OW_MOF_API InstanceDeclaration
{
public:
	InstanceDeclaration(
		blocxx::List<Qualifier*>* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		blocxx::List<ValueInitializer*>* pNewValueInitializer,
		const LineInfo& li)
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

	blocxx::AutoPtr< blocxx::List<Qualifier*> > pQualifier;
	blocxx::AutoPtr<const ClassName> pClassName;
	blocxx::AutoPtr<const Alias> pAlias;
	blocxx::AutoPtr< blocxx::List<ValueInitializer*> > pValueInitializer;
	LineInfo theLineInfo;
};
class OW_MOF_API DefaultFlavor
{
public:
	DefaultFlavor( blocxx::List<Flavor*>* pNewFlavor )
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

	blocxx::AutoPtr< blocxx::List<Flavor*> > pFlavor;
};
class OW_MOF_API MetaElement
{
public:
	MetaElement( const blocxx::String* pNewMetaElement, const LineInfo& li )
		: pMetaElement(pNewMetaElement)
		, theLineInfo(li)
	{}
	virtual ~MetaElement(){}
	void Accept( Visitor *pV ) const { pV->VisitMetaElement( this ); }
	blocxx::AutoPtr<const blocxx::String> pMetaElement;
	LineInfo theLineInfo;
};
class OW_MOF_API Scope
{
public:
	Scope( blocxx::List<MetaElement*>* pNewMetaElement )
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

	blocxx::AutoPtr< blocxx::List<MetaElement*> > pMetaElement;
};
class OW_MOF_API DataType
{
public:
	DataType( const blocxx::String* pNewDataType )
		: pDataType(pNewDataType)
	{}
	virtual ~DataType(){}
	void Accept( Visitor *pV ) const { pV->VisitDataType( this ); }
	blocxx::AutoPtr<const blocxx::String> pDataType;
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
	blocxx::AutoPtr<const IntegerValue> pArray;
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

	blocxx::AutoPtr<const DataType> pDataType;
	blocxx::AutoPtr<const Array> pArray;
	blocxx::AutoPtr<const DefaultValue> pDefaultValue;
};

class OW_MOF_API QualifierDeclaration
{
public:
	QualifierDeclaration(
		const QualifierName* pNewQualifierName,
		const QualifierType* pNewQualifierType,
		const Scope* pNewScope,
		const DefaultFlavor* pNewDefaultFlavor,
		const LineInfo& li)
		: pQualifierName(pNewQualifierName)
		, pQualifierType(pNewQualifierType)
		, pScope(pNewScope)
		, pDefaultFlavor(pNewDefaultFlavor)
		, theLineInfo(li)
	{}
	virtual ~QualifierDeclaration(){}
	void Accept( Visitor *pV ) const { pV->VisitQualifierDeclaration( this ); }
	blocxx::AutoPtr<const QualifierName> pQualifierName;
	blocxx::AutoPtr<const QualifierType> pQualifierType;
	blocxx::AutoPtr<const Scope> pScope;
	blocxx::AutoPtr<const DefaultFlavor> pDefaultFlavor;
	LineInfo theLineInfo;
};
class OW_MOF_API ReferenceName
{
public:
	ReferenceName( const blocxx::String* pNewReferenceName )
		: pReferenceName(pNewReferenceName)
	{}
	virtual ~ReferenceName(){}
	void Accept( Visitor *pV ) const { pV->VisitReferenceName( this ); }
	blocxx::AutoPtr<const blocxx::String> pReferenceName;
};
class OW_MOF_API IntegerValueBinaryValue : public IntegerValue
{
public:
	IntegerValueBinaryValue( const blocxx::String* pNewBinaryValue )
		: pBinaryValue(pNewBinaryValue)
	{}
	virtual ~IntegerValueBinaryValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueBinaryValue( this );
	}
	blocxx::AutoPtr<const blocxx::String> pBinaryValue;
};
class OW_MOF_API IntegerValueOctalValue : public IntegerValue
{
public:
	IntegerValueOctalValue( const blocxx::String* pNewOctalValue )
		: pOctalValue(pNewOctalValue)
	{}
	virtual ~IntegerValueOctalValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueOctalValue( this );
	}
	blocxx::AutoPtr<const blocxx::String> pOctalValue;
};
class OW_MOF_API IntegerValueDecimalValue : public IntegerValue
{
public:
	IntegerValueDecimalValue( const blocxx::String* pNewDecimalValue )
		: pDecimalValue(pNewDecimalValue)
	{}
	virtual ~IntegerValueDecimalValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueDecimalValue( this );
	}
	blocxx::AutoPtr<const blocxx::String> pDecimalValue;
};
class OW_MOF_API IntegerValueHexValue : public IntegerValue
{
public:
	IntegerValueHexValue( const blocxx::String* pNewHexValue )
		: pHexValue(pNewHexValue)
	{}
	virtual ~IntegerValueHexValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueHexValue( this );
	}
	blocxx::AutoPtr<const blocxx::String> pHexValue;
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
	blocxx::AutoPtr<const IntegerValue> pIntegerValue;
};
class OW_MOF_API ConstantValueFloatValue : public ConstantValue
{
public:
	ConstantValueFloatValue( const blocxx::String* pNewFloatValue )
		: pFloatValue(pNewFloatValue)
	{}
	virtual ~ConstantValueFloatValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueFloatValue( this );
	}
	blocxx::AutoPtr<const blocxx::String> pFloatValue;
};
class OW_MOF_API ConstantValueCharValue : public ConstantValue
{
public:
	ConstantValueCharValue( const blocxx::String* pNewCharValue )
		: pCharValue(pNewCharValue)
	{}
	virtual ~ConstantValueCharValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueCharValue( this );
	}
	blocxx::AutoPtr<const blocxx::String> pCharValue;
};
class OW_MOF_API ConstantValueStringValue : public ConstantValue
{
public:
	ConstantValueStringValue( const blocxx::String* pNewStringValue )
		: pStringValue(pNewStringValue)
	{}
	virtual ~ConstantValueStringValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueStringValue( this );
	}
	blocxx::AutoPtr<const blocxx::String> pStringValue;
};
class OW_MOF_API ConstantValueBooleanValue : public ConstantValue
{
public:
	ConstantValueBooleanValue( const blocxx::String* pNewBooleanValue )
		: pBooleanValue(pNewBooleanValue)
	{}
	virtual ~ConstantValueBooleanValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueBooleanValue( this );
	}
	blocxx::AutoPtr<const blocxx::String> pBooleanValue;
};
class OW_MOF_API ConstantValueNullValue : public ConstantValue
{
public:
	ConstantValueNullValue( const blocxx::String* pNewNullValue )
		: pNullValue(pNewNullValue)
	{}
	virtual ~ConstantValueNullValue(){}
	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueNullValue( this );
	}
	blocxx::AutoPtr<const blocxx::String> pNullValue;
};
class OW_MOF_API ObjectHandle
{
public:
	ObjectHandle( const blocxx::String* pNewObjectHandle )
		: pObjectHandle(pNewObjectHandle)
	{}
	virtual ~ObjectHandle(){}
	void Accept( Visitor *pV ) const { pV->VisitObjectHandle( this ); }
	blocxx::AutoPtr<const blocxx::String> pObjectHandle;
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

	blocxx::AutoPtr<const AliasIdentifier> pAliasIdentifier;
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
	blocxx::AutoPtr<const ObjectHandle> pObjectHandle;
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
	blocxx::AutoPtr<const ReferenceInitializer> pReferenceInitializer;
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
	blocxx::AutoPtr<const ArrayInitializer> pArrayInitializer;
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
	blocxx::AutoPtr<const ConstantValue> pConstantValue;
};
class OW_MOF_API ParameterName
{
public:
	ParameterName( const blocxx::String* pNewParameterName )
		: pParameterName(pNewParameterName)
	{}
	virtual ~ParameterName(){}
	void Accept( Visitor *pV ) const { pV->VisitParameterName( this ); }
	blocxx::AutoPtr<const blocxx::String> pParameterName;
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
	blocxx::AutoPtr<const ClassName> pClassName;
};
class OW_MOF_API ParameterObjectRef : public Parameter
{
public:
	ParameterObjectRef(
		blocxx::List<Qualifier*>* pNewQualifier,
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
	blocxx::AutoPtr< blocxx::List<Qualifier*> > pQualifier;
	blocxx::AutoPtr<const ObjectRef> pObjectRef;
	blocxx::AutoPtr<const ParameterName> pParameterName;
	blocxx::AutoPtr<const Array> pArray;
};
class OW_MOF_API ParameterDataType : public Parameter
{
public:
	ParameterDataType(
		blocxx::List<Qualifier*>* pNewQualifier,
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
	blocxx::AutoPtr< blocxx::List<Qualifier*> > pQualifier;
	blocxx::AutoPtr<const DataType> pDataType;
	blocxx::AutoPtr<const ParameterName> pParameterName;
	blocxx::AutoPtr<const Array> pArray;
};
class OW_MOF_API MethodName
{
public:
	MethodName( const blocxx::String* pNewMethodName )
		: pMethodName(pNewMethodName)
	{}
	virtual ~MethodName(){}
	void Accept( Visitor *pV ) const { pV->VisitMethodName( this ); }
	blocxx::AutoPtr<const blocxx::String> pMethodName;
};
class OW_MOF_API MethodDeclaration
{
public:
	MethodDeclaration(
		blocxx::List<Qualifier*>* pNewQualifier,
		const DataType* pNewDataType,
		const MethodName* pNewMethodName,
		blocxx::List<Parameter*>* pNewParameter )
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

	blocxx::AutoPtr< blocxx::List<Qualifier*> > pQualifier;
	blocxx::AutoPtr<const DataType> pDataType;
	blocxx::AutoPtr<const MethodName> pMethodName;
	blocxx::AutoPtr< blocxx::List<Parameter*> > pParameter;
};
class OW_MOF_API ReferenceDeclaration
{
public:
	ReferenceDeclaration(
		blocxx::List<Qualifier*>* pNewQualifier,
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

	blocxx::AutoPtr< blocxx::List<Qualifier*> > pQualifier;
	blocxx::AutoPtr<const ObjectRef> pObjectRef;
	blocxx::AutoPtr<const ReferenceName> pReferenceName;
	blocxx::AutoPtr<const DefaultValue> pDefaultValue;
};
class OW_MOF_API PropertyDeclaration
{
public:
	PropertyDeclaration(
		blocxx::List<Qualifier*>* pNewQualifier,
		const DataType* pNewDataType,
		const PropertyName* pNewPropertyName,
		const Array* pNewArray,
		const DefaultValue* pNewDefaultValue,
		const LineInfo& li)
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
	blocxx::AutoPtr< blocxx::List<Qualifier*> > pQualifier;
	blocxx::AutoPtr<const DataType> pDataType;
	blocxx::AutoPtr<const PropertyName> pPropertyName;
	blocxx::AutoPtr<const Array> pArray;
	blocxx::AutoPtr<const DefaultValue> pDefaultValue;
	LineInfo theLineInfo;
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

	blocxx::AutoPtr<const ClassFeature> pClassFeature;
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

	blocxx::AutoPtr<const MethodDeclaration> pMethodDeclaration;
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

	blocxx::AutoPtr<const PropertyDeclaration> pPropertyDeclaration;
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

	blocxx::AutoPtr<const ReferenceDeclaration> pReferenceDeclaration;
};
class OW_MOF_API SuperClass
{
public:
	SuperClass( const ClassName* pNewClassName )
		: pClassName(pNewClassName)
	{}
	virtual ~SuperClass(){}
	void Accept( Visitor *pV ) const { pV->VisitSuperClass( this ); }

	blocxx::AutoPtr<const ClassName> pClassName;
};
class OW_MOF_API IndicDeclaration
{
public:
	IndicDeclaration(
		blocxx::List<Qualifier*>* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		const SuperClass* pNewSuperClass,
		blocxx::List<ClassFeature*>* pNewClassFeature,
		const LineInfo& li)
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

	blocxx::AutoPtr< blocxx::List<Qualifier*> > pQualifier;
	blocxx::AutoPtr<const ClassName> pClassName;
	blocxx::AutoPtr<const Alias> pAlias;
	blocxx::AutoPtr<const SuperClass> pSuperClass;
	blocxx::AutoPtr< blocxx::List<ClassFeature*> > pClassFeature;
	LineInfo theLineInfo;
};
class OW_MOF_API AssocDeclaration
{
public:
	AssocDeclaration(
		blocxx::List<Qualifier*>* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		const SuperClass* pNewSuperClass,
		blocxx::List<AssociationFeature*>* pNewAssociationFeature,
		const LineInfo& li )
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

	blocxx::AutoPtr< blocxx::List<Qualifier*> > pQualifier;
	blocxx::AutoPtr<const ClassName> pClassName;
	blocxx::AutoPtr<const Alias> pAlias;
	blocxx::AutoPtr<const SuperClass> pSuperClass;
	blocxx::AutoPtr< blocxx::List<AssociationFeature*> > pAssociationFeature;
	LineInfo theLineInfo;
};
class OW_MOF_API ClassDeclaration
{
public:
	ClassDeclaration(
		blocxx::List<Qualifier*>* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		const SuperClass* pNewSuperClass,
		blocxx::List<ClassFeature*>* pNewClassFeature,
		const LineInfo& li )
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

	blocxx::AutoPtr< blocxx::List<Qualifier*> > pQualifier;
	blocxx::AutoPtr<const ClassName> pClassName;
	blocxx::AutoPtr<const Alias> pAlias;
	blocxx::AutoPtr<const SuperClass> pSuperClass;
	blocxx::AutoPtr< blocxx::List<ClassFeature*> > pClassFeature;
	LineInfo theLineInfo;
};
class OW_MOF_API PragmaParameter
{
public:
	PragmaParameter( const blocxx::String* pNewPragmaParameter )
		: pPragmaParameter(pNewPragmaParameter)
	{}
	virtual ~PragmaParameter(){}
	void Accept( Visitor *pV ) const { pV->VisitPragmaParameter( this ); }
	blocxx::AutoPtr<const blocxx::String> pPragmaParameter;
};
class OW_MOF_API PragmaName
{
public:
	PragmaName( const blocxx::String* pNewPragmaName )
		: pPragmaName(pNewPragmaName)
	{}
	virtual ~PragmaName(){}
	void Accept( Visitor *pV ) const { pV->VisitPragmaName( this ); }
	blocxx::AutoPtr<const blocxx::String> pPragmaName;
};
class OW_MOF_API CompilerDirective
{
public:
	CompilerDirective(
		const PragmaName* pNewPragmaName,
		const PragmaParameter* pNewPragmaParameter,
		const LineInfo& li)
		: pPragmaName(pNewPragmaName)
		, pPragmaParameter(pNewPragmaParameter)
		, theLineInfo(li)
	{}
	virtual ~CompilerDirective(){}
	void Accept( Visitor *pV ) const { pV->VisitCompilerDirective( this ); }

	blocxx::AutoPtr<const PragmaName> pPragmaName;
	blocxx::AutoPtr<const PragmaParameter> pPragmaParameter;
	LineInfo theLineInfo;
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

	blocxx::AutoPtr<const InstanceDeclaration> pInstanceDeclaration;
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

	blocxx::AutoPtr<const QualifierDeclaration> pQualifierDeclaration;
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

	blocxx::AutoPtr<const IndicDeclaration> pIndicDeclaration;
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

	blocxx::AutoPtr<const AssocDeclaration> pAssocDeclaration;
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

	blocxx::AutoPtr<const ClassDeclaration> pClassDeclaration;
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
	blocxx::AutoPtr<const CompilerDirective> pCompilerDirective;
};
class OW_MOF_API MOFSpecification {
public:
	MOFSpecification( blocxx::List<MOFProduction*>* pNewMOFProduction )
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

	blocxx::AutoPtr< blocxx::List<MOFProduction*> > pMOFProduction;
};

} // end namespace MOF
} // end namespace OW_NAMESPACE

#ifdef OW_WIN32
#pragma warning (pop)
#endif

#endif
