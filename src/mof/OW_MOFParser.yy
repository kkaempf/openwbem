/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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


%{
/*************** Includes and Defines *****************************/
#include <stdio.h> /* for debugging */
#include <assert.h>
#include "OW_MOFGrammar.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_MOFCompiler.hpp"
#include "OW_MOFParserDecls.hpp"
#include "OW_Format.hpp"

using namespace OpenWBEM;
using namespace OpenWBEM::MOF;

// Lexer functions
void lexIncludeFile( CompilerState* context, const String& filename );
void owmoferror( YYLTYPE * p_loc, CompilerState* MOF_COMPILER_STATE, ParseError * p_err, const char* );

%}

%defines
/* avoid non-reentrant global variables */
%pure_parser
%parse-param {CompilerState * MOF_COMPILER_STATE}
%parse-param {ParseError * p_err}
%lex-param {CompilerState * MOF_COMPILER_STATE}
%lex-param {ParseError * p_err}

%name-prefix="owmof"
%error-verbose
%locations
%debug

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
%token SCANNER_ERROR

/* here is the stuff to make our C++ AST work */
%union {
	OpenWBEM::List<OpenWBEM::MOF::MOFProduction*>*          pMOFProductionList;
	OpenWBEM::MOF::MOFProduction*                           pMOFProduction;
	OpenWBEM::MOF::IndicDeclaration*                        pIndicDeclaration;
	OpenWBEM::MOF::ClassDeclaration*                        pClassDeclaration;
	OpenWBEM::MOF::PropertyDeclaration*                     pPropertyDeclaration;
	OpenWBEM::MOF::ObjectRef*                               pObjectRef;
	OpenWBEM::MOF::Parameter*                               pParameter;
	OpenWBEM::MOF::Array*                                   pArray;
	OpenWBEM::MOF::CompilerDirective*                       pCompilerDirective;
	OpenWBEM::MOF::MetaElement*                             pMetaElement;
	OpenWBEM::List<OpenWBEM::MOF::MetaElement*>*            pMetaElementList;
	OpenWBEM::MOF::Initializer*                             pInitializer;
	OpenWBEM::MOF::SuperClass*                              pSuperClass;
	OpenWBEM::MOF::AssociationFeature*                      pAssociationFeature;
	OpenWBEM::List<OpenWBEM::MOF::AssociationFeature*>*     pAssociationFeatureList;
	OpenWBEM::MOF::QualifierParameter*                      pQualifierParameter;
	OpenWBEM::MOF::QualifierDeclaration*                    pQualifierDeclaration;
	OpenWBEM::MOF::PragmaParameter*                         pPragmaParameter;
	OpenWBEM::MOF::AssocDeclaration*                        pAssocDeclaration;
	OpenWBEM::MOF::DefaultValue*                            pDefaultValue;
	OpenWBEM::MOF::ClassFeature*                            pClassFeature;
	OpenWBEM::List<OpenWBEM::MOF::ClassFeature*>*           pClassFeatureList;
	OpenWBEM::MOF::ReferenceInitializer*                    pReferenceInitializer;
	OpenWBEM::MOF::QualifierType*                           pQualifierType;
	OpenWBEM::MOF::DefaultFlavor*                           pDefaultFlavor;
	OpenWBEM::String*                                       pString;
	OpenWBEM::MOF::IntegerValue*                            pIntegerValue;
	OpenWBEM::MOF::ConstantValue*                           pConstantValue;
	OpenWBEM::MOF::ArrayInitializer*                        pArrayInitializer;
	OpenWBEM::MOF::ValueInitializer*                        pValueInitializer;
	OpenWBEM::List<OpenWBEM::MOF::ValueInitializer*>*       pValueInitializerList;
	OpenWBEM::MOF::Flavor*                                  pFlavor;
	OpenWBEM::List<OpenWBEM::MOF::Flavor*>*                 pFlavorList;
	OpenWBEM::List<OpenWBEM::MOF::ConstantValue*>*          pConstantValueList;
	OpenWBEM::MOF::Alias*                                   pAlias;
	OpenWBEM::List<OpenWBEM::MOF::Qualifier*>*              pQualifierList;
	OpenWBEM::MOF::ReferenceDeclaration*                    pReferenceDeclaration;
	OpenWBEM::MOF::MethodDeclaration*                       pMethodDeclaration;
	OpenWBEM::MOF::Qualifier*                               pQualifier;
	OpenWBEM::List<OpenWBEM::MOF::Parameter*>*              pParameterList;
	OpenWBEM::MOF::Scope*                                   pScope;
	OpenWBEM::MOF::InstanceDeclaration*                     pInstanceDeclaration;
	OpenWBEM::MOF::PragmaName*                              pPragmaName;
	OpenWBEM::MOF::ClassName*                               pClassName;
	OpenWBEM::MOF::AliasIdentifier*                         pAliasIdentifier;
	OpenWBEM::MOF::QualifierName*                           pQualifierName;
	OpenWBEM::MOF::PropertyName*                            pPropertyName;
	OpenWBEM::MOF::ReferenceName*                           pReferenceName;
	OpenWBEM::MOF::MethodName*                              pMethodName;
	OpenWBEM::MOF::ParameterName*                           pParameterName;
	OpenWBEM::MOF::DataType*                                pDataType;
	OpenWBEM::MOF::ObjectHandle*                            pObjectHandle;
}

