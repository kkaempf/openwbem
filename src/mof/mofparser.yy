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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

%{
/*************** Includes and Defines *****************************/
#include <stdio.h> /* for debugging */
#include <assert.h>
#include "Grammar.h"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "MofCompiler.hpp"

//#define YYDEBUG 1

// Lexer functions
void lexIncludeFile( void* context, const OW_String& filename );
void yyerror( char* );

/* This is so we can avoid static variables and pass a pointer into yyparse */
#define YYPARSE_PARAM context
#define MOF_COMPILER (reinterpret_cast<MofCompiler*>(context))
%}

/* avoid non-reentrant global variables */
%pure_parser


/* List of tokens */
%token ANY_TOK AS_TOK ASSOCIATION_TOK CLASS_TOK DISABLEOVERRIDE_TOK
%token DT_BOOL_TOK DT_CHAR16_TOK
%token DT_DATETIME_TOK DT_REAL32_TOK DT_REAL64_TOK
%token DT_SINT16_TOK DT_SINT32_TOK DT_SINT64_TOK
%token DT_SINT8_TOK DT_STR_TOK DT_UINT16_TOK DT_UINT32_TOK
%token DT_UINT64_TOK DT_UINT8_TOK ENABLEOVERRIDE_TOK
%token FALSE_TOK FLAVOR_TOK INDICATION_TOK INSTANCE_TOK
%token METHOD_TOK NULL_TOK OF_TOK PARAMETER_TOK PRAGMA_TOK
%token PROPERTY_TOK QUALIFIER_TOK REF_TOK REFERENCE_TOK
%token RESTRICTED_TOK SCHEMA_TOK SCOPE_TOK TOSUBCLASS_TOK
%token TRANSLATABLE_TOK TRUE_TOK
%token LPAREN_TOK RPAREN_TOK LBRACE_TOK RBRACE_TOK SEMICOLON_TOK
%token LBRACK_TOK RBRACK_TOK COMMA_TOK DOLLAR_TOK COLON_TOK EQUALS_TOK
%token IDENTIFIER_TOK stringValue floatValue charValue
%token binaryValue octalValue decimalValue hexValue

/* here is the stuff to make our C++ AST work */
%union {
	MOFSpecification*				pMOFSpecification;
	OW_List<MOFProduction*>*		pMOFProductionList;
	MOFProduction*					pMOFProduction;
	IndicDeclaration*				pIndicDeclaration;
	ClassDeclaration*				pClassDeclaration;
	PropertyDeclaration*			pPropertyDeclaration;
	ObjectRef*						pObjectRef;
	Parameter*						pParameter;
	Array*							pArray;
	CompilerDirective*			pCompilerDirective;
	MetaElement*					pMetaElement;
	OW_List<MetaElement*>*			pMetaElementList;
	Initializer*					pInitializer;
	SuperClass*						pSuperClass;
	AssociationFeature*			pAssociationFeature;
	OW_List<AssociationFeature*>*	pAssociationFeatureList;
	QualifierParameter*			pQualifierParameter;
	QualifierDeclaration*		pQualifierDeclaration;
	PragmaParameter*				pPragmaParameter;
	AssocDeclaration*				pAssocDeclaration;
	DefaultValue*					pDefaultValue;
	ClassFeature*					pClassFeature;
	OW_List<ClassFeature*>*			pClassFeatureList;
	ReferenceInitializer*		pReferenceInitializer;
	QualifierType*					pQualifierType;
	DefaultFlavor*					pDefaultFlavor;
	OW_String*						pString;
        IntegerValue*                   pIntegerValue;
        ConstantValue*                  pConstantValue;
	ArrayInitializer*				pArrayInitializer;
	ValueInitializer*				pValueInitializer;
	OW_List<ValueInitializer*>*	pValueInitializerList;
	Flavor*							pFlavor;
	OW_List<Flavor*>*					pFlavorList;
	OW_List<ConstantValue*>*				pConstantValueList;
	Alias*							pAlias;
	OW_List<Qualifier*>*				pQualifierList;
	ReferenceDeclaration*		pReferenceDeclaration;
	MethodDeclaration*			pMethodDeclaration;
	Qualifier*						pQualifier;
	OW_List<Parameter*>*				pParameterList;
	Scope*							pScope;
	InstanceDeclaration*			pInstanceDeclaration;
	PragmaName*						pPragmaName;
	ClassName*						pClassName;
	AliasIdentifier*				pAliasIdentifier;
	QualifierName*					pQualifierName;
	PropertyName*					pPropertyName;
	ReferenceName*					pReferenceName;
	MethodName*						pMethodName;
	ParameterName*					pParameterName;
	DataType*						pDataType;
	ObjectHandle*					pObjectHandle;
}

