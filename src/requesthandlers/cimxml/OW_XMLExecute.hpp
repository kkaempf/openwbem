/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#ifndef OW_XMLEXECUTE_HPP_INCLUDE_GUARD_
#define OW_XMLEXECUTE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMOMHandleIFC.hpp"
#include "blocxx/String.hpp"
#include "blocxx/Map.hpp"
#include "OW_XMLQualifier.hpp"
#include "OW_RequestHandlerIFCXML.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_CIMObjectPath.hpp"

#include <iosfwd>

namespace OW_NAMESPACE
{

class XMLExecute : public RequestHandlerIFCXML
{
public:
	XMLExecute();
	virtual ~XMLExecute();
	int executeXML(CIMXMLParser& parser, std::ostream* ostrEntity,
		std::ostream* ostrError, OperationContext& userName);
	virtual RequestHandlerIFC* clone() const;
protected:
	virtual void doOptions(CIMFeatures& cf, OperationContext& context);
	virtual void outputError(CIMException::ErrNoType errorCode,
		const String& msg, std::ostream& ostr);

	virtual String getName() const;
	virtual StringArray getDependencies() const;
	virtual void init(const ServiceEnvironmentIFCRef& env);
	virtual void shutdown();
private:
	// These pointers will always be NULL when clone() is called
	// (clone() insures this).  They are assigned in executeXML().
	std::ostream* m_ostrEntity;
	std::ostream* m_ostrError;
	bool m_isIntrinsic;
	String m_functionName;
	/**
	 * @throws IOException
	 */
	void executeIntrinsic(std::ostream& osrt, CIMXMLParser& parser, CIMOMHandleIFC& hdl,
			const String& ns);
	void executeExtrinsic(std::ostream& ostr, CIMXMLParser& parser,
		CIMOMHandleIFC& lch);
	void doInvokeMethod(std::ostream& ostr, CIMXMLParser& parser,
		const String& methodName, CIMOMHandleIFC& lch);
	void processSimpleReq(CIMXMLParser& parser, std::ostream& ostrEntity,
		std::ostream& ostrError, OperationContext& context);
	static void getParameters(CIMXMLParser& parser,
		CIMParamValueArray& params);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	void associatorNames(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
	void associators(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
#endif
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	void createClass(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
	void modifyClass(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
	void deleteClass(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	void createInstance(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
	void deleteInstance(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
	void modifyInstance(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	void setProperty(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif
	void enumerateClassNames(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
	void enumerateClasses( std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl) ;
	void enumerateInstanceNames(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
	void enumerateInstances(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl) ;
	void getClass(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl) ;
	void getInstance(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	void getProperty(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	void getQualifier(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	void referenceNames(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl) ;
	void references(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl) ;
#endif
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	void setQualifier(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
	void deleteQualifier(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
	void enumerateQualifiers(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
#endif
	void execQuery(std::ostream& ostr, CIMXMLParser& parser,
		const String& ns, CIMOMHandleIFC& hdl);
	typedef void (XMLExecute::*execFuncPtr_t)(std::ostream& ostr,
		CIMXMLParser& qualNode, const String& ns, CIMOMHandleIFC& hdl);
	struct FuncEntry
	{
		const char* name;
		execFuncPtr_t func;
	};

	static FuncEntry g_funcs[];
	static bool funcEntryCompare(const FuncEntry& f1, const FuncEntry& f2);
	static FuncEntry* g_funcsEnd;

	// this is the path to the interop instance of CIM_CIMXMLCommunicationMechanism this class manages.
	CIMObjectPath m_commMechPath;
	// this is the path to the interop instance of CIM_HostedAccessPoint this class manages.
	CIMObjectPath m_hostedAccessPointPath;
	// this is the path to the interop instance of OpenWBEM_CIMXMLCommMechanismForOpenWBEMManager this class manages.
	CIMObjectPath m_commMechForManager;
};

} // end namespace OW_NAMESPACE

#endif