%{
#define YYLEX_PARAM MOF_COMPILER_STATE, p_err
int owmoflex(YYSTYPE *yylval, YYLTYPE * llocp, OpenWBEM::MOF::CompilerState* MOF_COMPILER_STATE, OpenWBEM::MOF::ParseError* p_err);
%}

%type <pMOFProductionList>			mofProductionList
%destructor { delete $$; } mofProductionList
%type <pMOFProduction>				mofProduction
%destructor { delete $$; } mofProduction
%type <pIndicDeclaration>			indicDeclaration
%destructor { delete $$; } indicDeclaration
%type <pClassDeclaration>			classDeclaration
%destructor { delete $$; } classDeclaration
%type <pPropertyDeclaration>		propertyDeclaration
%destructor { delete $$; } propertyDeclaration
%type <pObjectRef>					objectRef
%destructor { delete $$; } objectRef
%type <pParameter>					parameter
%destructor { delete $$; } parameter
%type <pArray>							array
%destructor { delete $$; } array
%type <pCompilerDirective>			compilerDirective
%destructor { delete $$; } compilerDirective
%type <pMetaElement>					metaElement
%destructor { delete $$; } metaElement
%type <pMetaElementList>			metaElementList
%destructor { delete $$; } metaElementList
%type <pInitializer>					initializer
%destructor { delete $$; } initializer
%type <pSuperClass>					superClass
%destructor { delete $$; } superClass
%type <pAssociationFeature>		associationFeature
%destructor { delete $$; } associationFeature
%type <pAssociationFeatureList>	associationFeatureList
%destructor { delete $$; } associationFeatureList
%type <pQualifierParameter>		qualifierParameter
%destructor { delete $$; } qualifierParameter
%type <pQualifierDeclaration>		qualifierDeclaration
%destructor { delete $$; } qualifierDeclaration
%type <pPragmaParameter>			pragmaParameter
%destructor { delete $$; } pragmaParameter
%type <pAssocDeclaration>			assocDeclaration
%destructor { delete $$; } assocDeclaration
%type <pDefaultValue>				defaultValue
%destructor { delete $$; } defaultValue
%type <pClassFeature>				classFeature
%destructor { delete $$; } classFeature
%type <pClassFeatureList>			classFeatureList
%destructor { delete $$; } classFeatureList
%type <pReferenceInitializer>		referenceInitializer
%destructor { delete $$; } referenceInitializer
%type <pQualifierType>				qualifierType
%destructor { delete $$; } qualifierType
%type <pDefaultFlavor>				defaultFlavor
%destructor { delete $$; } defaultFlavor

