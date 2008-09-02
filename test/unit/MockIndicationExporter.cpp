/*******************************************************************************
* Copyright (C) 2008, Quest Software, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of 
*       Quest Software, Inc., 
*       nor the names of its contributors or employees may be used to 
*       endorse or promote products derived from this software without 
*       specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
// @author Kevin S. Van Horn

#include "MockIndicationExporter.hpp"
#include "CIMInstanceUtils.hpp"

using namespace blocxx;
using namespace OpenWBEM;
using namespace CIMInstanceUtils;

MockIndicationExporter::MockIndicationExporter()
: m_beginExportCalled(false),
  m_endExportCalled(false),
  m_indicationIndex(0),
  m_indicationsExpected()
{
}

MockIndicationExporter::~MockIndicationExporter()
{
}

void MockIndicationExporter::expectIndication(CIMInstance const & ind)
{
	m_indicationsExpected.push_back(ind);
}

void MockIndicationExporter::doVerify()
{
	mockAssert(
		m_beginExportCalled && m_endExportCalled,
		"MockIndicationExporter::verify (begin/end)");
	mockAssert(m_indicationIndex == m_indicationsExpected.size(),
		"MockIndicationExporter::verify (# indications)");
}

void MockIndicationExporter::beginExport()
{
	mockAssert(!m_beginExportCalled,
		"MockIndicationExporter::beginExport");
	m_beginExportCalled = true;
}

void MockIndicationExporter::endExport()
{
	mockAssert(
		m_beginExportCalled && !m_endExportCalled,
		"MockIndicationExporter::endExport");
	m_endExportCalled = true;
}

void MockIndicationExporter::sendIndication(CIMInstance const & ci)
{
	mockAssert(
		m_beginExportCalled && !m_endExportCalled,
		"MockIndicationExporter::sendIndication (ordering)");
	mockAssert(m_indicationIndex < m_indicationsExpected.size(),
		"MockIndicationExporter: Number of indications sent");
	mockAssert(basicEqual(m_indicationsExpected[m_indicationIndex], ci),
		"MockIndicationExporter: Indication value");
	++m_indicationIndex;
}
