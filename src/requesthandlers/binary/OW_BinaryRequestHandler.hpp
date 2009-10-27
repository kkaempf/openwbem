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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_BINARYREQUESTHANDLER_HPP_INCLUDE_GUARD_
#define OW_BINARYREQUESTHANDLER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_CIMFwd.hpp"

namespace OW_NAMESPACE
{

class BinaryRequestHandler : public RequestHandlerIFC
{
public:
	BinaryRequestHandler();

	enum ESupportExportIndicationFlag
	{
		E_SUPPORT_EXPORT_INDICATION,
		E_DONT_SUPPORT_EXPORT_INDICATION
	};

	BinaryRequestHandler(ESupportExportIndicationFlag supportExportIndicationFlag);

public:
	virtual RequestHandlerIFC* clone() const;
	virtual void doProcess(std::istream * istr, std::ostream * ostrEntity,
			std::ostream * ostrError, OperationContext& context);
	void doProcess(std::streambuf * istr, std::streambuf * ostrEntity,
			std::streambuf * ostrError, OperationContext& context);
	virtual void doOptions(CIMFeatures &cf, OperationContext& context);
	virtual blocxx::StringArray getSupportedContentTypes() const;
	virtual blocxx::String getContentType() const;

	virtual blocxx::String getName() const;
	virtual void init(const ServiceEnvironmentIFCRef& env);
	virtual void shutdown();
private:
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	void createClass(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void modifyClass(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void deleteClass(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	void createInstance(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void deleteInstance(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void modifyInstance(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	void setProperty(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

	void enumClasses(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void getClass(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);
	void getInstance(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void getQual(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	void setQual(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void enumQualifiers(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void deleteQual(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	void getProperty(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

	void enumClassNames(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void enumInstances(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void enumInstanceNames(const CIMOMHandleIFCRef & chdl,
		std::streambuf & ostrm,	std::streambuf & istrm);

	void invokeMethod(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void execQuery(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	void associators(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void associatorNames(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void references(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);

	void referenceNames(const CIMOMHandleIFCRef & chdl, std::streambuf & ostrm,
		std::streambuf & istrm);
#endif

	void getServerFeatures(const CIMOMHandleIFCRef & chdl,
		std::streambuf & ostrm,	std::streambuf & istrm);

	void exportIndication(const CIMOMHandleIFCRef & chdl,
		std::streambuf & ostrm,	std::streambuf & istrm);

	void writeError(std::streambuf & ostrm, const char* msg);

private:
	bool m_supportExportIndication;
};

} // end namespace OW_NAMESPACE

#endif
