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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_XMLCIMFactory.hpp"
#include "OW_XMLClass.hpp"
#include "OW_String.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"

namespace OpenWBEM
{

namespace XMLClass
{
//////////////////////////////////////////////////////////////////////////////		
String
getNameSpace(CIMXMLParser& parser)
{
	String nameSpace;
	bool firstTime = true;
	while (parser.tokenIsId(CIMXMLParser::E_NAMESPACE))
	{
		String pname = parser.mustGetAttribute(CIMXMLParser::A_NAME);
		if (pname.empty())
		{
			//OW_THROWCIM(CIMException::INVALID_PARAMETER);
			// can't do this, because some clients send invalid xml, and
			// interoperability is more important than spec purity.
		}
		else if (firstTime)
		{
			firstTime=false;
			nameSpace += pname;
		}
		else
		{
			nameSpace += "/" + pname;
		}
		parser.mustGetNextTag();
		parser.mustGetEndTag();
	}
	return nameSpace;
}
//////////////////////////////////////////////////////////////////////////////		
CIMObjectPath
getObjectWithPath(CIMXMLParser& parser, CIMClass& c,
	CIMInstance& i)
{
	CIMXMLParser::tokenId token = parser.getToken();
	parser.mustGetChild(); // pass <VALUE.OBJECTWITHPATH> or <VALUE.OBJECTWITHLOCALPATH>
	
	if (token == CIMXMLParser::E_VALUE_OBJECTWITHPATH)
	{
		token = parser.getToken();
		
		CIMObjectPath tmpcop = XMLCIMFactory::createObjectPath(parser);
			
		if (token == CIMXMLParser::E_CLASSPATH)
		{
			parser.mustTokenIsId(CIMXMLParser::E_CLASS);
			c = readClass(parser,tmpcop);
		}
		else if (token==CIMXMLParser::E_INSTANCEPATH)
		{
			parser.mustTokenIsId(CIMXMLParser::E_INSTANCE);
			i = readInstance(parser,tmpcop);
		}
		else
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				Format("Require instance or class in object with path declaration. token = %1, parser = %2", token, parser).c_str());
		}
		
		parser.mustGetEndTag(); // pass </VALUE.OBJECTWITHPATH>
		return tmpcop;
	}
	else if (token==CIMXMLParser::E_VALUE_OBJECTWITHLOCALPATH)
	{
		token = parser.getToken();
		CIMObjectPath tmpcop = XMLCIMFactory::createObjectPath(parser);
			
		if (token == CIMXMLParser::E_LOCALCLASSPATH)
		{
			parser.mustTokenIsId(CIMXMLParser::E_CLASS);
			c = readClass(parser, tmpcop);
		}
		else if (token == CIMXMLParser::E_LOCALINSTANCEPATH)
		{
			parser.mustTokenIsId(CIMXMLParser::E_INSTANCE);
			i = readInstance(parser, tmpcop);
		}
		else
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				"Require instance or class in object with path declaration");
		}
		
		parser.mustGetEndTag(); // pass </VALUE.OBJECTWITHLOCALPATH>
		return tmpcop;
	}
	OW_THROWCIMMSG(CIMException::FAILED,
		Format("Require instance or class in object with path declaration. token = %1, parser = %2", token, parser).c_str());
}
//////////////////////////////////////////////////////////////////////////////		
CIMClass
readClass(CIMXMLParser& childNode, CIMObjectPath& path)
{
	CIMClass cimClass = XMLCIMFactory::createClass(childNode);
	path.setClassName(cimClass.getName());
	return cimClass;
}
//////////////////////////////////////////////////////////////////////////////		
CIMInstance
readInstance(CIMXMLParser& childNode, CIMObjectPath& path)
{
	CIMInstance cimInstance = XMLCIMFactory::createInstance(childNode);
	return cimInstance;
}

} // end namespace XMLClass
} // end namespace OpenWBEM

