#ifndef OW_WQLAST_HPP_HPP_GUARD_
#define OW_WQLAST_HPP_HPP_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_List.hpp"
#include "OW_WQLVisitor.hpp"

class node
{
	public:
		virtual ~node() {}
		virtual void accept( OW_WQLVisitor * ) const = 0;
};

class stmt: public node
{
	public:
		stmt()
			{}
		virtual ~stmt() {}

};

class stmt_selectStmt_optSemicolon : public stmt
{
	public:
		stmt_selectStmt_optSemicolon(
			selectStmt* pNewselectStmt1,
			optSemicolon* pNewoptSemicolon2
		)
			: stmt()
			, m_pselectStmt1(pNewselectStmt1)
			, m_poptSemicolon2(pNewoptSemicolon2)
		{}

		virtual ~stmt_selectStmt_optSemicolon();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_stmt_selectStmt_optSemicolon( this );
		}

		selectStmt* m_pselectStmt1;
		optSemicolon* m_poptSemicolon2;
};

class stmt_updateStmt_optSemicolon : public stmt
{
	public:
		stmt_updateStmt_optSemicolon(
			updateStmt* pNewupdateStmt1,
			optSemicolon* pNewoptSemicolon2
		)
			: stmt()
			, m_pupdateStmt1(pNewupdateStmt1)
			, m_poptSemicolon2(pNewoptSemicolon2)
		{}

		virtual ~stmt_updateStmt_optSemicolon();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_stmt_updateStmt_optSemicolon( this );
		}

		updateStmt* m_pupdateStmt1;
		optSemicolon* m_poptSemicolon2;
};

class stmt_insertStmt_optSemicolon : public stmt
{
	public:
		stmt_insertStmt_optSemicolon(
			insertStmt* pNewinsertStmt1,
			optSemicolon* pNewoptSemicolon2
		)
			: stmt()
			, m_pinsertStmt1(pNewinsertStmt1)
			, m_poptSemicolon2(pNewoptSemicolon2)
		{}

		virtual ~stmt_insertStmt_optSemicolon();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_stmt_insertStmt_optSemicolon( this );
		}

		insertStmt* m_pinsertStmt1;
		optSemicolon* m_poptSemicolon2;
};

class stmt_deleteStmt_optSemicolon : public stmt
{
	public:
		stmt_deleteStmt_optSemicolon(
			deleteStmt* pNewdeleteStmt1,
			optSemicolon* pNewoptSemicolon2
		)
			: stmt()
			, m_pdeleteStmt1(pNewdeleteStmt1)
			, m_poptSemicolon2(pNewoptSemicolon2)
		{}

		virtual ~stmt_deleteStmt_optSemicolon();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_stmt_deleteStmt_optSemicolon( this );
		}

		deleteStmt* m_pdeleteStmt1;
		optSemicolon* m_poptSemicolon2;
};

class optSemicolon: public node
{
	public:
		optSemicolon()
			{}
		virtual ~optSemicolon() {}

};

class optSemicolon_empty : public optSemicolon
{
	public:
		optSemicolon_empty(
		)
			: optSemicolon()
		{}

		virtual ~optSemicolon_empty();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optSemicolon_empty( this );
		}

};

class optSemicolon_SEMICOLON : public optSemicolon
{
	public:
		optSemicolon_SEMICOLON(
			OW_String* pNewSEMICOLON1
		)
			: optSemicolon()
			, m_pSEMICOLON1(pNewSEMICOLON1)
		{}

		virtual ~optSemicolon_SEMICOLON();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optSemicolon_SEMICOLON( this );
		}

		OW_String* m_pSEMICOLON1;
};

class insertStmt : public node
{
	public:
		insertStmt(
			OW_String* pNewINSERT1,
			OW_String* pNewINTO2,
			OW_String* pNewstrRelationName3,
			insertRest* pNewinsertRest4
		)
			: m_pINSERT1(pNewINSERT1)
			, m_pINTO2(pNewINTO2)
			, m_pstrRelationName3(pNewstrRelationName3)
			, m_pinsertRest4(pNewinsertRest4)
		{}

		virtual ~insertStmt();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_insertStmt( this );
		}

		OW_String* m_pINSERT1;
		OW_String* m_pINTO2;
		OW_String* m_pstrRelationName3;
		insertRest* m_pinsertRest4;
};

class insertRest: public node
{
	public:
		insertRest()
			{}
		virtual ~insertRest() {}

};

class insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN : public insertRest
{
	public:
		insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN(
			OW_String* pNewVALUES1,
			OW_String* pNewLEFTPAREN2,
			OW_List< targetEl* >* pNewtargetList3,
			OW_String* pNewRIGHTPAREN4
		)
			: insertRest()
			, m_pVALUES1(pNewVALUES1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_ptargetList3(pNewtargetList3)
			, m_pRIGHTPAREN4(pNewRIGHTPAREN4)
		{}

		virtual ~insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN( this );
		}

		OW_String* m_pVALUES1;
		OW_String* m_pLEFTPAREN2;
		OW_List< targetEl* >* m_ptargetList3;
		OW_String* m_pRIGHTPAREN4;
};

class insertRest_DEFAULT_VALUES : public insertRest
{
	public:
		insertRest_DEFAULT_VALUES(
			OW_String* pNewDEFAULT1,
			OW_String* pNewVALUES2
		)
			: insertRest()
			, m_pDEFAULT1(pNewDEFAULT1)
			, m_pVALUES2(pNewVALUES2)
		{}

		virtual ~insertRest_DEFAULT_VALUES();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_insertRest_DEFAULT_VALUES( this );
		}

		OW_String* m_pDEFAULT1;
		OW_String* m_pVALUES2;
};

class insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN : public insertRest
{
	public:
		insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN(
			OW_String* pNewLEFTPAREN1,
			OW_List< OW_String* >* pNewcolumnList2,
			OW_String* pNewRIGHTPAREN3,
			OW_String* pNewVALUES4,
			OW_String* pNewLEFTPAREN5,
			OW_List< targetEl* >* pNewtargetList6,
			OW_String* pNewRIGHTPAREN7
		)
			: insertRest()
			, m_pLEFTPAREN1(pNewLEFTPAREN1)
			, m_pcolumnList2(pNewcolumnList2)
			, m_pRIGHTPAREN3(pNewRIGHTPAREN3)
			, m_pVALUES4(pNewVALUES4)
			, m_pLEFTPAREN5(pNewLEFTPAREN5)
			, m_ptargetList6(pNewtargetList6)
			, m_pRIGHTPAREN7(pNewRIGHTPAREN7)
		{}

		virtual ~insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN( this );
		}

		OW_String* m_pLEFTPAREN1;
		OW_List< OW_String* >* m_pcolumnList2;
		OW_String* m_pRIGHTPAREN3;
		OW_String* m_pVALUES4;
		OW_String* m_pLEFTPAREN5;
		OW_List< targetEl* >* m_ptargetList6;
		OW_String* m_pRIGHTPAREN7;
};

class deleteStmt : public node
{
	public:
		deleteStmt(
			OW_String* pNewDELETE1,
			OW_String* pNewFROM2,
			OW_String* pNewstrRelationName3,
			optWhereClause* pNewoptWhereClause4
		)
			: m_pDELETE1(pNewDELETE1)
			, m_pFROM2(pNewFROM2)
			, m_pstrRelationName3(pNewstrRelationName3)
			, m_poptWhereClause4(pNewoptWhereClause4)
		{}

		virtual ~deleteStmt();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_deleteStmt( this );
		}

		OW_String* m_pDELETE1;
		OW_String* m_pFROM2;
		OW_String* m_pstrRelationName3;
		optWhereClause* m_poptWhereClause4;
};

class updateStmt : public node
{
	public:
		updateStmt(
			OW_String* pNewUPDATE1,
			OW_String* pNewstrRelationName2,
			OW_String* pNewSET3,
			OW_List< updateTargetEl* >* pNewupdateTargetList4,
			optWhereClause* pNewoptWhereClause5
		)
			: m_pUPDATE1(pNewUPDATE1)
			, m_pstrRelationName2(pNewstrRelationName2)
			, m_pSET3(pNewSET3)
			, m_pupdateTargetList4(pNewupdateTargetList4)
			, m_poptWhereClause5(pNewoptWhereClause5)
		{}

		virtual ~updateStmt();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_updateStmt( this );
		}

		OW_String* m_pUPDATE1;
		OW_String* m_pstrRelationName2;
		OW_String* m_pSET3;
		OW_List< updateTargetEl* >* m_pupdateTargetList4;
		optWhereClause* m_poptWhereClause5;
};

class selectStmt : public node
{
	public:
		selectStmt(
			OW_String* pNewSELECT1,
			optDistinct* pNewoptDistinct2,
			OW_List< targetEl* >* pNewtargetList3,
			optFromClause* pNewoptFromClause4,
			optWhereClause* pNewoptWhereClause5,
			optGroupClause* pNewoptGroupClause6,
			optHavingClause* pNewoptHavingClause7,
			optSortClause* pNewoptSortClause8
		)
			: m_pSELECT1(pNewSELECT1)
			, m_poptDistinct2(pNewoptDistinct2)
			, m_ptargetList3(pNewtargetList3)
			, m_poptFromClause4(pNewoptFromClause4)
			, m_poptWhereClause5(pNewoptWhereClause5)
			, m_poptGroupClause6(pNewoptGroupClause6)
			, m_poptHavingClause7(pNewoptHavingClause7)
			, m_poptSortClause8(pNewoptSortClause8)
		{}

		virtual ~selectStmt();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_selectStmt( this );
		}

		OW_String* m_pSELECT1;
		optDistinct* m_poptDistinct2;
		OW_List< targetEl* >* m_ptargetList3;
		optFromClause* m_poptFromClause4;
		optWhereClause* m_poptWhereClause5;
		optGroupClause* m_poptGroupClause6;
		optHavingClause* m_poptHavingClause7;
		optSortClause* m_poptSortClause8;
};

class exprSeq: public node
{
	public:
		exprSeq()
			{}
		virtual ~exprSeq() {}

};

class exprSeq_aExpr : public exprSeq
{
	public:
		exprSeq_aExpr(
			aExpr* pNewaExpr1
		)
			: exprSeq()
			, m_paExpr1(pNewaExpr1)
		{}

		virtual ~exprSeq_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_exprSeq_aExpr( this );
		}

		aExpr* m_paExpr1;
};

class exprSeq_exprSeq_COMMA_aExpr : public exprSeq
{
	public:
		exprSeq_exprSeq_COMMA_aExpr(
			exprSeq* pNewexprSeq1,
			OW_String* pNewCOMMA2,
			aExpr* pNewaExpr3
		)
			: exprSeq()
			, m_pexprSeq1(pNewexprSeq1)
			, m_pCOMMA2(pNewCOMMA2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~exprSeq_exprSeq_COMMA_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_exprSeq_exprSeq_COMMA_aExpr( this );
		}

		exprSeq* m_pexprSeq1;
		OW_String* m_pCOMMA2;
		aExpr* m_paExpr3;
};

class exprSeq_exprSeq_USING_aExpr : public exprSeq
{
	public:
		exprSeq_exprSeq_USING_aExpr(
			exprSeq* pNewexprSeq1,
			OW_String* pNewUSING2,
			aExpr* pNewaExpr3
		)
			: exprSeq()
			, m_pexprSeq1(pNewexprSeq1)
			, m_pUSING2(pNewUSING2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~exprSeq_exprSeq_USING_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_exprSeq_exprSeq_USING_aExpr( this );
		}

		exprSeq* m_pexprSeq1;
		OW_String* m_pUSING2;
		aExpr* m_paExpr3;
};

class optDistinct: public node
{
	public:
		optDistinct()
			{}
		virtual ~optDistinct() {}

};

class optDistinct_empty : public optDistinct
{
	public:
		optDistinct_empty(
		)
			: optDistinct()
		{}

		virtual ~optDistinct_empty();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optDistinct_empty( this );
		}

};

class optDistinct_DISTINCT : public optDistinct
{
	public:
		optDistinct_DISTINCT(
			OW_String* pNewDISTINCT1
		)
			: optDistinct()
			, m_pDISTINCT1(pNewDISTINCT1)
		{}

		virtual ~optDistinct_DISTINCT();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optDistinct_DISTINCT( this );
		}

		OW_String* m_pDISTINCT1;
};

class optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN : public optDistinct
{
	public:
		optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN(
			OW_String* pNewDISTINCT1,
			OW_String* pNewON2,
			OW_String* pNewLEFTPAREN3,
			exprSeq* pNewexprSeq4,
			OW_String* pNewRIGHTPAREN5
		)
			: optDistinct()
			, m_pDISTINCT1(pNewDISTINCT1)
			, m_pON2(pNewON2)
			, m_pLEFTPAREN3(pNewLEFTPAREN3)
			, m_pexprSeq4(pNewexprSeq4)
			, m_pRIGHTPAREN5(pNewRIGHTPAREN5)
		{}

		virtual ~optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN( this );
		}

		OW_String* m_pDISTINCT1;
		OW_String* m_pON2;
		OW_String* m_pLEFTPAREN3;
		exprSeq* m_pexprSeq4;
		OW_String* m_pRIGHTPAREN5;
};

class optDistinct_ALL : public optDistinct
{
	public:
		optDistinct_ALL(
			OW_String* pNewALL1
		)
			: optDistinct()
			, m_pALL1(pNewALL1)
		{}

		virtual ~optDistinct_ALL();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optDistinct_ALL( this );
		}

		OW_String* m_pALL1;
};

class sortClause : public node
{
	public:
		sortClause(
			OW_String* pNewORDER1,
			OW_String* pNewBY2,
			OW_List< sortby* >* pNewsortbyList3
		)
			: m_pORDER1(pNewORDER1)
			, m_pBY2(pNewBY2)
			, m_psortbyList3(pNewsortbyList3)
		{}

		virtual ~sortClause();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_sortClause( this );
		}

		OW_String* m_pORDER1;
		OW_String* m_pBY2;
		OW_List< sortby* >* m_psortbyList3;
};

class optSortClause: public node
{
	public:
		optSortClause()
			{}
		virtual ~optSortClause() {}

};

class optSortClause_empty : public optSortClause
{
	public:
		optSortClause_empty(
		)
			: optSortClause()
		{}

		virtual ~optSortClause_empty();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optSortClause_empty( this );
		}

};

class optSortClause_sortClause : public optSortClause
{
	public:
		optSortClause_sortClause(
			sortClause* pNewsortClause1
		)
			: optSortClause()
			, m_psortClause1(pNewsortClause1)
		{}

		virtual ~optSortClause_sortClause();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optSortClause_sortClause( this );
		}

		sortClause* m_psortClause1;
};

class sortby : public node
{
	public:
		sortby(
			aExpr* pNewaExpr1,
			OW_String* pNewstrOptOrderSpecification2
		)
			: m_paExpr1(pNewaExpr1)
			, m_pstrOptOrderSpecification2(pNewstrOptOrderSpecification2)
		{}

		virtual ~sortby();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_sortby( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pstrOptOrderSpecification2;
};

class optGroupClause: public node
{
	public:
		optGroupClause()
			{}
		virtual ~optGroupClause() {}

};

class optGroupClause_empty : public optGroupClause
{
	public:
		optGroupClause_empty(
		)
			: optGroupClause()
		{}

