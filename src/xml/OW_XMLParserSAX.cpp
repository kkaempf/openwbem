/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */


#include "OW_config.h"
#include "OW_XMLParserSAX.hpp"
#include "OW_XMLParserCore.hpp"
#include "OW_XMLParseException.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_XMLUnescape.hpp"

#ifdef OW_HAVE_ISTREAM
#include <istream>
#else
#include <iostream>
#endif

using std::istream;

namespace OW_NAMESPACE
{
namespace XMLParserSAX
{

/////////////////////////////////////////////////////////////////////////////
SAXDocumentHandler::~SAXDocumentHandler()
{
}

/////////////////////////////////////////////////////////////////////////////
SAXErrorHandler::~SAXErrorHandler()
{
}

/////////////////////////////////////////////////////////////////////////////
void parse(istream& istr, SAXDocumentHandler& docHandler, SAXErrorHandler& errHandler)
{
	XMLParserCore parser(istr);

	XMLToken entry;
	docHandler.startDocument();
	try
	{
		while (parser.next(entry))
		{
			switch (entry.type)
			{
				case XMLToken::INVALID:
					break;
				case XMLToken::XML_DECLARATION:
					break;
				case XMLToken::START_TAG:
					for (size_t i = 0; i < entry.attributeCount; ++i)
					{						  
						entry.attributes[i].value = XMLUnescape(entry.attributes[i].value.c_str(), entry.attributes[i].value.length());
					}
					docHandler.startElement(entry);
					break;
				case XMLToken::END_TAG:
					docHandler.endElement(XMLUnescape(entry.text.c_str(), entry.text.length()));
					break;
				case XMLToken::COMMENT:
					break;
				case XMLToken::CDATA:
					docHandler.characters(entry.text); // cdata isn't escaped
					break;
				case XMLToken::DOCTYPE:
					break;
				case XMLToken::CONTENT:
					docHandler.characters(XMLUnescape(entry.text.c_str(), entry.text.length()));
					break;
				default:
					break;
			}
		}
	}
	catch (XMLParseException& e)
	{
		errHandler.fatalError(e);
	}

	docHandler.endDocument();

}

/////////////////////////////////////////////////////////////////////////////
void 
parse(const String& xmlData, SAXDocumentHandler& docHandler, SAXErrorHandler& errHandler)
{
	TempFileStream tfs;
	tfs << xmlData;
	tfs.rewind();
	parse(tfs, docHandler, errHandler);
}

} // end namespace XMLParser

} // end namespace OW_NAMESPACE



