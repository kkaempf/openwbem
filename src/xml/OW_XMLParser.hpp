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

//
// OW_XMLParser.hpp
//
//
//

#ifndef __OW_XMLPARSER_HPP__
#define __OW_XMLPARSER_HPP__

#include	"OW_config.h"
#include	"OW_String.hpp"
#include	"OW_Array.hpp"
#include	"OW_XMLNode.hpp"
#include "OW_XMLParserSax.hpp"
#if defined(OW_HAVE_ISTREAM)
#include <istream>
#elif defined(OW_HAVE_ISTREAM_H)
#include	<istream.h>
#endif


/**
 * OW_XMLParser
 * Class used to parse XML Documents from iostreams, buffers,
 * or files.  Initial implementation based on snia XMLParse
 * class in Java adapted to use a c++ sax parser.
 *
 * @author Calvin R. Gaisford
 * @version 1.0
 * @since 1-24-2001
 */
class OW_XMLParser /*: public HandlerBase*/
{
public:
	/**
	 * Constructs an OW_XMLParser object setup to parse "fileName"
	 *
	 * @param fileName Full path of XML file to parse
	 */
	OW_XMLParser(const OW_String& fileName);
	
	/**
	 * Constructs an OW_XMLParser object initialized to parse from the
	 * data std::istream
	 *
	 * @param data   std::istream containing XML document to be parsed
	 */
	OW_XMLParser(std::istream *data);
	
	/**
	 * Constructs an empty OW_XMLParser object
	 */
	OW_XMLParser();

	/**
	 * Destructor
	 */
	~OW_XMLParser();

	/**
	 * Parse the XML document contained in the file named fileName
	 *
	 * @param fileName The name of the file containing the XML document to parse
	 * @return OW_XMLNode which is the root of the XML document parsed
	 */
	OW_XMLNode parse(const OW_String& fileName);
	
	/**
	 * Parse the XML document to be read from the std::istream data
	 *
	 * @param data   std::istream to read the XML document to be parsed
	 * @return OW_XMLNode which is the root of the XML document parsed
	 */
	OW_XMLNode parse(std::istream *data);
	
	/**
	 * Parse the XML document from the source that was specified
	 * in the constructor of the XMLParser
	 *
	 * @return OW_XMLNode which is the root of the XML document parsed
	 */
	OW_XMLNode parse();
	
	// -----------------------------------------------------------------------
	//  Implementations of the SAX DocumentHandler interface
	// -----------------------------------------------------------------------
	void endDocument();
	void endElement(const OW_StringBuffer& name);
	void characters(const OW_StringBuffer& chars);
	void startDocument();
	void startElement(const OW_XMLToken& entry);

	// -----------------------------------------------------------------------
	//  Implementations of the SAX ErrorHandler interface
	// -----------------------------------------------------------------------
	void warning(const OW_XMLParseException& exception);
	void error(const OW_XMLParseException& exception);
	void fatalError(const OW_XMLParseException& exception);

private :
	
	OW_XMLNode			m_currentNode;
	OW_XMLNode			m_topNode;
	OW_XMLNodeArray		m_nodeArray;
	OW_XMLParserSax			m_SAXParser;
	std::istream				*m_pIStream;
	OW_String			m_fileName;
	OW_Bool				m_isFile;

	void doParse(std::istream& istr);

	OW_XMLParser(const OW_XMLParser& arg);	// Shouldn't happen
	OW_XMLParser& operator= (const OW_XMLParser& arg); // shouldn't happen
};


#endif // __OW_XMLPARSER_HPP__

