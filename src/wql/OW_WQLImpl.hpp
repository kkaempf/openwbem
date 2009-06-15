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

#ifndef OW_WQLIMPL_HPP_
#define OW_WQLIMPL_HPP_
#include "OW_config.h"
#include "OW_WQLIFC.hpp"
#include "blocxx/Mutex.hpp"
#include <cstdio>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{
namespace WQL
{
class stmt;

class OW_WQL_API WQLImpl : public WQLIFC
{
public:
	virtual ~WQLImpl();
	
	String getName() const;
	virtual void init(const ServiceEnvironmentIFCRef& env);
	virtual void shutdown();

	virtual void evaluate(const String& nameSpace,
		CIMInstanceResultHandlerIFC& result,
		const String& query, const String& queryLanguage,
		const CIMOMHandleIFCRef& hdl);
	virtual void evaluate(const String& nameSpace,
		CIMInstanceResultHandlerIFC& result,
		const String& query, const String& queryLanguage,
		const RepositoryIFCRef& hdl,
		OperationContext& oc);
	virtual WQLSelectStatement createSelectStatement(const String& query);
	virtual bool supportsQueryLanguage(const String& lang);

	static WQL::stmt* setStatement(WQL::stmt* statement)
	{
		s_statement = statement;
		return statement;
	}

	static const char* getParserInput()
	{
		return s_parserInput;
	}
private:
	static Mutex s_classLock;
	static const char* s_parserInput;
	static WQL::stmt* s_statement;
};

class ParseError;

} // end namespace WQL
} // end namespace OW_NAMESPACE

extern int owwqldebug;
extern "C" int owwqlparse(OpenWBEM::WQL::ParseError* p_err);
extern FILE* owwqlin;
extern void WQLscanner_init();


#endif
