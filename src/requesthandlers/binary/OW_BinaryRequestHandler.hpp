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
#ifndef OW_BINARYREQUESTHANDLER_HPP_INCLUDE_GUARD_
#define OW_BINARYREQUESTHANDLER_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_CIMFwd.hpp"

class OW_BinaryRequestHandler : public OW_RequestHandlerIFC
{
public:
	OW_BinaryRequestHandler();

	virtual OW_RequestHandlerIFC* clone() const;

	virtual void doProcess(std::istream *istr, std::ostream *ostrEntity,
            std::ostream *ostrError, const OW_SortedVectorMap<OW_String, OW_String>& handlerVars);

	virtual void doOptions(OW_CIMFeatures &cf, const OW_SortedVectorMap<OW_String, OW_String>& handlerVars);

	virtual void setEnvironment(OW_ServiceEnvironmentIFCRef env);

	virtual OW_StringArray getSupportedContentTypes() const;

	virtual OW_String getContentType() const;

private:

	void createClass(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void createInstance(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void enumClasses(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void deleteClass(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void deleteInstance(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void deleteQual(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void getClass(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void getInstance(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void getQual(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void setQual(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void modifyClass(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void modifyInstance(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void setProperty(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void getProperty(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void enumClassNames(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void enumInstances(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void enumInstanceNames(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
            std::istream& istrm);

	void enumQualifiers(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void invokeMethod(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void execQuery(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	void associators(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void associatorNames(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
        std::istream& istrm);

	void references(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void referenceNames(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);
#endif

	void getServerFeatures(OW_CIMOMHandleIFCRef chdl, std::ostream& ostrm,
		std::istream& istrm);

	void writeError(std::ostream& ostrm, const char* msg);

	OW_Bool writeFileName(std::ostream& ostrm, const OW_String& fname);

	OW_UserId m_userId;
};

#endif



