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

#ifndef OW_XMLPARSERSAX_HPP_INCLUDE_GUARD_
#define OW_XMLPARSERSAX_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include <iosfwd>


namespace OpenWBEM
{

class String;
class StringBuffer;
class XMLToken;
class XMLParseException;

namespace XMLParserSAX 
{

// -----------------------------------------------------------------------
//  SAXDocumentHandler abstract interface
// -----------------------------------------------------------------------
class SAXDocumentHandler
{
public:
	virtual ~SAXDocumentHandler();

	virtual void endDocument() = 0;
	// name is not unescaped (implementation has to do it if necessary)
	virtual void endElement(const StringBuffer& name) = 0;
	// chars is not unescaped (implementation has to do it if necessary)
	virtual void characters(const StringBuffer& chars) = 0;
	virtual void startDocument() = 0;
	virtual void startElement(const XMLToken& entry) = 0;
};

// -----------------------------------------------------------------------
//  SAXErrorHandler abstract interface
// -----------------------------------------------------------------------
class SAXErrorHandler
{
public:
	virtual ~SAXErrorHandler();

	virtual void warning(const XMLParseException& exception) = 0;
	virtual void error(const XMLParseException& exception) = 0;
	virtual void fatalError(const XMLParseException& exception) = 0;
};


/**
 * Parse the XML document contained in the file named fileName
 *
 * @param fileName The name of the file containing the XML document to parse
 */
void parse(const String& xmlData, SAXDocumentHandler& docHandler, SAXErrorHandler& errHandler);

/**
 * Parse the XML document to be read from the std::istream data
 *
 * @param data   std::istream to read the XML document to be parsed
 */
void parse(std::istream& data, SAXDocumentHandler& docHandler, SAXErrorHandler& errHandler);
	

} // end namespace XMLParserSAX
} // end namespace OpenWBEM


#endif

