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

#ifndef OW_CIMTOXML_HPP_
#define OW_CIMTOXML_HPP_

#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include <iosfwd>

namespace OW_CIMtoXMLFlags
{

//////////////////////////////////////////////////////
enum do_local_flag
{
	doLocal,
	dontDoLocal
};

//////////////////////////////////////////////////////
enum local_only_flag
{
	localOnly,
	notLocalOnly
};

//////////////////////////////////////////////////////
enum include_qualifiers_flag
{
	includeQualifiers,
	dontIncludeQualifiers
};


//////////////////////////////////////////////////////
enum include_class_origin_flag
{
	includeClassOrigin,
	dontIncludeClassOrigin
};


//////////////////////////////////////////////////////
enum property_list_flag
{
	ignorePropertyList,
	usePropertyList
};

//////////////////////////////////////////////////////
enum is_instance_name_flag
{
	isInstanceName,
	isNotInstanceName
};


}


/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMNameSpace const& ns, std::ostream& ostr,
	OW_CIMtoXMLFlags::do_local_flag const& doLocal);
	// doLocal used to default to true

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMQualifierType const& cqt, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMObjectPath const& cop, std::ostream& ostr,
	OW_CIMtoXMLFlags::is_instance_name_flag const& i);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMClass const& cc, std::ostream& ostr,
	OW_CIMtoXMLFlags::local_only_flag const& localOnly,
	OW_CIMtoXMLFlags::include_qualifiers_flag const& includeQualifiers,
	OW_CIMtoXMLFlags::include_class_origin_flag const& includeClassOrigin,
	OW_StringArray const& propertyList,
	bool noProps = false);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMInstance const& ci, std::ostream& ostr,
	OW_CIMObjectPath const& cop,
	OW_CIMtoXMLFlags::local_only_flag const& localOnly,
	OW_CIMtoXMLFlags::include_qualifiers_flag const& includeQualifiers,
	OW_CIMtoXMLFlags::include_class_origin_flag const& includeClassOrigin,
	OW_StringArray const& propertyList,
	bool noProps = false);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMValue const& cv, std::ostream& out);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMDataType const& cdt, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMFlavor const& cf, std::ostream& ostr);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMQualifier const& cq, std::ostream& ostr,
	OW_CIMtoXMLFlags::local_only_flag const& localOnly);

/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMProperty const& cp, std::ostream& ostr,
	OW_CIMtoXMLFlags::local_only_flag const& localOnly,
	OW_CIMtoXMLFlags::include_qualifiers_flag const& includeQualifiers,
	OW_CIMtoXMLFlags::include_class_origin_flag const& includeClassOrigin);
				
/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMMethod const& cm, std::ostream& ostr,
	OW_CIMtoXMLFlags::local_only_flag const& localOnly,
	OW_CIMtoXMLFlags::include_qualifiers_flag const& includeQualifiers,
	OW_CIMtoXMLFlags::include_class_origin_flag const& includeClassOrigin);
				
/////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMParameter const& cp, std::ostream& ostr);

#endif				
