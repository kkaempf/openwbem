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
#include "OW_XMLPrettyPrint.hpp"
#include "OW_XMLParser.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_Assertion.hpp"

static void addIndent(OW_StringBuffer& sb, int indent)
{
	while (indent--)
	{
		sb += "  ";
	}
}

static void outputAttrs(OW_StringBuffer& sb, const OW_XMLToken& tok)
{
	for (size_t i = 0; i < tok.attributeCount; ++i)
	{
		sb += ' ';
		sb += tok.attributes[i].name;
		sb += "=\"";
		sb += tok.attributes[i].value;
		sb += "\"";
	}
}

OW_String OW_XMLPrettyPrint(std::istream& istr)
{
	OW_XMLParser p(istr);
	OW_StringBuffer rval;
	int indent = 0;
	OW_XMLToken tok;
	bool good = p.next(tok);
	while (good)
	{
		switch (tok.type)
		{
			case OW_XMLToken::XML_DECLARATION:
				addIndent(rval, indent);
				rval += "<?xml";
				outputAttrs(rval, tok);
				rval += " ?>\n";
				break;

			case OW_XMLToken::START_TAG:
				addIndent(rval, indent);
				rval += "<";
				rval += tok.text;
				outputAttrs(rval, tok);
				rval += ">\n";
				++indent;
				break;

			case OW_XMLToken::END_TAG:
				--indent;
				OW_ASSERT(indent >= 0);
				addIndent(rval, indent);
				rval += "</";
				rval += tok.text;
				rval += ">\n";
				break;

			case OW_XMLToken::COMMENT:
				addIndent(rval, indent);
				rval += "<!--";
				rval += tok.text;
				rval += "-->\n";
				break;

			case OW_XMLToken::CDATA:
				addIndent(rval, indent);
				rval += "<![CDATA[";
				rval += tok.text;
				rval += "]]>\n";
				break;

			case OW_XMLToken::DOCTYPE:
				addIndent(rval, indent);
				rval += "<!DOCTYPE";
				rval += tok.text;
				rval += ">\n";
				break;

			case OW_XMLToken::CONTENT:
				addIndent(rval, indent);
				rval += tok.text;
				rval += "\n";
				break;

			default:
				OW_ASSERT(0);
		}
		
		good = p.next(tok);
	}

	return rval.releaseString();
}

OW_String OW_XMLPrettyPrint(const OW_String& s)
{
	OW_TempFileStream tfs;
	tfs << s;
	tfs.rewind();
	return OW_XMLPrettyPrint(tfs);
}



