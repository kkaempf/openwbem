
stmt :
		selectStmt optSemicolon
		| updateStmt optSemicolon
		| insertStmt optSemicolon
		| deleteStmt optSemicolon
		;

optSemicolon:
		| SEMICOLON
		;

insertStmt:  INSERT INTO strRelationName insertRest
		;

targetList: targetEl
		| targetList COMMA targetEl
		;

columnList:  strColumnElem
		| columnList COMMA strColumnElem
		;

insertRest:  VALUES LEFTPAREN targetList RIGHTPAREN
		| DEFAULT VALUES
		| LEFTPAREN columnList RIGHTPAREN VALUES LEFTPAREN targetList RIGHTPAREN
		;

strColumnElem:  strColId
		;


deleteStmt:  DELETE FROM strRelationName optWhereClause
		;


updateTargetList: updateTargetEl
		| updateTargetList COMMA updateTargetEl
		;

updateStmt:  UPDATE strRelationName
			  SET updateTargetList
			  optWhereClause
		;


selectStmt: SELECT optDistinct targetList
			 optFromClause optWhereClause
			 optGroupClause optHavingClause optSortClause
		;


exprSeq:  aExpr
		| exprSeq COMMA aExpr
		| exprSeq USING aExpr
		;

optDistinct:
		| DISTINCT							
		| DISTINCT ON LEFTPAREN exprSeq RIGHTPAREN			
		| ALL									
		;

sortbyList:  sortby							
		| sortbyList COMMA sortby				
		;

sortClause:  ORDER BY sortbyList				
		;
		
optSortClause:
		| sortClause
		;

sortby: aExpr strOptOrderSpecification
		;

strOptOrderSpecification:
		| ASC
		| DESC									
		;

nameList:  strName
		| nameList COMMA strName
		;

optGroupClause:
		| GROUP BY exprSeq				
		;

optHavingClause:
		| HAVING aExpr
		;


fromList: tableRef
		| fromList COMMA tableRef				
		;

optFromClause:
		| FROM fromList					
		;

tableRef:  relationExpr
		| relationExpr aliasClause
		| joinedTable
		| LEFTPAREN joinedTable RIGHTPAREN aliasClause
		;

joinedTable:  LEFTPAREN joinedTable RIGHTPAREN
		| tableRef CROSS JOIN tableRef
		| tableRef UNIONJOIN tableRef
		| tableRef joinType JOIN tableRef joinQual
		| tableRef JOIN tableRef joinQual
		| tableRef NATURAL joinType JOIN tableRef
		| tableRef NATURAL JOIN tableRef
		;

aliasClause:  AS strColId LEFTPAREN nameList RIGHTPAREN
		| AS strColId
		| strColId LEFTPAREN nameList RIGHTPAREN
		| strColId
		;

joinType:  FULL strOptJoinOuter						
		| LEFT strOptJoinOuter						
		| RIGHT strOptJoinOuter						
		| INNERP								
		;


strOptJoinOuter:
		| OUTERP							
		;

joinQual:  USING LEFTPAREN nameList RIGHTPAREN				
		| ON aExpr								
		;


relationExpr:	strRelationName
		| strRelationName ASTERISK
		| ONLY strRelationName
		;

optWhereClause:
		| WHERE aExpr						
		;


strDatetime:  YEARP								
		| MONTHP								
		| DAYP									
		| HOURP								
		| MINUTEP								
		| SECONDP								
		;

rowExpr: LEFTPAREN rowDescriptor RIGHTPAREN strAllOp LEFTPAREN rowDescriptor RIGHTPAREN
		;

rowList: aExpr
		| rowList COMMA aExpr
		;

rowDescriptor:  rowList COMMA aExpr
		;

strAllOp:
		PLUS
		| MINUS
		| ASTERISK
		| SOLIDUS
		| PERCENT
		| LESSTHAN
		| LESSTHANOREQUALS
		| GREATERTHAN
		| GREATERTHANOREQUALS
		| EQUALS
		| NOTEQUALS
		| CONCATENATION
		| BITAND
		| BITOR
		| BITSHIFTLEFT
		| BITSHIFTRIGHT
		;

aExpr:  cExpr
		| aExpr AT TIME ZONE cExpr
		| PLUS aExpr
		| MINUS aExpr
		| BITINVERT aExpr
		| aExpr PLUS aExpr
		| aExpr MINUS aExpr
		| aExpr ASTERISK aExpr
		| aExpr SOLIDUS aExpr
		| aExpr PERCENT aExpr
		| aExpr BITAND aExpr
		| aExpr BITOR aExpr
		| aExpr BITSHIFTLEFT aExpr
		| aExpr BITSHIFTRIGHT aExpr
		| aExpr LESSTHAN aExpr
		| aExpr LESSTHANOREQUALS aExpr
		| aExpr GREATERTHAN aExpr
		| aExpr GREATERTHANOREQUALS aExpr
		| aExpr EQUALS aExpr
		| aExpr NOTEQUALS aExpr
		| aExpr AND aExpr
		| aExpr OR aExpr
		| NOT aExpr
		| aExpr CONCATENATION aExpr
		| aExpr LIKE aExpr
		| aExpr LIKE aExpr ESCAPE aExpr
		| aExpr NOT LIKE aExpr
		| aExpr NOT LIKE aExpr ESCAPE aExpr
		| aExpr ISNULL
		| aExpr IS NULLP
		| aExpr NOTNULL
		| aExpr IS NOT NULLP
		| aExpr IS TRUEP
		| aExpr IS NOT FALSEP
		| aExpr IS FALSEP
		| aExpr IS NOT TRUEP
		| aExpr ISA aExpr
		| rowExpr
		;