%type <pString>						IDENTIFIER stringValue stringValueList binaryValue octalValue decimalValue hexValue floatValue charValue booleanValue nullValue IDENTIFIER_TOK ANY_TOK AS_TOK CLASS_TOK FLAVOR_TOK INSTANCE_TOK METHOD_TOK OF_TOK PARAMETER_TOK PRAGMA_TOK PROPERTY_TOK QUALIFIER_TOK REFERENCE_TOK RESTRICTED_TOK SCHEMA_TOK SCOPE_TOK ENABLEOVERRIDE_TOK DISABLEOVERRIDE_TOK TOSUBCLASS_TOK TRANSLATABLE_TOK DT_UINT8_TOK DT_SINT8_TOK DT_UINT16_TOK DT_SINT16_TOK DT_UINT32_TOK DT_SINT32_TOK DT_UINT64_TOK DT_SINT64_TOK DT_REAL32_TOK DT_REAL64_TOK DT_CHAR16_TOK DT_STR_TOK DT_BOOL_TOK DT_DATETIME_TOK ASSOCIATION_TOK INDICATION_TOK LPAREN_TOK RPAREN_TOK LBRACE_TOK RBRACE_TOK SEMICOLON_TOK LBRACK_TOK RBRACK_TOK COMMA_TOK DOLLAR_TOK COLON_TOK EQUALS_TOK FALSE_TOK TRUE_TOK NULL_TOK REF_TOK
%destructor { delete $$; } IDENTIFIER stringValue stringValueList binaryValue octalValue decimalValue hexValue floatValue charValue booleanValue nullValue IDENTIFIER_TOK ANY_TOK AS_TOK CLASS_TOK FLAVOR_TOK INSTANCE_TOK METHOD_TOK OF_TOK PARAMETER_TOK PRAGMA_TOK PROPERTY_TOK QUALIFIER_TOK REFERENCE_TOK RESTRICTED_TOK SCHEMA_TOK SCOPE_TOK ENABLEOVERRIDE_TOK DISABLEOVERRIDE_TOK TOSUBCLASS_TOK TRANSLATABLE_TOK DT_UINT8_TOK DT_SINT8_TOK DT_UINT16_TOK DT_SINT16_TOK DT_UINT32_TOK DT_SINT32_TOK DT_UINT64_TOK DT_SINT64_TOK DT_REAL32_TOK DT_REAL64_TOK DT_CHAR16_TOK DT_STR_TOK DT_BOOL_TOK DT_DATETIME_TOK ASSOCIATION_TOK INDICATION_TOK LPAREN_TOK RPAREN_TOK LBRACE_TOK RBRACE_TOK SEMICOLON_TOK LBRACK_TOK RBRACK_TOK COMMA_TOK DOLLAR_TOK COLON_TOK EQUALS_TOK FALSE_TOK TRUE_TOK NULL_TOK REF_TOK