		virtual ~optGroupClause_empty();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optGroupClause_empty( this );
		}

};

class optGroupClause_GROUP_BY_exprSeq : public optGroupClause
{
	public:
		optGroupClause_GROUP_BY_exprSeq(
			OW_String* pNewGROUP1,
			OW_String* pNewBY2,
			exprSeq* pNewexprSeq3
		)
			: optGroupClause()
			, m_pGROUP1(pNewGROUP1)
			, m_pBY2(pNewBY2)
			, m_pexprSeq3(pNewexprSeq3)
		{}

		virtual ~optGroupClause_GROUP_BY_exprSeq();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optGroupClause_GROUP_BY_exprSeq( this );
		}

		OW_String* m_pGROUP1;
		OW_String* m_pBY2;
		exprSeq* m_pexprSeq3;
};

class optHavingClause: public node
{
	public:
		optHavingClause()
			{}
		virtual ~optHavingClause() {}

};

class optHavingClause_empty : public optHavingClause
{
	public:
		optHavingClause_empty(
		)
			: optHavingClause()
		{}

		virtual ~optHavingClause_empty();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optHavingClause_empty( this );
		}

};

class optHavingClause_HAVING_aExpr : public optHavingClause
{
	public:
		optHavingClause_HAVING_aExpr(
			OW_String* pNewHAVING1,
			aExpr* pNewaExpr2
		)
			: optHavingClause()
			, m_pHAVING1(pNewHAVING1)
			, m_paExpr2(pNewaExpr2)
		{}

		virtual ~optHavingClause_HAVING_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optHavingClause_HAVING_aExpr( this );
		}

		OW_String* m_pHAVING1;
		aExpr* m_paExpr2;
};

class optFromClause: public node
{
	public:
		optFromClause()
			{}
		virtual ~optFromClause() {}

};

class optFromClause_empty : public optFromClause
{
	public:
		optFromClause_empty(
		)
			: optFromClause()
		{}

		virtual ~optFromClause_empty();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optFromClause_empty( this );
		}

};

class optFromClause_FROM_fromList : public optFromClause
{
	public:
		optFromClause_FROM_fromList(
			OW_String* pNewFROM1,
			OW_List< tableRef* >* pNewfromList2
		)
			: optFromClause()
			, m_pFROM1(pNewFROM1)
			, m_pfromList2(pNewfromList2)
		{}

		virtual ~optFromClause_FROM_fromList();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optFromClause_FROM_fromList( this );
		}

		OW_String* m_pFROM1;
		OW_List< tableRef* >* m_pfromList2;
};

class tableRef: public node
{
	public:
		tableRef()
			{}
		virtual ~tableRef() {}

};

class tableRef_relationExpr : public tableRef
{
	public:
		tableRef_relationExpr(
			relationExpr* pNewrelationExpr1
		)
			: tableRef()
			, m_prelationExpr1(pNewrelationExpr1)
		{}

		virtual ~tableRef_relationExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_tableRef_relationExpr( this );
		}

		relationExpr* m_prelationExpr1;
};

class tableRef_relationExpr_aliasClause : public tableRef
{
	public:
		tableRef_relationExpr_aliasClause(
			relationExpr* pNewrelationExpr1,
			aliasClause* pNewaliasClause2
		)
			: tableRef()
			, m_prelationExpr1(pNewrelationExpr1)
			, m_paliasClause2(pNewaliasClause2)
		{}

		virtual ~tableRef_relationExpr_aliasClause();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_tableRef_relationExpr_aliasClause( this );
		}

		relationExpr* m_prelationExpr1;
		aliasClause* m_paliasClause2;
};

class tableRef_joinedTable : public tableRef
{
	public:
		tableRef_joinedTable(
			joinedTable* pNewjoinedTable1
		)
			: tableRef()
			, m_pjoinedTable1(pNewjoinedTable1)
		{}

		virtual ~tableRef_joinedTable();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_tableRef_joinedTable( this );
		}

		joinedTable* m_pjoinedTable1;
};

class tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause : public tableRef
{
	public:
		tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause(
			OW_String* pNewLEFTPAREN1,
			joinedTable* pNewjoinedTable2,
			OW_String* pNewRIGHTPAREN3,
			aliasClause* pNewaliasClause4
		)
			: tableRef()
			, m_pLEFTPAREN1(pNewLEFTPAREN1)
			, m_pjoinedTable2(pNewjoinedTable2)
			, m_pRIGHTPAREN3(pNewRIGHTPAREN3)
			, m_paliasClause4(pNewaliasClause4)
		{}

		virtual ~tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause( this );
		}

		OW_String* m_pLEFTPAREN1;
		joinedTable* m_pjoinedTable2;
		OW_String* m_pRIGHTPAREN3;
		aliasClause* m_paliasClause4;
};

class joinedTable: public node
{
	public:
		joinedTable()
			{}
		virtual ~joinedTable() {}

};

class joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN : public joinedTable
{
	public:
		joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN(
			OW_String* pNewLEFTPAREN1,
			joinedTable* pNewjoinedTable2,
			OW_String* pNewRIGHTPAREN3
		)
			: joinedTable()
			, m_pLEFTPAREN1(pNewLEFTPAREN1)
			, m_pjoinedTable2(pNewjoinedTable2)
			, m_pRIGHTPAREN3(pNewRIGHTPAREN3)
		{}

		virtual ~joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN( this );
		}

		OW_String* m_pLEFTPAREN1;
		joinedTable* m_pjoinedTable2;
		OW_String* m_pRIGHTPAREN3;
};

class joinedTable_tableRef_CROSS_JOIN_tableRef : public joinedTable
{
	public:
		joinedTable_tableRef_CROSS_JOIN_tableRef(
			tableRef* pNewtableRef1,
			OW_String* pNewCROSS2,
			OW_String* pNewJOIN3,
			tableRef* pNewtableRef4
		)
			: joinedTable()
			, m_ptableRef1(pNewtableRef1)
			, m_pCROSS2(pNewCROSS2)
			, m_pJOIN3(pNewJOIN3)
			, m_ptableRef4(pNewtableRef4)
		{}

		virtual ~joinedTable_tableRef_CROSS_JOIN_tableRef();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinedTable_tableRef_CROSS_JOIN_tableRef( this );
		}

		tableRef* m_ptableRef1;
		OW_String* m_pCROSS2;
		OW_String* m_pJOIN3;
		tableRef* m_ptableRef4;
};

class joinedTable_tableRef_UNIONJOIN_tableRef : public joinedTable
{
	public:
		joinedTable_tableRef_UNIONJOIN_tableRef(
			tableRef* pNewtableRef1,
			OW_String* pNewUNIONJOIN2,
			tableRef* pNewtableRef3
		)
			: joinedTable()
			, m_ptableRef1(pNewtableRef1)
			, m_pUNIONJOIN2(pNewUNIONJOIN2)
			, m_ptableRef3(pNewtableRef3)
		{}

		virtual ~joinedTable_tableRef_UNIONJOIN_tableRef();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinedTable_tableRef_UNIONJOIN_tableRef( this );
		}

		tableRef* m_ptableRef1;
		OW_String* m_pUNIONJOIN2;
		tableRef* m_ptableRef3;
};

class joinedTable_tableRef_joinType_JOIN_tableRef_joinQual : public joinedTable
{
	public:
		joinedTable_tableRef_joinType_JOIN_tableRef_joinQual(
			tableRef* pNewtableRef1,
			joinType* pNewjoinType2,
			OW_String* pNewJOIN3,
			tableRef* pNewtableRef4,
			joinQual* pNewjoinQual5
		)
			: joinedTable()
			, m_ptableRef1(pNewtableRef1)
			, m_pjoinType2(pNewjoinType2)
			, m_pJOIN3(pNewJOIN3)
			, m_ptableRef4(pNewtableRef4)
			, m_pjoinQual5(pNewjoinQual5)
		{}

		virtual ~joinedTable_tableRef_joinType_JOIN_tableRef_joinQual();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinedTable_tableRef_joinType_JOIN_tableRef_joinQual( this );
		}

		tableRef* m_ptableRef1;
		joinType* m_pjoinType2;
		OW_String* m_pJOIN3;
		tableRef* m_ptableRef4;
		joinQual* m_pjoinQual5;
};

class joinedTable_tableRef_JOIN_tableRef_joinQual : public joinedTable
{
	public:
		joinedTable_tableRef_JOIN_tableRef_joinQual(
			tableRef* pNewtableRef1,
			OW_String* pNewJOIN2,
			tableRef* pNewtableRef3,
			joinQual* pNewjoinQual4
		)
			: joinedTable()
			, m_ptableRef1(pNewtableRef1)
			, m_pJOIN2(pNewJOIN2)
			, m_ptableRef3(pNewtableRef3)
			, m_pjoinQual4(pNewjoinQual4)
		{}

		virtual ~joinedTable_tableRef_JOIN_tableRef_joinQual();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinedTable_tableRef_JOIN_tableRef_joinQual( this );
		}

		tableRef* m_ptableRef1;
		OW_String* m_pJOIN2;
		tableRef* m_ptableRef3;
		joinQual* m_pjoinQual4;
};

class joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef : public joinedTable
{
	public:
		joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef(
			tableRef* pNewtableRef1,
			OW_String* pNewNATURAL2,
			joinType* pNewjoinType3,
			OW_String* pNewJOIN4,
			tableRef* pNewtableRef5
		)
			: joinedTable()
			, m_ptableRef1(pNewtableRef1)
			, m_pNATURAL2(pNewNATURAL2)
			, m_pjoinType3(pNewjoinType3)
			, m_pJOIN4(pNewJOIN4)
			, m_ptableRef5(pNewtableRef5)
		{}

		virtual ~joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef( this );
		}

		tableRef* m_ptableRef1;
		OW_String* m_pNATURAL2;
		joinType* m_pjoinType3;
		OW_String* m_pJOIN4;
		tableRef* m_ptableRef5;
};

class joinedTable_tableRef_NATURAL_JOIN_tableRef : public joinedTable
{
	public:
		joinedTable_tableRef_NATURAL_JOIN_tableRef(
			tableRef* pNewtableRef1,
			OW_String* pNewNATURAL2,
			OW_String* pNewJOIN3,
			tableRef* pNewtableRef4
		)
			: joinedTable()
			, m_ptableRef1(pNewtableRef1)
			, m_pNATURAL2(pNewNATURAL2)
			, m_pJOIN3(pNewJOIN3)
			, m_ptableRef4(pNewtableRef4)
		{}

		virtual ~joinedTable_tableRef_NATURAL_JOIN_tableRef();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinedTable_tableRef_NATURAL_JOIN_tableRef( this );
		}

		tableRef* m_ptableRef1;
		OW_String* m_pNATURAL2;
		OW_String* m_pJOIN3;
		tableRef* m_ptableRef4;
};

class aliasClause: public node
{
	public:
		aliasClause()
			{}
		virtual ~aliasClause() {}

};

class aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN : public aliasClause
{
	public:
		aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN(
			OW_String* pNewAS1,
			OW_String* pNewstrColId2,
			OW_String* pNewLEFTPAREN3,
			OW_List< OW_String* >* pNewnameList4,
			OW_String* pNewRIGHTPAREN5
		)
			: aliasClause()
			, m_pAS1(pNewAS1)
			, m_pstrColId2(pNewstrColId2)
			, m_pLEFTPAREN3(pNewLEFTPAREN3)
			, m_pnameList4(pNewnameList4)
			, m_pRIGHTPAREN5(pNewRIGHTPAREN5)
		{}

		virtual ~aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN( this );
		}

		OW_String* m_pAS1;
		OW_String* m_pstrColId2;
		OW_String* m_pLEFTPAREN3;
		OW_List< OW_String* >* m_pnameList4;
		OW_String* m_pRIGHTPAREN5;
};

class aliasClause_AS_strColId : public aliasClause
{
	public:
		aliasClause_AS_strColId(
			OW_String* pNewAS1,
			OW_String* pNewstrColId2
		)
			: aliasClause()
			, m_pAS1(pNewAS1)
			, m_pstrColId2(pNewstrColId2)
		{}

		virtual ~aliasClause_AS_strColId();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aliasClause_AS_strColId( this );
		}

		OW_String* m_pAS1;
		OW_String* m_pstrColId2;
};

class aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN : public aliasClause
{
	public:
		aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN(
			OW_String* pNewstrColId1,
			OW_String* pNewLEFTPAREN2,
			OW_List< OW_String* >* pNewnameList3,
			OW_String* pNewRIGHTPAREN4
		)
			: aliasClause()
			, m_pstrColId1(pNewstrColId1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_pnameList3(pNewnameList3)
			, m_pRIGHTPAREN4(pNewRIGHTPAREN4)
		{}

		virtual ~aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN( this );
		}

		OW_String* m_pstrColId1;
		OW_String* m_pLEFTPAREN2;
		OW_List< OW_String* >* m_pnameList3;
		OW_String* m_pRIGHTPAREN4;
};

class aliasClause_strColId : public aliasClause
{
	public:
		aliasClause_strColId(
			OW_String* pNewstrColId1
		)
			: aliasClause()
			, m_pstrColId1(pNewstrColId1)
		{}

		virtual ~aliasClause_strColId();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aliasClause_strColId( this );
		}

		OW_String* m_pstrColId1;
};

class joinType: public node
{
	public:
		joinType()
			{}
		virtual ~joinType() {}

};

class joinType_FULL_strOptJoinOuter : public joinType
{
	public:
		joinType_FULL_strOptJoinOuter(
			OW_String* pNewFULL1,
			OW_String* pNewstrOptJoinOuter2
		)
			: joinType()
			, m_pFULL1(pNewFULL1)
			, m_pstrOptJoinOuter2(pNewstrOptJoinOuter2)
		{}

		virtual ~joinType_FULL_strOptJoinOuter();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinType_FULL_strOptJoinOuter( this );
		}

		OW_String* m_pFULL1;
		OW_String* m_pstrOptJoinOuter2;
};

class joinType_LEFT_strOptJoinOuter : public joinType
{
	public:
		joinType_LEFT_strOptJoinOuter(
			OW_String* pNewLEFT1,
			OW_String* pNewstrOptJoinOuter2
		)
			: joinType()
			, m_pLEFT1(pNewLEFT1)
			, m_pstrOptJoinOuter2(pNewstrOptJoinOuter2)
		{}

		virtual ~joinType_LEFT_strOptJoinOuter();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinType_LEFT_strOptJoinOuter( this );
		}

		OW_String* m_pLEFT1;
		OW_String* m_pstrOptJoinOuter2;
};

class joinType_RIGHT_strOptJoinOuter : public joinType
{
	public:
		joinType_RIGHT_strOptJoinOuter(
			OW_String* pNewRIGHT1,
			OW_String* pNewstrOptJoinOuter2
		)
			: joinType()
			, m_pRIGHT1(pNewRIGHT1)
			, m_pstrOptJoinOuter2(pNewstrOptJoinOuter2)
		{}

		virtual ~joinType_RIGHT_strOptJoinOuter();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinType_RIGHT_strOptJoinOuter( this );
		}

		OW_String* m_pRIGHT1;
		OW_String* m_pstrOptJoinOuter2;
};

