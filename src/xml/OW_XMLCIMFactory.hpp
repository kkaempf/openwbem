/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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

#ifndef OW_XMLCIMFACTORY_HPP_
#define OW_XMLCIMFACTORY_HPP_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_XMLFwd.hpp"

namespace OW_NAMESPACE
{

namespace XMLCIMFactory
{
	OW_XML_API CIMObjectPath createObjectPath(CIMXMLParser& parser);
	OW_XML_API CIMClass createClass(CIMXMLParser& parser);
	OW_XML_API CIMInstance createInstance(CIMXMLParser& parser);
	enum EEmbeddedObjectFlag
	{
		E_VALUE_IS_EMBEDDED_OBJECT,
		E_VALUE_IS_EMBEDDED_INSTANCE,
		E_VALUE_NOT_EMBEDDED_OBJECT
	};
	/**
	 * parser must be positioned on a node that has the EmbeddedObject attribute
	 */
	OW_XML_API EEmbeddedObjectFlag getEmbeddedObjectType(const CIMXMLParser& parser);
	OW_XML_API CIMValue createValue(CIMXMLParser& parser, blocxx::String const& type, EEmbeddedObjectFlag embeddedObjectFlag);
	OW_XML_API CIMQualifier createQualifier(CIMXMLParser& parser);
	OW_XML_API CIMMethod createMethod(CIMXMLParser& parser);
	OW_XML_API CIMProperty createProperty(CIMXMLParser& parser);
	OW_XML_API CIMParameter createParameter(CIMXMLParser& parser);
};

} // end namespace OW_NAMESPACE

namespace OW_XMLCIMFactory = OW_NAMESPACE::XMLCIMFactory;

#endif