%type <pIntegerValue>                   integerValue
%destructor { delete $$; } integerValue
%type <pConstantValue>                  constantValue
%destructor { delete $$; } constantValue
%type <pPragmaName>					pragmaName
%destructor { delete $$; } pragmaName
%type <pClassName>					className
%destructor { delete $$; } className
%type <pQualifierName>				qualifierName
%destructor { delete $$; } qualifierName
%type <pPropertyName>				propertyName
%destructor { delete $$; } propertyName
%type <pReferenceName>				referenceName
%destructor { delete $$; } referenceName
%type <pMethodName>					methodName
%destructor { delete $$; } methodName
%type <pParameterName>				parameterName
%destructor { delete $$; } parameterName
%type <pAliasIdentifier>			aliasIdentifier
%destructor { delete $$; } aliasIdentifier
%type <pObjectHandle>				objectHandle
%destructor { delete $$; } objectHandle
%type <pDataType>						dataType
%destructor { delete $$; } dataType
%type <pArrayInitializer>			arrayInitializer
%destructor { delete $$; } arrayInitializer
%type <pValueInitializer>			valueInitializer
%destructor { delete $$; } valueInitializer
%type <pValueInitializerList>		valueInitializerList
%destructor { delete $$; } valueInitializerList
%type <pFlavor>						flavor
%destructor { delete $$; } flavor
%type <pFlavorList>					flavorList flavorListWithComma
%destructor { delete $$; } flavorList flavorListWithComma
%type <pConstantValueList>			constantValueList
%destructor { delete $$; } constantValueList
%type <pAlias>							alias
%destructor { delete $$; } alias
%type <pQualifierList>				qualifierList qualifierListEmpty
%destructor { delete $$; } qualifierList qualifierListEmpty
%type <pReferenceDeclaration>		referenceDeclaration
%destructor { delete $$; } referenceDeclaration
%type <pMethodDeclaration>			methodDeclaration
%destructor { delete $$; } methodDeclaration
%type <pQualifier>					qualifier
%destructor { delete $$; } qualifier
%type <pParameterList>				parameterList
%destructor { delete $$; } parameterList
%type <pScope>							scope
%destructor { delete $$; } scope
%type <pInstanceDeclaration>		instanceDeclaration
%destructor { delete $$; } instanceDeclaration


%start mofSpecification

%%

/* rules */
mofSpecification:
	mofProductionList {
		MOF_COMPILER_STATE->mofSpecification = new MOFSpecification($1);
		}
	;

mofProductionList: /* empty */ {$$ = new List<MOFProduction*>; }
	| mofProductionList mofProduction {$1->push_back($2); $$ = $1; }
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
			$$ = new CompilerDirective($2, $4, MOF_COMPILER_STATE->getLineInfo());
			if ($2->pPragmaName->equalsIgnoreCase("include"))
			{
				String includeFile;
				try
				{
					// either fixParsedString or lexIncludeFile can throw, which we have to catch because bison isn't exception aware and can leak if an exception is thrown.
					includeFile = OpenWBEM::MOF::Compiler::fixParsedString(*($4->pPragmaParameter));
					lexIncludeFile(MOF_COMPILER_STATE, includeFile);
				}
				catch (Exception& e)
				{
					owmoferror(&yylloc, MOF_COMPILER_STATE, p_err, OpenWBEM::Format("Failed to process #pragma include %1: %2", includeFile, e).c_str());
					YYERROR;
				}
				catch (...)
				{
					owmoferror(&yylloc, MOF_COMPILER_STATE, p_err, OpenWBEM::Format("Failed to process #pragma include %1", includeFile).c_str());
					YYERROR;
				}
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
                $$ = new ClassDeclaration(0, $2, 0, 0, $4, MOF_COMPILER_STATE->getLineInfo());
                delete $1;
                delete $3;
                delete $5;
                delete $6;
        }
	| CLASS_TOK className superClass LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration(0, $2, 0, $3, $5, MOF_COMPILER_STATE->getLineInfo());
                delete $1;
                delete $4;
                delete $6;
                delete $7;
        }
	| CLASS_TOK className alias LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration(0, $2, $3, 0, $5, MOF_COMPILER_STATE->getLineInfo());
                delete $1;
                delete $4;
                delete $6;
                delete $7;
        }
	| CLASS_TOK className alias superClass LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration(0, $2, $3, $4, $6, MOF_COMPILER_STATE->getLineInfo());
                delete $1;
                delete $5;
                delete $7;
                delete $8;
        }
	| qualifierList CLASS_TOK className LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration($1, $3, 0, 0, $5, MOF_COMPILER_STATE->getLineInfo());
                delete $2;
                delete $4;
                delete $6;
                delete $7;
        }
	| qualifierList CLASS_TOK className superClass LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration($1, $3, 0, $4, $6, MOF_COMPILER_STATE->getLineInfo());
                delete $2;
                delete $5;
                delete $7;
                delete $8;
        }
	| qualifierList CLASS_TOK className alias LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration($1, $3, $4, 0, $6, MOF_COMPILER_STATE->getLineInfo());
                delete $2;
                delete $5;
                delete $7;
                delete $8;
        }
	| qualifierList CLASS_TOK className alias superClass LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new ClassDeclaration($1, $3, $4, $5, $7, MOF_COMPILER_STATE->getLineInfo());
                delete $2;
                delete $6;
                delete $8;
                delete $9;
        }
	;

