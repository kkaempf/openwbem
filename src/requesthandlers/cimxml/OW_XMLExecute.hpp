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
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_XMLEXECUTE_HPP_INCLUDE_GUARD_
#define OW_XMLEXECUTE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_String.hpp"
#include "OW_Map.hpp"
#include "OW_XMLQualifier.hpp"
#include "OW_RequestHandlerIFCXML.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include <iosfwd>

class OW_CIMOMHandleIFC;

class OW_XMLExecute : public OW_RequestHandlerIFCXML, OW_XMLQualifier
{
public:
	OW_XMLExecute();
	virtual ~OW_XMLExecute();
	int executeXML(OW_CIMXMLParser& parser, std::ostream* ostrEntity,
		std::ostream* ostrError, const OW_String& userName);

	virtual OW_RequestHandlerIFC* clone() const;

protected:
	virtual void doOptions(OW_CIMFeatures& cf, const OW_SortedVectorMap<OW_String, OW_String>& handlerVars);
	virtual void doLogDebug(const OW_String& message);
	virtual void doLogError(const OW_String& message);
	virtual void doLogCustInfo(const OW_String& message);
	virtual void outputError(OW_CIMException::ErrNoType errorCode,
		OW_String msg, std::ostream& ostr);

private:

	// These pointers will always be NULL when clone() is called
	// (clone() insures this).  They are assigned in executeXML().
	std::ostream* m_ostrEntity;
	std::ostream* m_ostrError;

	bool m_isIntrinsic;
	OW_String m_functionName;


	/**
	 * @throws OW_IOException
	 */
	void executeIntrinsic(std::ostream& osrt, OW_CIMXMLParser& parser, OW_CIMOMHandleIFC& hdl,
			const OW_String& ns);
	void executeExtrinsic(std::ostream& ostr, OW_CIMXMLParser& parser,
		OW_CIMOMHandleIFC& lch);

	void doInvokeMethod(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& methodName, OW_CIMOMHandleIFC& lch);

	void processSimpleReq(OW_CIMXMLParser& parser, std::ostream& ostrEntity,
		std::ostream& ostrError, const OW_String& userName);

	static void getParameters(OW_CIMXMLParser& parser,
		OW_CIMParamValueArray& params);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	void associatorNames(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void associators(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
#endif
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	void createClass(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void modifyClass(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void deleteClass(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	void createInstance(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void deleteInstance(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void modifyInstance(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void setProperty(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
#endif
	void enumerateClassNames(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void enumerateClasses( std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl) ;
	void enumerateInstanceNames(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void enumerateInstances(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl) ;
	void getClass(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl) ;
	void getInstance(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void getProperty(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void getQualifier(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	void referenceNames(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl) ;
	void references(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl) ;
#endif
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	void setQualifier(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void deleteQualifier(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
	void enumerateQualifiers(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);
#endif
	void execQuery(std::ostream& ostr, OW_CIMXMLParser& parser,
		const OW_String& ns, OW_CIMOMHandleIFC& hdl);

	typedef void (OW_XMLExecute::*execFuncPtr_t)(std::ostream& ostr,
		OW_CIMXMLParser& qualNode, const OW_String& ns, OW_CIMOMHandleIFC& hdl);

	struct FuncEntry
	{
		const char* name;
		execFuncPtr_t func;
	};
	
	static FuncEntry g_funcs[];
	static bool funcEntryCompare(const FuncEntry& f1, const FuncEntry& f2);
	static FuncEntry* g_funcsEnd;

};


#endif

