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

//////////////////////////////////////////////////////////////////////////////		
OW_String
OW_XMLClass::getNameSpace(OW_CIMXMLParser& parser)
{
	OW_String nameSpace;
	OW_Bool firstTime = true;
	while (parser.tokenIs(OW_CIMXMLParser::E_NAMESPACE))
	{
		OW_String pname = parser.mustGetAttribute(OW_CIMXMLParser::A_NAME);
		if(pname.length() == 0)
		{
			OW_THROWCIM(OW_CIMException::INVALID_PARAMETER);
		}
		if (firstTime)
		{
			firstTime=false;
			nameSpace += pname;
		}
		else
		{
			nameSpace += "/" + pname;
		}
		parser.mustGetNext();
		parser.mustGetEndTag();
	}
	return nameSpace;
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMObjectPath
OW_XMLClass::getObjectWithPath(OW_CIMXMLParser& parser, OW_CIMClass& c,
	OW_CIMInstance& i)
{
	OW_CIMXMLParser::tokenId token = parser.getToken();

	parser.mustGetChild(); // pass <VALUE.OBJECTWITHPATH> or <VALUE.OBJECTWITHLOCALPATH>
	
	if (token == OW_CIMXMLParser::E_VALUE_OBJECTWITHPATH)
	{
		token = parser.getToken();
		
		OW_CIMObjectPath tmpcop = OW_XMLCIMFactory::createObjectPath(parser);
			
		if (token == OW_CIMXMLParser::E_CLASSPATH)
		{
			parser.mustTokenIs(OW_CIMXMLParser::E_CLASS);
		    c = readClass(parser,tmpcop);
		}
		else if (token==OW_CIMXMLParser::E_INSTANCEPATH)
		{
			parser.mustTokenIs(OW_CIMXMLParser::E_INSTANCE);
		    i = readInstance(parser,tmpcop);
		}
		else
		{
		    OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("Require instance or class in object with path declaration. token = %1, parser = %2", token, parser).c_str());
		}
		
		parser.mustGetEndTag(); // pass </VALUE.OBJECTWITHPATH>
		return tmpcop;
	}
	else if (token==OW_CIMXMLParser::E_VALUE_OBJECTWITHLOCALPATH)
	{
	    token = parser.getToken();

	    OW_CIMObjectPath tmpcop = OW_XMLCIMFactory::createObjectPath(parser);
			
	    if (token == OW_CIMXMLParser::E_LOCALCLASSPATH)
		{
			parser.mustTokenIs(OW_CIMXMLParser::E_CLASS);
		    c = readClass(parser, tmpcop);
	    }
		else if (token == OW_CIMXMLParser::E_LOCALINSTANCEPATH)
		{
			parser.mustTokenIs(OW_CIMXMLParser::E_INSTANCE);
		    i = readInstance(parser, tmpcop);
	    }
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Require instance or class in object with path declaration");
		}
		
		parser.mustGetEndTag(); // pass </VALUE.OBJECTWITHLOCALPATH>
	    return tmpcop;
	}
	OW_THROWCIMMSG(OW_CIMException::FAILED,
		format("Require instance or class in object with path declaration. token = %1, parser = %2", token, parser).c_str());
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMClass
OW_XMLClass::readClass(OW_CIMXMLParser& childNode, OW_CIMObjectPath& path)
{
	OW_CIMClass cimClass = OW_XMLCIMFactory::createClass(childNode);

	path.setObjectName(cimClass.getName());

	return cimClass;
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMInstance
OW_XMLClass::readInstance(OW_CIMXMLParser& childNode, OW_CIMObjectPath& path)
{
	(void)path;
	OW_CIMInstance cimInstance = OW_XMLCIMFactory::createInstance(childNode);
	return cimInstance;
}



