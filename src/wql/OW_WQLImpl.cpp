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
#include "OW_WQLImpl.hpp"
#include "OW_MutexLock.hpp"
#include "OW_WQLProcessor.hpp"
#include "OW_WQLAst.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_CIMException.hpp"

OW_Mutex OW_WQLImpl::classLock;
const char* OW_WQLImpl::parserInput;
stmt* OW_WQLImpl::statement;

OW_CIMInstanceArray OW_WQLImpl::evaluate(const OW_CIMNameSpace& nameSpace, const OW_String& query, const OW_String& queryLanguage,
	OW_Reference<OW_CIMOMHandle> hdl)
{
	(void)queryLanguage;
	OW_MutexLock lock(classLock);

	// set up the parser's input
	parserInput = query.c_str();
	OW_WQLscanner_init();

#ifdef YYOW_DEBUG
	yydebug = 1;
#endif

	int yyresult = yyparse();
	if (yyresult)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Parse failed");
	}
	else
	{
		//OW_LOGDEBUG("Parse succeeded");
	}

	OW_WQLProcessor p(hdl, nameSpace);
	OW_AutoPtr<stmt> pAST(OW_WQLImpl::statement);
	lock.release();

	if (pAST.get())
	{
		pAST->accept(&p);
	}
	else
	{
		//OW_LOGDEBUG("pAST was NULL!");
	}
	return p.instances;
}

OW_Bool OW_WQLImpl::supportsQueryLanguage(const OW_String& lang)
{
	if (lang.equalsIgnoreCase("wql1"))
	{
		return true;
	}
	else if (lang.equalsIgnoreCase("wql2"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

OW_WQLFACTORY(OW_WQLImpl);


