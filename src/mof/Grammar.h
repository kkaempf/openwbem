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

#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "OW_config.h"

#include "OW_String.hpp"
#include "OW_List.hpp"
#include "OW_AutoPtr.hpp"

#include "Visitor.h"
#include "LineInfo.hpp"

class Initializer
{
public:
	virtual ~Initializer() {}
	virtual void Accept( Visitor * ) const = 0;
};


class Flavor
{
public:
	Flavor( const OW_String *pNewFlavor, const lineInfo& li )
	: pFlavor(pNewFlavor)
	, theLineInfo(li)
	{}
	virtual ~Flavor(){}
	
	void Accept( Visitor *pV ) const { pV->VisitFlavor( this ); }

	OW_AutoPtrNoVec< const OW_String > pFlavor;
	lineInfo theLineInfo;
};


class QualifierParameter
{
public:
	virtual ~QualifierParameter() {}
	virtual void Accept( Visitor * ) const = 0;
};


class ConstantValue
{
public:
	virtual ~ConstantValue() {}
	virtual void Accept( Visitor * ) const = 0;
};


class ArrayInitializer
{
public:
	ArrayInitializer( OW_List< ConstantValue * >* pNewConstantValue )
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

	OW_AutoPtrNoVec< OW_List< ConstantValue * > > pConstantValue;
};


class QualifierParameterArrayInitializer : public QualifierParameter
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
	
	OW_AutoPtrNoVec< const ArrayInitializer > pArrayInitializer;
	lineInfo theLineInfo;
};


class QualifierParameterConstantValue : public QualifierParameter
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
	
	OW_AutoPtrNoVec< const ConstantValue > pConstantValue;
	lineInfo theLineInfo;
};

class QualifierName
{
public:
	QualifierName( const OW_String* pNewQualifierName )
		: pQualifierName(pNewQualifierName)
	{}
	virtual ~QualifierName() {}

	void Accept( Visitor *pV ) const { pV->VisitQualifierName( this ); }

	OW_AutoPtrNoVec< const OW_String > pQualifierName;
};



class Qualifier
{
public:
	Qualifier( const QualifierName* pNewQualifierName,
		const QualifierParameter* pNewQualifierParameter,
		OW_List< Flavor * >* pNewFlavor,
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
	
	OW_AutoPtrNoVec< const QualifierName > pQualifierName;
	OW_AutoPtrNoVec< const QualifierParameter > pQualifierParameter;
	OW_AutoPtrNoVec< OW_List< Flavor * > > pFlavor;
	lineInfo theLineInfo;
};

class DefaultValue
{
public:
	DefaultValue( const Initializer* pNewInitializer)
		: pInitializer(pNewInitializer)
	{}
	virtual ~DefaultValue(){}

	void Accept( Visitor *pV ) const { pV->VisitDefaultValue( this ); }

	OW_AutoPtrNoVec< const Initializer > pInitializer;
};



class ValueInitializer
{
public:
	ValueInitializer( OW_List< Qualifier * >* pNewQualifier,
		const OW_String* pNewValueInitializer,
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

	OW_AutoPtrNoVec< OW_List< Qualifier * > > pQualifier;
	OW_AutoPtrNoVec< const OW_String > pValueInitializer;
	OW_AutoPtrNoVec< const DefaultValue > pDefaultValue;
};


class PropertyName
{
public:
	PropertyName( const OW_String* pNewPropertyName )
		: pPropertyName(pNewPropertyName)
	{}
	virtual ~PropertyName() {}

	void Accept( Visitor *pV ) const { pV->VisitPropertyName( this ); }

	OW_AutoPtrNoVec< const OW_String > pPropertyName;
};

class ClassName
{
public:
	ClassName( const OW_String* pNewClassName )
		: pClassName(pNewClassName)
	{}
	virtual ~ClassName(){}

	void Accept( Visitor *pV ) const { pV->VisitClassName( this ); }

	OW_AutoPtrNoVec< const OW_String > pClassName;
};

class AliasIdentifier
{
public:
	AliasIdentifier( const OW_String* pNewAliasIdentifier, lineInfo li )
		: pAliasIdentifier(pNewAliasIdentifier)
		, theLineInfo(li)
	{}

	virtual ~AliasIdentifier(){}

	void Accept( Visitor *pV ) const { pV->VisitAliasIdentifier( this ); }