class joinType_INNERP : public joinType
{
	public:
		joinType_INNERP(
			OW_String* pNewINNERP1
		)
			: joinType()
			, m_pINNERP1(pNewINNERP1)
		{}

		virtual ~joinType_INNERP();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinType_INNERP( this );
		}

		OW_String* m_pINNERP1;
};

class joinQual: public node
{
	public:
		joinQual()
			{}
		virtual ~joinQual() {}

};

class joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN : public joinQual
{
	public:
		joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN(
			OW_String* pNewUSING1,
			OW_String* pNewLEFTPAREN2,
			OW_List< OW_String* >* pNewnameList3,
			OW_String* pNewRIGHTPAREN4
		)
			: joinQual()
			, m_pUSING1(pNewUSING1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_pnameList3(pNewnameList3)
			, m_pRIGHTPAREN4(pNewRIGHTPAREN4)
		{}

		virtual ~joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN( this );
		}

		OW_String* m_pUSING1;
		OW_String* m_pLEFTPAREN2;
		OW_List< OW_String* >* m_pnameList3;
		OW_String* m_pRIGHTPAREN4;
};

class joinQual_ON_aExpr : public joinQual
{
	public:
		joinQual_ON_aExpr(
			OW_String* pNewON1,
			aExpr* pNewaExpr2
		)
			: joinQual()
			, m_pON1(pNewON1)
			, m_paExpr2(pNewaExpr2)
		{}

		virtual ~joinQual_ON_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_joinQual_ON_aExpr( this );
		}

		OW_String* m_pON1;
		aExpr* m_paExpr2;
};

class relationExpr: public node
{
	public:
		relationExpr()
			{}
		virtual ~relationExpr() {}

};

class relationExpr_strRelationName : public relationExpr
{
	public:
		relationExpr_strRelationName(
			OW_String* pNewstrRelationName1
		)
			: relationExpr()
			, m_pstrRelationName1(pNewstrRelationName1)
		{}

		virtual ~relationExpr_strRelationName();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_relationExpr_strRelationName( this );
		}

		OW_String* m_pstrRelationName1;
};

class relationExpr_strRelationName_ASTERISK : public relationExpr
{
	public:
		relationExpr_strRelationName_ASTERISK(
			OW_String* pNewstrRelationName1,
			OW_String* pNewASTERISK2
		)
			: relationExpr()
			, m_pstrRelationName1(pNewstrRelationName1)
			, m_pASTERISK2(pNewASTERISK2)
		{}

		virtual ~relationExpr_strRelationName_ASTERISK();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_relationExpr_strRelationName_ASTERISK( this );
		}

		OW_String* m_pstrRelationName1;
		OW_String* m_pASTERISK2;
};

class relationExpr_ONLY_strRelationName : public relationExpr
{
	public:
		relationExpr_ONLY_strRelationName(
			OW_String* pNewONLY1,
			OW_String* pNewstrRelationName2
		)
			: relationExpr()
			, m_pONLY1(pNewONLY1)
			, m_pstrRelationName2(pNewstrRelationName2)
		{}

		virtual ~relationExpr_ONLY_strRelationName();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_relationExpr_ONLY_strRelationName( this );
		}

		OW_String* m_pONLY1;
		OW_String* m_pstrRelationName2;
};

class optWhereClause: public node
{
	public:
		optWhereClause()
			{}
		virtual ~optWhereClause() {}

};

class optWhereClause_empty : public optWhereClause
{
	public:
		optWhereClause_empty(
		)
			: optWhereClause()
		{}

		virtual ~optWhereClause_empty();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optWhereClause_empty( this );
		}

};

class optWhereClause_WHERE_aExpr : public optWhereClause
{
	public:
		optWhereClause_WHERE_aExpr(
			OW_String* pNewWHERE1,
			aExpr* pNewaExpr2
		)
			: optWhereClause()
			, m_pWHERE1(pNewWHERE1)
			, m_paExpr2(pNewaExpr2)
		{}

		virtual ~optWhereClause_WHERE_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optWhereClause_WHERE_aExpr( this );
		}

		OW_String* m_pWHERE1;
		aExpr* m_paExpr2;
};

class rowExpr : public node
{
	public:
		rowExpr(
			OW_String* pNewLEFTPAREN1,
			rowDescriptor* pNewrowDescriptor2,
			OW_String* pNewRIGHTPAREN3,
			OW_String* pNewstrAllOp4,
			OW_String* pNewLEFTPAREN5,
			rowDescriptor* pNewrowDescriptor6,
			OW_String* pNewRIGHTPAREN7
		)
			: m_pLEFTPAREN1(pNewLEFTPAREN1)
			, m_prowDescriptor2(pNewrowDescriptor2)
			, m_pRIGHTPAREN3(pNewRIGHTPAREN3)
			, m_pstrAllOp4(pNewstrAllOp4)
			, m_pLEFTPAREN5(pNewLEFTPAREN5)
			, m_prowDescriptor6(pNewrowDescriptor6)
			, m_pRIGHTPAREN7(pNewRIGHTPAREN7)
		{}

		virtual ~rowExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_rowExpr( this );
		}

		OW_String* m_pLEFTPAREN1;
		rowDescriptor* m_prowDescriptor2;
		OW_String* m_pRIGHTPAREN3;
		OW_String* m_pstrAllOp4;
		OW_String* m_pLEFTPAREN5;
		rowDescriptor* m_prowDescriptor6;
		OW_String* m_pRIGHTPAREN7;
};

class rowDescriptor : public node
{
	public:
		rowDescriptor(
			OW_List< aExpr* >* pNewrowList1,
			OW_String* pNewCOMMA2,
			aExpr* pNewaExpr3
		)
			: m_prowList1(pNewrowList1)
			, m_pCOMMA2(pNewCOMMA2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~rowDescriptor();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_rowDescriptor( this );
		}

		OW_List< aExpr* >* m_prowList1;
		OW_String* m_pCOMMA2;
		aExpr* m_paExpr3;
};

class aExpr: public node
{
	public:
		aExpr()
			{}
		virtual ~aExpr() {}

};

class aExpr_cExpr : public aExpr
{
	public:
		aExpr_cExpr(
			cExpr* pNewcExpr1
		)
			: aExpr()
			, m_pcExpr1(pNewcExpr1)
		{}

		virtual ~aExpr_cExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_cExpr( this );
		}

		cExpr* m_pcExpr1;
};

class aExpr_aExpr_AT_TIME_ZONE_cExpr : public aExpr
{
	public:
		aExpr_aExpr_AT_TIME_ZONE_cExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewAT2,
			OW_String* pNewTIME3,
			OW_String* pNewZONE4,
			cExpr* pNewcExpr5
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pAT2(pNewAT2)
			, m_pTIME3(pNewTIME3)
			, m_pZONE4(pNewZONE4)
			, m_pcExpr5(pNewcExpr5)
		{}

		virtual ~aExpr_aExpr_AT_TIME_ZONE_cExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_AT_TIME_ZONE_cExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pAT2;
		OW_String* m_pTIME3;
		OW_String* m_pZONE4;
		cExpr* m_pcExpr5;
};

class aExpr_PLUS_aExpr : public aExpr
{
	public:
		aExpr_PLUS_aExpr(
			OW_String* pNewPLUS1,
			aExpr* pNewaExpr2
		)
			: aExpr()
			, m_pPLUS1(pNewPLUS1)
			, m_paExpr2(pNewaExpr2)
		{}

		virtual ~aExpr_PLUS_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_PLUS_aExpr( this );
		}

		OW_String* m_pPLUS1;
		aExpr* m_paExpr2;
};

class aExpr_MINUS_aExpr : public aExpr
{
	public:
		aExpr_MINUS_aExpr(
			OW_String* pNewMINUS1,
			aExpr* pNewaExpr2
		)
			: aExpr()
			, m_pMINUS1(pNewMINUS1)
			, m_paExpr2(pNewaExpr2)
		{}

		virtual ~aExpr_MINUS_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_MINUS_aExpr( this );
		}

		OW_String* m_pMINUS1;
		aExpr* m_paExpr2;
};

class aExpr_BITINVERT_aExpr : public aExpr
{
	public:
		aExpr_BITINVERT_aExpr(
			OW_String* pNewBITINVERT1,
			aExpr* pNewaExpr2
		)
			: aExpr()
			, m_pBITINVERT1(pNewBITINVERT1)
			, m_paExpr2(pNewaExpr2)
		{}

		virtual ~aExpr_BITINVERT_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_BITINVERT_aExpr( this );
		}

		OW_String* m_pBITINVERT1;
		aExpr* m_paExpr2;
};