%{
#define YYLEX_PARAM context
int yylex(YYSTYPE *yylval, void* YYLEX_PARAM);
%}

%type <pMOFSpecification>			mofSpecification
%type <pMOFProductionList>			mofProductionList
%type <pMOFProduction>				mofProduction
%type <pIndicDeclaration>			indicDeclaration
%type <pClassDeclaration>			classDeclaration
%type <pPropertyDeclaration>		propertyDeclaration
%type <pObjectRef>					objectRef
%type <pParameter>					parameter
%type <pArray>							array
%type <pCompilerDirective>			compilerDirective
%type <pMetaElement>					metaElement
%type <pMetaElementList>			metaElementList
%type <pInitializer>					initializer
%type <pSuperClass>					superClass
%type <pAssociationFeature>		associationFeature
%type <pAssociationFeatureList>	associationFeatureList
%type <pQualifierParameter>		qualifierParameter
%type <pQualifierDeclaration>		qualifierDeclaration
%type <pPragmaParameter>			pragmaParameter
%type <pAssocDeclaration>			assocDeclaration
%type <pDefaultValue>				defaultValue
%type <pClassFeature>				classFeature
%type <pClassFeatureList>			classFeatureList
%type <pReferenceInitializer>		referenceInitializer
%type <pQualifierType>				qualifierType
%type <pDefaultFlavor>				defaultFlavor

%type <pString>						IDENTIFIER stringValue binaryValue octalValue decimalValue hexValue floatValue charValue booleanValue nullValue IDENTIFIER_TOK ANY_TOK AS_TOK CLASS_TOK FLAVOR_TOK INSTANCE_TOK METHOD_TOK OF_TOK PARAMETER_TOK PRAGMA_TOK PROPERTY_TOK QUALIFIER_TOK REFERENCE_TOK RESTRICTED_TOK SCHEMA_TOK SCOPE_TOK ENABLEOVERRIDE_TOK DISABLEOVERRIDE_TOK TOSUBCLASS_TOK TRANSLATABLE_TOK DT_UINT8_TOK DT_SINT8_TOK DT_UINT16_TOK DT_SINT16_TOK DT_UINT32_TOK DT_SINT32_TOK DT_UINT64_TOK DT_SINT64_TOK DT_REAL32_TOK DT_REAL64_TOK DT_CHAR16_TOK DT_STR_TOK DT_BOOL_TOK DT_DATETIME_TOK ASSOCIATION_TOK INDICATION_TOK LPAREN_TOK RPAREN_TOK LBRACE_TOK RBRACE_TOK SEMICOLON_TOK LBRACK_TOK RBRACK_TOK COMMA_TOK DOLLAR_TOK COLON_TOK EQUALS_TOK FALSE_TOK TRUE_TOK NULL_TOK REF_TOK


%type <pIntegerValue>                   integerValue
%type <pConstantValue>                  constantValue
%type <pPragmaName>					pragmaName
%type <pClassName>					className
%type <pQualifierName>				qualifierName
%type <pPropertyName>				propertyName
%type <pReferenceName>				referenceName
%type <pMethodName>					methodName
%type <pParameterName>				parameterName
%type <pAliasIdentifier>			aliasIdentifier
%type <pObjectHandle>				objectHandle
%type <pDataType>						dataType
%type <pArrayInitializer>			arrayInitializer
%type <pValueInitializer>			valueInitializer
%type <pValueInitializerList>		valueInitializerList
%type <pFlavor>						flavor
%type <pFlavorList>					flavorList flavorListWithComma
%type <pConstantValueList>			constantValueList
%type <pAlias>							alias
%type <pQualifierList>				qualifierList qualifierListEmpty
%type <pReferenceDeclaration>		referenceDeclaration
%type <pMethodDeclaration>			methodDeclaration
%type <pQualifier>					qualifier
%type <pParameterList>				parameterList
%type <pScope>							scope
%type <pInstanceDeclaration>		instanceDeclaration