	OW_AutoPtrNoVec< const OW_String > pAliasIdentifier;
	lineInfo theLineInfo;
};



class Alias
{
public:
	Alias( const AliasIdentifier* pNewAliasIdentifier )
		: pAliasIdentifier(pNewAliasIdentifier)
	{}
	virtual ~Alias(){}

	void Accept( Visitor *pV ) const { pV->VisitAlias( this ); }

	OW_AutoPtrNoVec< const AliasIdentifier > pAliasIdentifier;
};



class InstanceDeclaration
{
public:
	InstanceDeclaration(
		OW_List< Qualifier * >* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		OW_List< ValueInitializer * >* pNewValueInitializer,
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
	
    OW_AutoPtrNoVec< OW_List< Qualifier * > > pQualifier;
    OW_AutoPtrNoVec< const ClassName > pClassName;
    OW_AutoPtrNoVec< const Alias > pAlias;
    OW_AutoPtrNoVec< OW_List< ValueInitializer * > > pValueInitializer;
	 lineInfo theLineInfo;
};


class DefaultFlavor
{
public:
	DefaultFlavor( OW_List< Flavor * >* pNewFlavor )
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
	
    OW_AutoPtrNoVec< OW_List< Flavor * > > pFlavor;
};


class MetaElement
{
public:
	MetaElement( const OW_String* pNewMetaElement, const lineInfo& li )
		: pMetaElement(pNewMetaElement)
		, theLineInfo(li)
	{}

	virtual ~MetaElement(){}

	void Accept( Visitor *pV ) const { pV->VisitMetaElement( this ); }

	OW_AutoPtrNoVec< const OW_String > pMetaElement;
	lineInfo theLineInfo;
};


class Scope
{
public:
	Scope( OW_List< MetaElement * >* pNewMetaElement )
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
	
	OW_AutoPtrNoVec< OW_List< MetaElement * > > pMetaElement;
};



class DataType
{
public:
	DataType( const OW_String* pNewDataType )
		: pDataType(pNewDataType)
	{}

	virtual ~DataType(){}

	void Accept( Visitor *pV ) const { pV->VisitDataType( this ); }

	OW_AutoPtrNoVec< const OW_String > pDataType;
};

class IntegerValue
{
public:
	virtual ~IntegerValue() {}
	virtual void Accept( Visitor * ) const = 0;
};


class Array
{
public:
	Array( const IntegerValue* pNewArray )
		: pArray(pNewArray)
	{}

	virtual ~Array(){}

	void Accept( Visitor *pV ) const { pV->VisitArray( this ); }

	OW_AutoPtrNoVec< const IntegerValue > pArray;
};




class QualifierType
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
	
	OW_AutoPtrNoVec< const DataType > pDataType;
	OW_AutoPtrNoVec< const Array > pArray;
	OW_AutoPtrNoVec< const DefaultValue > pDefaultValue;
};
	

class QualifierDeclaration
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

	OW_AutoPtrNoVec< const QualifierName > pQualifierName;
	OW_AutoPtrNoVec< const QualifierType > pQualifierType;
	OW_AutoPtrNoVec< const Scope > pScope;
	OW_AutoPtrNoVec< const DefaultFlavor > pDefaultFlavor;
	lineInfo theLineInfo;
};


class ReferenceName
{
public:
	ReferenceName( const OW_String* pNewReferenceName )
		: pReferenceName(pNewReferenceName)
	{}

	virtual ~ReferenceName(){}

	void Accept( Visitor *pV ) const { pV->VisitReferenceName( this ); }

	OW_AutoPtrNoVec< const OW_String > pReferenceName;
};

class IntegerValueBinaryValue : public IntegerValue
{
public:
	IntegerValueBinaryValue( const OW_String* pNewBinaryValue )
		: pBinaryValue(pNewBinaryValue)
	{}

	virtual ~IntegerValueBinaryValue(){}

	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueBinaryValue( this );
	}

	OW_AutoPtrNoVec< const OW_String > pBinaryValue;
};

class IntegerValueOctalValue : public IntegerValue
{
public:
	IntegerValueOctalValue( const OW_String* pNewOctalValue )
		: pOctalValue(pNewOctalValue)
	{}

	virtual ~IntegerValueOctalValue(){}

	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueOctalValue( this );
	}

	OW_AutoPtrNoVec< const OW_String > pOctalValue;
};

class IntegerValueDecimalValue : public IntegerValue
{
public:
	IntegerValueDecimalValue( const OW_String* pNewDecimalValue )
		: pDecimalValue(pNewDecimalValue)
	{}

