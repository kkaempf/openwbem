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

#ifndef OW_CIMTOXML_HPP_
#define OW_CIMTOXML_HPP_

#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include <iosfwd>

/////////////////////////////////////////////////////////////
void OW_CIMNameSpacetoXML(OW_CIMNameSpace const& ns, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_LocalCIMNameSpacetoXML(OW_CIMNameSpace const& ns, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMQualifierType const& cqt, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMClassPathtoXML(OW_CIMObjectPath const& cop, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMInstancePathtoXML(OW_CIMObjectPath const& cop, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMInstanceNametoXML(OW_CIMObjectPath const& cop, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMClass const& cc, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMInstancetoXML(OW_CIMInstance const& ci, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMInstanceNameAndInstancetoXML(OW_CIMInstance const& instance, 
	std::ostream& ostr, OW_CIMObjectPath const& instanceName);

/////////////////////////////////////////////////////////////
void OW_CIMInstancePathAndInstancetoXML(OW_CIMInstance const& instance, 
	std::ostream& ostr, OW_CIMObjectPath const& instancePath);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMValue const& cv, std::ostream& out);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMDataType const& cdt, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMFlavor const& cf, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMQualifier const& cq, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMProperty const& cp, std::ostream& ostr);
				
/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMMethod const& cm, std::ostream& ostr);
				
/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMParameter const& cp, std::ostream& ostr);

/////////////////////////////////////////////////////////////
//void OW_CIMClassPathtoXML(OW_CIMObjectPath const& cop, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMParamValueToXML(OW_CIMParamValue const& pv, std::ostream& ostr);

#endif				