%start mofSpecification

%%

/* rules */
mofSpecification:
	mofProductionList {
		OW_AutoPtr<MOFSpecification> p(new MOFSpecification($1));
		MOF_COMPILER->mofSpecification = p;
		}
	;

mofProductionList: /* empty */ {$$ = new OW_List<MOFProduction*>; }
	| mofProductionList mofProduction {$1->push_back($2); $$ = $1;}
	;

mofProduction:
	compilerDirective {$$ = new MOFProductionCompilerDirective($1); }
	| classDeclaration {$$ = new MOFProductionClassDeclaration($1); }
	| assocDeclaration {$$ = new MOFProductionAssocDeclaration($1); }
	| indicDeclaration {$$ = new MOFProductionIndicDeclaration($1); }
	| qualifierDeclaration {$$ = new MOFProductionQualifierDeclaration($1); }
	| instanceDeclaration {$$ = new MOFProductionInstanceDeclaration($1); }
	;

compilerDirective:
	PRAGMA_TOK pragmaName LPAREN_TOK pragmaParameter RPAREN_TOK
		{
			$$ = new CompilerDirective($2, $4, MOF_COMPILER->theLineInfo);
			if ($2->pPragmaName->equalsIgnoreCase("include"))
			{
				lexIncludeFile(MOF_COMPILER, MOF_COMPILER->fixParsedString(*($4->pPragmaParameter)));
			}
                        delete $1;
                        delete $3;
                        delete $5;
		}
	;

pragmaName:
	IDENTIFIER {$$ = new PragmaName($1); }
	;

pragmaParameter:
	stringValue {$$ = new PragmaParameter($1); }
	;

classDeclaration:
	CLASS_TOK className LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration(0, $2, 0, 0, $4, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $3;
                delete $5;
                delete $6;
        }
	| CLASS_TOK className superClass LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration(0, $2, 0, $3, $5, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $4;
                delete $6;
                delete $7;
        }
	| CLASS_TOK className alias LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration(0, $2, $3, 0, $5, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $4;
                delete $6;
                delete $7;
        }
	| CLASS_TOK className alias superClass LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration(0, $2, $3, $4, $6, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $5;
                delete $7;
                delete $8;
        }
	| qualifierList CLASS_TOK className LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration($1, $3, 0, 0, $5, MOF_COMPILER->theLineInfo);
                delete $2;
                delete $4;
                delete $6;
                delete $7;
        }
	| qualifierList CLASS_TOK className superClass LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration($1, $3, 0, $4, $6, MOF_COMPILER->theLineInfo);
                delete $2;
                delete $5;
                delete $7;
                delete $8;
        }
	| qualifierList CLASS_TOK className alias LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration($1, $3, $4, 0, $6, MOF_COMPILER->theLineInfo);
                delete $2;
                delete $5;
                delete $7;
                delete $8;
        }
	| qualifierList CLASS_TOK className alias superClass LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration($1, $3, $4, $5, $7, MOF_COMPILER->theLineInfo);
                delete $2;
                delete $5;
                delete $8;
                delete $9;
        }
	;

classFeatureList: /* empty */ {$$ = new OW_List<ClassFeature*>; }
	| classFeatureList classFeature {$1->push_back($2); $$ = $1;}
	;