	virtual ~IntegerValueDecimalValue(){}

	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueDecimalValue( this );
	}

	OW_AutoPtrNoVec< const OW_String > pDecimalValue;
};

class IntegerValueHexValue : public IntegerValue
{
public:
	IntegerValueHexValue( const OW_String* pNewHexValue )
		: pHexValue(pNewHexValue)
	{}

	virtual ~IntegerValueHexValue(){}

	virtual void Accept(Visitor *pV) const
	{
		pV->VisitIntegerValueHexValue( this );
	}

	OW_AutoPtrNoVec< const OW_String > pHexValue;
};


class ConstantValueIntegerValue : public ConstantValue
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

	OW_AutoPtrNoVec< const IntegerValue > pIntegerValue;
};

class ConstantValueFloatValue : public ConstantValue
{
public:
	ConstantValueFloatValue( const OW_String* pNewFloatValue )
		: pFloatValue(pNewFloatValue)
	{}

	virtual ~ConstantValueFloatValue(){}

	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueFloatValue( this );
	}

	OW_AutoPtrNoVec< const OW_String > pFloatValue;
};

class ConstantValueCharValue : public ConstantValue
{
public:
	ConstantValueCharValue( const OW_String* pNewCharValue )
		: pCharValue(pNewCharValue)
	{}

	virtual ~ConstantValueCharValue(){}

	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueCharValue( this );
	}

	OW_AutoPtrNoVec< const OW_String > pCharValue;
};

class ConstantValueStringValue : public ConstantValue
{
public:
	ConstantValueStringValue( const OW_String* pNewStringValue )
		: pStringValue(pNewStringValue)
	{}

	virtual ~ConstantValueStringValue(){}

	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueStringValue( this );
	}

	OW_AutoPtrNoVec< const OW_String > pStringValue;
};

class ConstantValueBooleanValue : public ConstantValue
{
public:
	ConstantValueBooleanValue( const OW_String* pNewBooleanValue )
		: pBooleanValue(pNewBooleanValue)
	{}

	virtual ~ConstantValueBooleanValue(){}

	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueBooleanValue( this );
	}

	OW_AutoPtrNoVec< const OW_String > pBooleanValue;
};

class ConstantValueNullValue : public ConstantValue
{
public:
	ConstantValueNullValue( const OW_String* pNewNullValue )
		: pNullValue(pNewNullValue)
	{}

	virtual ~ConstantValueNullValue(){}

	virtual void Accept(Visitor *pV) const
	{
		pV->VisitConstantValueNullValue( this );
	}

	OW_AutoPtrNoVec< const OW_String > pNullValue;
};


class ObjectHandle
{
public:
	ObjectHandle( const OW_String* pNewObjectHandle )
		: pObjectHandle(pNewObjectHandle)
	{}

	virtual ~ObjectHandle(){}

	void Accept( Visitor *pV ) const { pV->VisitObjectHandle( this ); }

	OW_AutoPtrNoVec< const OW_String > pObjectHandle;
};


class ReferenceInitializer
{
public:
	virtual ~ReferenceInitializer() {}
	virtual void Accept( Visitor * ) const = 0;
};


class ReferenceInitializerAliasIdentifier : public ReferenceInitializer
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
	
	OW_AutoPtrNoVec< const AliasIdentifier > pAliasIdentifier;
};


class ReferenceInitializerObjectHandle : public ReferenceInitializer
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

	OW_AutoPtrNoVec< const ObjectHandle > pObjectHandle;
};



class InitializerReferenceInitializer : public Initializer
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

	OW_AutoPtrNoVec< const ReferenceInitializer > pReferenceInitializer;
};		


class InitializerArrayInitializer : public Initializer
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

	OW_AutoPtrNoVec< const ArrayInitializer > pArrayInitializer;
};		


class InitializerConstantValue : public Initializer
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

	OW_AutoPtrNoVec< const ConstantValue > pConstantValue;
};		


class ParameterName
{
public:
	ParameterName( const OW_String* pNewParameterName )
		: pParameterName(pNewParameterName)
	{}

	virtual ~ParameterName(){}

	void Accept( Visitor *pV ) const { pV->VisitParameterName( this ); }

	OW_AutoPtrNoVec< const OW_String > pParameterName;
};


class Parameter
{
public:
	virtual ~Parameter() {}
	virtual void Accept( Visitor * ) const = 0;
};


