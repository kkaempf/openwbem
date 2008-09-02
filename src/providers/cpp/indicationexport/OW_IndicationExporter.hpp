/*******************************************************************************
* Copyright (C) 2001-2008 Quest Software, Inc. All rights reserved.
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
 * @author Kevin S. Van Horn
 */

#ifndef OW_INDICATION_EXPORTER_HPP_
#define OW_INDICATION_EXPORTER_HPP_
#include "OW_config.h"
#include "OW_CIMProtocolIFC.hpp"
#include "OW_XMLFwd.hpp"
#include "OW_IntrusiveCountableBase.hpp"

namespace OW_NAMESPACE
{

struct IndicationExporter : IntrusiveCountableBase
{
	virtual ~IndicationExporter();
	virtual void beginExport() = 0;
	virtual void endExport() = 0;
	virtual void sendIndication(CIMInstance const & ci) = 0;
};

typedef IntrusiveReference<IndicationExporter> IndicationExporterRef;

class IndicationExporterImpl : public IndicationExporter
{
public:
	IndicationExporterImpl( CIMProtocolIFCRef prot );
	virtual ~IndicationExporterImpl();
	virtual void beginExport();
	virtual void endExport();

	// PROMISE: calls Thread::testCancel() to ensure timely shutdown
	// when a cooperative cancel has been requested.
	virtual void sendIndication(CIMInstance const & ci);

	static IndicationExporterRef create(const CIMInstance & indHandlerInst);

private:
	void sendXMLHeader(const String& cimProtocolVersion);
	void sendXMLTrailer();
	void doSendRequest(
		const String& methodName, const String& cimProtocolVersion);
	/**
	 * @throws CIMException
	 */
	void checkNodeForCIMError(CIMXMLParser& parser,
		const String& operation);
	
	CIMProtocolIFCRef m_protocol;
	Reference<std::ostream> m_ostrm;
	Int32 m_iMessageID;
};


} // end namespace OW_NAMESPACE

#endif