classFeatureList: /* empty */ {$$ = new List<ClassFeature*>; }
	| classFeatureList classFeature {$1->push_back($2); $$ = $1;}
	;

assocDeclaration:
	LBRACK_TOK ASSOCIATION_TOK qualifierListEmpty RBRACK_TOK CLASS_TOK className LBRACE_TOK associationFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new AssocDeclaration($3, $6, 0, 0, $8, MOF_COMPILER_STATE->getLineInfo());
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
                $$ = new AssocDeclaration($3, $6, 0, $7, $9, MOF_COMPILER_STATE->getLineInfo());
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
                $$ = new AssocDeclaration($3, $6, $7, 0, $9, MOF_COMPILER_STATE->getLineInfo());
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
                $$ = new AssocDeclaration($3, $6, $7, $8, $10, MOF_COMPILER_STATE->getLineInfo());
                delete $1;
                delete $2;
                delete $4;
                delete $5;
                delete $9;
                delete $11;
                delete $12;
        }
	;
	
qualifierListEmpty: /* empty */ {$$ = new List<Qualifier*>; }
	| qualifierListEmpty COMMA_TOK qualifier
        {$1->push_back($3); $$ = $1; delete $2; }
	;

associationFeatureList: /* empty */ {$$ = new List<AssociationFeature*>; }
	| associationFeatureList associationFeature {$1->push_back($2); $$ = $1;}
	;

