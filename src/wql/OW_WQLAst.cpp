/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

#include "OW_WQLAst.hpp"

namespace OpenWBEM
{

stmt_selectStmt_optSemicolon::~stmt_selectStmt_optSemicolon()
{
	delete m_pselectStmt1;
	delete m_poptSemicolon2;
}
stmt_updateStmt_optSemicolon::~stmt_updateStmt_optSemicolon()
{
	delete m_pupdateStmt1;
	delete m_poptSemicolon2;
}
stmt_insertStmt_optSemicolon::~stmt_insertStmt_optSemicolon()
{
	delete m_pinsertStmt1;
	delete m_poptSemicolon2;
}
stmt_deleteStmt_optSemicolon::~stmt_deleteStmt_optSemicolon()
{
	delete m_pdeleteStmt1;
	delete m_poptSemicolon2;
}
optSemicolon_empty::~optSemicolon_empty()
{
}
optSemicolon_SEMICOLON::~optSemicolon_SEMICOLON()
{
	delete m_pSEMICOLON1;
}
insertStmt::~insertStmt()
{
	delete m_pINSERT1;
	delete m_pINTO2;
	delete m_pstrRelationName3;
	delete m_pinsertRest4;
}
insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN::~insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN()
{
	delete m_pVALUES1;
	delete m_pLEFTPAREN2;
	while (!m_ptargetList3->empty())
	{
		delete m_ptargetList3->front();
		m_ptargetList3->pop_front();
	}
	delete m_ptargetList3;
	delete m_pRIGHTPAREN4;
}
insertRest_DEFAULT_VALUES::~insertRest_DEFAULT_VALUES()
{
	delete m_pDEFAULT1;
	delete m_pVALUES2;
}
insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN::~insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN()
{
	delete m_pLEFTPAREN1;
	while (!m_pcolumnList2->empty())
	{
		delete m_pcolumnList2->front();
		m_pcolumnList2->pop_front();
	}
	delete m_pcolumnList2;
	delete m_pRIGHTPAREN3;
	delete m_pVALUES4;
	delete m_pLEFTPAREN5;
	while (!m_ptargetList6->empty())
	{
		delete m_ptargetList6->front();
		m_ptargetList6->pop_front();
	}
	delete m_ptargetList6;
	delete m_pRIGHTPAREN7;
}
deleteStmt::~deleteStmt()
{
	delete m_pDELETE1;
	delete m_pFROM2;
	delete m_pstrRelationName3;
	delete m_poptWhereClause4;
}
updateStmt::~updateStmt()
{
	delete m_pUPDATE1;
	delete m_pstrRelationName2;
	delete m_pSET3;
	while (!m_pupdateTargetList4->empty())
	{
		delete m_pupdateTargetList4->front();
		m_pupdateTargetList4->pop_front();
	}
	delete m_pupdateTargetList4;
	delete m_poptWhereClause5;
}
selectStmt::~selectStmt()
{
	delete m_pSELECT1;
	delete m_poptDistinct2;
	while (!m_ptargetList3->empty())
	{
		delete m_ptargetList3->front();
		m_ptargetList3->pop_front();
	}
	delete m_ptargetList3;
	delete m_poptFromClause4;
	delete m_poptWhereClause5;
	delete m_poptGroupClause6;
	delete m_poptHavingClause7;
	delete m_poptSortClause8;
}
exprSeq_aExpr::~exprSeq_aExpr()
{
	delete m_paExpr1;
}
exprSeq_exprSeq_COMMA_aExpr::~exprSeq_exprSeq_COMMA_aExpr()
{
	delete m_pexprSeq1;
	delete m_pCOMMA2;
	delete m_paExpr3;
}
exprSeq_exprSeq_USING_aExpr::~exprSeq_exprSeq_USING_aExpr()
{
	delete m_pexprSeq1;
	delete m_pUSING2;
	delete m_paExpr3;
}
optDistinct_empty::~optDistinct_empty()
{
}
optDistinct_DISTINCT::~optDistinct_DISTINCT()
{
	delete m_pDISTINCT1;
}
optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN::~optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN()
{
	delete m_pDISTINCT1;
	delete m_pON2;
	delete m_pLEFTPAREN3;
	delete m_pexprSeq4;
	delete m_pRIGHTPAREN5;
}
optDistinct_ALL::~optDistinct_ALL()
{
	delete m_pALL1;
}
sortClause::~sortClause()
{
	delete m_pORDER1;
	delete m_pBY2;
	while (!m_psortbyList3->empty())
	{
		delete m_psortbyList3->front();
		m_psortbyList3->pop_front();
	}
	delete m_psortbyList3;
}
optSortClause_empty::~optSortClause_empty()
{
}
optSortClause_sortClause::~optSortClause_sortClause()
{
	delete m_psortClause1;
}
sortby::~sortby()
{
	delete m_paExpr1;
	delete m_pstrOptOrderSpecification2;
}
optGroupClause_empty::~optGroupClause_empty()
{
}
optGroupClause_GROUP_BY_exprSeq::~optGroupClause_GROUP_BY_exprSeq()
{
	delete m_pGROUP1;
	delete m_pBY2;
	delete m_pexprSeq3;
}
optHavingClause_empty::~optHavingClause_empty()
{
}
optHavingClause_HAVING_aExpr::~optHavingClause_HAVING_aExpr()
{
	delete m_pHAVING1;
	delete m_paExpr2;
}
optFromClause_empty::~optFromClause_empty()
{
}
optFromClause_FROM_fromList::~optFromClause_FROM_fromList()
{
	delete m_pFROM1;
	while (!m_pfromList2->empty())
	{
		delete m_pfromList2->front();
		m_pfromList2->pop_front();
	}
	delete m_pfromList2;
}
tableRef_relationExpr::~tableRef_relationExpr()
{
	delete m_prelationExpr1;
}
tableRef_relationExpr_aliasClause::~tableRef_relationExpr_aliasClause()
{
	delete m_prelationExpr1;
	delete m_paliasClause2;
}
tableRef_joinedTable::~tableRef_joinedTable()
{
	delete m_pjoinedTable1;
}
tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause::~tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause()
{
	delete m_pLEFTPAREN1;
	delete m_pjoinedTable2;
	delete m_pRIGHTPAREN3;
	delete m_paliasClause4;
}
joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN::~joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN()
{
	delete m_pLEFTPAREN1;
	delete m_pjoinedTable2;
	delete m_pRIGHTPAREN3;
}
joinedTable_tableRef_CROSS_JOIN_tableRef::~joinedTable_tableRef_CROSS_JOIN_tableRef()
{
	delete m_ptableRef1;
	delete m_pCROSS2;
	delete m_pJOIN3;
	delete m_ptableRef4;
}
joinedTable_tableRef_UNIONJOIN_tableRef::~joinedTable_tableRef_UNIONJOIN_tableRef()
{
	delete m_ptableRef1;
	delete m_pUNIONJOIN2;
	delete m_ptableRef3;
}
joinedTable_tableRef_joinType_JOIN_tableRef_joinQual::~joinedTable_tableRef_joinType_JOIN_tableRef_joinQual()
{
	delete m_ptableRef1;
	delete m_pjoinType2;
	delete m_pJOIN3;
	delete m_ptableRef4;
	delete m_pjoinQual5;
}
joinedTable_tableRef_JOIN_tableRef_joinQual::~joinedTable_tableRef_JOIN_tableRef_joinQual()
{
	delete m_ptableRef1;
	delete m_pJOIN2;
	delete m_ptableRef3;
	delete m_pjoinQual4;
}
joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef::~joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef()
{
	delete m_ptableRef1;
	delete m_pNATURAL2;
	delete m_pjoinType3;
	delete m_pJOIN4;
	delete m_ptableRef5;
}
joinedTable_tableRef_NATURAL_JOIN_tableRef::~joinedTable_tableRef_NATURAL_JOIN_tableRef()
{
	delete m_ptableRef1;
	delete m_pNATURAL2;
	delete m_pJOIN3;
	delete m_ptableRef4;
}
aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN::~aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN()
{
	delete m_pAS1;
	delete m_pstrColId2;
	delete m_pLEFTPAREN3;
	while (!m_pnameList4->empty())
	{
		delete m_pnameList4->front();
		m_pnameList4->pop_front();
	}
	delete m_pnameList4;
	delete m_pRIGHTPAREN5;
}
aliasClause_AS_strColId::~aliasClause_AS_strColId()
{
	delete m_pAS1;
	delete m_pstrColId2;
}
aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN::~aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN()
{
	delete m_pstrColId1;
	delete m_pLEFTPAREN2;
	while (!m_pnameList3->empty())
	{
		delete m_pnameList3->front();
		m_pnameList3->pop_front();
	}
	delete m_pnameList3;
	delete m_pRIGHTPAREN4;
}
aliasClause_strColId::~aliasClause_strColId()
{
	delete m_pstrColId1;
}
joinType_FULL_strOptJoinOuter::~joinType_FULL_strOptJoinOuter()
{
	delete m_pFULL1;
	delete m_pstrOptJoinOuter2;
}
joinType_LEFT_strOptJoinOuter::~joinType_LEFT_strOptJoinOuter()
{
	delete m_pLEFT1;
	delete m_pstrOptJoinOuter2;
}
joinType_RIGHT_strOptJoinOuter::~joinType_RIGHT_strOptJoinOuter()
{
	delete m_pRIGHT1;
	delete m_pstrOptJoinOuter2;
}
joinType_INNERP::~joinType_INNERP()
{
	delete m_pINNERP1;
}
joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN::~joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN()
{
	delete m_pUSING1;
	delete m_pLEFTPAREN2;
	while (!m_pnameList3->empty())
	{
		delete m_pnameList3->front();
		m_pnameList3->pop_front();
	}
	delete m_pnameList3;
	delete m_pRIGHTPAREN4;
}
joinQual_ON_aExpr::~joinQual_ON_aExpr()
{
	delete m_pON1;
	delete m_paExpr2;
}
relationExpr_strRelationName::~relationExpr_strRelationName()
{
	delete m_pstrRelationName1;
}
relationExpr_strRelationName_ASTERISK::~relationExpr_strRelationName_ASTERISK()
{
	delete m_pstrRelationName1;
	delete m_pASTERISK2;
}
relationExpr_ONLY_strRelationName::~relationExpr_ONLY_strRelationName()
{
	delete m_pONLY1;
	delete m_pstrRelationName2;
}
optWhereClause_empty::~optWhereClause_empty()
{
}
optWhereClause_WHERE_aExpr::~optWhereClause_WHERE_aExpr()
{
	delete m_pWHERE1;
	delete m_paExpr2;
}
rowExpr::~rowExpr()
{
	delete m_pLEFTPAREN1;
	delete m_prowDescriptor2;
	delete m_pRIGHTPAREN3;
	delete m_pstrAllOp4;
	delete m_pLEFTPAREN5;
	delete m_prowDescriptor6;
	delete m_pRIGHTPAREN7;
}
rowDescriptor::~rowDescriptor()
{
	while (!m_prowList1->empty())
	{
		delete m_prowList1->front();
		m_prowList1->pop_front();
	}
	delete m_prowList1;
	delete m_pCOMMA2;
	delete m_paExpr3;
}
aExpr_cExpr::~aExpr_cExpr()
{
	delete m_pcExpr1;
}
aExpr_aExpr_AT_TIME_ZONE_cExpr::~aExpr_aExpr_AT_TIME_ZONE_cExpr()
{
	delete m_paExpr1;
	delete m_pAT2;
	delete m_pTIME3;
	delete m_pZONE4;
	delete m_pcExpr5;
}
aExpr_PLUS_aExpr::~aExpr_PLUS_aExpr()
{
	delete m_pPLUS1;
	delete m_paExpr2;
}
aExpr_MINUS_aExpr::~aExpr_MINUS_aExpr()
{
	delete m_pMINUS1;
	delete m_paExpr2;
}
aExpr_BITINVERT_aExpr::~aExpr_BITINVERT_aExpr()
{
	delete m_pBITINVERT1;
	delete m_paExpr2;
}
aExpr_aExpr_PLUS_aExpr::~aExpr_aExpr_PLUS_aExpr()
{
	delete m_paExpr1;
	delete m_pPLUS2;
	delete m_paExpr3;
}
aExpr_aExpr_MINUS_aExpr::~aExpr_aExpr_MINUS_aExpr()
{
	delete m_paExpr1;
	delete m_pMINUS2;
	delete m_paExpr3;
}
aExpr_aExpr_ASTERISK_aExpr::~aExpr_aExpr_ASTERISK_aExpr()
{
	delete m_paExpr1;
	delete m_pASTERISK2;
	delete m_paExpr3;
}
aExpr_aExpr_SOLIDUS_aExpr::~aExpr_aExpr_SOLIDUS_aExpr()
{
	delete m_paExpr1;
	delete m_pSOLIDUS2;
	delete m_paExpr3;
}
aExpr_aExpr_PERCENT_aExpr::~aExpr_aExpr_PERCENT_aExpr()
{
	delete m_paExpr1;
	delete m_pPERCENT2;
	delete m_paExpr3;
}
aExpr_aExpr_BITAND_aExpr::~aExpr_aExpr_BITAND_aExpr()
{
	delete m_paExpr1;
	delete m_pBITAND2;
	delete m_paExpr3;
}
aExpr_aExpr_BITOR_aExpr::~aExpr_aExpr_BITOR_aExpr()
{
	delete m_paExpr1;
	delete m_pBITOR2;
	delete m_paExpr3;
}
aExpr_aExpr_BITSHIFTLEFT_aExpr::~aExpr_aExpr_BITSHIFTLEFT_aExpr()
{
	delete m_paExpr1;
	delete m_pBITSHIFTLEFT2;
	delete m_paExpr3;
}
aExpr_aExpr_BITSHIFTRIGHT_aExpr::~aExpr_aExpr_BITSHIFTRIGHT_aExpr()
{
	delete m_paExpr1;
	delete m_pBITSHIFTRIGHT2;
	delete m_paExpr3;
}
aExpr_aExpr_LESSTHAN_aExpr::~aExpr_aExpr_LESSTHAN_aExpr()
{
	delete m_paExpr1;
	delete m_pLESSTHAN2;
	delete m_paExpr3;
}
aExpr_aExpr_LESSTHANOREQUALS_aExpr::~aExpr_aExpr_LESSTHANOREQUALS_aExpr()
{
	delete m_paExpr1;
	delete m_pLESSTHANOREQUALS2;
	delete m_paExpr3;
}
aExpr_aExpr_GREATERTHAN_aExpr::~aExpr_aExpr_GREATERTHAN_aExpr()
{
	delete m_paExpr1;
	delete m_pGREATERTHAN2;
	delete m_paExpr3;
}
aExpr_aExpr_GREATERTHANOREQUALS_aExpr::~aExpr_aExpr_GREATERTHANOREQUALS_aExpr()
{
	delete m_paExpr1;
	delete m_pGREATERTHANOREQUALS2;
	delete m_paExpr3;
}
aExpr_aExpr_EQUALS_aExpr::~aExpr_aExpr_EQUALS_aExpr()
{
	delete m_paExpr1;
	delete m_pEQUALS2;
	delete m_paExpr3;
}
aExpr_aExpr_NOTEQUALS_aExpr::~aExpr_aExpr_NOTEQUALS_aExpr()
{
	delete m_paExpr1;
	delete m_pNOTEQUALS2;
	delete m_paExpr3;
}
aExpr_aExpr_AND_aExpr::~aExpr_aExpr_AND_aExpr()
{
	delete m_paExpr1;
	delete m_pAND2;
	delete m_paExpr3;
}
aExpr_aExpr_OR_aExpr::~aExpr_aExpr_OR_aExpr()
{
	delete m_paExpr1;
	delete m_pOR2;
	delete m_paExpr3;
}
aExpr_NOT_aExpr::~aExpr_NOT_aExpr()
{
	delete m_pNOT1;
	delete m_paExpr2;
}
aExpr_aExpr_CONCATENATION_aExpr::~aExpr_aExpr_CONCATENATION_aExpr()
{
	delete m_paExpr1;
	delete m_pCONCATENATION2;
	delete m_paExpr3;
}
aExpr_aExpr_LIKE_aExpr::~aExpr_aExpr_LIKE_aExpr()
{
	delete m_paExpr1;
	delete m_pLIKE2;
	delete m_paExpr3;
}
aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr::~aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr()
{
	delete m_paExpr1;
	delete m_pLIKE2;
	delete m_paExpr3;
	delete m_pESCAPE4;
	delete m_paExpr5;
}
aExpr_aExpr_NOT_LIKE_aExpr::~aExpr_aExpr_NOT_LIKE_aExpr()
{
	delete m_paExpr1;
	delete m_pNOT2;
	delete m_pLIKE3;
	delete m_paExpr4;
}
aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr::~aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr()
{
	delete m_paExpr1;
	delete m_pNOT2;
	delete m_pLIKE3;
	delete m_paExpr4;
	delete m_pESCAPE5;
	delete m_paExpr6;
}
aExpr_aExpr_ISNULL::~aExpr_aExpr_ISNULL()
{
	delete m_paExpr1;
	delete m_pISNULL2;
}
aExpr_aExpr_IS_NULLP::~aExpr_aExpr_IS_NULLP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pNULLP3;
}
aExpr_aExpr_NOTNULL::~aExpr_aExpr_NOTNULL()
{
	delete m_paExpr1;
	delete m_pNOTNULL2;
}
aExpr_aExpr_IS_NOT_NULLP::~aExpr_aExpr_IS_NOT_NULLP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pNOT3;
	delete m_pNULLP4;
}
aExpr_aExpr_IS_TRUEP::~aExpr_aExpr_IS_TRUEP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pTRUEP3;
}
aExpr_aExpr_IS_NOT_FALSEP::~aExpr_aExpr_IS_NOT_FALSEP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pNOT3;
	delete m_pFALSEP4;
}
aExpr_aExpr_IS_FALSEP::~aExpr_aExpr_IS_FALSEP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pFALSEP3;
}
aExpr_aExpr_IS_NOT_TRUEP::~aExpr_aExpr_IS_NOT_TRUEP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pNOT3;
	delete m_pTRUEP4;
}
aExpr_aExpr_ISA_aExpr::~aExpr_aExpr_ISA_aExpr()
{
	delete m_paExpr1;
	delete m_pISA2;
	delete m_paExpr3;
}
aExpr_rowExpr::~aExpr_rowExpr()
{
	delete m_prowExpr1;
}
bExpr_cExpr::~bExpr_cExpr()
{
	delete m_pcExpr1;
}
bExpr_PLUS_bExpr::~bExpr_PLUS_bExpr()
{
	delete m_pPLUS1;
	delete m_pbExpr2;
}
bExpr_MINUS_bExpr::~bExpr_MINUS_bExpr()
{
	delete m_pMINUS1;
	delete m_pbExpr2;
}
bExpr_BITINVERT_bExpr::~bExpr_BITINVERT_bExpr()
{
	delete m_pBITINVERT1;
	delete m_pbExpr2;
}
bExpr_bExpr_PLUS_bExpr::~bExpr_bExpr_PLUS_bExpr()
{
	delete m_pbExpr1;
	delete m_pPLUS2;
	delete m_pbExpr3;
}
bExpr_bExpr_MINUS_bExpr::~bExpr_bExpr_MINUS_bExpr()
{
	delete m_pbExpr1;
	delete m_pMINUS2;
	delete m_pbExpr3;
}
bExpr_bExpr_ASTERISK_bExpr::~bExpr_bExpr_ASTERISK_bExpr()
{
	delete m_pbExpr1;
	delete m_pASTERISK2;
	delete m_pbExpr3;
}
bExpr_bExpr_SOLIDUS_bExpr::~bExpr_bExpr_SOLIDUS_bExpr()
{
	delete m_pbExpr1;
	delete m_pSOLIDUS2;
	delete m_pbExpr3;
}
bExpr_bExpr_PERCENT_bExpr::~bExpr_bExpr_PERCENT_bExpr()
{
	delete m_pbExpr1;
	delete m_pPERCENT2;
	delete m_pbExpr3;
}
bExpr_bExpr_BITAND_bExpr::~bExpr_bExpr_BITAND_bExpr()
{
	delete m_pbExpr1;
	delete m_pBITAND2;
	delete m_pbExpr3;
}
bExpr_bExpr_BITOR_bExpr::~bExpr_bExpr_BITOR_bExpr()
{
	delete m_pbExpr1;
	delete m_pBITOR2;
	delete m_pbExpr3;
}
bExpr_bExpr_BITSHIFTLEFT_bExpr::~bExpr_bExpr_BITSHIFTLEFT_bExpr()
{
	delete m_pbExpr1;
	delete m_pBITSHIFTLEFT2;
	delete m_pbExpr3;
}
bExpr_bExpr_BITSHIFTRIGHT_bExpr::~bExpr_bExpr_BITSHIFTRIGHT_bExpr()
{
	delete m_pbExpr1;
	delete m_pBITSHIFTRIGHT2;
	delete m_pbExpr3;
}
bExpr_bExpr_LESSTHAN_bExpr::~bExpr_bExpr_LESSTHAN_bExpr()
{
	delete m_pbExpr1;
	delete m_pLESSTHAN2;
	delete m_pbExpr3;
}
bExpr_bExpr_LESSTHANOREQUALS_bExpr::~bExpr_bExpr_LESSTHANOREQUALS_bExpr()
{
	delete m_pbExpr1;
	delete m_pLESSTHANOREQUALS2;
	delete m_pbExpr3;
}
bExpr_bExpr_GREATERTHAN_bExpr::~bExpr_bExpr_GREATERTHAN_bExpr()
{
	delete m_pbExpr1;
	delete m_pGREATERTHAN2;
	delete m_pbExpr3;
}
bExpr_bExpr_GREATERTHANOREQUALS_bExpr::~bExpr_bExpr_GREATERTHANOREQUALS_bExpr()
{
	delete m_pbExpr1;
	delete m_pGREATERTHANOREQUALS2;
	delete m_pbExpr3;
}
bExpr_bExpr_EQUALS_bExpr::~bExpr_bExpr_EQUALS_bExpr()
{
	delete m_pbExpr1;
	delete m_pEQUALS2;
	delete m_pbExpr3;
}
bExpr_bExpr_NOTEQUALS_bExpr::~bExpr_bExpr_NOTEQUALS_bExpr()
{
	delete m_pbExpr1;
	delete m_pNOTEQUALS2;
	delete m_pbExpr3;
}
bExpr_bExpr_CONCATENATION_bExpr::~bExpr_bExpr_CONCATENATION_bExpr()
{
	delete m_pbExpr1;
	delete m_pCONCATENATION2;
	delete m_pbExpr3;
}
cExpr_attr::~cExpr_attr()
{
	delete m_pattr1;
}
cExpr_strColId_optIndirection::~cExpr_strColId_optIndirection()
{
	delete m_pstrColId1;
	delete m_poptIndirection2;
}
cExpr_aExprConst::~cExpr_aExprConst()
{
	delete m_paExprConst1;
}
cExpr_LEFTPAREN_aExpr_RIGHTPAREN::~cExpr_LEFTPAREN_aExpr_RIGHTPAREN()
{
	delete m_pLEFTPAREN1;
	delete m_paExpr2;
	delete m_pRIGHTPAREN3;
}
cExpr_strFuncName_LEFTPAREN_RIGHTPAREN::~cExpr_strFuncName_LEFTPAREN_RIGHTPAREN()
{
	delete m_pstrFuncName1;
	delete m_pLEFTPAREN2;
	delete m_pRIGHTPAREN3;
}
cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN::~cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN()
{
	delete m_pstrFuncName1;
	delete m_pLEFTPAREN2;
	delete m_pexprSeq3;
	delete m_pRIGHTPAREN4;
}
cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN::~cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN()
{
	delete m_pstrFuncName1;
	delete m_pLEFTPAREN2;
	delete m_pALL3;
	delete m_pexprSeq4;
	delete m_pRIGHTPAREN5;
}
cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN::~cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN()
{
	delete m_pstrFuncName1;
	delete m_pLEFTPAREN2;
	delete m_pDISTINCT3;
	delete m_pexprSeq4;
	delete m_pRIGHTPAREN5;
}
cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN::~cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN()
{
	delete m_pstrFuncName1;
	delete m_pLEFTPAREN2;
	delete m_pASTERISK3;
	delete m_pRIGHTPAREN4;
}
cExpr_CURRENTDATE::~cExpr_CURRENTDATE()
{
	delete m_pCURRENTDATE1;
}
cExpr_CURRENTTIME::~cExpr_CURRENTTIME()
{
	delete m_pCURRENTTIME1;
}
cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN::~cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN()
{
	delete m_pCURRENTTIME1;
	delete m_pLEFTPAREN2;
	delete m_pICONST3;
	delete m_pRIGHTPAREN4;
}
cExpr_CURRENTTIMESTAMP::~cExpr_CURRENTTIMESTAMP()
{
	delete m_pCURRENTTIMESTAMP1;
}
cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN::~cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN()
{
	delete m_pCURRENTTIMESTAMP1;
	delete m_pLEFTPAREN2;
	delete m_pICONST3;
	delete m_pRIGHTPAREN4;
}
cExpr_CURRENTUSER::~cExpr_CURRENTUSER()
{
	delete m_pCURRENTUSER1;
}
cExpr_SESSIONUSER::~cExpr_SESSIONUSER()
{
	delete m_pSESSIONUSER1;
}
cExpr_USER::~cExpr_USER()
{
	delete m_pUSER1;
}
cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN::~cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN()
{
	delete m_pEXTRACT1;
	delete m_pLEFTPAREN2;
	delete m_poptExtract3;
	delete m_pRIGHTPAREN4;
}
cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN::~cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN()
{
	delete m_pPOSITION1;
	delete m_pLEFTPAREN2;
	delete m_ppositionExpr3;
	delete m_pRIGHTPAREN4;
}
cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN::~cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN()
{
	delete m_pSUBSTRING1;
	delete m_pLEFTPAREN2;
	delete m_poptSubstrExpr3;
	delete m_pRIGHTPAREN4;
}
cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN::~cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN()
{
	delete m_pTRIM1;
	delete m_pLEFTPAREN2;
	delete m_pLEADING3;
	delete m_ptrimExpr4;
	delete m_pRIGHTPAREN5;
}
cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN::~cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN()
{
	delete m_pTRIM1;
	delete m_pLEFTPAREN2;
	delete m_pTRAILING3;
	delete m_ptrimExpr4;
	delete m_pRIGHTPAREN5;
}
cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN::~cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN()
{
	delete m_pTRIM1;
	delete m_pLEFTPAREN2;
	delete m_ptrimExpr3;
	delete m_pRIGHTPAREN4;
}
optIndirection_empty::~optIndirection_empty()
{
}
optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET::~optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET()
{
	delete m_poptIndirection1;
	delete m_pLEFTBRACKET2;
	delete m_paExpr3;
	delete m_pRIGHTBRACKET4;
}
optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET::~optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET()
{
	delete m_poptIndirection1;
	delete m_pLEFTBRACKET2;
	delete m_paExpr3;
	delete m_pCOLON4;
	delete m_paExpr5;
	delete m_pRIGHTBRACKET6;
}
optExtract_empty::~optExtract_empty()
{
}
optExtract_strExtractArg_FROM_aExpr::~optExtract_strExtractArg_FROM_aExpr()
{
	delete m_pstrExtractArg1;
	delete m_pFROM2;
	delete m_paExpr3;
}
positionExpr_bExpr_IN_bExpr::~positionExpr_bExpr_IN_bExpr()
{
	delete m_pbExpr1;
	delete m_pIN2;
	delete m_pbExpr3;
}
positionExpr_empty::~positionExpr_empty()
{
}
optSubstrExpr_empty::~optSubstrExpr_empty()
{
}
optSubstrExpr_aExpr_substrFrom_substrFor::~optSubstrExpr_aExpr_substrFrom_substrFor()
{
	delete m_paExpr1;
	delete m_psubstrFrom2;
	delete m_psubstrFor3;
}
optSubstrExpr_aExpr_substrFor_substrFrom::~optSubstrExpr_aExpr_substrFor_substrFrom()
{
	delete m_paExpr1;
	delete m_psubstrFor2;
	delete m_psubstrFrom3;
}
optSubstrExpr_aExpr_substrFrom::~optSubstrExpr_aExpr_substrFrom()
{
	delete m_paExpr1;
	delete m_psubstrFrom2;
}
optSubstrExpr_aExpr_substrFor::~optSubstrExpr_aExpr_substrFor()
{
	delete m_paExpr1;
	delete m_psubstrFor2;
}
optSubstrExpr_exprSeq::~optSubstrExpr_exprSeq()
{
	delete m_pexprSeq1;
}
substrFrom::~substrFrom()
{
	delete m_pFROM1;
	delete m_paExpr2;
}
substrFor::~substrFor()
{
	delete m_pFOR1;
	delete m_paExpr2;
}
trimExpr_aExpr_FROM_exprSeq::~trimExpr_aExpr_FROM_exprSeq()
{
	delete m_paExpr1;
	delete m_pFROM2;
	delete m_pexprSeq3;
}
trimExpr_FROM_exprSeq::~trimExpr_FROM_exprSeq()
{
	delete m_pFROM1;
	delete m_pexprSeq2;
}
trimExpr_exprSeq::~trimExpr_exprSeq()
{
	delete m_pexprSeq1;
}
attr::~attr()
{
	delete m_pstrRelationName1;
	delete m_pPERIOD2;
	delete m_pattrs3;
	delete m_poptIndirection4;
}
attrs_strAttrName::~attrs_strAttrName()
{
	delete m_pstrAttrName1;
}
attrs_attrs_PERIOD_strAttrName::~attrs_attrs_PERIOD_strAttrName()
{
	delete m_pattrs1;
	delete m_pPERIOD2;
	delete m_pstrAttrName3;
}
attrs_attrs_PERIOD_ASTERISK::~attrs_attrs_PERIOD_ASTERISK()
{
	delete m_pattrs1;
	delete m_pPERIOD2;
	delete m_pASTERISK3;
}
targetEl_aExpr_AS_strColLabel::~targetEl_aExpr_AS_strColLabel()
{
	delete m_paExpr1;
	delete m_pAS2;
	delete m_pstrColLabel3;
}
targetEl_aExpr::~targetEl_aExpr()
{
	delete m_paExpr1;
}
targetEl_strRelationName_PERIOD_ASTERISK::~targetEl_strRelationName_PERIOD_ASTERISK()
{
	delete m_pstrRelationName1;
	delete m_pPERIOD2;
	delete m_pASTERISK3;
}
targetEl_ASTERISK::~targetEl_ASTERISK()
{
	delete m_pASTERISK1;
}
updateTargetEl::~updateTargetEl()
{
	delete m_pstrColId1;
	delete m_poptIndirection2;
	delete m_pEQUALS3;
	delete m_paExpr4;
}
aExprConst_ICONST::~aExprConst_ICONST()
{
	delete m_pICONST1;
}
aExprConst_FCONST::~aExprConst_FCONST()
{
	delete m_pFCONST1;
}
aExprConst_SCONST::~aExprConst_SCONST()
{
	delete m_pSCONST1;
}
aExprConst_BITCONST::~aExprConst_BITCONST()
{
	delete m_pBITCONST1;
}
aExprConst_HEXCONST::~aExprConst_HEXCONST()
{
	delete m_pHEXCONST1;
}
aExprConst_TRUEP::~aExprConst_TRUEP()
{
	delete m_pTRUEP1;
}
aExprConst_FALSEP::~aExprConst_FALSEP()
{
	delete m_pFALSEP1;
}
aExprConst_NULLP::~aExprConst_NULLP()
{
	delete m_pNULLP1;
}
} // end namespace OpenWBEM