assocDeclaration:
	LBRACK_TOK ASSOCIATION_TOK qualifierListEmpty RBRACK_TOK CLASS_TOK className LBRACE_TOK associationFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new AssocDeclaration($3, $6, 0, 0, $8, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $2;
                delete $4;
                delete $5;
                delete $7;
                delete $9;
                delete $10;
        }
	| LBRACK_TOK ASSOCIATION_TOK qualifierListEmpty RBRACK_TOK CLASS_TOK className superClass LBRACE_TOK associationFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new AssocDeclaration($3, $6, 0, $7, $9, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $2;
                delete $4;
                delete $5;
                delete $8;
                delete $10;
                delete $11;
        }
	| LBRACK_TOK ASSOCIATION_TOK qualifierListEmpty RBRACK_TOK CLASS_TOK className alias LBRACE_TOK associationFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new AssocDeclaration($3, $6, $7, 0, $9, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $2;
                delete $4;
                delete $5;
                delete $8;
                delete $10;
                delete $11;
        }
	| LBRACK_TOK ASSOCIATION_TOK qualifierListEmpty RBRACK_TOK CLASS_TOK className alias superClass LBRACE_TOK associationFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new AssocDeclaration($3, $6, $7, $8, $10, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $2;
                delete $4;
                delete $5;
                delete $9;
                delete $11;
                delete $12;
        }
	;
	
qualifierListEmpty: /* empty */ {$$ = new OW_List<Qualifier*>; }
	| qualifierListEmpty COMMA_TOK qualifier
        {$1->push_back($3); $$ = $1; delete $2; }
	;

associationFeatureList: /* empty */ {$$ = new OW_List<AssociationFeature*>; }
	| associationFeatureList associationFeature {$1->push_back($2); $$ = $1;}
	;

indicDeclaration:
	LBRACK_TOK INDICATION_TOK qualifierListEmpty RBRACK_TOK CLASS_TOK className LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new IndicDeclaration($3, $6, 0, 0, $8, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $2;
                delete $4;
                delete $5;
                delete $7;
                delete $9;
                delete $10;
        }
	| LBRACK_TOK INDICATION_TOK qualifierListEmpty RBRACK_TOK CLASS_TOK className superClass LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new IndicDeclaration($3, $6, 0, $7, $9, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $2;
                delete $4;
                delete $5;
                delete $8;
                delete $10;
                delete $11;
        }
	| LBRACK_TOK INDICATION_TOK qualifierListEmpty RBRACK_TOK CLASS_TOK className alias LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new IndicDeclaration($3, $6, $7, 0, $9, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $2;
                delete $4;
                delete $5;
                delete $8;
                delete $10;
                delete $11;
        }
	| LBRACK_TOK INDICATION_TOK qualifierListEmpty RBRACK_TOK CLASS_TOK className alias superClass LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new IndicDeclaration($3, $6, $7, $8, $10, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $2;
                delete $4;
                delete $5;
                delete $9;
                delete $11;
                delete $12;
        }
	;

className:
	IDENTIFIER {$$ = new ClassName($1); }
	;

alias:
	AS_TOK aliasIdentifier {$$ = new Alias($2); delete $1;}
	;

aliasIdentifier:
	DOLLAR_TOK IDENTIFIER
        {
                $$ = new AliasIdentifier($2, MOF_COMPILER->theLineInfo);
                delete $1;
        }
	;

superClass:
	COLON_TOK className {$$ = new SuperClass($2); delete $1; }
	;

classFeature:
	propertyDeclaration {$$ = new ClassFeaturePropertyDeclaration($1); }
	| methodDeclaration {$$ = new ClassFeatureMethodDeclaration($1); }
	| referenceDeclaration {$$ = new ClassFeatureReferenceDeclaration($1); }
	;

associationFeature:
	classFeature {$$ = new AssociationFeatureClassFeature($1); }
	/*	| referenceDeclaration */
	;

qualifierList:
	LBRACK_TOK qualifier qualifierListEmpty RBRACK_TOK
		{$3->push_front($2); $$ = $3; delete $1; delete $4; }
	;

qualifier:
	qualifierName {$$ = new Qualifier($1, 0, 0, MOF_COMPILER->theLineInfo); }
	| qualifierName COLON_TOK flavorList
        {
                $$ = new Qualifier($1, 0, $3, MOF_COMPILER->theLineInfo);
                delete $2;
        }
	| qualifierName qualifierParameter{$$ = new Qualifier($1, $2, 0, MOF_COMPILER->theLineInfo); }
	| qualifierName qualifierParameter COLON_TOK flavorList
	{
                $$ = new Qualifier($1, $2, $4, MOF_COMPILER->theLineInfo);
                delete $3;
        }
	;