bExpr:  cExpr
		| PLUS bExpr
		| MINUS bExpr
		| BITINVERT bExpr
		| bExpr PLUS bExpr
		| bExpr MINUS bExpr
		| bExpr ASTERISK bExpr
		| bExpr SOLIDUS bExpr
		| bExpr PERCENT bExpr
		| bExpr BITAND bExpr
		| bExpr BITOR bExpr
		| bExpr BITSHIFTLEFT bExpr
		| bExpr BITSHIFTRIGHT bExpr
		| bExpr LESSTHAN bExpr
		| bExpr LESSTHANOREQUALS bExpr
		| bExpr GREATERTHAN bExpr
		| bExpr GREATERTHANOREQUALS bExpr
		| bExpr EQUALS bExpr
		| bExpr NOTEQUALS bExpr
		| bExpr CONCATENATION bExpr
		;

cExpr:  attr
		| strColId optIndirection
		| aExprConst
		| LEFTPAREN aExpr RIGHTPAREN
		| strFuncName LEFTPAREN RIGHTPAREN
		| strFuncName LEFTPAREN exprSeq RIGHTPAREN
		| strFuncName LEFTPAREN ALL exprSeq RIGHTPAREN
		| strFuncName LEFTPAREN DISTINCT exprSeq RIGHTPAREN
		| strFuncName LEFTPAREN ASTERISK RIGHTPAREN
		| CURRENTDATE
		| CURRENTTIME
		| CURRENTTIME LEFTPAREN ICONST RIGHTPAREN
		| CURRENTTIMESTAMP
		| CURRENTTIMESTAMP LEFTPAREN ICONST RIGHTPAREN
		| CURRENTUSER
		| SESSIONUSER
		| USER
		| EXTRACT LEFTPAREN optExtract RIGHTPAREN
		| POSITION LEFTPAREN positionExpr RIGHTPAREN
		| SUBSTRING LEFTPAREN optSubstrExpr RIGHTPAREN
		| TRIM LEFTPAREN LEADING trimExpr RIGHTPAREN
		| TRIM LEFTPAREN TRAILING trimExpr RIGHTPAREN
		| TRIM LEFTPAREN trimExpr RIGHTPAREN
		;

optIndirection:
		| optIndirection LEFTBRACKET aExpr RIGHTBRACKET
		| optIndirection LEFTBRACKET aExpr COLON aExpr RIGHTBRACKET
		;

optExtract:
		| strExtractArg FROM aExpr
		;

strExtractArg:  strDatetime						
		| SCONST							
		| IDENT								
		| TIMEZONEHOUR						
		| TIMEZONEMINUTE					
		;



positionExpr:  bExpr IN bExpr
		|
		;

optSubstrExpr:
		| aExpr substrFrom substrFor
		| aExpr substrFor substrFrom
		| aExpr substrFrom
		| aExpr substrFor
		| exprSeq
		;

substrFrom:  FROM aExpr
		;

substrFor:  FOR aExpr
		;

trimExpr:  aExpr FROM exprSeq
		| FROM exprSeq
		| exprSeq
		;

attr:  strRelationName PERIOD attrs optIndirection
		;

attrs:	  strAttrName
		| attrs PERIOD strAttrName
		| attrs PERIOD ASTERISK
		;



targetEl:  aExpr AS strColLabel
		| aExpr
		| strRelationName PERIOD ASTERISK
		| ASTERISK
		;



updateTargetEl:  strColId optIndirection EQUALS aExpr
		;

strRelationName:	strColId
		;

strName:	strColId
		;
		
strAttrName: strColId
		;

strFuncName:  strColId						
		| IN							
		| IS							
		| ISNULL						
		| LIKE							
		| NOTNULL						
		;

aExprConst:  ICONST
		| FCONST
		| SCONST
		| BITCONST
		| HEXCONST
		| TRUEP
		| FALSEP
		| NULLP
		;

strColId:  IDENT							
		| strDatetime						
		| AT							
		| BY							
		| DELETE						
		| ESCAPE						
		| INSERT						
		| SET							
		| TIMEZONEHOUR					
		| TIMEZONEMINUTE				
		| UPDATE						
		| VALUES						
		| ZONE							
		| INTERVAL						
		| NATIONAL						
		| TIME							
		| TIMESTAMP						
		;

strColLabel:  strColId						
		| ALL							
		| AND							
		| ASC							
		| CROSS							
		| CURRENTDATE					
		| CURRENTTIME					
		| CURRENTTIMESTAMP				
		| CURRENTUSER					
		| DEFAULT						
		| DESC							
		| DISTINCT						
		| EXTRACT						
		| FALSEP						
		| FOR							
		| FROM							
		| FULL							
		| GROUP							
		| HAVING						
		| IN							
		| INNERP						
		| INTO							
		| IS							
		| ISA
		| ISNULL						
		| JOIN							
		| LEADING						
		| LEFT							
		| LIKE							
		| NATURAL						
		| NOT							
		| NOTNULL						
		| NULLP						
		| ON							
		| ONLY							
		| OR							
		| ORDER							
		| OUTERP						
		| POSITION						
		| RIGHT							
		| SELECT						
		| SESSIONUSER					
		| SUBSTRING						
		| TRAILING						
		| TRIM							
		| TRUEP						
		| UNION							
		| USER							
		| USING							
		| WHERE							
		;



