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

#include "OW_config.h"
#include "OW_WQLProcessor.hpp"
#include "OW_WQLAst.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_WQLScanUtils.hpp"
#include <errno.h>
#include <iterator> // for back_inserter
#include <algorithm> // for set_union

namespace OpenWBEM
{

using namespace WBEMFlags;
const char * typeStrings[] =
{
	"CIMInstanceArray",
	"String",
	"IntType",
	"Real",
	"Bool",
	"ColumnName",
	"Null",
	"Invalid"
};
WQLProcessor::WQLProcessor(
	const CIMOMHandleIFCRef& hdl,
	const String& ns)
	: m_hdl(hdl)
	, m_ns(ns)
	, m_doingSelect(false)
{
}
void WQLProcessor::visit_stmt_selectStmt_optSemicolon(
	const stmt_selectStmt_optSemicolon* pstmt_selectStmt_optSemicolon
	)
{
	pstmt_selectStmt_optSemicolon->m_pselectStmt1->accept(this);
	if (pstmt_selectStmt_optSemicolon->m_poptSemicolon2)
	{
		pstmt_selectStmt_optSemicolon->m_poptSemicolon2->accept(this);
	}
}
void WQLProcessor::visit_stmt_updateStmt_optSemicolon(
	const stmt_updateStmt_optSemicolon* pstmt_updateStmt_optSemicolon
	)
{
	pstmt_updateStmt_optSemicolon->m_pupdateStmt1->accept(this);
	if (pstmt_updateStmt_optSemicolon->m_poptSemicolon2)
	{
		pstmt_updateStmt_optSemicolon->m_poptSemicolon2->accept(this);
	}
}
void WQLProcessor::visit_stmt_insertStmt_optSemicolon(
	const stmt_insertStmt_optSemicolon* pstmt_insertStmt_optSemicolon
	)
{
	pstmt_insertStmt_optSemicolon->m_pinsertStmt1->accept(this);
	if (pstmt_insertStmt_optSemicolon->m_poptSemicolon2)
	{
		pstmt_insertStmt_optSemicolon->m_poptSemicolon2->accept(this);
	}
}
void WQLProcessor::visit_stmt_deleteStmt_optSemicolon(
	const stmt_deleteStmt_optSemicolon* pstmt_deleteStmt_optSemicolon
	)
{
	pstmt_deleteStmt_optSemicolon->m_pdeleteStmt1->accept(this);
	if (pstmt_deleteStmt_optSemicolon->m_poptSemicolon2)
	{
		pstmt_deleteStmt_optSemicolon->m_poptSemicolon2->accept(this);
	}
}
void WQLProcessor::visit_optSemicolon_empty(
	const optSemicolon_empty* poptSemicolon_empty
	)
{
}
void WQLProcessor::visit_optSemicolon_SEMICOLON(
	const optSemicolon_SEMICOLON* poptSemicolon_SEMICOLON
	)
{
}
void WQLProcessor::visit_insertStmt(
	const insertStmt* pinsertStmt
	)
{
	m_tableRef = *pinsertStmt->m_pstrRelationName3;
	pinsertStmt->m_pinsertRest4->accept(this);
}
void WQLProcessor::visit_insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN(
	const insertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN* pinsertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN
	)
{
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	CIMClass cc = m_hdl->getClass(m_ns, m_tableRef, E_NOT_LOCAL_ONLY, 
		E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
	CIMInstance ci = cc.newInstance();
	CIMPropertyArray cpa = ci.getProperties();
	if (pinsertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN->m_ptargetList3->size() !=
		cpa.size())
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Number of values must be the same as the number of properties in the class");
	}
	CIMPropertyArray::const_iterator curProperty = cpa.begin();
	for (List<targetEl*>::const_iterator i = pinsertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN->m_ptargetList3->begin();
		i != pinsertRest_VALUES_LEFTPAREN_targetList_RIGHTPAREN->m_ptargetList3->end();
		++i, ++curProperty )
	{
		(*i)->accept(this);
		// Fill out the properties on the instance
		CIMProperty cp = *curProperty;
		
		CIMValue newVal(CIMNULL);
		switch (m_exprValue.type)
		{
			case DataType::BoolType:
			{
				newVal = CIMValueCast::castValueToDataType(CIMValue(m_exprValue.b), cp.getDataType());
			}
			break;
			case DataType::IntType:
			{
				newVal = CIMValueCast::castValueToDataType(CIMValue(m_exprValue.i), cp.getDataType());
			}
			break;
			case DataType::RealType:
			{
				newVal = CIMValueCast::castValueToDataType(CIMValue(m_exprValue.r), cp.getDataType());
			}
			break;
			case DataType::StringType:
			{
				newVal = CIMValueCast::castValueToDataType(CIMValue(m_exprValue.str), cp.getDataType());
			}
			break;
			default:
			{
				OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Invalid value");
			}
			break;
		}
		cp.setValue(newVal);
		ci.setProperty(cp);
	}
	
	// create the instance
	//LOGDEBUG(Format("About to create instance: %1\nObjectPath = %2", ci.toString(), cop.toString()));
	m_hdl->createInstance(m_ns, ci);
	instances.clear();
	instances.push_back(ci);
#else
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
}
void WQLProcessor::visit_insertRest_DEFAULT_VALUES(
	const insertRest_DEFAULT_VALUES* pinsertRest_DEFAULT_VALUES
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "You must specify key values to create an instance.");
}
void WQLProcessor::visit_insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN(
	const insertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN* pinsertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN
	)
{
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	CIMClass cc = m_hdl->getClass(m_ns, m_tableRef, E_NOT_LOCAL_ONLY, 
		E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
	if (pinsertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN->m_pcolumnList2->size() !=
		pinsertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN->m_ptargetList6->size())
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Number of columns and values must be the same");
	}
	CIMInstance ci = cc.newInstance();
	StringArray columns;
	for (List<String*>::const_iterator i = pinsertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN->m_pcolumnList2->begin();
		i != pinsertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN->m_pcolumnList2->end();
		++i )
	{
		columns.push_back(**i);
	}
	StringArray::const_iterator column = columns.begin();
	for (List<targetEl*>::const_iterator i = pinsertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN->m_ptargetList6->begin();
		i != pinsertRest_LEFTPAREN_columnList_RIGHTPAREN_VALUES_LEFTPAREN_targetList_RIGHTPAREN->m_ptargetList6->end();
		++i, ++column )
	{
		(*i)->accept(this);
		// Fill out the properties on the instance
		CIMProperty cp = ci.getProperty(*column);
		if (!cp)
		{
			OW_THROWCIMMSG(CIMException::INVALID_QUERY, Format("Property %1 does not exist on class", *column).c_str());
		}
		CIMValue newVal(CIMNULL);
		switch (m_exprValue.type)
		{
			case DataType::BoolType:
			{
				newVal = CIMValueCast::castValueToDataType(CIMValue(m_exprValue.b), cp.getDataType());
			}
			break;
			case DataType::IntType:
			{
				newVal = CIMValueCast::castValueToDataType(CIMValue(m_exprValue.i), cp.getDataType());
			}
			break;
			case DataType::RealType:
			{
				newVal = CIMValueCast::castValueToDataType(CIMValue(m_exprValue.r), cp.getDataType());
			}
			break;
			case DataType::StringType:
			{
				newVal = CIMValueCast::castValueToDataType(CIMValue(m_exprValue.str), cp.getDataType());
			}
			break;
			default:
			{
				OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Invalid value");
			}
			break;
		}
		cp.setValue(newVal);
		ci.setProperty(cp);
	}
	// create the instance
	//LOGDEBUG(Format("About to create instance: %1\nObjectPath = %2", ci.toString(), cop.toString()));
	m_hdl->createInstance(m_ns, ci);
	instances.clear();
	instances.push_back(ci);
#else
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
}
void WQLProcessor::visit_deleteStmt(
	const deleteStmt* pdeleteStmt
	)
{
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	populateInstances(*pdeleteStmt->m_pstrRelationName3);
	if (pdeleteStmt->m_poptWhereClause4)
	{
		pdeleteStmt->m_poptWhereClause4->accept(this);
	}

	// Delete all the instances
	for (CIMInstanceArray::const_iterator i = instances.begin();
		 i != instances.end();
		 ++i)
	{
		CIMObjectPath cop(m_ns, *i);
		//LOGDEBUG(Format("Deleting instance:\n%1", cop.toString()));
		m_hdl->deleteInstance(m_ns, cop);
	}
#else
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
}
void WQLProcessor::visit_updateStmt(
	const updateStmt* pupdateStmt
	)
{
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	populateInstances(*pupdateStmt->m_pstrRelationName2);
	// Filter out the instances
	if (pupdateStmt->m_poptWhereClause5)
	{
		pupdateStmt->m_poptWhereClause5->accept(this);
	}
	// Gather up the property names and values
	for (List<updateTargetEl*>::const_iterator i = pupdateStmt->m_pupdateTargetList4->begin();
		i != pupdateStmt->m_pupdateTargetList4->end();
		++i )
	{
		(*i)->accept(this);
	}
	
	// loop through the instances
	for (CIMInstanceArray::iterator curInstance = instances.begin();
		 curInstance != instances.end();
		 ++curInstance)
	{
		CIMInstance& ci = *curInstance;
		// loop through the properties
		StringArray::const_iterator curProperty = m_propertyArray.begin();
		for (Array<DataType>::const_iterator curValue = m_valueArray.begin();
			 curValue != m_valueArray.end() && curProperty != m_propertyArray.end();
			++curValue, ++curProperty )
		{
			// Fill out the properties on the instance
			CIMProperty cp = ci.getProperty(*curProperty);
			if (!cp)
			{
				OW_THROWCIMMSG(CIMException::INVALID_QUERY, Format("Property %1 does not exist on class", *curProperty).c_str());
			}
			CIMValue newVal(CIMNULL);
			switch (curValue->type)
			{
				case DataType::BoolType:
				{
					newVal = CIMValueCast::castValueToDataType(CIMValue(curValue->b), cp.getDataType());
				}
				break;
				case DataType::IntType:
				{
					newVal = CIMValueCast::castValueToDataType(CIMValue(curValue->i), cp.getDataType());
				}
				break;
				case DataType::RealType:
				{
					newVal = CIMValueCast::castValueToDataType(CIMValue(curValue->r), cp.getDataType());
				}
				break;
				case DataType::StringType:
				{
					newVal = CIMValueCast::castValueToDataType(CIMValue(curValue->str), cp.getDataType());
				}
				break;
				case DataType::NullType:
				{
					newVal.setNull();
				}
				break;
				default:
				{
					OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Invalid value");
				}
				break;
			}
			cp.setValue(newVal);
			ci.setProperty(cp);
		}
		// update the instance
		//LOGDEBUG(Format("About to update instance: %1\nObjectPath = %2", ci.toString(), cop.toString()));
		m_hdl->modifyInstance(m_ns, ci);
	}
#else
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	
}
/*********************************************************
 * SELECT statement
 *  SELECT [distinct] targetEl* [FromClause] [Where Clause]
 *   [Group Clause] [Having Clause] [Sort Clause]
 *********************************************************/