flavorList:
	flavor {$$ = new OW_List<Flavor*>(1, $1); }
	| flavorList flavor {$1->push_back($2); $$ = $1;}
	;

qualifierParameter:
	LPAREN_TOK constantValue RPAREN_TOK
        {
                $$ = new QualifierParameterConstantValue($2, MOF_COMPILER->theLineInfo);
                delete $1;
                delete $3;
        }
	| arrayInitializer {$$ = new QualifierParameterArrayInitializer($1, MOF_COMPILER->theLineInfo); }
	;

flavor:
	ENABLEOVERRIDE_TOK {$$ = new Flavor($1, MOF_COMPILER->theLineInfo); }
	| DISABLEOVERRIDE_TOK {$$ = new Flavor($1, MOF_COMPILER->theLineInfo); }
	| RESTRICTED_TOK {$$ = new Flavor($1, MOF_COMPILER->theLineInfo); }
	| TOSUBCLASS_TOK {$$ = new Flavor($1, MOF_COMPILER->theLineInfo); }
	| TRANSLATABLE_TOK {$$ = new Flavor($1, MOF_COMPILER->theLineInfo); }
	;

propertyDeclaration:
	dataType propertyName SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration(0, $1, $2, 0, 0, MOF_COMPILER->theLineInfo);
		delete $3;
	}
	| dataType propertyName defaultValue SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration(0, $1, $2, 0, $3, MOF_COMPILER->theLineInfo);
		delete $4;
	}
	| dataType propertyName array SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration(0, $1, $2, $3, 0, MOF_COMPILER->theLineInfo);
		delete $4;
	}
	| dataType propertyName array defaultValue SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration(0, $1, $2, $3, $4, MOF_COMPILER->theLineInfo);
		delete $5;
	}
	| qualifierList dataType propertyName SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration($1, $2, $3, 0, 0, MOF_COMPILER->theLineInfo);
		delete $4;
	}
	| qualifierList dataType propertyName defaultValue SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration($1, $2, $3, 0, $4, MOF_COMPILER->theLineInfo);
		delete $5;
	}
	| qualifierList dataType propertyName array SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration($1, $2, $3, $4, 0, MOF_COMPILER->theLineInfo);
		delete $5;
	}
	| qualifierList dataType propertyName array defaultValue SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration($1, $2, $3, $4, $5, MOF_COMPILER->theLineInfo);
		delete $6;
	}
	;

referenceDeclaration:
	objectRef referenceName SEMICOLON_TOK
	{
			$$ = new ReferenceDeclaration(0, $1, $2, 0);
			delete $3;
	}
	| objectRef referenceName defaultValue SEMICOLON_TOK
	{
			$$ = new ReferenceDeclaration(0, $1, $2, $3);
			delete $4;
	}
	| qualifierList objectRef referenceName SEMICOLON_TOK
	{
			$$ = new ReferenceDeclaration($1, $2, $3, 0);
			delete $4;
	}
	| qualifierList objectRef referenceName defaultValue SEMICOLON_TOK
	{
			$$ = new ReferenceDeclaration($1, $2, $3, $4);
			delete $5;
	}
	;

methodDeclaration:
	dataType methodName LPAREN_TOK RPAREN_TOK SEMICOLON_TOK
	{
			$$ = new MethodDeclaration(0, $1, $2, 0);
			delete $4;
			delete $5;
			delete $3;
	}
	| dataType methodName LPAREN_TOK parameterList RPAREN_TOK SEMICOLON_TOK
	{
			$$ = new MethodDeclaration(0, $1, $2, $4);
			delete $3;
			delete $5;
			delete $6;
	}
	| qualifierList dataType methodName LPAREN_TOK RPAREN_TOK SEMICOLON_TOK
	{
			$$ = new MethodDeclaration($1, $2, $3, 0);
			delete $4;
			delete $5;
			delete $6;
	}
	| qualifierList dataType methodName LPAREN_TOK parameterList RPAREN_TOK SEMICOLON_TOK
	{
			$$ = new MethodDeclaration($1, $2, $3, $5);
			delete $4;
			delete $6;
			delete $7;
	}
	;

