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

#include "OW_WQLImpl.hpp"
#include "OW_MutexLock.hpp"
#include "OW_WQLProcessor.hpp"
#include "OW_WQLAst.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_CIMException.hpp"
#include "OW_WQLSelectStatementGen.hpp"

namespace OpenWBEM
{

Mutex WQLImpl::s_classLock;
const char* WQLImpl::s_parserInput = 0;
stmt* WQLImpl::s_statement = 0;

WQLImpl::~WQLImpl()
{
}

void WQLImpl::evaluate(const String& nameSpace,
	CIMInstanceResultHandlerIFC& result,
	const String& query, const String& queryLanguage,
	const CIMOMHandleIFCRef& hdl)
{
	MutexLock lock(s_classLock);
	// set up the parser's input
	s_parserInput = query.c_str();
	WQLscanner_init();
#ifdef YYOW_DEBUG
	owwqldebug = 1;
#endif
	int owwqlresult = owwqlparse();
	if (owwqlresult)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Parse failed");
	}
	else
	{
		//LOGDEBUG("Parse succeeded");
	}
	WQLProcessor p(hdl, nameSpace);
	AutoPtr<stmt> pAST(WQLImpl::s_statement);
	lock.release();
	if (pAST.get())
	{
		pAST->accept(&p);
	}
	else
	{
		//LOGDEBUG("pAST was NULL!");
	}
	CIMInstanceArray instances = p.getInstances();
	for (size_t i = 0; i < instances.size(); ++i)
	{
		result.handle(instances[i]);
	}

	s_parserInput = 0;
}
 
WQLSelectStatement
WQLImpl::createSelectStatement(const String& query)
{
	MutexLock lock(s_classLock);
	// set up the parser's input
	s_parserInput = query.c_str();
	WQLscanner_init();
#ifdef YYOW_DEBUG
	owwqldebug = 1;
#endif
	int owwqlresult = owwqlparse();
	if (owwqlresult)
	{
		OW_THROWCIMMSG(CIMException::INVALID_QUERY, "Parse failed");
	}
	else
	{
		//LOGDEBUG("Parse succeeded");
	}
	WQLSelectStatementGen p;
	AutoPtr<stmt> pAST(WQLImpl::s_statement);
	lock.release();
	if (pAST.get())
	{
		pAST->accept(&p);
	}
	else
	{
		//LOGDEBUG("pAST was NULL!");
	}
	
	s_parserInput = 0;

	return p.getSelectStatement();
}
bool WQLImpl::supportsQueryLanguage(const String& lang)
{
	if (lang.equalsIgnoreCase("wql1"))
	{
		return true;
	}
	else if (lang.equalsIgnoreCase("wql2"))
	{
		return true;
	}
	else if (lang.equalsIgnoreCase("wql"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

} // end namespace OpenWBEM

OW_WQLFACTORY(OpenWBEM::WQLImpl,wqlimpl);