void WQLProcessor::visit_selectStmt(
	const selectStmt* pselectStmt
	)
{
	m_doingSelect = true;
	if (pselectStmt->m_poptDistinct2)
	{
		pselectStmt->m_poptDistinct2->accept(this);
	}
	// FROM clause will populate the instances array
	if (pselectStmt->m_poptFromClause4)
	{
		pselectStmt->m_poptFromClause4->accept(this);
	}
	// WHERE will apply filter out the instance array
	if (pselectStmt->m_poptWhereClause5)
	{
		pselectStmt->m_poptWhereClause5->accept(this);
	}
	if (pselectStmt->m_poptGroupClause6)
	{
		pselectStmt->m_poptGroupClause6->accept(this);
	}
	if (pselectStmt->m_poptHavingClause7)
	{
		pselectStmt->m_poptHavingClause7->accept(this);
	}
	if (pselectStmt->m_poptSortClause8)
	{
		pselectStmt->m_poptSortClause8->accept(this);
	}
	// Now filter the properties on the instances
	for (List<targetEl*>::const_iterator i = pselectStmt->m_ptargetList3->begin();
		i != pselectStmt->m_ptargetList3->end();
		++i )
	{
		(*i)->accept(this);
	}
	//LOGDEBUG("Filtering the properties");
	for (size_t i = 0; i < m_propertyArray.size(); ++i)
	{
		size_t j = m_propertyArray[i].indexOf('.');
		if (j != String::npos)
		{
			m_propertyArray[i] = m_propertyArray[i].substring(j+1);
		}
		//LOGDEBUG(m_propertyArray[i].toString());
	}
	if (m_propertyArray.size() > 1 || m_propertyArray[0] != "*")
	{
		for (size_t i = 0; i < instances.size(); ++i)
		{
			// TODO: Use the filterInstance function from IndicationServerImpl
			// TODO: Handle __Path here
			instances[i] = instances[i].filterProperties(m_propertyArray, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN);
		}
	}
}
void WQLProcessor::visit_exprSeq_aExpr(
	const exprSeq_aExpr* pexprSeq_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pexprSeq_aExpr->m_paExpr1->accept(this);
}
void WQLProcessor::visit_exprSeq_exprSeq_COMMA_aExpr(
	const exprSeq_exprSeq_COMMA_aExpr* pexprSeq_exprSeq_COMMA_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pexprSeq_exprSeq_COMMA_aExpr->m_pexprSeq1->accept(this);
	pexprSeq_exprSeq_COMMA_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_exprSeq_exprSeq_USING_aExpr(
	const exprSeq_exprSeq_USING_aExpr* pexprSeq_exprSeq_USING_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pexprSeq_exprSeq_USING_aExpr->m_pexprSeq1->accept(this);
	pexprSeq_exprSeq_USING_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_optDistinct_empty(
	const optDistinct_empty* poptDistinct_empty
	)
{
}
void WQLProcessor::visit_optDistinct_DISTINCT(
	const optDistinct_DISTINCT* poptDistinct_DISTINCT
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN(
	const optDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN* poptDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	poptDistinct_DISTINCT_ON_LEFTPAREN_exprSeq_RIGHTPAREN->m_pexprSeq4->accept(this);
}
void WQLProcessor::visit_optDistinct_ALL(
	const optDistinct_ALL* poptDistinct_ALL
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_sortClause(
	const sortClause* psortClause
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	for (List<sortby*>::const_iterator i = psortClause->m_psortbyList3->begin();
		i != psortClause->m_psortbyList3->end();
		++i )
	{
		(*i)->accept(this);
	}
}
void WQLProcessor::visit_optSortClause_empty(
	const optSortClause_empty* poptSortClause_empty
	)
{
}
void WQLProcessor::visit_optSortClause_sortClause(
	const optSortClause_sortClause* poptSortClause_sortClause
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	poptSortClause_sortClause->m_psortClause1->accept(this);
}
void WQLProcessor::visit_sortby(
	const sortby* psortby
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	psortby->m_paExpr1->accept(this);
}
void WQLProcessor::visit_optGroupClause_empty(
	const optGroupClause_empty* poptGroupClause_empty
	)
{
}
void WQLProcessor::visit_optGroupClause_GROUP_BY_exprSeq(
	const optGroupClause_GROUP_BY_exprSeq* poptGroupClause_GROUP_BY_exprSeq
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	poptGroupClause_GROUP_BY_exprSeq->m_pexprSeq3->accept(this);
}
void WQLProcessor::visit_optHavingClause_empty(
	const optHavingClause_empty* poptHavingClause_empty
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_optHavingClause_HAVING_aExpr(
	const optHavingClause_HAVING_aExpr* poptHavingClause_HAVING_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	poptHavingClause_HAVING_aExpr->m_paExpr2->accept(this);
}
void WQLProcessor::visit_optFromClause_empty(
	const optFromClause_empty* poptFromClause_empty
	)
{
}
void WQLProcessor::visit_optFromClause_FROM_fromList(
	const optFromClause_FROM_fromList* poptFromClause_FROM_fromList
	)
{
	for (List<tableRef*>::const_iterator i = poptFromClause_FROM_fromList->m_pfromList2->begin();
		i != poptFromClause_FROM_fromList->m_pfromList2->end();
		++i )
	{
		(*i)->accept(this);
		populateInstances();
	}
}
void WQLProcessor::visit_tableRef_relationExpr(
	const tableRef_relationExpr* ptableRef_relationExpr
	)
{
	ptableRef_relationExpr->m_prelationExpr1->accept(this);
}
void WQLProcessor::visit_tableRef_relationExpr_aliasClause(
	const tableRef_relationExpr_aliasClause* ptableRef_relationExpr_aliasClause
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	ptableRef_relationExpr_aliasClause->m_prelationExpr1->accept(this);
	ptableRef_relationExpr_aliasClause->m_paliasClause2->accept(this);
}
void WQLProcessor::visit_tableRef_joinedTable(
	const tableRef_joinedTable* ptableRef_joinedTable
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	ptableRef_joinedTable->m_pjoinedTable1->accept(this);
}
void WQLProcessor::visit_tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause(
	const tableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause* ptableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	ptableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause->m_pjoinedTable2->accept(this);
	ptableRef_LEFTPAREN_joinedTable_RIGHTPAREN_aliasClause->m_paliasClause4->accept(this);
}
void WQLProcessor::visit_joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN(
	const joinedTable_LEFTPAREN_joinedTable_RIGHTPAREN* pjoinedTable_LEFTPAREN_joinedTable_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pjoinedTable_LEFTPAREN_joinedTable_RIGHTPAREN->m_pjoinedTable2->accept(this);
}
void WQLProcessor::visit_joinedTable_tableRef_CROSS_JOIN_tableRef(
	const joinedTable_tableRef_CROSS_JOIN_tableRef* pjoinedTable_tableRef_CROSS_JOIN_tableRef
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pjoinedTable_tableRef_CROSS_JOIN_tableRef->m_ptableRef1->accept(this);
	pjoinedTable_tableRef_CROSS_JOIN_tableRef->m_ptableRef4->accept(this);
}
void WQLProcessor::visit_joinedTable_tableRef_UNIONJOIN_tableRef(
	const joinedTable_tableRef_UNIONJOIN_tableRef* pjoinedTable_tableRef_UNIONJOIN_tableRef
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pjoinedTable_tableRef_UNIONJOIN_tableRef->m_ptableRef1->accept(this);
	pjoinedTable_tableRef_UNIONJOIN_tableRef->m_ptableRef3->accept(this);
}
void WQLProcessor::visit_joinedTable_tableRef_joinType_JOIN_tableRef_joinQual(
	const joinedTable_tableRef_joinType_JOIN_tableRef_joinQual* pjoinedTable_tableRef_joinType_JOIN_tableRef_joinQual
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pjoinedTable_tableRef_joinType_JOIN_tableRef_joinQual->m_ptableRef1->accept(this);
	pjoinedTable_tableRef_joinType_JOIN_tableRef_joinQual->m_pjoinType2->accept(this);
	pjoinedTable_tableRef_joinType_JOIN_tableRef_joinQual->m_ptableRef4->accept(this);
	pjoinedTable_tableRef_joinType_JOIN_tableRef_joinQual->m_pjoinQual5->accept(this);
}
void WQLProcessor::visit_joinedTable_tableRef_JOIN_tableRef_joinQual(
	const joinedTable_tableRef_JOIN_tableRef_joinQual* pjoinedTable_tableRef_JOIN_tableRef_joinQual
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pjoinedTable_tableRef_JOIN_tableRef_joinQual->m_ptableRef1->accept(this);
	pjoinedTable_tableRef_JOIN_tableRef_joinQual->m_ptableRef3->accept(this);
	pjoinedTable_tableRef_JOIN_tableRef_joinQual->m_pjoinQual4->accept(this);
}
void WQLProcessor::visit_joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef(
	const joinedTable_tableRef_NATURAL_joinType_JOIN_tableRef* pjoinedTable_tableRef_NATURAL_joinType_JOIN_tableRef
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pjoinedTable_tableRef_NATURAL_joinType_JOIN_tableRef->m_ptableRef1->accept(this);
	pjoinedTable_tableRef_NATURAL_joinType_JOIN_tableRef->m_pjoinType3->accept(this);
	pjoinedTable_tableRef_NATURAL_joinType_JOIN_tableRef->m_ptableRef5->accept(this);
}
void WQLProcessor::visit_joinedTable_tableRef_NATURAL_JOIN_tableRef(
	const joinedTable_tableRef_NATURAL_JOIN_tableRef* pjoinedTable_tableRef_NATURAL_JOIN_tableRef
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pjoinedTable_tableRef_NATURAL_JOIN_tableRef->m_ptableRef1->accept(this);
	pjoinedTable_tableRef_NATURAL_JOIN_tableRef->m_ptableRef4->accept(this);
}
void WQLProcessor::visit_aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN(
	const aliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN* paliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	for (List<String*>::const_iterator i = paliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN->m_pnameList4->begin();
		i != paliasClause_AS_strColId_LEFTPAREN_nameList_RIGHTPAREN->m_pnameList4->end();
		++i )
	{
	}
}
void WQLProcessor::visit_aliasClause_AS_strColId(
	const aliasClause_AS_strColId* paliasClause_AS_strColId
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN(
	const aliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN* paliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	for (List<String*>::const_iterator i = paliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN->m_pnameList3->begin();
		i != paliasClause_strColId_LEFTPAREN_nameList_RIGHTPAREN->m_pnameList3->end();
		++i )
	{
	}
}
void WQLProcessor::visit_aliasClause_strColId(
	const aliasClause_strColId* paliasClause_strColId
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_joinType_FULL_strOptJoinOuter(
	const joinType_FULL_strOptJoinOuter* pjoinType_FULL_strOptJoinOuter
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_joinType_LEFT_strOptJoinOuter(
	const joinType_LEFT_strOptJoinOuter* pjoinType_LEFT_strOptJoinOuter
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_joinType_RIGHT_strOptJoinOuter(
	const joinType_RIGHT_strOptJoinOuter* pjoinType_RIGHT_strOptJoinOuter
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_joinType_INNERP(
	const joinType_INNERP* pjoinType_INNERP
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN(
	const joinQual_USING_LEFTPAREN_nameList_RIGHTPAREN* pjoinQual_USING_LEFTPAREN_nameList_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	for (List<String*>::const_iterator i = pjoinQual_USING_LEFTPAREN_nameList_RIGHTPAREN->m_pnameList3->begin();
		i != pjoinQual_USING_LEFTPAREN_nameList_RIGHTPAREN->m_pnameList3->end();
		++i )
	{
	}
}
void WQLProcessor::visit_joinQual_ON_aExpr(
	const joinQual_ON_aExpr* pjoinQual_ON_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pjoinQual_ON_aExpr->m_paExpr2->accept(this);
}
void WQLProcessor::visit_relationExpr_strRelationName(
	const relationExpr_strRelationName* prelationExpr_strRelationName
	)
{
	m_tableRef = *prelationExpr_strRelationName->m_pstrRelationName1;
	//LOGDEBUG(Format("Setting m_tableRef to: %1", m_tableRef.toString()));
}
void WQLProcessor::visit_relationExpr_strRelationName_ASTERISK(
	const relationExpr_strRelationName_ASTERISK* prelationExpr_strRelationName_ASTERISK
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_relationExpr_ONLY_strRelationName(
	const relationExpr_ONLY_strRelationName* prelationExpr_ONLY_strRelationName
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_optWhereClause_empty(
	const optWhereClause_empty* poptWhereClause_empty
	)
{
}
void WQLProcessor::visit_optWhereClause_WHERE_aExpr(
	const optWhereClause_WHERE_aExpr* poptWhereClause_WHERE_aExpr
	)
{
	poptWhereClause_WHERE_aExpr->m_paExpr2->accept(this);
	if (m_exprValue.type == DataType::CIMInstanceArrayType)
	{
		instances = m_exprValue.cia;
	}
	else
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "WHERE clause did not evalue to instances");
	}
}
void WQLProcessor::visit_rowExpr(
	const rowExpr* prowExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	prowExpr->m_prowDescriptor2->accept(this);
	prowExpr->m_prowDescriptor6->accept(this);
}
void WQLProcessor::visit_rowDescriptor(
	const rowDescriptor* prowDescriptor
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	for (List<aExpr*>::const_iterator i = prowDescriptor->m_prowList1->begin();
		i != prowDescriptor->m_prowList1->end();
		++i )
	{
		(*i)->accept(this);
	}
	prowDescriptor->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_cExpr(
	const aExpr_cExpr* paExpr_cExpr
	)
{
	paExpr_cExpr->m_pcExpr1->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_AT_TIME_ZONE_cExpr(
	const aExpr_aExpr_AT_TIME_ZONE_cExpr* paExpr_aExpr_AT_TIME_ZONE_cExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_AT_TIME_ZONE_cExpr->m_paExpr1->accept(this);
	paExpr_aExpr_AT_TIME_ZONE_cExpr->m_pcExpr5->accept(this);
}
void WQLProcessor::visit_aExpr_PLUS_aExpr(
	const aExpr_PLUS_aExpr* paExpr_PLUS_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_PLUS_aExpr->m_paExpr2->accept(this);
}
void WQLProcessor::visit_aExpr_MINUS_aExpr(
	const aExpr_MINUS_aExpr* paExpr_MINUS_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_MINUS_aExpr->m_paExpr2->accept(this);
}
void WQLProcessor::visit_aExpr_BITINVERT_aExpr(
	const aExpr_BITINVERT_aExpr* paExpr_BITINVERT_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_BITINVERT_aExpr->m_paExpr2->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_PLUS_aExpr(
	const aExpr_aExpr_PLUS_aExpr* paExpr_aExpr_PLUS_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_PLUS_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_PLUS_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_MINUS_aExpr(
	const aExpr_aExpr_MINUS_aExpr* paExpr_aExpr_MINUS_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_MINUS_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_MINUS_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_ASTERISK_aExpr(
	const aExpr_aExpr_ASTERISK_aExpr* paExpr_aExpr_ASTERISK_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_ASTERISK_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_ASTERISK_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_SOLIDUS_aExpr(
	const aExpr_aExpr_SOLIDUS_aExpr* paExpr_aExpr_SOLIDUS_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_SOLIDUS_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_SOLIDUS_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_PERCENT_aExpr(
	const aExpr_aExpr_PERCENT_aExpr* paExpr_aExpr_PERCENT_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_PERCENT_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_PERCENT_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_BITAND_aExpr(
	const aExpr_aExpr_BITAND_aExpr* paExpr_aExpr_BITAND_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_BITAND_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_BITAND_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_BITOR_aExpr(
	const aExpr_aExpr_BITOR_aExpr* paExpr_aExpr_BITOR_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_BITOR_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_BITOR_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_BITSHIFTLEFT_aExpr(
	const aExpr_aExpr_BITSHIFTLEFT_aExpr* paExpr_aExpr_BITSHIFTLEFT_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_BITSHIFTLEFT_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_BITSHIFTLEFT_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_BITSHIFTRIGHT_aExpr(
	const aExpr_aExpr_BITSHIFTRIGHT_aExpr* paExpr_aExpr_BITSHIFTRIGHT_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_BITSHIFTRIGHT_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_BITSHIFTRIGHT_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_LESSTHAN_aExpr(
	const aExpr_aExpr_LESSTHAN_aExpr* paExpr_aExpr_LESSTHAN_aExpr
	)
{
	paExpr_aExpr_LESSTHAN_aExpr->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	paExpr_aExpr_LESSTHAN_aExpr->m_paExpr3->accept(this);
	DataType rhs = m_exprValue;
	
	doComparison(lhs, rhs, Compare(Compare::LessThanType));
}
void WQLProcessor::visit_aExpr_aExpr_LESSTHANOREQUALS_aExpr(
	const aExpr_aExpr_LESSTHANOREQUALS_aExpr* paExpr_aExpr_LESSTHANOREQUALS_aExpr
	)
{
	paExpr_aExpr_LESSTHANOREQUALS_aExpr->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	
	paExpr_aExpr_LESSTHANOREQUALS_aExpr->m_paExpr3->accept(this);
	DataType rhs = m_exprValue;
	
	doComparison(lhs, rhs, Compare(Compare::LessThanOrEqualsType));
}
void WQLProcessor::visit_aExpr_aExpr_GREATERTHAN_aExpr(
	const aExpr_aExpr_GREATERTHAN_aExpr* paExpr_aExpr_GREATERTHAN_aExpr
	)
{
	paExpr_aExpr_GREATERTHAN_aExpr->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	
	paExpr_aExpr_GREATERTHAN_aExpr->m_paExpr3->accept(this);
	DataType rhs = m_exprValue;
	
	doComparison(lhs, rhs, Compare(Compare::GreaterThanType));
}
void WQLProcessor::visit_aExpr_aExpr_GREATERTHANOREQUALS_aExpr(
	const aExpr_aExpr_GREATERTHANOREQUALS_aExpr* paExpr_aExpr_GREATERTHANOREQUALS_aExpr
	)
{
	paExpr_aExpr_GREATERTHANOREQUALS_aExpr->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	
	paExpr_aExpr_GREATERTHANOREQUALS_aExpr->m_paExpr3->accept(this);
	DataType rhs = m_exprValue;
	doComparison(lhs, rhs, Compare(Compare::GreaterThanOrEqualsType));
}
void WQLProcessor::visit_aExpr_aExpr_EQUALS_aExpr(
	const aExpr_aExpr_EQUALS_aExpr* paExpr_aExpr_EQUALS_aExpr
	)
{
	paExpr_aExpr_EQUALS_aExpr->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	paExpr_aExpr_EQUALS_aExpr->m_paExpr3->accept(this);
	DataType rhs = m_exprValue;
	doComparison(lhs, rhs, Compare(Compare::EqualsType));
}
void WQLProcessor::doComparison(const DataType& lhs, const DataType& rhs, Compare compare)
{
	switch (lhs.type)
	{
		case DataType::ColumnNameType:
		{
			switch (rhs.type)
			{
				case DataType::StringType:
				{
					m_exprValue = DataType(filterInstancesOnPropertyValue(lhs.str, CIMValue(rhs.str), compare));
				}
				break;
				case DataType::IntType:
				{
					m_exprValue = DataType(filterInstancesOnPropertyValue(lhs.str, CIMValue(rhs.i), compare));
				}
				break;
				case DataType::BoolType:
				{
					m_exprValue = DataType(filterInstancesOnPropertyValue(lhs.str, CIMValue(rhs.b), compare));
				}
				break;
				case DataType::RealType:
				{
					m_exprValue = DataType(filterInstancesOnPropertyValue(lhs.str, CIMValue(rhs.r), compare));
				}
				break;
				case DataType::NullType:
				{
					m_exprValue = DataType(filterInstancesOnPropertyValue(lhs.str, CIMValue(CIMNULL), compare));
				}
				break;
				default:
				{
					OW_THROWCIMMSG(CIMException::INVALID_QUERY, Format("Unsupported comparison. lhs.type = %1, rhs.type = %2", typeStrings[lhs.type], typeStrings[rhs.type]).c_str());
				}
				break;
			}
		}
		break;
		case DataType::StringType:
		{
			switch (rhs.type)
			{
				case DataType::ColumnNameType:
				{
					m_exprValue = DataType(filterInstancesOnPropertyValue(rhs.str, CIMValue(lhs.str), compare.reverseOrder()));
				}
				break;
				
				default:
				{
					OW_THROWCIMMSG(CIMException::INVALID_QUERY, Format("Unsupported comparison. lhs.type = %1, rhs.type = %2", typeStrings[lhs.type], typeStrings[rhs.type]).c_str());
				}
				break;
			}
		}
		break;
		case DataType::IntType:
		{
			switch (rhs.type)
			{
				case DataType::ColumnNameType:
				{
					m_exprValue = DataType(filterInstancesOnPropertyValue(rhs.str, CIMValue(lhs.i), compare.reverseOrder()));
				}
				break;
				
				default:
				{
					OW_THROWCIMMSG(CIMException::INVALID_QUERY, Format("Unsupported comparison. lhs.type = %1, rhs.type = %2", typeStrings[lhs.type], typeStrings[rhs.type]).c_str());
				}
				break;
			}
		}
		break;
		case DataType::BoolType:
		{
			switch (rhs.type)
			{
				case DataType::ColumnNameType:
				{
					m_exprValue = DataType(filterInstancesOnPropertyValue(rhs.str, CIMValue(lhs.b), compare.reverseOrder()));
				}
				break;
				
				default:
				{
					OW_THROWCIMMSG(CIMException::INVALID_QUERY, Format("Unsupported comparison. lhs.type = %1, rhs.type = %2", typeStrings[lhs.type], typeStrings[rhs.type]).c_str());
				}
				break;
			}
		}
		break;
		
		case DataType::RealType:
		{
			switch (rhs.type)
			{
				case DataType::ColumnNameType:
				{
					m_exprValue = DataType(filterInstancesOnPropertyValue(rhs.str, CIMValue(lhs.r), compare.reverseOrder()));
				}
				break;
				
				default:
				{
					OW_THROWCIMMSG(CIMException::INVALID_QUERY, Format("Unsupported comparison. lhs.type = %1, rhs.type = %2", typeStrings[lhs.type], typeStrings[rhs.type]).c_str());
				}
				break;
			}
		}
		break;
		
		case DataType::NullType:
		{
			switch (rhs.type)
			{
				case DataType::ColumnNameType:
				{
					m_exprValue = DataType(filterInstancesOnPropertyValue(rhs.str, CIMValue(CIMNULL), compare.reverseOrder()));
				}
				break;
				
				default:
				{
					OW_THROWCIMMSG(CIMException::INVALID_QUERY, Format("Unsupported comparison. lhs.type = %1, rhs.type = %2", typeStrings[lhs.type], typeStrings[rhs.type]).c_str());
				}
				break;
			}
		}
		break;
		
		default:
		{
			OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Unsupported comparison");
		}
		break;
	}
}
void WQLProcessor::visit_aExpr_aExpr_NOTEQUALS_aExpr(
	const aExpr_aExpr_NOTEQUALS_aExpr* paExpr_aExpr_NOTEQUALS_aExpr
	)
{
	paExpr_aExpr_NOTEQUALS_aExpr->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	paExpr_aExpr_NOTEQUALS_aExpr->m_paExpr3->accept(this);
	DataType rhs = m_exprValue;
	doComparison(lhs, rhs, Compare(Compare::NotEqualsType));
}
void WQLProcessor::visit_aExpr_aExpr_AND_aExpr(
	const aExpr_aExpr_AND_aExpr* paExpr_aExpr_AND_aExpr
	)
{
	paExpr_aExpr_AND_aExpr->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	if (lhs.type != DataType::CIMInstanceArrayType)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "invalid OR argument");
	}
	
	paExpr_aExpr_AND_aExpr->m_paExpr3->accept(this);
	DataType rhs = m_exprValue;
	if (rhs.type != DataType::CIMInstanceArrayType)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "invalid OR argument");
	}
	
	// set_union requires sorted containers
	//LOGDEBUG("Beginning first sort");
	std::sort(lhs.cia.begin(), lhs.cia.end(), CIMInstanceSortCriterion);
	//LOGDEBUG("Beginning second sort");
	std::sort(rhs.cia.begin(), rhs.cia.end(), CIMInstanceSortCriterion);
	// take the union of the two and assign it to the return value.
	//LOGDEBUG("Calling set_intersection");
	//LOGDEBUG(Format("lhs.cia.size() = %1, rhs.cia.size() = %2", lhs.cia.size(), rhs.cia.size()));
	CIMInstanceArray rVal;
	std::set_intersection(lhs.cia.begin(), lhs.cia.end(), rhs.cia.begin(), rhs.cia.end(), std::back_inserter(rVal), CIMInstanceSortCriterion);
	//LOGDEBUG(Format("set_intersection finished.  rVal.size() = %1", rVal.size()));
	m_exprValue = DataType(rVal);
}
void WQLProcessor::visit_aExpr_aExpr_OR_aExpr(
	const aExpr_aExpr_OR_aExpr* paExpr_aExpr_OR_aExpr
	)
{
	paExpr_aExpr_OR_aExpr->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	if (lhs.type != DataType::CIMInstanceArrayType)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "invalid OR argument");
	}
	
	paExpr_aExpr_OR_aExpr->m_paExpr3->accept(this);
	DataType rhs = m_exprValue;
	if (rhs.type != DataType::CIMInstanceArrayType)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "invalid OR argument");
	}
	// set_union requires sorted containers
	std::sort(lhs.cia.begin(), lhs.cia.end(), CIMInstanceSortCriterion);
	std::sort(rhs.cia.begin(), rhs.cia.end(), CIMInstanceSortCriterion);
	// take the union of the two and assign it to the return value.
	//LOGDEBUG(Format("About to call set_union. lhs.cia.size() = %1, rhs.cia.size() = %2", lhs.cia.size(), rhs.cia.size()));
	CIMInstanceArray unionArray;
	std::set_union(lhs.cia.begin(), lhs.cia.end(), rhs.cia.begin(), rhs.cia.end(), std::back_inserter(unionArray), CIMInstanceSortCriterion);
	//LOGDEBUG(Format("set_union finished.  unionArray.size() = %1", unionArray.size()));
	m_exprValue = DataType(unionArray);
}
bool CIMInstanceSortCriterion( const CIMInstance& lhs, const CIMInstance& rhs)
{
	//LOGDEBUG(Format("in CIMInstanceSortCriterion.  Comparing:\n%1\n%2\n%3", lhs.toString(), rhs.toString(), lhs.toString() < rhs.toString()));
	return lhs.toString() < rhs.toString();
}
void WQLProcessor::visit_aExpr_NOT_aExpr(
	const aExpr_NOT_aExpr* paExpr_NOT_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_NOT_aExpr->m_paExpr2->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_CONCATENATION_aExpr(
	const aExpr_aExpr_CONCATENATION_aExpr* paExpr_aExpr_CONCATENATION_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_CONCATENATION_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_CONCATENATION_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_LIKE_aExpr(
	const aExpr_aExpr_LIKE_aExpr* paExpr_aExpr_LIKE_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_LIKE_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_LIKE_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr(
	const aExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr* paExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr->m_paExpr3->accept(this);
	paExpr_aExpr_LIKE_aExpr_ESCAPE_aExpr->m_paExpr5->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_NOT_LIKE_aExpr(
	const aExpr_aExpr_NOT_LIKE_aExpr* paExpr_aExpr_NOT_LIKE_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_NOT_LIKE_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_NOT_LIKE_aExpr->m_paExpr4->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr(
	const aExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr* paExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr->m_paExpr1->accept(this);
	paExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr->m_paExpr4->accept(this);
	paExpr_aExpr_NOT_LIKE_aExpr_ESCAPE_aExpr->m_paExpr6->accept(this);
}
void WQLProcessor::visit_aExpr_aExpr_ISNULL(
	const aExpr_aExpr_ISNULL* paExpr_aExpr_ISNULL
	)
{
	paExpr_aExpr_ISNULL->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	DataType rhs = DataType(DataType::NullType);
	doComparison(lhs, rhs, Compare(Compare::EqualsType));
}
void WQLProcessor::visit_aExpr_aExpr_IS_NULLP(
	const aExpr_aExpr_IS_NULLP* paExpr_aExpr_IS_NULLP
	)
{
	paExpr_aExpr_IS_NULLP->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	DataType rhs = DataType(DataType::NullType);
	doComparison(lhs, rhs, Compare(Compare::EqualsType));
}
void WQLProcessor::visit_aExpr_aExpr_NOTNULL(
	const aExpr_aExpr_NOTNULL* paExpr_aExpr_NOTNULL
	)
{
	paExpr_aExpr_NOTNULL->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	DataType rhs = DataType(DataType::NullType);
	doComparison(lhs, rhs, Compare(Compare::NotEqualsType));
}
void WQLProcessor::visit_aExpr_aExpr_IS_NOT_NULLP(
	const aExpr_aExpr_IS_NOT_NULLP* paExpr_aExpr_IS_NOT_NULLP
	)
{
	paExpr_aExpr_IS_NOT_NULLP->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	DataType rhs = DataType(DataType::NullType);
	doComparison(lhs, rhs, Compare(Compare::NotEqualsType));
}
void WQLProcessor::visit_aExpr_aExpr_IS_TRUEP(
	const aExpr_aExpr_IS_TRUEP* paExpr_aExpr_IS_TRUEP
	)
{
	paExpr_aExpr_IS_TRUEP->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	DataType rhs = DataType(Bool(true));
	doComparison(lhs, rhs, Compare(Compare::EqualsType));
}
void WQLProcessor::visit_aExpr_aExpr_IS_NOT_FALSEP(
	const aExpr_aExpr_IS_NOT_FALSEP* paExpr_aExpr_IS_NOT_FALSEP
	)
{
	paExpr_aExpr_IS_NOT_FALSEP->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	DataType rhs = DataType(Bool(false));
	doComparison(lhs, rhs, Compare(Compare::NotEqualsType));
}
void WQLProcessor::visit_aExpr_aExpr_IS_FALSEP(
	const aExpr_aExpr_IS_FALSEP* paExpr_aExpr_IS_FALSEP
	)
{
	paExpr_aExpr_IS_FALSEP->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	DataType rhs = DataType(Bool(false));
	doComparison(lhs, rhs, Compare(Compare::EqualsType));
}
void WQLProcessor::visit_aExpr_aExpr_IS_NOT_TRUEP(
	const aExpr_aExpr_IS_NOT_TRUEP* paExpr_aExpr_IS_NOT_TRUEP
	)
{
	paExpr_aExpr_IS_NOT_TRUEP->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	DataType rhs = DataType(Bool(true));
	doComparison(lhs, rhs, Compare(Compare::NotEqualsType));
}
bool WQLProcessor::instanceIsDerivedFrom(const CIMInstance& inst,
	const String& className)
{
	return classIsDerivedFrom(inst.getClassName(), className);
}
bool WQLProcessor::classIsDerivedFrom(const String& cls,
	const String& className)
{
	String curClassName = cls;
	while (!curClassName.empty())
	{
		if (curClassName.equalsIgnoreCase(className))
		{
			return true;
		}
		// didn't match, so try the superclass of curClassName
		CIMClass cls2 = m_hdl->getClass(m_ns, curClassName);
		curClassName = cls2.getSuperClass();
	}
	return false;
}
void WQLProcessor::visit_aExpr_aExpr_ISA_aExpr(
		const aExpr_aExpr_ISA_aExpr* paExpr_aExpr_ISA_aExpr
		)
{
	paExpr_aExpr_ISA_aExpr->m_paExpr1->accept(this);
	DataType lhs = m_exprValue;
	if (lhs.type != DataType::ColumnNameType)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Invalid first parameter type for ISA (should be a property name)");
	}
	String propName = lhs.str;
	paExpr_aExpr_ISA_aExpr->m_paExpr3->accept(this);
	DataType rhs = m_exprValue;
	if (rhs.type != DataType::StringType && rhs.type != DataType::ColumnNameType)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Invalid second parameter type for ISA (should be a string or class name)");
	}
	String className = rhs.str;
	CIMInstanceArray newInstances;
	for (size_t i = 0; i < instances.size(); ++i)
	{
		CIMInstance ci = instances[i];
		if (ci)
		{
			CIMProperty cp = ci.getProperty(propName);
			if (cp)
			{
				CIMValue cv = cp.getValue();
				if (cv)
				{
					int valType = cv.getType();
					if (valType == CIMDataType::EMBEDDEDINSTANCE)
					{
						CIMInstance embeddedinst(CIMNULL);
						cv.get(embeddedinst);
						if (instanceIsDerivedFrom(embeddedinst, className))
						{
							newInstances.push_back(ci);
						}
					}
					else if (valType == CIMDataType::EMBEDDEDCLASS)
					{
						CIMClass embeddedcls(CIMNULL);
						cv.get(embeddedcls);
						if (classIsDerivedFrom(embeddedcls.getName(), className))
						{
							newInstances.push_back(ci);
						}
					}
				}
			}
		}
	}
	m_exprValue = DataType(newInstances);
}
void WQLProcessor::visit_aExpr_rowExpr(
	const aExpr_rowExpr* paExpr_rowExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	paExpr_rowExpr->m_prowExpr1->accept(this);
}
void WQLProcessor::visit_bExpr_cExpr(
	const bExpr_cExpr* pbExpr_cExpr
	)
{
	pbExpr_cExpr->m_pcExpr1->accept(this);
}
void WQLProcessor::visit_bExpr_PLUS_bExpr(
	const bExpr_PLUS_bExpr* pbExpr_PLUS_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_PLUS_bExpr->m_pbExpr2->accept(this);
}
void WQLProcessor::visit_bExpr_MINUS_bExpr(
	const bExpr_MINUS_bExpr* pbExpr_MINUS_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_MINUS_bExpr->m_pbExpr2->accept(this);
}
void WQLProcessor::visit_bExpr_BITINVERT_bExpr(
	const bExpr_BITINVERT_bExpr* pbExpr_BITINVERT_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_BITINVERT_bExpr->m_pbExpr2->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_PLUS_bExpr(
	const bExpr_bExpr_PLUS_bExpr* pbExpr_bExpr_PLUS_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_PLUS_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_PLUS_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_MINUS_bExpr(
	const bExpr_bExpr_MINUS_bExpr* pbExpr_bExpr_MINUS_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_MINUS_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_MINUS_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_ASTERISK_bExpr(
	const bExpr_bExpr_ASTERISK_bExpr* pbExpr_bExpr_ASTERISK_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_ASTERISK_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_ASTERISK_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_SOLIDUS_bExpr(
	const bExpr_bExpr_SOLIDUS_bExpr* pbExpr_bExpr_SOLIDUS_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_SOLIDUS_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_SOLIDUS_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_PERCENT_bExpr(
	const bExpr_bExpr_PERCENT_bExpr* pbExpr_bExpr_PERCENT_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_PERCENT_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_PERCENT_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_BITAND_bExpr(
	const bExpr_bExpr_BITAND_bExpr* pbExpr_bExpr_BITAND_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_BITAND_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_BITAND_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_BITOR_bExpr(
	const bExpr_bExpr_BITOR_bExpr* pbExpr_bExpr_BITOR_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_BITOR_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_BITOR_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_BITSHIFTLEFT_bExpr(
	const bExpr_bExpr_BITSHIFTLEFT_bExpr* pbExpr_bExpr_BITSHIFTLEFT_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_BITSHIFTLEFT_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_BITSHIFTLEFT_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_BITSHIFTRIGHT_bExpr(
	const bExpr_bExpr_BITSHIFTRIGHT_bExpr* pbExpr_bExpr_BITSHIFTRIGHT_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_BITSHIFTRIGHT_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_BITSHIFTRIGHT_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_LESSTHAN_bExpr(
	const bExpr_bExpr_LESSTHAN_bExpr* pbExpr_bExpr_LESSTHAN_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_LESSTHAN_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_LESSTHAN_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_LESSTHANOREQUALS_bExpr(
	const bExpr_bExpr_LESSTHANOREQUALS_bExpr* pbExpr_bExpr_LESSTHANOREQUALS_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_LESSTHANOREQUALS_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_LESSTHANOREQUALS_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_GREATERTHAN_bExpr(
	const bExpr_bExpr_GREATERTHAN_bExpr* pbExpr_bExpr_GREATERTHAN_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_GREATERTHAN_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_GREATERTHAN_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_GREATERTHANOREQUALS_bExpr(
	const bExpr_bExpr_GREATERTHANOREQUALS_bExpr* pbExpr_bExpr_GREATERTHANOREQUALS_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_GREATERTHANOREQUALS_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_GREATERTHANOREQUALS_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_EQUALS_bExpr(
	const bExpr_bExpr_EQUALS_bExpr* pbExpr_bExpr_EQUALS_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_EQUALS_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_EQUALS_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_NOTEQUALS_bExpr(
	const bExpr_bExpr_NOTEQUALS_bExpr* pbExpr_bExpr_NOTEQUALS_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_NOTEQUALS_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_NOTEQUALS_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_bExpr_bExpr_CONCATENATION_bExpr(
	const bExpr_bExpr_CONCATENATION_bExpr* pbExpr_bExpr_CONCATENATION_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pbExpr_bExpr_CONCATENATION_bExpr->m_pbExpr1->accept(this);
	pbExpr_bExpr_CONCATENATION_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_cExpr_attr(
	const cExpr_attr* pcExpr_attr
	)
{
	pcExpr_attr->m_pattr1->accept(this);
}
void WQLProcessor::visit_cExpr_strColId_optIndirection(
	const cExpr_strColId_optIndirection* pcExpr_strColId_optIndirection
	)
{
	m_exprValue = DataType(*pcExpr_strColId_optIndirection->m_pstrColId1, DataType::ColumnNameType);
	// TODO: What does indirection do? Array index
	if (pcExpr_strColId_optIndirection->m_poptIndirection2)
	{
		pcExpr_strColId_optIndirection->m_poptIndirection2->accept(this);
	}
}
void WQLProcessor::visit_cExpr_aExprConst(
	const cExpr_aExprConst* pcExpr_aExprConst
	)
{
	pcExpr_aExprConst->m_paExprConst1->accept(this);
}
void WQLProcessor::visit_cExpr_LEFTPAREN_aExpr_RIGHTPAREN(
	const cExpr_LEFTPAREN_aExpr_RIGHTPAREN* pcExpr_LEFTPAREN_aExpr_RIGHTPAREN
	)
{
	pcExpr_LEFTPAREN_aExpr_RIGHTPAREN->m_paExpr2->accept(this);
}
void WQLProcessor::visit_cExpr_strFuncName_LEFTPAREN_RIGHTPAREN(
	const cExpr_strFuncName_LEFTPAREN_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN(
	const cExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pcExpr_strFuncName_LEFTPAREN_exprSeq_RIGHTPAREN->m_pexprSeq3->accept(this);
}
void WQLProcessor::visit_cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN(
	const cExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pcExpr_strFuncName_LEFTPAREN_ALL_exprSeq_RIGHTPAREN->m_pexprSeq4->accept(this);
}
void WQLProcessor::visit_cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN(
	const cExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pcExpr_strFuncName_LEFTPAREN_DISTINCT_exprSeq_RIGHTPAREN->m_pexprSeq4->accept(this);
}
void WQLProcessor::visit_cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN(
	const cExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN* pcExpr_strFuncName_LEFTPAREN_ASTERISK_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_cExpr_CURRENTDATE(
	const cExpr_CURRENTDATE* pcExpr_CURRENTDATE
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_cExpr_CURRENTTIME(
	const cExpr_CURRENTTIME* pcExpr_CURRENTTIME
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN(
	const cExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN* pcExpr_CURRENTTIME_LEFTPAREN_ICONST_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_cExpr_CURRENTTIMESTAMP(
	const cExpr_CURRENTTIMESTAMP* pcExpr_CURRENTTIMESTAMP
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN(
	const cExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN* pcExpr_CURRENTTIMESTAMP_LEFTPAREN_ICONST_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_cExpr_CURRENTUSER(
	const cExpr_CURRENTUSER* pcExpr_CURRENTUSER
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_cExpr_SESSIONUSER(
	const cExpr_SESSIONUSER* pcExpr_SESSIONUSER
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_cExpr_USER(
	const cExpr_USER* pcExpr_USER
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
}
void WQLProcessor::visit_cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN(
	const cExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN* pcExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	if (pcExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN->m_poptExtract3)
	{
		pcExpr_EXTRACT_LEFTPAREN_optExtract_RIGHTPAREN->m_poptExtract3->accept(this);
	}
}
void WQLProcessor::visit_cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN(
	const cExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN* pcExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pcExpr_POSITION_LEFTPAREN_positionExpr_RIGHTPAREN->m_ppositionExpr3->accept(this);
}
void WQLProcessor::visit_cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN(
	const cExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN* pcExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	if (pcExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN->m_poptSubstrExpr3)
	{
		pcExpr_SUBSTRING_LEFTPAREN_optSubstrExpr_RIGHTPAREN->m_poptSubstrExpr3->accept(this);
	}
}
void WQLProcessor::visit_cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN(
	const cExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN* pcExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pcExpr_TRIM_LEFTPAREN_LEADING_trimExpr_RIGHTPAREN->m_ptrimExpr4->accept(this);
}
void WQLProcessor::visit_cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN(
	const cExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN* pcExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pcExpr_TRIM_LEFTPAREN_TRAILING_trimExpr_RIGHTPAREN->m_ptrimExpr4->accept(this);
}
void WQLProcessor::visit_cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN(
	const cExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN* pcExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	pcExpr_TRIM_LEFTPAREN_trimExpr_RIGHTPAREN->m_ptrimExpr3->accept(this);
}
void WQLProcessor::visit_optIndirection_empty(
	const optIndirection_empty* poptIndirection_empty
	)
{
}
void WQLProcessor::visit_optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET(
	const optIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET* poptIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	if (poptIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET->m_poptIndirection1)
	{
		poptIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET->m_poptIndirection1->accept(this);
	}
	poptIndirection_optIndirection_LEFTBRACKET_aExpr_RIGHTBRACKET->m_paExpr3->accept(this);
}
void WQLProcessor::visit_optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET(
	const optIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET* poptIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	if (poptIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET->m_poptIndirection1)
	{
		poptIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET->m_poptIndirection1->accept(this);
	}
	poptIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET->m_paExpr3->accept(this);
	poptIndirection_optIndirection_LEFTBRACKET_aExpr_COLON_aExpr_RIGHTBRACKET->m_paExpr5->accept(this);
}
void WQLProcessor::visit_optExtract_empty(
	const optExtract_empty* poptExtract_empty
	)
{
}
void WQLProcessor::visit_optExtract_strExtractArg_FROM_aExpr(
	const optExtract_strExtractArg_FROM_aExpr* poptExtract_strExtractArg_FROM_aExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	poptExtract_strExtractArg_FROM_aExpr->m_paExpr3->accept(this);
}
void WQLProcessor::visit_positionExpr_bExpr_IN_bExpr(
	const positionExpr_bExpr_IN_bExpr* ppositionExpr_bExpr_IN_bExpr
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	ppositionExpr_bExpr_IN_bExpr->m_pbExpr1->accept(this);
	ppositionExpr_bExpr_IN_bExpr->m_pbExpr3->accept(this);
}
void WQLProcessor::visit_positionExpr_empty(
	const positionExpr_empty* ppositionExpr_empty
	)
{
}
void WQLProcessor::visit_optSubstrExpr_empty(
	const optSubstrExpr_empty* poptSubstrExpr_empty
	)
{
}
void WQLProcessor::visit_optSubstrExpr_aExpr_substrFrom_substrFor(
	const optSubstrExpr_aExpr_substrFrom_substrFor* poptSubstrExpr_aExpr_substrFrom_substrFor
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	poptSubstrExpr_aExpr_substrFrom_substrFor->m_paExpr1->accept(this);
	poptSubstrExpr_aExpr_substrFrom_substrFor->m_psubstrFrom2->accept(this);
	poptSubstrExpr_aExpr_substrFrom_substrFor->m_psubstrFor3->accept(this);
}
void WQLProcessor::visit_optSubstrExpr_aExpr_substrFor_substrFrom(
	const optSubstrExpr_aExpr_substrFor_substrFrom* poptSubstrExpr_aExpr_substrFor_substrFrom
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	poptSubstrExpr_aExpr_substrFor_substrFrom->m_paExpr1->accept(this);
	poptSubstrExpr_aExpr_substrFor_substrFrom->m_psubstrFor2->accept(this);
	poptSubstrExpr_aExpr_substrFor_substrFrom->m_psubstrFrom3->accept(this);
}
void WQLProcessor::visit_optSubstrExpr_aExpr_substrFrom(
	const optSubstrExpr_aExpr_substrFrom* poptSubstrExpr_aExpr_substrFrom
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	poptSubstrExpr_aExpr_substrFrom->m_paExpr1->accept(this);
	poptSubstrExpr_aExpr_substrFrom->m_psubstrFrom2->accept(this);
}
void WQLProcessor::visit_optSubstrExpr_aExpr_substrFor(
	const optSubstrExpr_aExpr_substrFor* poptSubstrExpr_aExpr_substrFor
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	poptSubstrExpr_aExpr_substrFor->m_paExpr1->accept(this);
	poptSubstrExpr_aExpr_substrFor->m_psubstrFor2->accept(this);
}
void WQLProcessor::visit_optSubstrExpr_exprSeq(
	const optSubstrExpr_exprSeq* poptSubstrExpr_exprSeq
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	poptSubstrExpr_exprSeq->m_pexprSeq1->accept(this);
}
void WQLProcessor::visit_substrFrom(
	const substrFrom* psubstrFrom
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	psubstrFrom->m_paExpr2->accept(this);
}
void WQLProcessor::visit_substrFor(
	const substrFor* psubstrFor
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	psubstrFor->m_paExpr2->accept(this);
}
void WQLProcessor::visit_trimExpr_aExpr_FROM_exprSeq(
	const trimExpr_aExpr_FROM_exprSeq* ptrimExpr_aExpr_FROM_exprSeq
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	ptrimExpr_aExpr_FROM_exprSeq->m_paExpr1->accept(this);
	ptrimExpr_aExpr_FROM_exprSeq->m_pexprSeq3->accept(this);
}
void WQLProcessor::visit_trimExpr_FROM_exprSeq(
	const trimExpr_FROM_exprSeq* ptrimExpr_FROM_exprSeq
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	ptrimExpr_FROM_exprSeq->m_pexprSeq2->accept(this);
}
void WQLProcessor::visit_trimExpr_exprSeq(
	const trimExpr_exprSeq* ptrimExpr_exprSeq
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	ptrimExpr_exprSeq->m_pexprSeq1->accept(this);
}
void WQLProcessor::visit_attr(
	const attr* pattr
	)
{
	m_exprValue = DataType(*pattr->m_pstrRelationName1, DataType::ColumnNameType);
	pattr->m_pattrs3->accept(this);
	// TODO: What does indirection mean? Array index
	if (pattr->m_poptIndirection4)
	{
		pattr->m_poptIndirection4->accept(this);
	}
}
void WQLProcessor::visit_attrs_strAttrName(
	const attrs_strAttrName* pattrs_strAttrName
	)
{
	m_exprValue = DataType(m_exprValue.str + "." + *pattrs_strAttrName->m_pstrAttrName1, DataType::ColumnNameType);
}
void WQLProcessor::visit_attrs_attrs_PERIOD_strAttrName(
	const attrs_attrs_PERIOD_strAttrName* pattrs_attrs_PERIOD_strAttrName
	)
{
	pattrs_attrs_PERIOD_strAttrName->m_pattrs1->accept(this);
	m_exprValue = DataType(m_exprValue.str + "." + *pattrs_attrs_PERIOD_strAttrName->m_pstrAttrName3, DataType::ColumnNameType);
}
void WQLProcessor::visit_attrs_attrs_PERIOD_ASTERISK(
	const attrs_attrs_PERIOD_ASTERISK* pattrs_attrs_PERIOD_ASTERISK
	)
{
	pattrs_attrs_PERIOD_ASTERISK->m_pattrs1->accept(this);
	m_exprValue = DataType(m_exprValue.str + ".*", DataType::ColumnNameType);
}
void WQLProcessor::visit_targetEl_aExpr_AS_strColLabel(
	const targetEl_aExpr_AS_strColLabel* ptargetEl_aExpr_AS_strColLabel
	)
{
	OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
	ptargetEl_aExpr_AS_strColLabel->m_paExpr1->accept(this);
}
void WQLProcessor::visit_targetEl_aExpr(
	const targetEl_aExpr* ptargetEl_aExpr
	)
{
	ptargetEl_aExpr->m_paExpr1->accept(this);
	if (m_doingSelect)
	{
		if (m_exprValue.type != DataType::ColumnNameType)
		{
			OW_THROWCIMMSG(CIMException::INVALID_QUERY, "FROM clause properties must be property names");
		}
		m_propertyArray.push_back(m_exprValue.str);
	}
	//else // doing INSERT
	//{
	// We'll just leave the value in m_exprValue to be picked up by insert.
	//}
}
void WQLProcessor::visit_targetEl_strRelationName_PERIOD_ASTERISK(
	const targetEl_strRelationName_PERIOD_ASTERISK* ptargetEl_strRelationName_PERIOD_ASTERISK
	)
{
	m_propertyArray.push_back(*ptargetEl_strRelationName_PERIOD_ASTERISK->m_pstrRelationName1 + ".*");
}
void WQLProcessor::visit_targetEl_ASTERISK(
	const targetEl_ASTERISK* ptargetEl_ASTERISK
	)
{
	m_propertyArray.push_back("*");
}
void WQLProcessor::visit_updateTargetEl(
	const updateTargetEl* pupdateTargetEl
	)
{
	m_propertyArray.push_back(*pupdateTargetEl->m_pstrColId1);
	
	if (pupdateTargetEl->m_poptIndirection2)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Internal Parser Error: unimplemented functionality");
		pupdateTargetEl->m_poptIndirection2->accept(this);
	}
	pupdateTargetEl->m_paExpr4->accept(this);
	m_valueArray.push_back(m_exprValue);
}
void WQLProcessor::visit_aExprConst_ICONST(
	const aExprConst_ICONST* paExprConst_ICONST
	)
{
	try
	{
		m_exprValue = DataType(paExprConst_ICONST->m_pICONST1->toInt64());
	}
	catch (const StringConversionException& e)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, e.getMessage());
	}
}
void WQLProcessor::visit_aExprConst_FCONST(
	const aExprConst_FCONST* paExprConst_FCONST
	)
{
	try
	{
		m_exprValue = DataType(paExprConst_FCONST->m_pFCONST1->toReal64());
	}
	catch (const StringConversionException& e)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, e.getMessage());
	}
}
void WQLProcessor::visit_aExprConst_SCONST(
	const aExprConst_SCONST* paExprConst_SCONST
	)
{
	m_exprValue = DataType(WQLRemoveStringEscapes(*paExprConst_SCONST->m_pSCONST1), DataType::StringType);
}
void WQLProcessor::visit_aExprConst_BITCONST(
	const aExprConst_BITCONST* paExprConst_BITCONST
	)
{
	char* endptr;
	const char* p = paExprConst_BITCONST->m_pBITCONST1->c_str();
	errno = 0;
	Int64 val = String::strtoll(p, &endptr, 2);
	if (*endptr != '\0' || errno == ERANGE)
	{
		OW_THROWCIMMSG( CIMException::INVALID_QUERY, Format( "Bad bitstring integer input '%1'",
			 p).c_str() );
	}
	
	m_exprValue = DataType(val);
}
void WQLProcessor::visit_aExprConst_HEXCONST(
	const aExprConst_HEXCONST* paExprConst_HEXCONST
	)
{
	char* endptr;
	const char* p = paExprConst_HEXCONST->m_pHEXCONST1->c_str();
	errno = 0;
	Int64 val = String::strtoll(p, &endptr, 16);
	if (*endptr != '\0' || errno == ERANGE)
	{
		OW_THROWCIMMSG( CIMException::INVALID_QUERY, Format( "Bad bitstring integer input '%1'",
			 p).c_str() );
	}
	
	m_exprValue = DataType(val);
}
void WQLProcessor::visit_aExprConst_TRUEP(
	const aExprConst_TRUEP* paExprConst_TRUEP
	)
{
	m_exprValue = DataType(Bool(true));
}
void WQLProcessor::visit_aExprConst_FALSEP(
	const aExprConst_FALSEP* paExprConst_FALSEP
	)
{
	m_exprValue = DataType(Bool(false));
}
void WQLProcessor::visit_aExprConst_NULLP(
	const aExprConst_NULLP* paExprConst_NULLP
	)
{
	m_exprValue = DataType(DataType::NullType);
}
CIMInstanceArray
WQLProcessor::filterInstancesOnPropertyValue(const String& propName, const CIMValue& val, const Compare& compare)
{
	//LOGDEBUG(Format("WQLProcessor::filterInstancesOnPropertyValue\n"
	//	"\tFiltering instances on property: %1 %2 %3", propName, compare.c_str(), val ? val.toString() : "NULL" ));
	CIMInstanceArray rval;
	for (size_t i = 0; i < instances.size(); ++i)
	{
		CIMInstance ci = instances[i];
		if (ci)
		{
			if (propName.equalsIgnoreCase("__Path"))
			{
				if (val.getType() == CIMDataType::STRING)
				{
					CIMObjectPath valCop = CIMObjectPath::parse(
						CIMObjectPath::unEscape(val.toString()));
					valCop.setNameSpace(m_ns);
					CIMObjectPath instCop(m_ns, ci);
					if (compare(CIMValue(instCop.toString()),
						CIMValue(valCop.toString())))
					{
						rval.push_back(ci);
					}
					
				}
				else
				{
					OW_THROWCIMMSG(CIMException::INVALID_QUERY,
						"The property __Path must be compared to a string");
				}
			}
			else
			{
				CIMProperty cp(CIMNULL);
				if (propName.indexOf('.') != String::npos)
				{
					// it's an embedded property
					String curPropName;
					CIMInstance curci = ci;
					StringArray propNames = propName.tokenize(".");
					for (size_t i = 0; i < propNames.size(); ++i)
					{
						cp = curci.getProperty(propNames[i]);
						if (cp)
						{
	
							if (i < propNames.size() - 1)
							{
								if (cp.getValue().getType() != CIMDataType::EMBEDDEDINSTANCE)
								{
									cp.setNull();
									break;
								}
								cp.getValue().get(curci);
							}
						}
					}
				}
				else
				{
					cp = ci.getProperty(propName);
				}
				
				if (cp)
				{
					CIMValue cv = cp.getValue();
					if (cv)
					{
						int valType = cv.getType();
						if (valType == CIMDataType::SINT8 ||
							valType == CIMDataType::SINT16 ||
							valType == CIMDataType::SINT32 ||
							valType == CIMDataType::SINT64 ||
							valType == CIMDataType::UINT8 ||
							valType == CIMDataType::UINT16 ||
							valType == CIMDataType::UINT32
							/* Don't handle UINT64 */)
						{
							// Upgrade cv to be a INT64, so the comparison will work
							cv = CIMValueCast::castValueToDataType(cv, CIMDataType::SINT64);
						}
						else if (valType == CIMDataType::REAL32)
						{
							// Upgrade a REAL32 to a REAL64
							cv = CIMValueCast::castValueToDataType(cv, CIMDataType::REAL64);
						}
					}
					if (compare(cv, val))
					{
						rval.push_back(ci);
					}
				}
			}
		}
	}
	//LOGDEBUG(Format("WQLProcessor::filterInstancesOnPropertyValue\n"
	//	"\treturning %1 instances", rval.size()));
	return rval;
}
bool WQLProcessor::Equals::operator()(const CIMValue& lhs, const CIMValue& rhs) const
{
	if (lhs && rhs)
	{
		return lhs == rhs;
	}
	else if (!lhs && !rhs)
	{
		return true; // both are NULL
	}
	else
	{
		return false;
	}
}
bool WQLProcessor::NotEquals::operator()(const CIMValue& lhs, const CIMValue& rhs) const
{
	if (lhs && rhs)
	{
		return !(lhs == rhs);
	}
	if (lhs && !rhs) // this is so that "... WHERE prop <> NULL" works
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool WQLProcessor::GreaterThanOrEquals::operator()(const CIMValue& lhs, const CIMValue& rhs) const
{
	if (lhs && rhs)
	{
		return lhs >= rhs;
	}
	else
	{
		return false;
	}
}
bool WQLProcessor::LessThanOrEquals::operator()(const CIMValue& lhs, const CIMValue& rhs) const
{
	if (lhs && rhs)
	{
		return lhs <= rhs;
	}
	else
	{
		return false;
	}
}
bool WQLProcessor::GreaterThan::operator()(const CIMValue& lhs, const CIMValue& rhs) const
{
	if (lhs && rhs)
	{
		return lhs > rhs;
	}
	else
	{
		return false;
	}
}
bool WQLProcessor::LessThan::operator()(const CIMValue& lhs, const CIMValue& rhs) const
{
	if (lhs && rhs)
	{
		return lhs < rhs;
	}
	else
	{
		return false;
	}
}
void WQLProcessor::populateInstances(const String& className)
{
	m_tableRef = className;
	populateInstances();
}
namespace
{
	class InstanceArrayBuilder : public CIMInstanceResultHandlerIFC
	{
	public:
		InstanceArrayBuilder(CIMInstanceArray& cia_)
		: cia(cia_)
		{}
	protected:
		virtual void doHandle(const CIMInstance &i)
		{
			cia.push_back(i);
		}
	private:
		CIMInstanceArray& cia;
	};
}
void WQLProcessor::populateInstances()
{
	InstanceArrayBuilder handler(instances);
	m_hdl->enumInstances(m_ns, m_tableRef, handler, E_DEEP);
}

} // end namespace OpenWBEM