propertyName:
	IDENTIFIER {$$ = new PropertyName($1); }
	;

referenceName:
	IDENTIFIER {$$ = new ReferenceName($1); }
	;

methodName:
	IDENTIFIER {$$ = new MethodName($1); }
	;

dataType:
	DT_UINT8_TOK {$$ = new DataType($1); }
	| DT_SINT8_TOK {$$ = new DataType($1); }
	| DT_UINT16_TOK {$$ = new DataType($1); }
	| DT_SINT16_TOK {$$ = new DataType($1); }
	| DT_UINT32_TOK {$$ = new DataType($1); }
	| DT_SINT32_TOK {$$ = new DataType($1); }
	| DT_UINT64_TOK {$$ = new DataType($1); }
	| DT_SINT64_TOK {$$ = new DataType($1); }
	| DT_REAL32_TOK {$$ = new DataType($1); }
	| DT_REAL64_TOK {$$ = new DataType($1); }
	| DT_CHAR16_TOK {$$ = new DataType($1); }
	| DT_STR_TOK {$$ = new DataType($1); }
	| DT_BOOL_TOK {$$ = new DataType($1); }
	| DT_DATETIME_TOK {$$ = new DataType($1); }
	;

objectRef:
	className REF_TOK {$$ = new ObjectRef($1); delete $2; }
	;

parameterList:
	parameter {$$ = new OW_List<Parameter*>(1, $1); }
	| parameterList COMMA_TOK parameter {$1->push_back($3); $$=$1; delete $2;}
	;

parameter:
	dataType parameterName
		{$$ = new ParameterDataType(0, $1, $2, 0); }
	| dataType parameterName array
		{$$ = new ParameterDataType(0, $1, $2, $3); }
	| qualifierList dataType parameterName
		{$$ = new ParameterDataType($1, $2, $3, 0); }
	| qualifierList dataType parameterName array
		{$$ = new ParameterDataType($1, $2, $3, $4); }
	| objectRef parameterName
		{$$ = new ParameterObjectRef(0, $1, $2, 0); }
	| objectRef parameterName array
		{$$ = new ParameterObjectRef(0, $1, $2, $3); }
	| qualifierList objectRef parameterName
		{$$ = new ParameterObjectRef($1, $2, $3, 0); }
	| qualifierList objectRef parameterName array
		{$$ = new ParameterObjectRef($1, $2, $3, $4); }
	;

parameterName:
	IDENTIFIER {$$ = new ParameterName($1); }
	;

array:
	LBRACK_TOK RBRACK_TOK {$$ = new Array(0); delete $1; delete $2;}
	| LBRACK_TOK integerValue RBRACK_TOK /* must be positive value */
		{$$ = new Array($2); delete $1; delete $3;}
	;

defaultValue:
	EQUALS_TOK initializer {$$ = new DefaultValue($2); delete $1;}
	;

initializer:
	constantValue {$$ = new InitializerConstantValue($1); }
	| arrayInitializer {$$ = new InitializerArrayInitializer($1); }
	| referenceInitializer {$$ = new InitializerReferenceInitializer($1); }
	;

arrayInitializer:
	LBRACE_TOK constantValueList RBRACE_TOK
		{$$ = new ArrayInitializer($2); delete $1; delete $3;}
	;

constantValueList:
	constantValue {$$ = new OW_List<ConstantValue*>(1, $1); }
	| constantValueList COMMA_TOK constantValue
		{$1->push_back($3); $$ = $1; delete $2;}
	;

constantValue:
	integerValue {$$ = new ConstantValueIntegerValue($1); }
	| floatValue {$$ = new ConstantValueFloatValue($1); }
	| charValue {$$ = new ConstantValueCharValue($1); }
	| stringValue {$$ = new ConstantValueStringValue($1); }
	| booleanValue {$$ = new ConstantValueBooleanValue($1); }
	| nullValue {$$ = new ConstantValueNullValue($1); }
	;