indicDeclaration:
	LBRACK_TOK INDICATION_TOK qualifierListEmpty RBRACK_TOK CLASS_TOK className LBRACE_TOK classFeatureList RBRACE_TOK SEMICOLON_TOK
	{
                $$ = new IndicDeclaration($3, $6, 0, 0, $8, MOF_COMPILER_STATE->getLineInfo());
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
                $$ = new IndicDeclaration($3, $6, 0, $7, $9, MOF_COMPILER_STATE->getLineInfo());
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
                $$ = new IndicDeclaration($3, $6, $7, 0, $9, MOF_COMPILER_STATE->getLineInfo());
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
                $$ = new IndicDeclaration($3, $6, $7, $8, $10, MOF_COMPILER_STATE->getLineInfo());
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
                $$ = new AliasIdentifier($2, MOF_COMPILER_STATE->getLineInfo());
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
	qualifierName {$$ = new Qualifier($1, 0, 0, MOF_COMPILER_STATE->getLineInfo()); }
	| qualifierName COLON_TOK flavorList
        {
                $$ = new Qualifier($1, 0, $3, MOF_COMPILER_STATE->getLineInfo());
                delete $2;
        }
	| qualifierName qualifierParameter{$$ = new Qualifier($1, $2, 0, MOF_COMPILER_STATE->getLineInfo()); }
	| qualifierName qualifierParameter COLON_TOK flavorList
	{
                $$ = new Qualifier($1, $2, $4, MOF_COMPILER_STATE->getLineInfo());
                delete $3;
        }
	;

flavorList:
	flavor {$$ = new List<Flavor*>(1, $1); }
	| flavorList flavor {$1->push_back($2); $$ = $1;}
	;

qualifierParameter:
	LPAREN_TOK constantValue RPAREN_TOK
        {
                $$ = new QualifierParameterConstantValue($2, MOF_COMPILER_STATE->getLineInfo());
                delete $1;
                delete $3;
        }
	| arrayInitializer {$$ = new QualifierParameterArrayInitializer($1, MOF_COMPILER_STATE->getLineInfo()); }
	;

flavor:
	ENABLEOVERRIDE_TOK {$$ = new Flavor($1, MOF_COMPILER_STATE->getLineInfo()); }
	| DISABLEOVERRIDE_TOK {$$ = new Flavor($1, MOF_COMPILER_STATE->getLineInfo()); }
	| RESTRICTED_TOK {$$ = new Flavor($1, MOF_COMPILER_STATE->getLineInfo()); }
	| TOSUBCLASS_TOK {$$ = new Flavor($1, MOF_COMPILER_STATE->getLineInfo()); }
	| TRANSLATABLE_TOK {$$ = new Flavor($1, MOF_COMPILER_STATE->getLineInfo()); }
	;

propertyDeclaration:
	dataType propertyName SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration(0, $1, $2, 0, 0, MOF_COMPILER_STATE->getLineInfo());
		delete $3;
	}
	| dataType propertyName defaultValue SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration(0, $1, $2, 0, $3, MOF_COMPILER_STATE->getLineInfo());
		delete $4;
	}
	| dataType propertyName array SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration(0, $1, $2, $3, 0, MOF_COMPILER_STATE->getLineInfo());
		delete $4;
	}
	| dataType propertyName array defaultValue SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration(0, $1, $2, $3, $4, MOF_COMPILER_STATE->getLineInfo());
		delete $5;
	}
	| qualifierList dataType propertyName SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration($1, $2, $3, 0, 0, MOF_COMPILER_STATE->getLineInfo());
		delete $4;
	}
	| qualifierList dataType propertyName defaultValue SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration($1, $2, $3, 0, $4, MOF_COMPILER_STATE->getLineInfo());
		delete $5;
	}
	| qualifierList dataType propertyName array SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration($1, $2, $3, $4, 0, MOF_COMPILER_STATE->getLineInfo());
		delete $5;
	}
	| qualifierList dataType propertyName array defaultValue SEMICOLON_TOK
	{
		$$ = new PropertyDeclaration($1, $2, $3, $4, $5, MOF_COMPILER_STATE->getLineInfo());
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
	parameter {$$ = new List<Parameter*>(1, $1); }
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
	LBRACK_TOK RBRACK_TOK {$$ = new ::OpenWBEM::MOF::Array(0); delete $1; delete $2;}
	| LBRACK_TOK integerValue RBRACK_TOK /* must be positive value */
		{$$ = new ::OpenWBEM::MOF::Array($2); delete $1; delete $3;}
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
	| LBRACE_TOK RBRACE_TOK
		{$$ = new ArrayInitializer(0); delete $1; delete $2;}
	;

constantValueList:
	constantValue {$$ = new List<ConstantValue*>(1, $1); }
	| constantValueList COMMA_TOK constantValue
		{$1->push_back($3); $$ = $1; delete $2;}
	;

/* do this to handle "string" "concatenation" */
stringValueList:
	stringValue {$$ = $1; }
	| stringValueList stringValue
		{$$ = $1; $$->concat(*$2); delete $2;}
	;

constantValue:
	integerValue {$$ = new ConstantValueIntegerValue($1); }
	| floatValue {$$ = new ConstantValueFloatValue($1); }
	| charValue {$$ = new ConstantValueCharValue($1); }
	| stringValueList {$$ = new ConstantValueStringValue($1); }
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

qualifierDeclaration:
	QUALIFIER_TOK qualifierName qualifierType scope SEMICOLON_TOK
	{
			$$= new QualifierDeclaration($2, $3, $4, 0, MOF_COMPILER_STATE->getLineInfo());
			delete $1;
			delete $5;
	}
	| QUALIFIER_TOK qualifierName qualifierType scope defaultFlavor SEMICOLON_TOK
	{
			$$= new QualifierDeclaration($2, $3, $4, $5, MOF_COMPILER_STATE->getLineInfo());
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
	metaElement {$$ = new List<MetaElement*>(1, $1); }
	| metaElementList COMMA_TOK metaElement
		{$1->push_back($3); $$ = $1; delete $2; }
	;

metaElement:
	SCHEMA_TOK {$$ = new MetaElement($1, MOF_COMPILER_STATE->getLineInfo()); }
	| CLASS_TOK {$$ = new MetaElement($1, MOF_COMPILER_STATE->getLineInfo()); }
	| ASSOCIATION_TOK {$$ = new MetaElement($1, MOF_COMPILER_STATE->getLineInfo()); }
	| INDICATION_TOK {$$ = new MetaElement($1, MOF_COMPILER_STATE->getLineInfo()); }
	| QUALIFIER_TOK {$$ = new MetaElement($1, MOF_COMPILER_STATE->getLineInfo()); }
	| PROPERTY_TOK {$$ = new MetaElement($1, MOF_COMPILER_STATE->getLineInfo()); }
	| REFERENCE_TOK {$$ = new MetaElement($1, MOF_COMPILER_STATE->getLineInfo()); }
	| METHOD_TOK {$$ = new MetaElement($1, MOF_COMPILER_STATE->getLineInfo()); }
	| PARAMETER_TOK {$$ = new MetaElement($1, MOF_COMPILER_STATE->getLineInfo()); }
	| ANY_TOK {$$ = new MetaElement($1, MOF_COMPILER_STATE->getLineInfo()); }
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
	flavor {$$ = new List<Flavor*>(1, $1); }
	| flavorListWithComma COMMA_TOK flavor
	{
		$1->push_back($3); $$ = $1;
		delete $2;
	}
	;

instanceDeclaration:
	INSTANCE_TOK OF_TOK className LBRACE_TOK valueInitializerList RBRACE_TOK SEMICOLON_TOK
	{
			$$ = new InstanceDeclaration(0, $3, 0, $5, MOF_COMPILER_STATE->getLineInfo());
			delete $1;
			delete $2;
			delete $4;
			delete $6;
			delete $7;
	}
	| INSTANCE_TOK OF_TOK className alias LBRACE_TOK valueInitializerList RBRACE_TOK SEMICOLON_TOK
	{
			$$ = new InstanceDeclaration(0, $3, $4, $6, MOF_COMPILER_STATE->getLineInfo());
			delete $1;
			delete $2;
			delete $5;
			delete $7;
			delete $8;
	}
	| qualifierList INSTANCE_TOK OF_TOK className LBRACE_TOK valueInitializerList RBRACE_TOK SEMICOLON_TOK
	{
			$$ = new InstanceDeclaration($1, $4, 0, $6, MOF_COMPILER_STATE->getLineInfo());
			delete $2;
			delete $3;
			delete $5;
			delete $7;
			delete $8;
	}
	| qualifierList INSTANCE_TOK OF_TOK className alias LBRACE_TOK valueInitializerList RBRACE_TOK SEMICOLON_TOK
	{
			$$ = new InstanceDeclaration($1, $4, $5, $7, MOF_COMPILER_STATE->getLineInfo());
			delete $2;
			delete $3;
			delete $6;
			delete $8;
			delete $9;
	}
	;

valueInitializerList:
	valueInitializer {$$ = new List<ValueInitializer*>(1, $1); }
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
	| dataType {$$ = const_cast<String*>($1->pDataType.release()); delete $1;}
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
void owmoferror(YYLTYPE * p_loc, CompilerState* MOF_COMPILER_STATE, ParseError * p_err, const char* msg)
{
	// If it's not empty, then the lexer reported an error, and we don't want to overwrite it.
	if (p_err->message.empty())
	{
		p_err->message = String(msg);
		p_err->column = p_loc->first_column;
		p_err->line = p_loc->first_line;
	}
}