class aExpr_aExpr_PLUS_aExpr : public aExpr
{
	public:
		aExpr_aExpr_PLUS_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewPLUS2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pPLUS2(pNewPLUS2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_PLUS_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_PLUS_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pPLUS2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_MINUS_aExpr : public aExpr
{
	public:
		aExpr_aExpr_MINUS_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewMINUS2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pMINUS2(pNewMINUS2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_MINUS_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_MINUS_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pMINUS2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_ASTERISK_aExpr : public aExpr
{
	public:
		aExpr_aExpr_ASTERISK_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewASTERISK2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pASTERISK2(pNewASTERISK2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_ASTERISK_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_ASTERISK_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pASTERISK2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_SOLIDUS_aExpr : public aExpr
{
	public:
		aExpr_aExpr_SOLIDUS_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewSOLIDUS2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pSOLIDUS2(pNewSOLIDUS2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_SOLIDUS_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_SOLIDUS_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pSOLIDUS2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_PERCENT_aExpr : public aExpr
{
	public:
		aExpr_aExpr_PERCENT_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewPERCENT2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pPERCENT2(pNewPERCENT2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_PERCENT_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_PERCENT_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pPERCENT2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_BITAND_aExpr : public aExpr
{
	public:
		aExpr_aExpr_BITAND_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewBITAND2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pBITAND2(pNewBITAND2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_BITAND_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_BITAND_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pBITAND2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_BITOR_aExpr : public aExpr
{
	public:
		aExpr_aExpr_BITOR_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewBITOR2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pBITOR2(pNewBITOR2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_BITOR_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_BITOR_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pBITOR2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_BITSHIFTLEFT_aExpr : public aExpr
{
	public:
		aExpr_aExpr_BITSHIFTLEFT_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewBITSHIFTLEFT2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pBITSHIFTLEFT2(pNewBITSHIFTLEFT2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_BITSHIFTLEFT_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_BITSHIFTLEFT_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pBITSHIFTLEFT2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_BITSHIFTRIGHT_aExpr : public aExpr
{
	public:
		aExpr_aExpr_BITSHIFTRIGHT_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewBITSHIFTRIGHT2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pBITSHIFTRIGHT2(pNewBITSHIFTRIGHT2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_BITSHIFTRIGHT_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_BITSHIFTRIGHT_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pBITSHIFTRIGHT2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_LESSTHAN_aExpr : public aExpr
{
	public:
		aExpr_aExpr_LESSTHAN_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewLESSTHAN2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pLESSTHAN2(pNewLESSTHAN2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_LESSTHAN_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_LESSTHAN_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pLESSTHAN2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_LESSTHANOREQUALS_aExpr : public aExpr
{
	public:
		aExpr_aExpr_LESSTHANOREQUALS_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewLESSTHANOREQUALS2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pLESSTHANOREQUALS2(pNewLESSTHANOREQUALS2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_LESSTHANOREQUALS_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_LESSTHANOREQUALS_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pLESSTHANOREQUALS2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_GREATERTHAN_aExpr : public aExpr
{
	public:
		aExpr_aExpr_GREATERTHAN_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewGREATERTHAN2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pGREATERTHAN2(pNewGREATERTHAN2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_GREATERTHAN_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_GREATERTHAN_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pGREATERTHAN2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_GREATERTHANOREQUALS_aExpr : public aExpr
{
	public:
		aExpr_aExpr_GREATERTHANOREQUALS_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewGREATERTHANOREQUALS2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pGREATERTHANOREQUALS2(pNewGREATERTHANOREQUALS2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_GREATERTHANOREQUALS_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_GREATERTHANOREQUALS_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pGREATERTHANOREQUALS2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_EQUALS_aExpr : public aExpr
{
	public:
		aExpr_aExpr_EQUALS_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewEQUALS2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pEQUALS2(pNewEQUALS2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_EQUALS_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_EQUALS_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pEQUALS2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_NOTEQUALS_aExpr : public aExpr
{
	public:
		aExpr_aExpr_NOTEQUALS_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewNOTEQUALS2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pNOTEQUALS2(pNewNOTEQUALS2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_NOTEQUALS_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_NOTEQUALS_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pNOTEQUALS2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_AND_aExpr : public aExpr
{
	public:
		aExpr_aExpr_AND_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewAND2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pAND2(pNewAND2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_AND_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_AND_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pAND2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_OR_aExpr : public aExpr
{
	public:
		aExpr_aExpr_OR_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewOR2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pOR2(pNewOR2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_OR_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_OR_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pOR2;
		aExpr* m_paExpr3;
};

class aExpr_NOT_aExpr : public aExpr
{
	public:
		aExpr_NOT_aExpr(
			OW_String* pNewNOT1,
			aExpr* pNewaExpr2
		)
			: aExpr()
			, m_pNOT1(pNewNOT1)
			, m_paExpr2(pNewaExpr2)
		{}

		virtual ~aExpr_NOT_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_NOT_aExpr( this );
		}

		OW_String* m_pNOT1;
		aExpr* m_paExpr2;
};

class aExpr_aExpr_CONCATENATION_aExpr : public aExpr
{
	public:
		aExpr_aExpr_CONCATENATION_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewCONCATENATION2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pCONCATENATION2(pNewCONCATENATION2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_CONCATENATION_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_CONCATENATION_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pCONCATENATION2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_LIKE_aExpr : public aExpr
{
	public:
		aExpr_aExpr_LIKE_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewLIKE2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pLIKE2(pNewLIKE2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_LIKE_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_LIKE_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pLIKE2;
		aExpr* m_paExpr3;
};

class aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr : public aExpr
{
	public:
		aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewLIKE2,
			aExpr* pNewaExpr3,
			OW_String* pNewESCAPE4,
			aExpr* pNewaExpr5
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pLIKE2(pNewLIKE2)
			, m_paExpr3(pNewaExpr3)
			, m_pESCAPE4(pNewESCAPE4)
			, m_paExpr5(pNewaExpr5)
		{}

		virtual ~aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pLIKE2;
		aExpr* m_paExpr3;
		OW_String* m_pESCAPE4;
		aExpr* m_paExpr5;
};

class aExpr_aExpr_NOT_LIKE_aExpr : public aExpr
{
	public:
		aExpr_aExpr_NOT_LIKE_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewNOT2,
			OW_String* pNewLIKE3,
			aExpr* pNewaExpr4
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pNOT2(pNewNOT2)
			, m_pLIKE3(pNewLIKE3)
			, m_paExpr4(pNewaExpr4)
		{}

		virtual ~aExpr_aExpr_NOT_LIKE_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_NOT_LIKE_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pNOT2;
		OW_String* m_pLIKE3;
		aExpr* m_paExpr4;
};

class aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr : public aExpr
{
	public:
		aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewNOT2,
			OW_String* pNewLIKE3,
			aExpr* pNewaExpr4,
			OW_String* pNewESCAPE5,
			aExpr* pNewaExpr6
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pNOT2(pNewNOT2)
			, m_pLIKE3(pNewLIKE3)
			, m_paExpr4(pNewaExpr4)
			, m_pESCAPE5(pNewESCAPE5)
			, m_paExpr6(pNewaExpr6)
		{}

		virtual ~aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pNOT2;
		OW_String* m_pLIKE3;
		aExpr* m_paExpr4;
		OW_String* m_pESCAPE5;
		aExpr* m_paExpr6;
};

class aExpr_aExpr_ISNULL : public aExpr
{
	public:
		aExpr_aExpr_ISNULL(
			aExpr* pNewaExpr1,
			OW_String* pNewISNULL2
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pISNULL2(pNewISNULL2)
		{}

		virtual ~aExpr_aExpr_ISNULL();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_ISNULL( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pISNULL2;
};

class aExpr_aExpr_IS_NULLP : public aExpr
{
	public:
		aExpr_aExpr_IS_NULLP(
			aExpr* pNewaExpr1,
			OW_String* pNewIS2,
			OW_String* pNewNULLP3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pIS2(pNewIS2)
			, m_pNULLP3(pNewNULLP3)
		{}

		virtual ~aExpr_aExpr_IS_NULLP();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_IS_NULLP( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pIS2;
		OW_String* m_pNULLP3;
};

class aExpr_aExpr_NOTNULL : public aExpr
{
	public:
		aExpr_aExpr_NOTNULL(
			aExpr* pNewaExpr1,
			OW_String* pNewNOTNULL2
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pNOTNULL2(pNewNOTNULL2)
		{}

		virtual ~aExpr_aExpr_NOTNULL();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_NOTNULL( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pNOTNULL2;
};

class aExpr_aExpr_IS_NOT_NULLP : public aExpr
{
	public:
		aExpr_aExpr_IS_NOT_NULLP(
			aExpr* pNewaExpr1,
			OW_String* pNewIS2,
			OW_String* pNewNOT3,
			OW_String* pNewNULLP4
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pIS2(pNewIS2)
			, m_pNOT3(pNewNOT3)
			, m_pNULLP4(pNewNULLP4)
		{}

		virtual ~aExpr_aExpr_IS_NOT_NULLP();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_IS_NOT_NULLP( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pIS2;
		OW_String* m_pNOT3;
		OW_String* m_pNULLP4;
};

class aExpr_aExpr_IS_TRUEP : public aExpr
{
	public:
		aExpr_aExpr_IS_TRUEP(
			aExpr* pNewaExpr1,
			OW_String* pNewIS2,
			OW_String* pNewTRUEP3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pIS2(pNewIS2)
			, m_pTRUEP3(pNewTRUEP3)
		{}

		virtual ~aExpr_aExpr_IS_TRUEP();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_IS_TRUEP( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pIS2;
		OW_String* m_pTRUEP3;
};

class aExpr_aExpr_IS_NOT_FALSEP : public aExpr
{
	public:
		aExpr_aExpr_IS_NOT_FALSEP(
			aExpr* pNewaExpr1,
			OW_String* pNewIS2,
			OW_String* pNewNOT3,
			OW_String* pNewFALSEP4
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pIS2(pNewIS2)
			, m_pNOT3(pNewNOT3)
			, m_pFALSEP4(pNewFALSEP4)
		{}

		virtual ~aExpr_aExpr_IS_NOT_FALSEP();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_IS_NOT_FALSEP( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pIS2;
		OW_String* m_pNOT3;
		OW_String* m_pFALSEP4;
};

class aExpr_aExpr_IS_FALSEP : public aExpr
{
	public:
		aExpr_aExpr_IS_FALSEP(
			aExpr* pNewaExpr1,
			OW_String* pNewIS2,
			OW_String* pNewFALSEP3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pIS2(pNewIS2)
			, m_pFALSEP3(pNewFALSEP3)
		{}

		virtual ~aExpr_aExpr_IS_FALSEP();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_IS_FALSEP( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pIS2;
		OW_String* m_pFALSEP3;
};

class aExpr_aExpr_IS_NOT_TRUEP : public aExpr
{
	public:
		aExpr_aExpr_IS_NOT_TRUEP(
			aExpr* pNewaExpr1,
			OW_String* pNewIS2,
			OW_String* pNewNOT3,
			OW_String* pNewTRUEP4
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pIS2(pNewIS2)
			, m_pNOT3(pNewNOT3)
			, m_pTRUEP4(pNewTRUEP4)
		{}

		virtual ~aExpr_aExpr_IS_NOT_TRUEP();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_IS_NOT_TRUEP( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pIS2;
		OW_String* m_pNOT3;
		OW_String* m_pTRUEP4;
};

class aExpr_aExpr_ISA_aExpr : public aExpr
{
	public:
		aExpr_aExpr_ISA_aExpr(
			aExpr* pNewaExpr1,
			OW_String* pNewISA2,
			aExpr* pNewaExpr3
		)
			: aExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pISA2(pNewISA2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~aExpr_aExpr_ISA_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_aExpr_ISA_aExpr( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pISA2;
		aExpr* m_paExpr3;
};

class aExpr_rowExpr : public aExpr
{
	public:
		aExpr_rowExpr(
			rowExpr* pNewrowExpr1
		)
			: aExpr()
			, m_prowExpr1(pNewrowExpr1)
		{}

		virtual ~aExpr_rowExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExpr_rowExpr( this );
		}

		rowExpr* m_prowExpr1;
};

class bExpr: public node
{
	public:
		bExpr()
			{}
		virtual ~bExpr() {}

};

class bExpr_cExpr : public bExpr
{
	public:
		bExpr_cExpr(
			cExpr* pNewcExpr1
		)
			: bExpr()
			, m_pcExpr1(pNewcExpr1)
		{}

		virtual ~bExpr_cExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_cExpr( this );
		}

		cExpr* m_pcExpr1;
};

class bExpr_PLUS_bExpr : public bExpr
{
	public:
		bExpr_PLUS_bExpr(
			OW_String* pNewPLUS1,
			bExpr* pNewbExpr2
		)
			: bExpr()
			, m_pPLUS1(pNewPLUS1)
			, m_pbExpr2(pNewbExpr2)
		{}

		virtual ~bExpr_PLUS_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_PLUS_bExpr( this );
		}

		OW_String* m_pPLUS1;
		bExpr* m_pbExpr2;
};

class bExpr_MINUS_bExpr : public bExpr
{
	public:
		bExpr_MINUS_bExpr(
			OW_String* pNewMINUS1,
			bExpr* pNewbExpr2
		)
			: bExpr()
			, m_pMINUS1(pNewMINUS1)
			, m_pbExpr2(pNewbExpr2)
		{}

		virtual ~bExpr_MINUS_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_MINUS_bExpr( this );
		}

		OW_String* m_pMINUS1;
		bExpr* m_pbExpr2;
};

class bExpr_BITINVERT_bExpr : public bExpr
{
	public:
		bExpr_BITINVERT_bExpr(
			OW_String* pNewBITINVERT1,
			bExpr* pNewbExpr2
		)
			: bExpr()
			, m_pBITINVERT1(pNewBITINVERT1)
			, m_pbExpr2(pNewbExpr2)
		{}

		virtual ~bExpr_BITINVERT_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_BITINVERT_bExpr( this );
		}

		OW_String* m_pBITINVERT1;
		bExpr* m_pbExpr2;
};

class bExpr_bExpr_PLUS_bExpr : public bExpr
{
	public:
		bExpr_bExpr_PLUS_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewPLUS2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pPLUS2(pNewPLUS2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_PLUS_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_PLUS_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pPLUS2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_MINUS_bExpr : public bExpr
{
	public:
		bExpr_bExpr_MINUS_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewMINUS2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pMINUS2(pNewMINUS2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_MINUS_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_MINUS_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pMINUS2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_ASTERISK_bExpr : public bExpr
{
	public:
		bExpr_bExpr_ASTERISK_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewASTERISK2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pASTERISK2(pNewASTERISK2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_ASTERISK_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_ASTERISK_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pASTERISK2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_SOLIDUS_bExpr : public bExpr
{
	public:
		bExpr_bExpr_SOLIDUS_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewSOLIDUS2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pSOLIDUS2(pNewSOLIDUS2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_SOLIDUS_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_SOLIDUS_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pSOLIDUS2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_PERCENT_bExpr : public bExpr
{
	public:
		bExpr_bExpr_PERCENT_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewPERCENT2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pPERCENT2(pNewPERCENT2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_PERCENT_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_PERCENT_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pPERCENT2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_BITAND_bExpr : public bExpr
{
	public:
		bExpr_bExpr_BITAND_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewBITAND2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pBITAND2(pNewBITAND2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_BITAND_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_BITAND_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pBITAND2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_BITOR_bExpr : public bExpr
{
	public:
		bExpr_bExpr_BITOR_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewBITOR2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pBITOR2(pNewBITOR2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_BITOR_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_BITOR_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pBITOR2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_BITSHIFTLEFT_bExpr : public bExpr
{
	public:
		bExpr_bExpr_BITSHIFTLEFT_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewBITSHIFTLEFT2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pBITSHIFTLEFT2(pNewBITSHIFTLEFT2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_BITSHIFTLEFT_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_BITSHIFTLEFT_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pBITSHIFTLEFT2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_BITSHIFTRIGHT_bExpr : public bExpr
{
	public:
		bExpr_bExpr_BITSHIFTRIGHT_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewBITSHIFTRIGHT2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pBITSHIFTRIGHT2(pNewBITSHIFTRIGHT2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_BITSHIFTRIGHT_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_BITSHIFTRIGHT_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pBITSHIFTRIGHT2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_LESSTHAN_bExpr : public bExpr
{
	public:
		bExpr_bExpr_LESSTHAN_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewLESSTHAN2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pLESSTHAN2(pNewLESSTHAN2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_LESSTHAN_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_LESSTHAN_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pLESSTHAN2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_LESSTHANOREQUALS_bExpr : public bExpr
{
	public:
		bExpr_bExpr_LESSTHANOREQUALS_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewLESSTHANOREQUALS2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pLESSTHANOREQUALS2(pNewLESSTHANOREQUALS2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_LESSTHANOREQUALS_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_LESSTHANOREQUALS_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pLESSTHANOREQUALS2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_GREATERTHAN_bExpr : public bExpr
{
	public:
		bExpr_bExpr_GREATERTHAN_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewGREATERTHAN2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pGREATERTHAN2(pNewGREATERTHAN2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_GREATERTHAN_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_GREATERTHAN_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pGREATERTHAN2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_GREATERTHANOREQUALS_bExpr : public bExpr
{
	public:
		bExpr_bExpr_GREATERTHANOREQUALS_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewGREATERTHANOREQUALS2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pGREATERTHANOREQUALS2(pNewGREATERTHANOREQUALS2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_GREATERTHANOREQUALS_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_GREATERTHANOREQUALS_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pGREATERTHANOREQUALS2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_EQUALS_bExpr : public bExpr
{
	public:
		bExpr_bExpr_EQUALS_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewEQUALS2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pEQUALS2(pNewEQUALS2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_EQUALS_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_EQUALS_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pEQUALS2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_NOTEQUALS_bExpr : public bExpr
{
	public:
		bExpr_bExpr_NOTEQUALS_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewNOTEQUALS2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pNOTEQUALS2(pNewNOTEQUALS2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_NOTEQUALS_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_NOTEQUALS_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pNOTEQUALS2;
		bExpr* m_pbExpr3;
};

class bExpr_bExpr_CONCATENATION_bExpr : public bExpr
{
	public:
		bExpr_bExpr_CONCATENATION_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewCONCATENATION2,
			bExpr* pNewbExpr3
		)
			: bExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pCONCATENATION2(pNewCONCATENATION2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~bExpr_bExpr_CONCATENATION_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_bExpr_bExpr_CONCATENATION_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pCONCATENATION2;
		bExpr* m_pbExpr3;
};

class cExpr: public node
{
	public:
		cExpr()
			{}
		virtual ~cExpr() {}

};

class cExpr_attr : public cExpr
{
	public:
		cExpr_attr(
			attr* pNewattr1
		)
			: cExpr()
			, m_pattr1(pNewattr1)
		{}

		virtual ~cExpr_attr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_attr( this );
		}

		attr* m_pattr1;
};

class cExpr_strColId_optIndirection : public cExpr
{
	public:
		cExpr_strColId_optIndirection(
			OW_String* pNewstrColId1,
			optIndirection* pNewoptIndirection2
		)
			: cExpr()
			, m_pstrColId1(pNewstrColId1)
			, m_poptIndirection2(pNewoptIndirection2)
		{}

		virtual ~cExpr_strColId_optIndirection();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_strColId_optIndirection( this );
		}

		OW_String* m_pstrColId1;
		optIndirection* m_poptIndirection2;
};

class cExpr_aExprConst : public cExpr
{
	public:
		cExpr_aExprConst(
			aExprConst* pNewaExprConst1
		)
			: cExpr()
			, m_paExprConst1(pNewaExprConst1)
		{}

		virtual ~cExpr_aExprConst();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_aExprConst( this );
		}

		aExprConst* m_paExprConst1;
};

class cExpr_LEFTPAREN_aExpr_RIGHTPAREN : public cExpr
{
	public:
		cExpr_LEFTPAREN_aExpr_RIGHTPAREN(
			OW_String* pNewLEFTPAREN1,
			aExpr* pNewaExpr2,
			OW_String* pNewRIGHTPAREN3
		)
			: cExpr()
			, m_pLEFTPAREN1(pNewLEFTPAREN1)
			, m_paExpr2(pNewaExpr2)
			, m_pRIGHTPAREN3(pNewRIGHTPAREN3)
		{}

		virtual ~cExpr_LEFTPAREN_aExpr_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_LEFTPAREN_aExpr_RIGHTPAREN( this );
		}

		OW_String* m_pLEFTPAREN1;
		aExpr* m_paExpr2;
		OW_String* m_pRIGHTPAREN3;
};

class cExpr_strFuncName_LEFTPAREN_RIGHTPAREN : public cExpr
{
	public:
		cExpr_strFuncName_LEFTPAREN_RIGHTPAREN(
			OW_String* pNewstrFuncName1,
			OW_String* pNewLEFTPAREN2,
			OW_String* pNewRIGHTPAREN3
		)
			: cExpr()
			, m_pstrFuncName1(pNewstrFuncName1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_pRIGHTPAREN3(pNewRIGHTPAREN3)
		{}

		virtual ~cExpr_strFuncName_LEFTPAREN_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_strFuncName_LEFTPAREN_RIGHTPAREN( this );
		}

		OW_String* m_pstrFuncName1;
		OW_String* m_pLEFTPAREN2;
		OW_String* m_pRIGHTPAREN3;
};

class cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN : public cExpr
{
	public:
		cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN(
			OW_String* pNewstrFuncName1,
			OW_String* pNewLEFTPAREN2,
			exprSeq* pNewexprSeq3,
			OW_String* pNewRIGHTPAREN4
		)
			: cExpr()
			, m_pstrFuncName1(pNewstrFuncName1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_pexprSeq3(pNewexprSeq3)
			, m_pRIGHTPAREN4(pNewRIGHTPAREN4)
		{}

		virtual ~cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN( this );
		}

		OW_String* m_pstrFuncName1;
		OW_String* m_pLEFTPAREN2;
		exprSeq* m_pexprSeq3;
		OW_String* m_pRIGHTPAREN4;
};

class cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN : public cExpr
{
	public:
		cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN(
			OW_String* pNewstrFuncName1,
			OW_String* pNewLEFTPAREN2,
			OW_String* pNewALL3,
			exprSeq* pNewexprSeq4,
			OW_String* pNewRIGHTPAREN5
		)
			: cExpr()
			, m_pstrFuncName1(pNewstrFuncName1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_pALL3(pNewALL3)
			, m_pexprSeq4(pNewexprSeq4)
			, m_pRIGHTPAREN5(pNewRIGHTPAREN5)
		{}

		virtual ~cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN( this );
		}

		OW_String* m_pstrFuncName1;
		OW_String* m_pLEFTPAREN2;
		OW_String* m_pALL3;
		exprSeq* m_pexprSeq4;
		OW_String* m_pRIGHTPAREN5;
};

class cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN : public cExpr
{
	public:
		cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN(
			OW_String* pNewstrFuncName1,
			OW_String* pNewLEFTPAREN2,
			OW_String* pNewDISTINCT3,
			exprSeq* pNewexprSeq4,
			OW_String* pNewRIGHTPAREN5
		)
			: cExpr()
			, m_pstrFuncName1(pNewstrFuncName1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_pDISTINCT3(pNewDISTINCT3)
			, m_pexprSeq4(pNewexprSeq4)
			, m_pRIGHTPAREN5(pNewRIGHTPAREN5)
		{}

		virtual ~cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN( this );
		}

		OW_String* m_pstrFuncName1;
		OW_String* m_pLEFTPAREN2;
		OW_String* m_pDISTINCT3;
		exprSeq* m_pexprSeq4;
		OW_String* m_pRIGHTPAREN5;
};

class cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN : public cExpr
{
	public:
		cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN(
			OW_String* pNewstrFuncName1,
			OW_String* pNewLEFTPAREN2,
			OW_String* pNewASTERISK3,
			OW_String* pNewRIGHTPAREN4
		)
			: cExpr()
			, m_pstrFuncName1(pNewstrFuncName1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_pASTERISK3(pNewASTERISK3)
			, m_pRIGHTPAREN4(pNewRIGHTPAREN4)
		{}

		virtual ~cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN( this );
		}

		OW_String* m_pstrFuncName1;
		OW_String* m_pLEFTPAREN2;
		OW_String* m_pASTERISK3;
		OW_String* m_pRIGHTPAREN4;
};

class cExpr_CURRENTDATE : public cExpr
{
	public:
		cExpr_CURRENTDATE(
			OW_String* pNewCURRENTDATE1
		)
			: cExpr()
			, m_pCURRENTDATE1(pNewCURRENTDATE1)
		{}

		virtual ~cExpr_CURRENTDATE();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_CURRENTDATE( this );
		}

		OW_String* m_pCURRENTDATE1;
};

class cExpr_CURRENTTIME : public cExpr
{
	public:
		cExpr_CURRENTTIME(
			OW_String* pNewCURRENTTIME1
		)
			: cExpr()
			, m_pCURRENTTIME1(pNewCURRENTTIME1)
		{}

		virtual ~cExpr_CURRENTTIME();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_CURRENTTIME( this );
		}

		OW_String* m_pCURRENTTIME1;
};

class cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN : public cExpr
{
	public:
		cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN(
			OW_String* pNewCURRENTTIME1,
			OW_String* pNewLEFTPAREN2,
			OW_String* pNewICONST3,
			OW_String* pNewRIGHTPAREN4
		)
			: cExpr()
			, m_pCURRENTTIME1(pNewCURRENTTIME1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_pICONST3(pNewICONST3)
			, m_pRIGHTPAREN4(pNewRIGHTPAREN4)
		{}

		virtual ~cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN( this );
		}

		OW_String* m_pCURRENTTIME1;
		OW_String* m_pLEFTPAREN2;
		OW_String* m_pICONST3;
		OW_String* m_pRIGHTPAREN4;
};

class cExpr_CURRENTTIMESTAMP : public cExpr
{
	public:
		cExpr_CURRENTTIMESTAMP(
			OW_String* pNewCURRENTTIMESTAMP1
		)
			: cExpr()
			, m_pCURRENTTIMESTAMP1(pNewCURRENTTIMESTAMP1)
		{}

		virtual ~cExpr_CURRENTTIMESTAMP();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_CURRENTTIMESTAMP( this );
		}

		OW_String* m_pCURRENTTIMESTAMP1;
};

class cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN : public cExpr
{
	public:
		cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN(
			OW_String* pNewCURRENTTIMESTAMP1,
			OW_String* pNewLEFTPAREN2,
			OW_String* pNewICONST3,
			OW_String* pNewRIGHTPAREN4
		)
			: cExpr()
			, m_pCURRENTTIMESTAMP1(pNewCURRENTTIMESTAMP1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_pICONST3(pNewICONST3)
			, m_pRIGHTPAREN4(pNewRIGHTPAREN4)
		{}

		virtual ~cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN( this );
		}

		OW_String* m_pCURRENTTIMESTAMP1;
		OW_String* m_pLEFTPAREN2;
		OW_String* m_pICONST3;
		OW_String* m_pRIGHTPAREN4;
};

class cExpr_CURRENTUSER : public cExpr
{
	public:
		cExpr_CURRENTUSER(
			OW_String* pNewCURRENTUSER1
		)
			: cExpr()
			, m_pCURRENTUSER1(pNewCURRENTUSER1)
		{}

		virtual ~cExpr_CURRENTUSER();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_CURRENTUSER( this );
		}

		OW_String* m_pCURRENTUSER1;
};

class cExpr_SESSIONUSER : public cExpr
{
	public:
		cExpr_SESSIONUSER(
			OW_String* pNewSESSIONUSER1
		)
			: cExpr()
			, m_pSESSIONUSER1(pNewSESSIONUSER1)
		{}

		virtual ~cExpr_SESSIONUSER();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_SESSIONUSER( this );
		}

		OW_String* m_pSESSIONUSER1;
};

class cExpr_USER : public cExpr
{
	public:
		cExpr_USER(
			OW_String* pNewUSER1
		)
			: cExpr()
			, m_pUSER1(pNewUSER1)
		{}

		virtual ~cExpr_USER();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_USER( this );
		}

		OW_String* m_pUSER1;
};

class cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN : public cExpr
{
	public:
		cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN(
			OW_String* pNewEXTRACT1,
			OW_String* pNewLEFTPAREN2,
			optExtract* pNewoptExtract3,
			OW_String* pNewRIGHTPAREN4
		)
			: cExpr()
			, m_pEXTRACT1(pNewEXTRACT1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_poptExtract3(pNewoptExtract3)
			, m_pRIGHTPAREN4(pNewRIGHTPAREN4)
		{}

		virtual ~cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN( this );
		}

		OW_String* m_pEXTRACT1;
		OW_String* m_pLEFTPAREN2;
		optExtract* m_poptExtract3;
		OW_String* m_pRIGHTPAREN4;
};

class cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN : public cExpr
{
	public:
		cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN(
			OW_String* pNewPOSITION1,
			OW_String* pNewLEFTPAREN2,
			positionExpr* pNewpositionExpr3,
			OW_String* pNewRIGHTPAREN4
		)
			: cExpr()
			, m_pPOSITION1(pNewPOSITION1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_ppositionExpr3(pNewpositionExpr3)
			, m_pRIGHTPAREN4(pNewRIGHTPAREN4)
		{}

		virtual ~cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN( this );
		}

		OW_String* m_pPOSITION1;
		OW_String* m_pLEFTPAREN2;
		positionExpr* m_ppositionExpr3;
		OW_String* m_pRIGHTPAREN4;
};

class cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN : public cExpr
{
	public:
		cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN(
			OW_String* pNewSUBSTRING1,
			OW_String* pNewLEFTPAREN2,
			optSubstrExpr* pNewoptSubstrExpr3,
			OW_String* pNewRIGHTPAREN4
		)
			: cExpr()
			, m_pSUBSTRING1(pNewSUBSTRING1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_poptSubstrExpr3(pNewoptSubstrExpr3)
			, m_pRIGHTPAREN4(pNewRIGHTPAREN4)
		{}

		virtual ~cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN( this );
		}

		OW_String* m_pSUBSTRING1;
		OW_String* m_pLEFTPAREN2;
		optSubstrExpr* m_poptSubstrExpr3;
		OW_String* m_pRIGHTPAREN4;
};

class cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN : public cExpr
{
	public:
		cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN(
			OW_String* pNewTRIM1,
			OW_String* pNewLEFTPAREN2,
			OW_String* pNewLEADING3,
			trimExpr* pNewtrimExpr4,
			OW_String* pNewRIGHTPAREN5
		)
			: cExpr()
			, m_pTRIM1(pNewTRIM1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_pLEADING3(pNewLEADING3)
			, m_ptrimExpr4(pNewtrimExpr4)
			, m_pRIGHTPAREN5(pNewRIGHTPAREN5)
		{}

		virtual ~cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN( this );
		}

		OW_String* m_pTRIM1;
		OW_String* m_pLEFTPAREN2;
		OW_String* m_pLEADING3;
		trimExpr* m_ptrimExpr4;
		OW_String* m_pRIGHTPAREN5;
};

class cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN : public cExpr
{
	public:
		cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN(
			OW_String* pNewTRIM1,
			OW_String* pNewLEFTPAREN2,
			OW_String* pNewTRAILING3,
			trimExpr* pNewtrimExpr4,
			OW_String* pNewRIGHTPAREN5
		)
			: cExpr()
			, m_pTRIM1(pNewTRIM1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_pTRAILING3(pNewTRAILING3)
			, m_ptrimExpr4(pNewtrimExpr4)
			, m_pRIGHTPAREN5(pNewRIGHTPAREN5)
		{}

		virtual ~cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN( this );
		}

		OW_String* m_pTRIM1;
		OW_String* m_pLEFTPAREN2;
		OW_String* m_pTRAILING3;
		trimExpr* m_ptrimExpr4;
		OW_String* m_pRIGHTPAREN5;
};

class cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN : public cExpr
{
	public:
		cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN(
			OW_String* pNewTRIM1,
			OW_String* pNewLEFTPAREN2,
			trimExpr* pNewtrimExpr3,
			OW_String* pNewRIGHTPAREN4
		)
			: cExpr()
			, m_pTRIM1(pNewTRIM1)
			, m_pLEFTPAREN2(pNewLEFTPAREN2)
			, m_ptrimExpr3(pNewtrimExpr3)
			, m_pRIGHTPAREN4(pNewRIGHTPAREN4)
		{}

		virtual ~cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN( this );
		}

		OW_String* m_pTRIM1;
		OW_String* m_pLEFTPAREN2;
		trimExpr* m_ptrimExpr3;
		OW_String* m_pRIGHTPAREN4;
};

class optIndirection: public node
{
	public:
		optIndirection()
			{}
		virtual ~optIndirection() {}

};

class optIndirection_empty : public optIndirection
{
	public:
		optIndirection_empty(
		)
			: optIndirection()
		{}

		virtual ~optIndirection_empty();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optIndirection_empty( this );
		}

};

class optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET : public optIndirection
{
	public:
		optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET(
			optIndirection* pNewoptIndirection1,
			OW_String* pNewLEFTBRACKET2,
			aExpr* pNewaExpr3,
			OW_String* pNewRIGHTBRACKET4
		)
			: optIndirection()
			, m_poptIndirection1(pNewoptIndirection1)
			, m_pLEFTBRACKET2(pNewLEFTBRACKET2)
			, m_paExpr3(pNewaExpr3)
			, m_pRIGHTBRACKET4(pNewRIGHTBRACKET4)
		{}

		virtual ~optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET( this );
		}

		optIndirection* m_poptIndirection1;
		OW_String* m_pLEFTBRACKET2;
		aExpr* m_paExpr3;
		OW_String* m_pRIGHTBRACKET4;
};

class optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET : public optIndirection
{
	public:
		optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET(
			optIndirection* pNewoptIndirection1,
			OW_String* pNewLEFTBRACKET2,
			aExpr* pNewaExpr3,
			OW_String* pNewCOLON4,
			aExpr* pNewaExpr5,
			OW_String* pNewRIGHTBRACKET6
		)
			: optIndirection()
			, m_poptIndirection1(pNewoptIndirection1)
			, m_pLEFTBRACKET2(pNewLEFTBRACKET2)
			, m_paExpr3(pNewaExpr3)
			, m_pCOLON4(pNewCOLON4)
			, m_paExpr5(pNewaExpr5)
			, m_pRIGHTBRACKET6(pNewRIGHTBRACKET6)
		{}

		virtual ~optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET( this );
		}

		optIndirection* m_poptIndirection1;
		OW_String* m_pLEFTBRACKET2;
		aExpr* m_paExpr3;
		OW_String* m_pCOLON4;
		aExpr* m_paExpr5;
		OW_String* m_pRIGHTBRACKET6;
};

class optExtract: public node
{
	public:
		optExtract()
			{}
		virtual ~optExtract() {}

};

class optExtract_empty : public optExtract
{
	public:
		optExtract_empty(
		)
			: optExtract()
		{}

		virtual ~optExtract_empty();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optExtract_empty( this );
		}

};

class optExtract_strExtractArg_FROM_aExpr : public optExtract
{
	public:
		optExtract_strExtractArg_FROM_aExpr(
			OW_String* pNewstrExtractArg1,
			OW_String* pNewFROM2,
			aExpr* pNewaExpr3
		)
			: optExtract()
			, m_pstrExtractArg1(pNewstrExtractArg1)
			, m_pFROM2(pNewFROM2)
			, m_paExpr3(pNewaExpr3)
		{}

		virtual ~optExtract_strExtractArg_FROM_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optExtract_strExtractArg_FROM_aExpr( this );
		}

		OW_String* m_pstrExtractArg1;
		OW_String* m_pFROM2;
		aExpr* m_paExpr3;
};

class positionExpr: public node
{
	public:
		positionExpr()
			{}
		virtual ~positionExpr() {}

};

class positionExpr_bExpr_IN_bExpr : public positionExpr
{
	public:
		positionExpr_bExpr_IN_bExpr(
			bExpr* pNewbExpr1,
			OW_String* pNewIN2,
			bExpr* pNewbExpr3
		)
			: positionExpr()
			, m_pbExpr1(pNewbExpr1)
			, m_pIN2(pNewIN2)
			, m_pbExpr3(pNewbExpr3)
		{}

		virtual ~positionExpr_bExpr_IN_bExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_positionExpr_bExpr_IN_bExpr( this );
		}

		bExpr* m_pbExpr1;
		OW_String* m_pIN2;
		bExpr* m_pbExpr3;
};

class positionExpr_empty : public positionExpr
{
	public:
		positionExpr_empty(
		)
			: positionExpr()
		{}

		virtual ~positionExpr_empty();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_positionExpr_empty( this );
		}

};

class optSubstrExpr: public node
{
	public:
		optSubstrExpr()
			{}
		virtual ~optSubstrExpr() {}

};

class optSubstrExpr_empty : public optSubstrExpr
{
	public:
		optSubstrExpr_empty(
		)
			: optSubstrExpr()
		{}

		virtual ~optSubstrExpr_empty();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optSubstrExpr_empty( this );
		}

};

class optSubstrExpr_aExpr_substrFrom_substrFor : public optSubstrExpr
{
	public:
		optSubstrExpr_aExpr_substrFrom_substrFor(
			aExpr* pNewaExpr1,
			substrFrom* pNewsubstrFrom2,
			substrFor* pNewsubstrFor3
		)
			: optSubstrExpr()
			, m_paExpr1(pNewaExpr1)
			, m_psubstrFrom2(pNewsubstrFrom2)
			, m_psubstrFor3(pNewsubstrFor3)
		{}

		virtual ~optSubstrExpr_aExpr_substrFrom_substrFor();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optSubstrExpr_aExpr_substrFrom_substrFor( this );
		}

		aExpr* m_paExpr1;
		substrFrom* m_psubstrFrom2;
		substrFor* m_psubstrFor3;
};

class optSubstrExpr_aExpr_substrFor_substrFrom : public optSubstrExpr
{
	public:
		optSubstrExpr_aExpr_substrFor_substrFrom(
			aExpr* pNewaExpr1,
			substrFor* pNewsubstrFor2,
			substrFrom* pNewsubstrFrom3
		)
			: optSubstrExpr()
			, m_paExpr1(pNewaExpr1)
			, m_psubstrFor2(pNewsubstrFor2)
			, m_psubstrFrom3(pNewsubstrFrom3)
		{}

		virtual ~optSubstrExpr_aExpr_substrFor_substrFrom();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optSubstrExpr_aExpr_substrFor_substrFrom( this );
		}

		aExpr* m_paExpr1;
		substrFor* m_psubstrFor2;
		substrFrom* m_psubstrFrom3;
};

class optSubstrExpr_aExpr_substrFrom : public optSubstrExpr
{
	public:
		optSubstrExpr_aExpr_substrFrom(
			aExpr* pNewaExpr1,
			substrFrom* pNewsubstrFrom2
		)
			: optSubstrExpr()
			, m_paExpr1(pNewaExpr1)
			, m_psubstrFrom2(pNewsubstrFrom2)
		{}

		virtual ~optSubstrExpr_aExpr_substrFrom();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optSubstrExpr_aExpr_substrFrom( this );
		}

		aExpr* m_paExpr1;
		substrFrom* m_psubstrFrom2;
};

class optSubstrExpr_aExpr_substrFor : public optSubstrExpr
{
	public:
		optSubstrExpr_aExpr_substrFor(
			aExpr* pNewaExpr1,
			substrFor* pNewsubstrFor2
		)
			: optSubstrExpr()
			, m_paExpr1(pNewaExpr1)
			, m_psubstrFor2(pNewsubstrFor2)
		{}

		virtual ~optSubstrExpr_aExpr_substrFor();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optSubstrExpr_aExpr_substrFor( this );
		}

		aExpr* m_paExpr1;
		substrFor* m_psubstrFor2;
};

class optSubstrExpr_exprSeq : public optSubstrExpr
{
	public:
		optSubstrExpr_exprSeq(
			exprSeq* pNewexprSeq1
		)
			: optSubstrExpr()
			, m_pexprSeq1(pNewexprSeq1)
		{}

		virtual ~optSubstrExpr_exprSeq();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_optSubstrExpr_exprSeq( this );
		}

		exprSeq* m_pexprSeq1;
};

class substrFrom : public node
{
	public:
		substrFrom(
			OW_String* pNewFROM1,
			aExpr* pNewaExpr2
		)
			: m_pFROM1(pNewFROM1)
			, m_paExpr2(pNewaExpr2)
		{}

		virtual ~substrFrom();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_substrFrom( this );
		}

		OW_String* m_pFROM1;
		aExpr* m_paExpr2;
};

class substrFor : public node
{
	public:
		substrFor(
			OW_String* pNewFOR1,
			aExpr* pNewaExpr2
		)
			: m_pFOR1(pNewFOR1)
			, m_paExpr2(pNewaExpr2)
		{}

		virtual ~substrFor();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_substrFor( this );
		}

		OW_String* m_pFOR1;
		aExpr* m_paExpr2;
};

class trimExpr: public node
{
	public:
		trimExpr()
			{}
		virtual ~trimExpr() {}

};

class trimExpr_aExpr_FROM_exprSeq : public trimExpr
{
	public:
		trimExpr_aExpr_FROM_exprSeq(
			aExpr* pNewaExpr1,
			OW_String* pNewFROM2,
			exprSeq* pNewexprSeq3
		)
			: trimExpr()
			, m_paExpr1(pNewaExpr1)
			, m_pFROM2(pNewFROM2)
			, m_pexprSeq3(pNewexprSeq3)
		{}

		virtual ~trimExpr_aExpr_FROM_exprSeq();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_trimExpr_aExpr_FROM_exprSeq( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pFROM2;
		exprSeq* m_pexprSeq3;
};

class trimExpr_FROM_exprSeq : public trimExpr
{
	public:
		trimExpr_FROM_exprSeq(
			OW_String* pNewFROM1,
			exprSeq* pNewexprSeq2
		)
			: trimExpr()
			, m_pFROM1(pNewFROM1)
			, m_pexprSeq2(pNewexprSeq2)
		{}

		virtual ~trimExpr_FROM_exprSeq();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_trimExpr_FROM_exprSeq( this );
		}

		OW_String* m_pFROM1;
		exprSeq* m_pexprSeq2;
};

class trimExpr_exprSeq : public trimExpr
{
	public:
		trimExpr_exprSeq(
			exprSeq* pNewexprSeq1
		)
			: trimExpr()
			, m_pexprSeq1(pNewexprSeq1)
		{}

		virtual ~trimExpr_exprSeq();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_trimExpr_exprSeq( this );
		}

		exprSeq* m_pexprSeq1;
};

class attr : public node
{
	public:
		attr(
			OW_String* pNewstrRelationName1,
			OW_String* pNewPERIOD2,
			attrs* pNewattrs3,
			optIndirection* pNewoptIndirection4
		)
			: m_pstrRelationName1(pNewstrRelationName1)
			, m_pPERIOD2(pNewPERIOD2)
			, m_pattrs3(pNewattrs3)
			, m_poptIndirection4(pNewoptIndirection4)
		{}

		virtual ~attr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_attr( this );
		}

		OW_String* m_pstrRelationName1;
		OW_String* m_pPERIOD2;
		attrs* m_pattrs3;
		optIndirection* m_poptIndirection4;
};

class attrs: public node
{
	public:
		attrs()
			{}
		virtual ~attrs() {}

};

class attrs_strAttrName : public attrs
{
	public:
		attrs_strAttrName(
			OW_String* pNewstrAttrName1
		)
			: attrs()
			, m_pstrAttrName1(pNewstrAttrName1)
		{}

		virtual ~attrs_strAttrName();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_attrs_strAttrName( this );
		}

		OW_String* m_pstrAttrName1;
};

class attrs_attrs_PERIOD_strAttrName : public attrs
{
	public:
		attrs_attrs_PERIOD_strAttrName(
			attrs* pNewattrs1,
			OW_String* pNewPERIOD2,
			OW_String* pNewstrAttrName3
		)
			: attrs()
			, m_pattrs1(pNewattrs1)
			, m_pPERIOD2(pNewPERIOD2)
			, m_pstrAttrName3(pNewstrAttrName3)
		{}

		virtual ~attrs_attrs_PERIOD_strAttrName();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_attrs_attrs_PERIOD_strAttrName( this );
		}

		attrs* m_pattrs1;
		OW_String* m_pPERIOD2;
		OW_String* m_pstrAttrName3;
};

class attrs_attrs_PERIOD_ASTERISK : public attrs
{
	public:
		attrs_attrs_PERIOD_ASTERISK(
			attrs* pNewattrs1,
			OW_String* pNewPERIOD2,
			OW_String* pNewASTERISK3
		)
			: attrs()
			, m_pattrs1(pNewattrs1)
			, m_pPERIOD2(pNewPERIOD2)
			, m_pASTERISK3(pNewASTERISK3)
		{}

		virtual ~attrs_attrs_PERIOD_ASTERISK();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_attrs_attrs_PERIOD_ASTERISK( this );
		}

		attrs* m_pattrs1;
		OW_String* m_pPERIOD2;
		OW_String* m_pASTERISK3;
};

class targetEl: public node
{
	public:
		targetEl()
			{}
		virtual ~targetEl() {}

};

class targetEl_aExpr_AS_strColLabel : public targetEl
{
	public:
		targetEl_aExpr_AS_strColLabel(
			aExpr* pNewaExpr1,
			OW_String* pNewAS2,
			OW_String* pNewstrColLabel3
		)
			: targetEl()
			, m_paExpr1(pNewaExpr1)
			, m_pAS2(pNewAS2)
			, m_pstrColLabel3(pNewstrColLabel3)
		{}

		virtual ~targetEl_aExpr_AS_strColLabel();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_targetEl_aExpr_AS_strColLabel( this );
		}

		aExpr* m_paExpr1;
		OW_String* m_pAS2;
		OW_String* m_pstrColLabel3;
};

class targetEl_aExpr : public targetEl
{
	public:
		targetEl_aExpr(
			aExpr* pNewaExpr1
		)
			: targetEl()
			, m_paExpr1(pNewaExpr1)
		{}

		virtual ~targetEl_aExpr();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_targetEl_aExpr( this );
		}

		aExpr* m_paExpr1;
};

class targetEl_strRelationName_PERIOD_ASTERISK : public targetEl
{
	public:
		targetEl_strRelationName_PERIOD_ASTERISK(
			OW_String* pNewstrRelationName1,
			OW_String* pNewPERIOD2,
			OW_String* pNewASTERISK3
		)
			: targetEl()
			, m_pstrRelationName1(pNewstrRelationName1)
			, m_pPERIOD2(pNewPERIOD2)
			, m_pASTERISK3(pNewASTERISK3)
		{}

		virtual ~targetEl_strRelationName_PERIOD_ASTERISK();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_targetEl_strRelationName_PERIOD_ASTERISK( this );
		}

		OW_String* m_pstrRelationName1;
		OW_String* m_pPERIOD2;
		OW_String* m_pASTERISK3;
};

class targetEl_ASTERISK : public targetEl
{
	public:
		targetEl_ASTERISK(
			OW_String* pNewASTERISK1
		)
			: targetEl()
			, m_pASTERISK1(pNewASTERISK1)
		{}

		virtual ~targetEl_ASTERISK();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_targetEl_ASTERISK( this );
		}

		OW_String* m_pASTERISK1;
};

class updateTargetEl : public node
{
	public:
		updateTargetEl(
			OW_String* pNewstrColId1,
			optIndirection* pNewoptIndirection2,
			OW_String* pNewEQUALS3,
			aExpr* pNewaExpr4
		)
			: m_pstrColId1(pNewstrColId1)
			, m_poptIndirection2(pNewoptIndirection2)
			, m_pEQUALS3(pNewEQUALS3)
			, m_paExpr4(pNewaExpr4)
		{}

		virtual ~updateTargetEl();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_updateTargetEl( this );
		}

		OW_String* m_pstrColId1;
		optIndirection* m_poptIndirection2;
		OW_String* m_pEQUALS3;
		aExpr* m_paExpr4;
};

class aExprConst: public node
{
	public:
		aExprConst()
			{}
		virtual ~aExprConst() {}

};

class aExprConst_ICONST : public aExprConst
{
	public:
		aExprConst_ICONST(
			OW_String* pNewICONST1
		)
			: aExprConst()
			, m_pICONST1(pNewICONST1)
		{}

		virtual ~aExprConst_ICONST();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExprConst_ICONST( this );
		}

		OW_String* m_pICONST1;
};

class aExprConst_FCONST : public aExprConst
{
	public:
		aExprConst_FCONST(
			OW_String* pNewFCONST1
		)
			: aExprConst()
			, m_pFCONST1(pNewFCONST1)
		{}

		virtual ~aExprConst_FCONST();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExprConst_FCONST( this );
		}

		OW_String* m_pFCONST1;
};

class aExprConst_SCONST : public aExprConst
{
	public:
		aExprConst_SCONST(
			OW_String* pNewSCONST1
		)
			: aExprConst()
			, m_pSCONST1(pNewSCONST1)
		{}

		virtual ~aExprConst_SCONST();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExprConst_SCONST( this );
		}

		OW_String* m_pSCONST1;
};

class aExprConst_BITCONST : public aExprConst
{
	public:
		aExprConst_BITCONST(
			OW_String* pNewBITCONST1
		)
			: aExprConst()
			, m_pBITCONST1(pNewBITCONST1)
		{}

		virtual ~aExprConst_BITCONST();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExprConst_BITCONST( this );
		}

		OW_String* m_pBITCONST1;
};

class aExprConst_HEXCONST : public aExprConst
{
	public:
		aExprConst_HEXCONST(
			OW_String* pNewHEXCONST1
		)
			: aExprConst()
			, m_pHEXCONST1(pNewHEXCONST1)
		{}

		virtual ~aExprConst_HEXCONST();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExprConst_HEXCONST( this );
		}

		OW_String* m_pHEXCONST1;
};

class aExprConst_TRUEP : public aExprConst
{
	public:
		aExprConst_TRUEP(
			OW_String* pNewTRUEP1
		)
			: aExprConst()
			, m_pTRUEP1(pNewTRUEP1)
		{}

		virtual ~aExprConst_TRUEP();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExprConst_TRUEP( this );
		}

		OW_String* m_pTRUEP1;
};

class aExprConst_FALSEP : public aExprConst
{
	public:
		aExprConst_FALSEP(
			OW_String* pNewFALSEP1
		)
			: aExprConst()
			, m_pFALSEP1(pNewFALSEP1)
		{}

		virtual ~aExprConst_FALSEP();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExprConst_FALSEP( this );
		}

		OW_String* m_pFALSEP1;
};

class aExprConst_NULLP : public aExprConst
{
	public:
		aExprConst_NULLP(
			OW_String* pNewNULLP1
		)
			: aExprConst()
			, m_pNULLP1(pNewNULLP1)
		{}

		virtual ~aExprConst_NULLP();

		void accept( OW_WQLVisitor* v ) const
		{
			v->visit_aExprConst_NULLP( this );
		}

		OW_String* m_pNULLP1;
};

inline stmt_selectStmt_optSemicolon::~stmt_selectStmt_optSemicolon()
{
	delete m_pselectStmt1;
	delete m_poptSemicolon2;
}

inline stmt_updateStmt_optSemicolon::~stmt_updateStmt_optSemicolon()
{
	delete m_pupdateStmt1;
	delete m_poptSemicolon2;
}

inline stmt_insertStmt_optSemicolon::~stmt_insertStmt_optSemicolon()
{
	delete m_pinsertStmt1;
	delete m_poptSemicolon2;
}

inline stmt_deleteStmt_optSemicolon::~stmt_deleteStmt_optSemicolon()
{
	delete m_pdeleteStmt1;
	delete m_poptSemicolon2;
}

inline optSemicolon_empty::~optSemicolon_empty()
{
}

inline optSemicolon_SEMICOLON::~optSemicolon_SEMICOLON()
{
	delete m_pSEMICOLON1;
}

inline insertStmt::~insertStmt()
{
	delete m_pINSERT1;
	delete m_pINTO2;
	delete m_pstrRelationName3;
	delete m_pinsertRest4;
}

inline insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN::~insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN()
{
	delete m_pVALUES1;
	delete m_pLEFTPAREN2;
	while(!m_ptargetList3->empty())
	{
		delete m_ptargetList3->front();
		m_ptargetList3->pop_front();
	}
	delete m_ptargetList3;
	delete m_pRIGHTPAREN4;
}

inline insertRest_DEFAULT_VALUES::~insertRest_DEFAULT_VALUES()
{
	delete m_pDEFAULT1;
	delete m_pVALUES2;
}

inline insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN::~insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN()
{
	delete m_pLEFTPAREN1;
	while(!m_pcolumnList2->empty())
	{
		delete m_pcolumnList2->front();
		m_pcolumnList2->pop_front();
	}
	delete m_pcolumnList2;
	delete m_pRIGHTPAREN3;
	delete m_pVALUES4;
	delete m_pLEFTPAREN5;
	while(!m_ptargetList6->empty())
	{
		delete m_ptargetList6->front();
		m_ptargetList6->pop_front();
	}
	delete m_ptargetList6;
	delete m_pRIGHTPAREN7;
}

inline deleteStmt::~deleteStmt()
{
	delete m_pDELETE1;
	delete m_pFROM2;
	delete m_pstrRelationName3;
	delete m_poptWhereClause4;
}

inline updateStmt::~updateStmt()
{
	delete m_pUPDATE1;
	delete m_pstrRelationName2;
	delete m_pSET3;
	while(!m_pupdateTargetList4->empty())
	{
		delete m_pupdateTargetList4->front();
		m_pupdateTargetList4->pop_front();
	}
	delete m_pupdateTargetList4;
	delete m_poptWhereClause5;
}

inline selectStmt::~selectStmt()
{
	delete m_pSELECT1;
	delete m_poptDistinct2;
	while(!m_ptargetList3->empty())
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

inline exprSeq_aExpr::~exprSeq_aExpr()
{
	delete m_paExpr1;
}

inline exprSeq_exprSeq_COMMA_aExpr::~exprSeq_exprSeq_COMMA_aExpr()
{
	delete m_pexprSeq1;
	delete m_pCOMMA2;
	delete m_paExpr3;
}

inline exprSeq_exprSeq_USING_aExpr::~exprSeq_exprSeq_USING_aExpr()
{
	delete m_pexprSeq1;
	delete m_pUSING2;
	delete m_paExpr3;
}

inline optDistinct_empty::~optDistinct_empty()
{
}

inline optDistinct_DISTINCT::~optDistinct_DISTINCT()
{
	delete m_pDISTINCT1;
}

inline optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN::~optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN()
{
	delete m_pDISTINCT1;
	delete m_pON2;
	delete m_pLEFTPAREN3;
	delete m_pexprSeq4;
	delete m_pRIGHTPAREN5;
}

inline optDistinct_ALL::~optDistinct_ALL()
{
	delete m_pALL1;
}

inline sortClause::~sortClause()
{
	delete m_pORDER1;
	delete m_pBY2;
	while(!m_psortbyList3->empty())
	{
		delete m_psortbyList3->front();
		m_psortbyList3->pop_front();
	}
	delete m_psortbyList3;
}

inline optSortClause_empty::~optSortClause_empty()
{
}

inline optSortClause_sortClause::~optSortClause_sortClause()
{
	delete m_psortClause1;
}

inline sortby::~sortby()
{
	delete m_paExpr1;
	delete m_pstrOptOrderSpecification2;
}

inline optGroupClause_empty::~optGroupClause_empty()
{
}

inline optGroupClause_GROUP_BY_exprSeq::~optGroupClause_GROUP_BY_exprSeq()
{
	delete m_pGROUP1;
	delete m_pBY2;
	delete m_pexprSeq3;
}

inline optHavingClause_empty::~optHavingClause_empty()
{
}

inline optHavingClause_HAVING_aExpr::~optHavingClause_HAVING_aExpr()
{
	delete m_pHAVING1;
	delete m_paExpr2;
}

inline optFromClause_empty::~optFromClause_empty()
{
}

inline optFromClause_FROM_fromList::~optFromClause_FROM_fromList()
{
	delete m_pFROM1;
	while(!m_pfromList2->empty())
	{
		delete m_pfromList2->front();
		m_pfromList2->pop_front();
	}
	delete m_pfromList2;
}

inline tableRef_relationExpr::~tableRef_relationExpr()
{
	delete m_prelationExpr1;
}

inline tableRef_relationExpr_aliasClause::~tableRef_relationExpr_aliasClause()
{
	delete m_prelationExpr1;
	delete m_paliasClause2;
}

inline tableRef_joinedTable::~tableRef_joinedTable()
{
	delete m_pjoinedTable1;
}

inline tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause::~tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause()
{
	delete m_pLEFTPAREN1;
	delete m_pjoinedTable2;
	delete m_pRIGHTPAREN3;
	delete m_paliasClause4;
}

inline joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN::~joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN()
{
	delete m_pLEFTPAREN1;
	delete m_pjoinedTable2;
	delete m_pRIGHTPAREN3;
}

inline joinedTable_tableRef_CROSS_JOIN_tableRef::~joinedTable_tableRef_CROSS_JOIN_tableRef()
{
	delete m_ptableRef1;
	delete m_pCROSS2;
	delete m_pJOIN3;
	delete m_ptableRef4;
}

inline joinedTable_tableRef_UNIONJOIN_tableRef::~joinedTable_tableRef_UNIONJOIN_tableRef()
{
	delete m_ptableRef1;
	delete m_pUNIONJOIN2;
	delete m_ptableRef3;
}

inline joinedTable_tableRef_joinType_JOIN_tableRef_joinQual::~joinedTable_tableRef_joinType_JOIN_tableRef_joinQual()
{
	delete m_ptableRef1;
	delete m_pjoinType2;
	delete m_pJOIN3;
	delete m_ptableRef4;
	delete m_pjoinQual5;
}

inline joinedTable_tableRef_JOIN_tableRef_joinQual::~joinedTable_tableRef_JOIN_tableRef_joinQual()
{
	delete m_ptableRef1;
	delete m_pJOIN2;
	delete m_ptableRef3;
	delete m_pjoinQual4;
}

inline joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef::~joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef()
{
	delete m_ptableRef1;
	delete m_pNATURAL2;
	delete m_pjoinType3;
	delete m_pJOIN4;
	delete m_ptableRef5;
}

inline joinedTable_tableRef_NATURAL_JOIN_tableRef::~joinedTable_tableRef_NATURAL_JOIN_tableRef()
{
	delete m_ptableRef1;
	delete m_pNATURAL2;
	delete m_pJOIN3;
	delete m_ptableRef4;
}

inline aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN::~aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN()
{
	delete m_pAS1;
	delete m_pstrColId2;
	delete m_pLEFTPAREN3;
	while(!m_pnameList4->empty())
	{
		delete m_pnameList4->front();
		m_pnameList4->pop_front();
	}
	delete m_pnameList4;
	delete m_pRIGHTPAREN5;
}

inline aliasClause_AS_strColId::~aliasClause_AS_strColId()
{
	delete m_pAS1;
	delete m_pstrColId2;
}

inline aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN::~aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN()
{
	delete m_pstrColId1;
	delete m_pLEFTPAREN2;
	while(!m_pnameList3->empty())
	{
		delete m_pnameList3->front();
		m_pnameList3->pop_front();
	}
	delete m_pnameList3;
	delete m_pRIGHTPAREN4;
}

inline aliasClause_strColId::~aliasClause_strColId()
{
	delete m_pstrColId1;
}

inline joinType_FULL_strOptJoinOuter::~joinType_FULL_strOptJoinOuter()
{
	delete m_pFULL1;
	delete m_pstrOptJoinOuter2;
}

inline joinType_LEFT_strOptJoinOuter::~joinType_LEFT_strOptJoinOuter()
{
	delete m_pLEFT1;
	delete m_pstrOptJoinOuter2;
}

inline joinType_RIGHT_strOptJoinOuter::~joinType_RIGHT_strOptJoinOuter()
{
	delete m_pRIGHT1;
	delete m_pstrOptJoinOuter2;
}

inline joinType_INNERP::~joinType_INNERP()
{
	delete m_pINNERP1;
}

inline joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN::~joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN()
{
	delete m_pUSING1;
	delete m_pLEFTPAREN2;
	while(!m_pnameList3->empty())
	{
		delete m_pnameList3->front();
		m_pnameList3->pop_front();
	}
	delete m_pnameList3;
	delete m_pRIGHTPAREN4;
}

inline joinQual_ON_aExpr::~joinQual_ON_aExpr()
{
	delete m_pON1;
	delete m_paExpr2;
}

inline relationExpr_strRelationName::~relationExpr_strRelationName()
{
	delete m_pstrRelationName1;
}

inline relationExpr_strRelationName_ASTERISK::~relationExpr_strRelationName_ASTERISK()
{
	delete m_pstrRelationName1;
	delete m_pASTERISK2;
}

inline relationExpr_ONLY_strRelationName::~relationExpr_ONLY_strRelationName()
{
	delete m_pONLY1;
	delete m_pstrRelationName2;
}

inline optWhereClause_empty::~optWhereClause_empty()
{
}

inline optWhereClause_WHERE_aExpr::~optWhereClause_WHERE_aExpr()
{
	delete m_pWHERE1;
	delete m_paExpr2;
}

inline rowExpr::~rowExpr()
{
	delete m_pLEFTPAREN1;
	delete m_prowDescriptor2;
	delete m_pRIGHTPAREN3;
	delete m_pstrAllOp4;
	delete m_pLEFTPAREN5;
	delete m_prowDescriptor6;
	delete m_pRIGHTPAREN7;
}

inline rowDescriptor::~rowDescriptor()
{
	while(!m_prowList1->empty())
	{
		delete m_prowList1->front();
		m_prowList1->pop_front();
	}
	delete m_prowList1;
	delete m_pCOMMA2;
	delete m_paExpr3;
}

inline aExpr_cExpr::~aExpr_cExpr()
{
	delete m_pcExpr1;
}

inline aExpr_aExpr_AT_TIME_ZONE_cExpr::~aExpr_aExpr_AT_TIME_ZONE_cExpr()
{
	delete m_paExpr1;
	delete m_pAT2;
	delete m_pTIME3;
	delete m_pZONE4;
	delete m_pcExpr5;
}

inline aExpr_PLUS_aExpr::~aExpr_PLUS_aExpr()
{
	delete m_pPLUS1;
	delete m_paExpr2;
}

inline aExpr_MINUS_aExpr::~aExpr_MINUS_aExpr()
{
	delete m_pMINUS1;
	delete m_paExpr2;
}

inline aExpr_BITINVERT_aExpr::~aExpr_BITINVERT_aExpr()
{
	delete m_pBITINVERT1;
	delete m_paExpr2;
}

inline aExpr_aExpr_PLUS_aExpr::~aExpr_aExpr_PLUS_aExpr()
{
	delete m_paExpr1;
	delete m_pPLUS2;
	delete m_paExpr3;
}

inline aExpr_aExpr_MINUS_aExpr::~aExpr_aExpr_MINUS_aExpr()
{
	delete m_paExpr1;
	delete m_pMINUS2;
	delete m_paExpr3;
}

inline aExpr_aExpr_ASTERISK_aExpr::~aExpr_aExpr_ASTERISK_aExpr()
{
	delete m_paExpr1;
	delete m_pASTERISK2;
	delete m_paExpr3;
}

inline aExpr_aExpr_SOLIDUS_aExpr::~aExpr_aExpr_SOLIDUS_aExpr()
{
	delete m_paExpr1;
	delete m_pSOLIDUS2;
	delete m_paExpr3;
}

inline aExpr_aExpr_PERCENT_aExpr::~aExpr_aExpr_PERCENT_aExpr()
{
	delete m_paExpr1;
	delete m_pPERCENT2;
	delete m_paExpr3;
}

inline aExpr_aExpr_BITAND_aExpr::~aExpr_aExpr_BITAND_aExpr()
{
	delete m_paExpr1;
	delete m_pBITAND2;
	delete m_paExpr3;
}

inline aExpr_aExpr_BITOR_aExpr::~aExpr_aExpr_BITOR_aExpr()
{
	delete m_paExpr1;
	delete m_pBITOR2;
	delete m_paExpr3;
}

inline aExpr_aExpr_BITSHIFTLEFT_aExpr::~aExpr_aExpr_BITSHIFTLEFT_aExpr()
{
	delete m_paExpr1;
	delete m_pBITSHIFTLEFT2;
	delete m_paExpr3;
}

inline aExpr_aExpr_BITSHIFTRIGHT_aExpr::~aExpr_aExpr_BITSHIFTRIGHT_aExpr()
{
	delete m_paExpr1;
	delete m_pBITSHIFTRIGHT2;
	delete m_paExpr3;
}

inline aExpr_aExpr_LESSTHAN_aExpr::~aExpr_aExpr_LESSTHAN_aExpr()
{
	delete m_paExpr1;
	delete m_pLESSTHAN2;
	delete m_paExpr3;
}

inline aExpr_aExpr_LESSTHANOREQUALS_aExpr::~aExpr_aExpr_LESSTHANOREQUALS_aExpr()
{
	delete m_paExpr1;
	delete m_pLESSTHANOREQUALS2;
	delete m_paExpr3;
}

inline aExpr_aExpr_GREATERTHAN_aExpr::~aExpr_aExpr_GREATERTHAN_aExpr()
{
	delete m_paExpr1;
	delete m_pGREATERTHAN2;
	delete m_paExpr3;
}

inline aExpr_aExpr_GREATERTHANOREQUALS_aExpr::~aExpr_aExpr_GREATERTHANOREQUALS_aExpr()
{
	delete m_paExpr1;
	delete m_pGREATERTHANOREQUALS2;
	delete m_paExpr3;
}

inline aExpr_aExpr_EQUALS_aExpr::~aExpr_aExpr_EQUALS_aExpr()
{
	delete m_paExpr1;
	delete m_pEQUALS2;
	delete m_paExpr3;
}

inline aExpr_aExpr_NOTEQUALS_aExpr::~aExpr_aExpr_NOTEQUALS_aExpr()
{
	delete m_paExpr1;
	delete m_pNOTEQUALS2;
	delete m_paExpr3;
}

inline aExpr_aExpr_AND_aExpr::~aExpr_aExpr_AND_aExpr()
{
	delete m_paExpr1;
	delete m_pAND2;
	delete m_paExpr3;
}

inline aExpr_aExpr_OR_aExpr::~aExpr_aExpr_OR_aExpr()
{
	delete m_paExpr1;
	delete m_pOR2;
	delete m_paExpr3;
}

inline aExpr_NOT_aExpr::~aExpr_NOT_aExpr()
{
	delete m_pNOT1;
	delete m_paExpr2;
}

inline aExpr_aExpr_CONCATENATION_aExpr::~aExpr_aExpr_CONCATENATION_aExpr()
{
	delete m_paExpr1;
	delete m_pCONCATENATION2;
	delete m_paExpr3;
}

inline aExpr_aExpr_LIKE_aExpr::~aExpr_aExpr_LIKE_aExpr()
{
	delete m_paExpr1;
	delete m_pLIKE2;
	delete m_paExpr3;
}

inline aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr::~aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr()
{
	delete m_paExpr1;
	delete m_pLIKE2;
	delete m_paExpr3;
	delete m_pESCAPE4;
	delete m_paExpr5;
}

inline aExpr_aExpr_NOT_LIKE_aExpr::~aExpr_aExpr_NOT_LIKE_aExpr()
{
	delete m_paExpr1;
	delete m_pNOT2;
	delete m_pLIKE3;
	delete m_paExpr4;
}

inline aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr::~aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr()
{
	delete m_paExpr1;
	delete m_pNOT2;
	delete m_pLIKE3;
	delete m_paExpr4;
	delete m_pESCAPE5;
	delete m_paExpr6;
}

inline aExpr_aExpr_ISNULL::~aExpr_aExpr_ISNULL()
{
	delete m_paExpr1;
	delete m_pISNULL2;
}

inline aExpr_aExpr_IS_NULLP::~aExpr_aExpr_IS_NULLP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pNULLP3;
}

inline aExpr_aExpr_NOTNULL::~aExpr_aExpr_NOTNULL()
{
	delete m_paExpr1;
	delete m_pNOTNULL2;
}

inline aExpr_aExpr_IS_NOT_NULLP::~aExpr_aExpr_IS_NOT_NULLP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pNOT3;
	delete m_pNULLP4;
}

inline aExpr_aExpr_IS_TRUEP::~aExpr_aExpr_IS_TRUEP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pTRUEP3;
}

inline aExpr_aExpr_IS_NOT_FALSEP::~aExpr_aExpr_IS_NOT_FALSEP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pNOT3;
	delete m_pFALSEP4;
}

inline aExpr_aExpr_IS_FALSEP::~aExpr_aExpr_IS_FALSEP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pFALSEP3;
}

inline aExpr_aExpr_IS_NOT_TRUEP::~aExpr_aExpr_IS_NOT_TRUEP()
{
	delete m_paExpr1;
	delete m_pIS2;
	delete m_pNOT3;
	delete m_pTRUEP4;
}

inline aExpr_aExpr_ISA_aExpr::~aExpr_aExpr_ISA_aExpr()
{
	delete m_paExpr1;
	delete m_pISA2;
	delete m_paExpr3;
}

inline aExpr_rowExpr::~aExpr_rowExpr()
{
	delete m_prowExpr1;
}

inline bExpr_cExpr::~bExpr_cExpr()
{
	delete m_pcExpr1;
}

inline bExpr_PLUS_bExpr::~bExpr_PLUS_bExpr()
{
	delete m_pPLUS1;
	delete m_pbExpr2;
}

inline bExpr_MINUS_bExpr::~bExpr_MINUS_bExpr()
{
	delete m_pMINUS1;
	delete m_pbExpr2;
}

inline bExpr_BITINVERT_bExpr::~bExpr_BITINVERT_bExpr()
{
	delete m_pBITINVERT1;
	delete m_pbExpr2;
}

inline bExpr_bExpr_PLUS_bExpr::~bExpr_bExpr_PLUS_bExpr()
{
	delete m_pbExpr1;
	delete m_pPLUS2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_MINUS_bExpr::~bExpr_bExpr_MINUS_bExpr()
{
	delete m_pbExpr1;
	delete m_pMINUS2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_ASTERISK_bExpr::~bExpr_bExpr_ASTERISK_bExpr()
{
	delete m_pbExpr1;
	delete m_pASTERISK2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_SOLIDUS_bExpr::~bExpr_bExpr_SOLIDUS_bExpr()
{
	delete m_pbExpr1;
	delete m_pSOLIDUS2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_PERCENT_bExpr::~bExpr_bExpr_PERCENT_bExpr()
{
	delete m_pbExpr1;
	delete m_pPERCENT2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_BITAND_bExpr::~bExpr_bExpr_BITAND_bExpr()
{
	delete m_pbExpr1;
	delete m_pBITAND2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_BITOR_bExpr::~bExpr_bExpr_BITOR_bExpr()
{
	delete m_pbExpr1;
	delete m_pBITOR2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_BITSHIFTLEFT_bExpr::~bExpr_bExpr_BITSHIFTLEFT_bExpr()
{
	delete m_pbExpr1;
	delete m_pBITSHIFTLEFT2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_BITSHIFTRIGHT_bExpr::~bExpr_bExpr_BITSHIFTRIGHT_bExpr()
{
	delete m_pbExpr1;
	delete m_pBITSHIFTRIGHT2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_LESSTHAN_bExpr::~bExpr_bExpr_LESSTHAN_bExpr()
{
	delete m_pbExpr1;
	delete m_pLESSTHAN2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_LESSTHANOREQUALS_bExpr::~bExpr_bExpr_LESSTHANOREQUALS_bExpr()
{
	delete m_pbExpr1;
	delete m_pLESSTHANOREQUALS2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_GREATERTHAN_bExpr::~bExpr_bExpr_GREATERTHAN_bExpr()
{
	delete m_pbExpr1;
	delete m_pGREATERTHAN2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_GREATERTHANOREQUALS_bExpr::~bExpr_bExpr_GREATERTHANOREQUALS_bExpr()
{
	delete m_pbExpr1;
	delete m_pGREATERTHANOREQUALS2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_EQUALS_bExpr::~bExpr_bExpr_EQUALS_bExpr()
{
	delete m_pbExpr1;
	delete m_pEQUALS2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_NOTEQUALS_bExpr::~bExpr_bExpr_NOTEQUALS_bExpr()
{
	delete m_pbExpr1;
	delete m_pNOTEQUALS2;
	delete m_pbExpr3;
}

inline bExpr_bExpr_CONCATENATION_bExpr::~bExpr_bExpr_CONCATENATION_bExpr()
{
	delete m_pbExpr1;
	delete m_pCONCATENATION2;
	delete m_pbExpr3;
}

inline cExpr_attr::~cExpr_attr()
{
	delete m_pattr1;
}

inline cExpr_strColId_optIndirection::~cExpr_strColId_optIndirection()
{
	delete m_pstrColId1;
	delete m_poptIndirection2;
}

inline cExpr_aExprConst::~cExpr_aExprConst()
{
	delete m_paExprConst1;
}

inline cExpr_LEFTPAREN_aExpr_RIGHTPAREN::~cExpr_LEFTPAREN_aExpr_RIGHTPAREN()
{
	delete m_pLEFTPAREN1;
	delete m_paExpr2;
	delete m_pRIGHTPAREN3;
}

inline cExpr_strFuncName_LEFTPAREN_RIGHTPAREN::~cExpr_strFuncName_LEFTPAREN_RIGHTPAREN()
{
	delete m_pstrFuncName1;
	delete m_pLEFTPAREN2;
	delete m_pRIGHTPAREN3;
}

inline cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN::~cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN()
{
	delete m_pstrFuncName1;
	delete m_pLEFTPAREN2;
	delete m_pexprSeq3;
	delete m_pRIGHTPAREN4;
}

inline cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN::~cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN()
{
	delete m_pstrFuncName1;
	delete m_pLEFTPAREN2;
	delete m_pALL3;
	delete m_pexprSeq4;
	delete m_pRIGHTPAREN5;
}

inline cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN::~cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN()
{
	delete m_pstrFuncName1;
	delete m_pLEFTPAREN2;
	delete m_pDISTINCT3;
	delete m_pexprSeq4;
	delete m_pRIGHTPAREN5;
}

inline cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN::~cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN()
{
	delete m_pstrFuncName1;
	delete m_pLEFTPAREN2;
	delete m_pASTERISK3;
	delete m_pRIGHTPAREN4;
}

inline cExpr_CURRENTDATE::~cExpr_CURRENTDATE()
{
	delete m_pCURRENTDATE1;
}

inline cExpr_CURRENTTIME::~cExpr_CURRENTTIME()
{
	delete m_pCURRENTTIME1;
}

inline cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN::~cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN()
{
	delete m_pCURRENTTIME1;
	delete m_pLEFTPAREN2;
	delete m_pICONST3;
	delete m_pRIGHTPAREN4;
}

inline cExpr_CURRENTTIMESTAMP::~cExpr_CURRENTTIMESTAMP()
{
	delete m_pCURRENTTIMESTAMP1;
}

inline cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN::~cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN()
{
	delete m_pCURRENTTIMESTAMP1;
	delete m_pLEFTPAREN2;
	delete m_pICONST3;
	delete m_pRIGHTPAREN4;
}

inline cExpr_CURRENTUSER::~cExpr_CURRENTUSER()
{
	delete m_pCURRENTUSER1;
}

inline cExpr_SESSIONUSER::~cExpr_SESSIONUSER()
{
	delete m_pSESSIONUSER1;
}

inline cExpr_USER::~cExpr_USER()
{
	delete m_pUSER1;
}

inline cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN::~cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN()
{
	delete m_pEXTRACT1;
	delete m_pLEFTPAREN2;
	delete m_poptExtract3;
	delete m_pRIGHTPAREN4;
}

inline cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN::~cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN()
{
	delete m_pPOSITION1;
	delete m_pLEFTPAREN2;
	delete m_ppositionExpr3;
	delete m_pRIGHTPAREN4;
}

inline cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN::~cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN()
{
	delete m_pSUBSTRING1;
	delete m_pLEFTPAREN2;
	delete m_poptSubstrExpr3;
	delete m_pRIGHTPAREN4;
}

inline cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN::~cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN()
{
	delete m_pTRIM1;
	delete m_pLEFTPAREN2;
	delete m_pLEADING3;
	delete m_ptrimExpr4;
	delete m_pRIGHTPAREN5;
}

inline cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN::~cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN()
{
	delete m_pTRIM1;
	delete m_pLEFTPAREN2;
	delete m_pTRAILING3;
	delete m_ptrimExpr4;
	delete m_pRIGHTPAREN5;
}

inline cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN::~cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN()
{
	delete m_pTRIM1;
	delete m_pLEFTPAREN2;
	delete m_ptrimExpr3;
	delete m_pRIGHTPAREN4;
}

inline optIndirection_empty::~optIndirection_empty()
{
}

inline optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET::~optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET()
{
	delete m_poptIndirection1;
	delete m_pLEFTBRACKET2;
	delete m_paExpr3;
	delete m_pRIGHTBRACKET4;
}

inline optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET::~optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET()
{
	delete m_poptIndirection1;
	delete m_pLEFTBRACKET2;
	delete m_paExpr3;
	delete m_pCOLON4;
	delete m_paExpr5;
	delete m_pRIGHTBRACKET6;
}

inline optExtract_empty::~optExtract_empty()
{
}

inline optExtract_strExtractArg_FROM_aExpr::~optExtract_strExtractArg_FROM_aExpr()
{
	delete m_pstrExtractArg1;
	delete m_pFROM2;
	delete m_paExpr3;
}

inline positionExpr_bExpr_IN_bExpr::~positionExpr_bExpr_IN_bExpr()
{
	delete m_pbExpr1;
	delete m_pIN2;
	delete m_pbExpr3;
}

inline positionExpr_empty::~positionExpr_empty()
{
}

inline optSubstrExpr_empty::~optSubstrExpr_empty()
{
}

inline optSubstrExpr_aExpr_substrFrom_substrFor::~optSubstrExpr_aExpr_substrFrom_substrFor()
{
	delete m_paExpr1;
	delete m_psubstrFrom2;
	delete m_psubstrFor3;
}

inline optSubstrExpr_aExpr_substrFor_substrFrom::~optSubstrExpr_aExpr_substrFor_substrFrom()
{
	delete m_paExpr1;
	delete m_psubstrFor2;
	delete m_psubstrFrom3;
}

inline optSubstrExpr_aExpr_substrFrom::~optSubstrExpr_aExpr_substrFrom()
{
	delete m_paExpr1;
	delete m_psubstrFrom2;
}

inline optSubstrExpr_aExpr_substrFor::~optSubstrExpr_aExpr_substrFor()
{
	delete m_paExpr1;
	delete m_psubstrFor2;
}

inline optSubstrExpr_exprSeq::~optSubstrExpr_exprSeq()
{
	delete m_pexprSeq1;
}

inline substrFrom::~substrFrom()
{
	delete m_pFROM1;
	delete m_paExpr2;
}

inline substrFor::~substrFor()
{
	delete m_pFOR1;
	delete m_paExpr2;
}

inline trimExpr_aExpr_FROM_exprSeq::~trimExpr_aExpr_FROM_exprSeq()
{
	delete m_paExpr1;
	delete m_pFROM2;
	delete m_pexprSeq3;
}

inline trimExpr_FROM_exprSeq::~trimExpr_FROM_exprSeq()
{
	delete m_pFROM1;
	delete m_pexprSeq2;
}

inline trimExpr_exprSeq::~trimExpr_exprSeq()
{
	delete m_pexprSeq1;
}

inline attr::~attr()
{
	delete m_pstrRelationName1;
	delete m_pPERIOD2;
	delete m_pattrs3;
	delete m_poptIndirection4;
}

inline attrs_strAttrName::~attrs_strAttrName()
{
	delete m_pstrAttrName1;
}

inline attrs_attrs_PERIOD_strAttrName::~attrs_attrs_PERIOD_strAttrName()
{
	delete m_pattrs1;
	delete m_pPERIOD2;
	delete m_pstrAttrName3;
}

inline attrs_attrs_PERIOD_ASTERISK::~attrs_attrs_PERIOD_ASTERISK()
{
	delete m_pattrs1;
	delete m_pPERIOD2;
	delete m_pASTERISK3;
}

inline targetEl_aExpr_AS_strColLabel::~targetEl_aExpr_AS_strColLabel()
{
	delete m_paExpr1;
	delete m_pAS2;
	delete m_pstrColLabel3;
}

inline targetEl_aExpr::~targetEl_aExpr()
{
	delete m_paExpr1;
}

inline targetEl_strRelationName_PERIOD_ASTERISK::~targetEl_strRelationName_PERIOD_ASTERISK()
{
	delete m_pstrRelationName1;
	delete m_pPERIOD2;
	delete m_pASTERISK3;
}

inline targetEl_ASTERISK::~targetEl_ASTERISK()
{
	delete m_pASTERISK1;
}

inline updateTargetEl::~updateTargetEl()
{
	delete m_pstrColId1;
	delete m_poptIndirection2;
	delete m_pEQUALS3;
	delete m_paExpr4;
}

inline aExprConst_ICONST::~aExprConst_ICONST()
{
	delete m_pICONST1;
}

inline aExprConst_FCONST::~aExprConst_FCONST()
{
	delete m_pFCONST1;
}

inline aExprConst_SCONST::~aExprConst_SCONST()
{
	delete m_pSCONST1;
}

inline aExprConst_BITCONST::~aExprConst_BITCONST()
{
	delete m_pBITCONST1;
}

inline aExprConst_HEXCONST::~aExprConst_HEXCONST()
{
	delete m_pHEXCONST1;
}

inline aExprConst_TRUEP::~aExprConst_TRUEP()
{
	delete m_pTRUEP1;
}

inline aExprConst_FALSEP::~aExprConst_FALSEP()
{
	delete m_pFALSEP1;
}

inline aExprConst_NULLP::~aExprConst_NULLP()
{
	delete m_pNULLP1;
}

#endif