integerValue:
	binaryValue {$$ = new IntegerValueBinaryValue($1); }
	| octalValue {$$ = new IntegerValueOctalValue($1); }
	| decimalValue {$$ = new IntegerValueDecimalValue($1); }
	| hexValue {$$ = new IntegerValueHexValue($1); }
	;

referenceInitializer:
	objectHandle {$$ = new ReferenceInitializerObjectHandle($1); }
	| aliasIdentifier {$$ = new ReferenceInitializerAliasIdentifier($1); }
	;

objectHandle:
	IDENTIFIER {$$ = new ObjectHandle($1); }
	;
/*
referenceInitializer:
	objectHandle
	| aliasIdentifier
	;

objectHandle:
	"\"" modelPath "\""
	| "\"" namespaceHandle COLON_TOK modelPath "\""
	;

namespaceHandle:
	IDENTIFIER
	;

modelPath:
	className DOT_TOK keyValuePairList
	;


keyValuePairList:
	keyValuePair
	| keyValuePairList COMMA_TOK keyValuePair
	;

keyValuePair:
	IDENTIFIER EQUALS_TOK initializer
	| IDENTIFIER EQUALS_TOK initializer
	;
*/
qualifierDeclaration:
	QUALIFIER_TOK qualifierName qualifierType scope SEMICOLON_TOK
	{
			$$= new QualifierDeclaration($2, $3, $4, 0, MOF_COMPILER->theLineInfo);
			delete $1;
			delete $5;
	}
	| QUALIFIER_TOK qualifierName qualifierType scope defaultFlavor SEMICOLON_TOK
	{
			$$= new QualifierDeclaration($2, $3, $4, $5, MOF_COMPILER->theLineInfo);
			delete $1;
			delete $6;
	}
	;

qualifierName:
	IDENTIFIER {$$ = new QualifierName($1); }
	;

qualifierType:
	COLON_TOK dataType
	{
			$$ = new QualifierType($2, 0, 0);
			delete $1;
	}
	| COLON_TOK dataType defaultValue
	{
			$$ = new QualifierType($2, 0, $3);
			delete $1;
	}
	| COLON_TOK dataType array
	{
			$$ = new QualifierType($2, $3, 0);
			delete $1;
	}
	| COLON_TOK dataType array defaultValue
	{
			$$ = new QualifierType($2, $3, $4);
			delete $1;
	}
	;

scope:
	COMMA_TOK SCOPE_TOK LPAREN_TOK metaElementList RPAREN_TOK
	{
			$$ = new Scope($4);
			delete $1;
			delete $2;
			delete $3;
			delete $5;
	}
	;

metaElementList:
	metaElement {$$ = new OW_List<MetaElement*>(1, $1); }
	| metaElementList COMMA_TOK metaElement
		{$1->push_back($3); $$ = $1; delete $2; }
	;

metaElement:
	SCHEMA_TOK {$$ = new MetaElement($1, MOF_COMPILER->theLineInfo); }
	| CLASS_TOK {$$ = new MetaElement($1, MOF_COMPILER->theLineInfo); }
	| ASSOCIATION_TOK {$$ = new MetaElement($1, MOF_COMPILER->theLineInfo); }
	| INDICATION_TOK {$$ = new MetaElement($1, MOF_COMPILER->theLineInfo); }
	| QUALIFIER_TOK {$$ = new MetaElement($1, MOF_COMPILER->theLineInfo); }
	| PROPERTY_TOK {$$ = new MetaElement($1, MOF_COMPILER->theLineInfo); }
	| REFERENCE_TOK {$$ = new MetaElement($1, MOF_COMPILER->theLineInfo); }
	| METHOD_TOK {$$ = new MetaElement($1, MOF_COMPILER->theLineInfo); }
	| PARAMETER_TOK {$$ = new MetaElement($1, MOF_COMPILER->theLineInfo); }
	| ANY_TOK {$$ = new MetaElement($1, MOF_COMPILER->theLineInfo); }
	;

defaultFlavor:
	COMMA_TOK FLAVOR_TOK LPAREN_TOK flavorListWithComma RPAREN_TOK
	{
			$$ = new DefaultFlavor($4);
			delete $1;
			delete $2;
			delete $3;
			delete $5;
	}
	;

