/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_CIMTOXML_HPP_
#define OW_CIMTOXML_HPP_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include <iosfwd>

namespace OpenWBEM
{

/////////////////////////////////////////////////////////////
void CIMNameSpacetoXML(CIMNameSpace const& ns, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void LocalCIMNameSpacetoXML(CIMNameSpace const& ns, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void CIMtoXML(CIMQualifierType const& cqt, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void CIMClassPathtoXML(CIMObjectPath const& cop, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void CIMInstancePathtoXML(CIMObjectPath const& cop, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void CIMInstanceNametoXML(CIMObjectPath const& cop, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void CIMtoXML(CIMClass const& cc, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void CIMInstancetoXML(CIMInstance const& ci, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void CIMInstanceNameAndInstancetoXML(CIMInstance const& instance, 
	std::ostream& ostr, CIMObjectPath const& instanceName);
/////////////////////////////////////////////////////////////
void CIMInstancePathAndInstancetoXML(CIMInstance const& instance, 
	std::ostream& ostr, CIMObjectPath const& instancePath);
/////////////////////////////////////////////////////////////
void CIMtoXML(CIMValue const& cv, std::ostream& out);
/////////////////////////////////////////////////////////////
void CIMtoXML(CIMDataType const& cdt, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void CIMtoXML(CIMFlavor const& cf, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void CIMtoXML(CIMQualifier const& cq, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void CIMtoXML(CIMProperty const& cp, std::ostream& ostr);
				
/////////////////////////////////////////////////////////////
void CIMtoXML(CIMMethod const& cm, std::ostream& ostr);
				
/////////////////////////////////////////////////////////////
void CIMtoXML(CIMParameter const& cp, std::ostream& ostr);
/////////////////////////////////////////////////////////////
//void CIMClassPathtoXML(CIMObjectPath const& cop, std::ostream& ostr);
/////////////////////////////////////////////////////////////
void CIMParamValueToXML(CIMParamValue const& pv, std::ostream& ostr);

} // end namespace OpenWBEM

#endif				