class ObjectRef
{
public:
	ObjectRef( const ClassName* pNewClassName )
		: pClassName(pNewClassName)
	{}

	virtual ~ObjectRef(){}

	void Accept( Visitor *pV ) const { pV->VisitObjectRef( this ); }

	OW_AutoPtrNoVec< const ClassName > pClassName;
};


class ParameterObjectRef : public Parameter
{
public:
	ParameterObjectRef(
		OW_List< Qualifier * >* pNewQualifier,
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

	OW_AutoPtrNoVec< OW_List< Qualifier * > > pQualifier;
	OW_AutoPtrNoVec< const ObjectRef > pObjectRef;
	OW_AutoPtrNoVec< const ParameterName > pParameterName;
	OW_AutoPtrNoVec< const Array > pArray;	
};


class ParameterDataType : public Parameter
{
public:
	ParameterDataType(
		OW_List< Qualifier * >* pNewQualifier,
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

	OW_AutoPtrNoVec< OW_List< Qualifier * > > pQualifier;
	OW_AutoPtrNoVec< const DataType > pDataType;
	OW_AutoPtrNoVec< const ParameterName > pParameterName;
	OW_AutoPtrNoVec< const Array > pArray;	
};


class MethodName
{
public:
	MethodName( const OW_String* pNewMethodName )
		: pMethodName(pNewMethodName)
	{}

	virtual ~MethodName(){}

	void Accept( Visitor *pV ) const { pV->VisitMethodName( this ); }

	OW_AutoPtrNoVec< const OW_String > pMethodName;
};


class MethodDeclaration
{
public:
	MethodDeclaration(
		OW_List< Qualifier * >* pNewQualifier,
		const DataType* pNewDataType,
		const MethodName* pNewMethodName,
		OW_List< Parameter * >* pNewParameter )
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
	
	OW_AutoPtrNoVec< OW_List< Qualifier * > > pQualifier;
	OW_AutoPtrNoVec< const DataType > pDataType;
	OW_AutoPtrNoVec< const MethodName > pMethodName;
	OW_AutoPtrNoVec< OW_List< Parameter * > > pParameter;
};


class ReferenceDeclaration
{
public:
	ReferenceDeclaration(
		OW_List< Qualifier * >* pNewQualifier,
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
	
	OW_AutoPtrNoVec< OW_List< Qualifier * > > pQualifier;
	OW_AutoPtrNoVec< const ObjectRef > pObjectRef;
	OW_AutoPtrNoVec< const ReferenceName > pReferenceName;
	OW_AutoPtrNoVec< const DefaultValue > pDefaultValue;
};


class PropertyDeclaration
{
public:
	PropertyDeclaration(
		OW_List< Qualifier * >* pNewQualifier,
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

	OW_AutoPtrNoVec< OW_List< Qualifier * > > pQualifier;
	OW_AutoPtrNoVec< const DataType > pDataType;
	OW_AutoPtrNoVec< const PropertyName > pPropertyName;
	OW_AutoPtrNoVec< const Array > pArray;
	OW_AutoPtrNoVec< const DefaultValue > pDefaultValue;
	lineInfo theLineInfo;
};


class AssociationFeature
{
public:
	virtual ~AssociationFeature() {}
	virtual void Accept( Visitor * ) const = 0;
};


class ClassFeature
{
public:
	virtual ~ClassFeature() {}
	virtual void Accept( Visitor * ) const = 0;
};


class AssociationFeatureClassFeature : public AssociationFeature
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
	
	OW_AutoPtrNoVec< const ClassFeature > pClassFeature;
};


class ClassFeatureMethodDeclaration : public ClassFeature
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
	
	OW_AutoPtrNoVec< const MethodDeclaration > pMethodDeclaration;
};


class ClassFeaturePropertyDeclaration : public ClassFeature
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
	
	OW_AutoPtrNoVec< const PropertyDeclaration > pPropertyDeclaration;
};


/* Note: This should be in AssociationFeature, but I found some MOF files
	that use this in ClassFeature */
class ClassFeatureReferenceDeclaration : public ClassFeature
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
	
	OW_AutoPtrNoVec< const ReferenceDeclaration > pReferenceDeclaration;
};


class SuperClass
{
public:
	SuperClass( const ClassName* pNewClassName )
		: pClassName(pNewClassName)
	{}

	virtual ~SuperClass(){}

	void Accept( Visitor *pV ) const { pV->VisitSuperClass( this ); }
	