flavorListWithComma:
	flavor {$$ = new OW_List<Flavor*>(1, $1); }
	| flavorListWithComma COMMA_TOK flavor
	{
		$1->push_back($3); $$ = $1;
		delete $2;
	}
	;

instanceDeclaration:
	INSTANCE_TOK OF_TOK className LBRACE_TOK valueInitializerList RBRACE_TOK SEMICOLON_TOK
	{
			$$ = new InstanceDeclaration(0, $3, 0, $5, MOF_COMPILER->theLineInfo);
			delete $1;
			delete $2;
			delete $4;
			delete $6;
			delete $7;
	}
	| INSTANCE_TOK OF_TOK className alias LBRACE_TOK valueInitializerList RBRACE_TOK SEMICOLON_TOK
	{
			$$ = new InstanceDeclaration(0, $3, $4, $6, MOF_COMPILER->theLineInfo);
			delete $1;
			delete $2;
			delete $5;
			delete $7;
			delete $8;
	}
	| qualifierList INSTANCE_TOK OF_TOK className LBRACE_TOK valueInitializerList RBRACE_TOK SEMICOLON_TOK
	{
			$$ = new InstanceDeclaration($1, $4, 0, $6, MOF_COMPILER->theLineInfo);
			delete $2;
			delete $3;
			delete $5;
			delete $7;
			delete $8;
	}
	| qualifierList INSTANCE_TOK OF_TOK className alias LBRACE_TOK valueInitializerList RBRACE_TOK SEMICOLON_TOK
	{
			$$ = new InstanceDeclaration($1, $4, $5, $7, MOF_COMPILER->theLineInfo);
			delete $2;
			delete $3;
			delete $6;
			delete $8;
			delete $9;
	}
	;

valueInitializerList:
	valueInitializer {$$ = new OW_List<ValueInitializer*>(1, $1); }
	| valueInitializerList valueInitializer
		{$1->push_back($2); $$ = $1;}
	;

valueInitializer:
	IDENTIFIER defaultValue SEMICOLON_TOK
		{$$ = new ValueInitializer(0, $1, $2); delete $3; }
	| qualifierList IDENTIFIER defaultValue SEMICOLON_TOK
		{$$ = new ValueInitializer($1, $2, $3); delete $4; }
	;

booleanValue:
	FALSE_TOK {$$ = $1;}
	| TRUE_TOK {$$ = $1;}
	;

nullValue:
	NULL_TOK {$$ = $1;}
	;
	
IDENTIFIER:
	IDENTIFIER_TOK {$$ = $1;}
	| ANY_TOK {$$ = $1;}
	| AS_TOK {$$ = $1;}
	/*| ASSOCIATION_TOK*/
	| CLASS_TOK {$$ = $1;}
	| DISABLEOVERRIDE_TOK {$$ = $1;}
	| dataType {$$ = const_cast<OW_String*>($1->pDataType.release()); delete $1;}
	| ENABLEOVERRIDE_TOK {$$ = $1;}
	| FLAVOR_TOK {$$ = $1;}
	/*| INDICATION_TOK*/
	| INSTANCE_TOK {$$ = $1;}
	| METHOD_TOK {$$ = $1;}
	/*| nullValue*/
	| OF_TOK {$$ = $1;}
	| PARAMETER_TOK {$$ = $1;}
	| PRAGMA_TOK {$$ = $1;}
	| PROPERTY_TOK {$$ = $1;}
	| QUALIFIER_TOK {$$ = $1;}
	/*| REF_TOK*/
	| REFERENCE_TOK {$$ = $1;}
	| RESTRICTED_TOK {$$ = $1;}
	| SCHEMA_TOK {$$ = $1;}
	| SCOPE_TOK {$$ = $1;}
	| TOSUBCLASS_TOK {$$ = $1;}
	| TRANSLATABLE_TOK {$$ = $1;}
	/*| booleanValue*/
	;

%%
void yyerror(char* string)
{
	OW_THROW(OW_Exception, string);
}