	OW_AutoPtrNoVec< const ClassName > pClassName;
};


class IndicDeclaration
{
public:
	IndicDeclaration(
		OW_List< Qualifier * >* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		const SuperClass* pNewSuperClass,
		OW_List< ClassFeature * >* pNewClassFeature,
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
	
	OW_AutoPtrNoVec< OW_List< Qualifier * > > pQualifier;
	OW_AutoPtrNoVec< const ClassName > pClassName;
	OW_AutoPtrNoVec< const Alias > pAlias;
	OW_AutoPtrNoVec< const SuperClass > pSuperClass;
	OW_AutoPtrNoVec< OW_List< ClassFeature * > > pClassFeature;
	lineInfo theLineInfo;
};


class AssocDeclaration
{
public:
	AssocDeclaration(
		OW_List< Qualifier * >* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		const SuperClass* pNewSuperClass,
		OW_List< AssociationFeature * >* pNewAssociationFeature,
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
	
	OW_AutoPtrNoVec< OW_List< Qualifier * > > pQualifier;
	OW_AutoPtrNoVec< const ClassName > pClassName;
	OW_AutoPtrNoVec< const Alias > pAlias;
	OW_AutoPtrNoVec< const SuperClass > pSuperClass;
	OW_AutoPtrNoVec< OW_List< AssociationFeature * > > pAssociationFeature;
	lineInfo theLineInfo;
};


class ClassDeclaration
{
public:
	ClassDeclaration(
		OW_List< Qualifier * >* pNewQualifier,
		const ClassName* pNewClassName,
		const Alias* pNewAlias,
		const SuperClass* pNewSuperClass,
		OW_List< ClassFeature * >* pNewClassFeature,
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
	
	OW_AutoPtrNoVec< OW_List< Qualifier * > > pQualifier;
	OW_AutoPtrNoVec< const ClassName > pClassName;
	OW_AutoPtrNoVec< const Alias > pAlias;
	OW_AutoPtrNoVec< const SuperClass > pSuperClass;
	OW_AutoPtrNoVec< OW_List< ClassFeature * > > pClassFeature;
	lineInfo theLineInfo;
};


class PragmaParameter
{
public:
	PragmaParameter( const OW_String* pNewPragmaParameter )
		: pPragmaParameter(pNewPragmaParameter)
	{}

	virtual ~PragmaParameter(){}

	void Accept( Visitor *pV ) const { pV->VisitPragmaParameter( this ); }

	OW_AutoPtrNoVec< const OW_String > pPragmaParameter;
};


class PragmaName
{
public:
	PragmaName( const OW_String* pNewPragmaName )
		: pPragmaName(pNewPragmaName)
	{}

	virtual ~PragmaName(){}

	void Accept( Visitor *pV ) const { pV->VisitPragmaName( this ); }

	OW_AutoPtrNoVec< const OW_String > pPragmaName;
};


class CompilerDirective
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
	
	OW_AutoPtrNoVec< const PragmaName > pPragmaName;
	OW_AutoPtrNoVec< const PragmaParameter > pPragmaParameter;
	lineInfo theLineInfo;
};


class MOFProduction {
public:
	virtual ~MOFProduction() {}
	virtual void Accept( Visitor * ) const = 0;
};


class MOFProductionInstanceDeclaration : public MOFProduction
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
	
	OW_AutoPtrNoVec< const InstanceDeclaration > pInstanceDeclaration;
};


class MOFProductionQualifierDeclaration : public MOFProduction
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
	
	OW_AutoPtrNoVec< const QualifierDeclaration > pQualifierDeclaration;
};


class MOFProductionIndicDeclaration : public MOFProduction
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
	
	OW_AutoPtrNoVec< const IndicDeclaration > pIndicDeclaration;
};


class MOFProductionAssocDeclaration : public MOFProduction
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
	
	OW_AutoPtrNoVec< const AssocDeclaration > pAssocDeclaration;
};


class MOFProductionClassDeclaration : public MOFProduction
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
	
	OW_AutoPtrNoVec< const ClassDeclaration > pClassDeclaration;
};


class MOFProductionCompilerDirective : public MOFProduction
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

	OW_AutoPtrNoVec< const CompilerDirective > pCompilerDirective;
};


class MOFSpecification {
public:
	MOFSpecification( OW_List< MOFProduction * >* pNewMOFProduction )
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
	
	OW_AutoPtrNoVec< OW_List< MOFProduction * > > pMOFProduction;
};




#endif
