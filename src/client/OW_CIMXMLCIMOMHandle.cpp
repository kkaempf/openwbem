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
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_HTTPChunkedOStream.hpp"
#include "OW_XMLOperationGeneric.hpp"
#include "OW_XMLParameters.hpp"
#include "OW_XMLParser.hpp"
#include "OW_Format.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_XMLEscape.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_XMLException.hpp"
#include "OW_XMLCIMFactory.hpp"
#include "OW_HTTPChunkedIStream.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_HTTPDeflateIStream.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_CIMUrl.hpp"

#include <iostream>

using std::ostream;
using std::iostream;
using std::istream;


//////////////////////////////////////////////////////////////////////////////
OW_CIMXMLCIMOMHandle::OW_CIMXMLCIMOMHandle(OW_CIMProtocolIFCRef prot)
: OW_CIMOMHandleIFC(), m_protocol(prot)
{
	m_iMessageID = 0;
	m_protocol->setContentType("application/xml");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::sendXMLHeader( const OW_String &sMethod,
												 const OW_CIMObjectPath& path,
												 ostream& ostr, bool intrinsic)
{
	if (++m_iMessageID > 65535)
	{
		m_iMessageID = 1;
	}
	ostr << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
	ostr << "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">";
	ostr << "<MESSAGE ID=\"" << m_iMessageID << "\" PROTOCOLVERSION=\"1.0\">";
	ostr << "<SIMPLEREQ>";
	if (intrinsic)
	{
		ostr << "<IMETHODCALL NAME=\"" << sMethod << "\">";
		OW_CIMtoXML(path.getFullNameSpace(), ostr, OW_CIMtoXMLFlags::doLocal );
		//path.getFullNameSpace().toXML( ostr, true );
	}
	else
	{
		ostr << "<METHODCALL NAME=\"" << sMethod << "\">";
		if (path.getKeys().size() > 0) // it's an instance
		{
			ostr << "<LOCALINSTANCEPATH>";
			//path.getFullNameSpace().toXML(ostr, true);
			//path.toXML(ostr);
			OW_CIMtoXML(path.getFullNameSpace(), ostr, OW_CIMtoXMLFlags::doLocal);
			OW_CIMtoXML(path, ostr, OW_CIMtoXMLFlags::isNotInstanceName);
			ostr << "</LOCALINSTANCEPATH>";
		}
		else // it's a class
		{
			ostr << "<LOCALCLASSPATH>";
			//path.getFullNameSpace().toXML(ostr, true);
			OW_CIMtoXML(path.getFullNameSpace(), ostr, OW_CIMtoXMLFlags::doLocal);
			ostr << "<CLASSNAME NAME=\"" << path.getObjectName() << "\"/>";
			ostr << "</LOCALCLASSPATH>";
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::sendXMLTrailer( ostream& ostr, bool intrinsic)
{
	if (intrinsic)
	{
		ostr << "</IMETHODCALL>";
	}
	else
	{
		ostr << "</METHODCALL>";
	}
	ostr << "</SIMPLEREQ>";
	ostr << "</MESSAGE>";
	ostr << "</CIM>";
	ostr << "\r\n";
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_CIMXMLCIMOMHandle::doSendRequest(OW_Reference<std::iostream> ostrRef, const OW_String& methodName,
												const OW_CIMObjectPath& path)
{
	istream& istr = m_protocol->endRequest(ostrRef, methodName,
											 path.getNameSpace());
	// Debug stuff
	/*
	OW_TempFileStream buf;
	buf << istr.rdbuf();
	ofstream ofstr("/tmp/rchXMLDump", ios::app);
	ofstr << "******* New dump ********" << endl;
	ofstr << buf.rdbuf() << endl;
	buf.rewind();
	OW_XMLParser parser(&buf);
	*/
	// end debug stuff
	OW_XMLParser parser(&istr);

	OW_XMLNode retval;
	try
	{
		retval = parser.parse();
	}
	catch (OW_XMLException& xmlE)
	{
		OW_HTTPUtils::eatEntity(istr);
		OW_HTTPChunkedIStream* chunkIstr = NULL;
#ifdef OW_HAVE_ZLIB_H
		OW_HTTPDeflateIStream* deflateistr = dynamic_cast<OW_HTTPDeflateIStream*>(&istr);
		if (deflateistr)
		{
			chunkIstr = dynamic_cast<OW_HTTPChunkedIStream*>
				(&deflateistr->getInputStreamOrig());
		}
		else
#endif
		{
			chunkIstr = dynamic_cast<OW_HTTPChunkedIStream*>(&istr);
		}
		if (chunkIstr)
		{
			OW_Map<OW_String, OW_String> trailers = chunkIstr->getTrailers();
			if (trailers.size() == 1)
			{
				// The trailer is escaped, so first unescape it
				OW_TempFileStream error(500);
				OW_String trailer = trailers.begin()->second;
				for (size_t i = 0; i < trailer.length(); ++i)
				{
					switch (trailer[i])
					{
						case '\\':
							if (i + 1 == trailer.length())
							{
								// last char was '\\'
								OW_THROWCIMMSG(OW_CIMException::FAILED, "Invalid Trailer");
							}
							switch (trailer[i + 1])
							{
								case '0':
									error << '\0';
									break;
								case 'n':
									error << '\n';
									break;
								case 'r':
									error << '\r';
									break;
								case '\\':
									error << '\\';
									break;
								default:
									OW_THROWCIMMSG(OW_CIMException::FAILED, "Invalid escape in trailer");
							}
							break;

						default:
							error << trailer[i];
							break;
					}
				}

				//error << trailers.begin()->second;
				OW_XMLParser errorParser(&error);
				OW_XMLNode errNode = errorParser.parse();
			  	return checkNodeForCIMError(errNode, methodName);
				// TODO this needs to be more robust.
			}
		}
	}
	return checkNodeForCIMError(retval, methodName);
}

OW_XMLNode
OW_CIMXMLCIMOMHandle::checkNodeForCIMError(OW_XMLNode reply,
	const OW_String& operation)
{
	//
	// Find <CIM> element
	//
	if (!reply)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Invalid XML");
	}
	reply = reply.mustFindElement(OW_XMLNode::XML_ELEMENT_CIM);
	OW_String cimattr = reply.mustGetAttribute(OW_XMLOperationGeneric::CIMVERSION);
	if (!cimattr.equals(OW_XMLOperationGeneric::CIMVERSION_VALUE))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return is for CIMVERSION " + cimattr).c_str());
	}

	cimattr = reply.mustGetAttribute(OW_XMLOperationGeneric::DTDVERSION);
	if (!cimattr.equals(OW_XMLOperationGeneric::DTDVERSION_VALUE))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return is for DTDVERSION " + cimattr).c_str());
	}

	//
	// Find <MESSAGE>
	//
	reply = reply.mustChildFindElement(OW_XMLNode::XML_ELEMENT_MESSAGE);
	cimattr=reply.mustGetAttribute(OW_XMLOperationGeneric::MSG_ID);
	if (!cimattr.equals(OW_String(m_iMessageID)))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return messageid="+cimattr+", expected="
										 +OW_String(m_iMessageID)).c_str());
	}

	cimattr = reply.mustGetAttribute(OW_XMLOperationGeneric::PROTOCOLVERSION);
	if (!cimattr.equals(OW_XMLOperationGeneric::PROTOCOLVERSION_VALUE))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Return is for PROTOCOLVERSION "+cimattr).c_str());
	}

	reply = reply.getChild();
	if (!reply)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"No XML_ELEMENT_IRETURNVALUE (22)");
	}

	//
	// Find <SIMPLERSP>
	//
	reply = reply.findElementChild(OW_XMLNode::XML_ELEMENT_SIMPLERSP);
	//
	// TODO-NICE: need to look for complex RSPs!!
	//
	if (!reply)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"No XML_ELEMENT_SIMPLERSP");
	}

	//
	// <METHODRESPONSE> or <IMETHODRESPONSE>
	//
	OW_Bool isIntrinsic = false;
	OW_XMLNode tempReply =
	reply.findElement(OW_XMLNode::XML_ELEMENT_METHODRESPONSE);
	if (!tempReply)
	{
		isIntrinsic = true;
		reply = reply.mustFindElement(OW_XMLNode::XML_ELEMENT_IMETHODRESPONSE);
	}

	OW_String nameOfMethod = reply.getAttribute("NAME");
	if (nameOfMethod.length() < 1)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"Response had no method name");
	}

	if (!nameOfMethod.equalsIgnoreCase(operation))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							OW_String("Called "+operation+" but response was for "+
										 nameOfMethod).c_str());
	}

	reply = reply.getChild();
	if (!reply)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"No XML_ELEMENT_IRETURNVALUE");
	}

	//
	// See if there was an error, and if there was throw an equivalent
	// exception on the client
	//
	OW_XMLNode errorNode = reply.findElement(OW_XMLNode::XML_ELEMENT_ERROR);
	if (errorNode)
	{
		OW_CIMException ce = OW_CIMException();
		OW_String errCode=errorNode.mustGetAttribute(
			OW_XMLParameters::paramErrorCode);
		OW_String description=errorNode.mustGetAttribute(
			OW_XMLParameters::paramErrorDescription);
		OW_THROWCIMMSG(errCode.toInt32(), description.c_str());
	}

	return reply;
}


//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::createNameSpace(const OW_CIMNameSpace& ns)
{
	OW_String nameSpace = ns.getNameSpace();
	int index = nameSpace.lastIndexOf('/');

	if (index==-1)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
							"A Namespace must only be created in an existing Namespace");
	}

	OW_String parentPath = nameSpace.substring(0,index);

	OW_String newNameSpace = nameSpace.substring(index + 1);

	OW_CIMObjectPath path("__Namespace", parentPath);

	OW_CIMClass cimClass = getClass(path,false);
	if (!cimClass)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
							"Could not find internal class __Namespace");
	}
	OW_CIMInstance cimInstance = cimClass.newInstance();
	OW_CIMValue cv(newNameSpace);
	cimInstance.setProperty("Name",cv);

	OW_CIMObjectPath cimPath("__Namespace", cimInstance.getKeyValuePairs());
	cimPath.setNameSpace(parentPath);

	createInstance(cimPath,cimInstance) ;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::deleteNameSpace(const OW_CIMNameSpace& ns)
{
	OW_String parentPath;
	OW_String nameSpace = ns.getNameSpace();
	int index = nameSpace.lastIndexOf('/');

	if (index==-1)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
							"A Namespace must only be created in an existing Namespace");
	}

	parentPath = nameSpace.substring(0,index);
	OW_String newNameSpace = nameSpace.substring(index + 1);


	OW_CIMPropertyArray v;
	OW_CIMValue cv(newNameSpace);
	OW_CIMProperty cp("Name", cv);
	cp.setDataType(OW_CIMDataType(OW_CIMDataType::STRING));
	v.push_back(cp);

	OW_CIMObjectPath path("__Namespace",v);
	path.setNameSpace(parentPath);
	deleteInstance(path);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::enumNameSpaceAux(const OW_CIMObjectPath& path,
	OW_StringArray& rval, OW_Bool deep)
{
	OW_CIMInstanceEnumeration e = enumInstances(path, deep);

	while (e.hasMoreElements())
	{
		OW_CIMProperty nameProp;

		OW_CIMInstance ci = e.nextElement();
		OW_CIMPropertyArray keys = ci.getKeyValuePairs();
		if (keys.size()==1)
		{
			nameProp = keys[0];
		}
		else
		{
			for (size_t i=0; i < keys.size(); i++)
			{
				if (keys[i].getName().equalsIgnoreCase("Name"))
				{
					nameProp = keys[i];
					break;
				}
			}
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Server error: No keys found in namespace instance");
		}
		OW_String tmp;
		nameProp.getValue().get(tmp);
		rval.push_back(path.getNameSpace() + "/" + tmp);
		if (deep)
		{
			OW_CIMObjectPath newObjPath("__Namespace",
												 path.getNameSpace()+ "/" + tmp);
			enumNameSpaceAux(newObjPath, rval, deep);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_CIMXMLCIMOMHandle::enumNameSpace(const OW_CIMNameSpace& path,
												OW_Bool deep)
{
	OW_CIMObjectPath cop("__Namespace", path.getNameSpace());
	OW_StringArray rval;
	rval.push_back(path.getNameSpace());
	enumNameSpaceAux(cop, rval, deep);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::deleteClass(const OW_CIMObjectPath& path)
{

	static const char* const commandName = "DeleteClass";

	OW_String nameSpace=path.getNameSpace();
	OW_String className=path.getObjectName();
	OW_Array<OW_Param> params;

	if (className.length() > 0)
	{
		params.push_back(OW_Param(XMLP_CLASSNAME,OW_Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS, "No ClassName passed"
							" to deleteClass()");
	}

	intrinsicMethod(path, commandName, params);
}


//////////////////////////////////////////////////////////////////////////////
static OW_String
instanceNameToKey(const OW_CIMObjectPath& path,
	const OW_String& parameterName)
{
	OW_String text;
	
	if(parameterName.length())
	{
		text = "<IPARAMVALUE NAME=\"" + parameterName + "\">";
	}
	
	try
	{
		OW_StringStream ss;
		OW_CIMtoXML(path, ss, OW_CIMtoXMLFlags::isNotInstanceName);
		text += ss.toString();
		//text += path.convertToXML();
	}
	catch(OW_CIMMalformedUrlException& me)
	{
	    OW_THROWCIMMSG(OW_CIMException::FAILED, me.getMessage());
	}

	if(parameterName.length())
	{
	    text += "</IPARAMVALUE>";
	}
	return(text);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::deleteInstance(const OW_CIMObjectPath& path)
{
	static const char* const commandName = "DeleteInstance";
	OW_Array<OW_Param> params;

	intrinsicMethod(path, commandName, params,
						 instanceNameToKey(path, "InstanceName"));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::deleteQualifierType(const OW_CIMObjectPath& path)
{
	static const char* const commandName = "DeleteQualifier";
	OW_Array<OW_Param> params;

	OW_String qualName = path.getObjectName();

	params.push_back(OW_Param(XMLP_QUALIFIERNAME, qualName));

	intrinsicMethod(path, commandName, params);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMXMLCIMOMHandle::enumClassNames(const OW_CIMObjectPath& path,
		OW_Bool deep)
{
	static const char* const commandName = "EnumerateClassNames";

	OW_String className = path.getObjectName();
	OW_Array<OW_Param> params;
	if (className.length() > 0)
	{
		params.push_back(OW_Param(XMLP_CLASSNAME,OW_Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	params.push_back(OW_Param(XMLP_DEEP,deep));


	OW_XMLNode node = intrinsicMethod(path, commandName, params);
	if (!node)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}

	node = node.getChild();
	OW_CIMObjectPathEnumeration retval;

	while (node)
	{
		//OW_CIMObjectPath cop(node);
		OW_CIMObjectPath cop = OW_XMLCIMFactory::createObjectPath(node);
		retval.addElement(cop);
		node = node.getNext();
	}

	return retval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_CIMXMLCIMOMHandle::enumClass(const OW_CIMObjectPath& path, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
	static const char* const commandName = "EnumerateClasses";

	OW_Array<OW_Param> params;
	OW_String className = path.getObjectName();

	if (className.length() > 0)
	{
		params.push_back(OW_Param(XMLP_CLASSNAME,OW_Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	params.push_back(OW_Param(XMLP_LOCAL,localOnly));
	params.push_back(OW_Param(XMLP_DEEP,deep));
	params.push_back(OW_Param(XMLP_QUAL, includeQualifiers));
	params.push_back(OW_Param(XMLP_ORIGIN, includeClassOrigin));

	OW_XMLNode node = intrinsicMethod(path, commandName, params);
	if (!node)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	node = node.getChild();
	OW_CIMClassEnumeration retval;
	while (node)
	{
		//OW_CIMClass cls(node);
		OW_CIMClass cls = OW_XMLCIMFactory::createClass(node);
		retval.addElement(cls);
		node = node.getNext();
	}
	return retval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMXMLCIMOMHandle::enumInstanceNames(const OW_CIMObjectPath& path,
	OW_Bool deep)
{
	static const char* const commandName = "EnumerateInstanceNames";

	OW_String className = path.getObjectName();
	OW_Array<OW_Param> params;

	if (className.length() > 0)
	{
		params.push_back(OW_Param(XMLP_CLASSNAME, OW_Param::VALUESET,
										  "<CLASSNAME NAME=\""+className+"\"/>"));
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, "Class was empty in "
							"EnumerateInstanceNames");
	}

	OW_XMLNode node = intrinsicMethod(path, commandName, params);
	if (!node)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	OW_CIMObjectPathEnumeration retVal;
	node = node.getChild();
	while (node)
	{
		OW_CIMObjectPath cop = OW_XMLCIMFactory::createObjectPath(node);
		if(!deep)
		{
			if(cop.getObjectName().equalsIgnoreCase(className))
			{
				retVal.addElement(cop);
			}
		}
		else
		{
			retVal.addElement(cop);
		}

		node = node.getNext();
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_CIMXMLCIMOMHandle::enumInstances(const OW_CIMObjectPath& path, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	static const char* const commandName = "EnumerateInstances";
	OW_StringStream extra(1000);
	OW_String className = path.getObjectName();
	OW_Array<OW_Param> params;

	if(className.length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, "Class was empty in "
			"EnumerateInstances");
	}

	params.push_back(OW_Param(XMLP_CLASSNAME, OW_Param::VALUESET,
		"<CLASSNAME NAME=\""+className+"\"/>"));

	params.push_back(OW_Param(XMLP_DEEP, deep));
	params.push_back(OW_Param(XMLP_LOCAL, localOnly));
	params.push_back(OW_Param(XMLP_QUAL, includeQualifiers));
	params.push_back(OW_Param(XMLP_ORIGIN, includeClassOrigin));

	if(propertyList)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_PROPERTYLIST <<
		"\"><VALUE.ARRAY>";
		for (size_t i = 0;i < propertyList->size(); i++)
		{
			extra << "<VALUE>" << (*propertyList)[i] << "</VALUE>";
		}
		extra << "</VALUE.ARRAY></IPARAMVALUE>";
	}

	OW_XMLNode node = intrinsicMethod(path, commandName, params,
		extra.toString());

	if (!node)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}

	OW_CIMInstanceEnumeration retVal;
	node = node.getChild();

	while (node)
	{
		OW_CIMObjectPath iop(OW_XMLCIMFactory::createObjectPath(node));
		node = node.getNext();

		if (node)
		{
			OW_CIMInstance ci = OW_XMLCIMFactory::createInstance(node);
			ci.setKeys(iop.getKeys());
			retVal.addElement(ci);
			node = node.getNext();
		}
	}

	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierTypeEnumeration
OW_CIMXMLCIMOMHandle::enumQualifierTypes(const OW_CIMObjectPath& path)
{
	static const char* const commandName = "EnumerateQualifiers";
	OW_String qualName = path.getObjectName();

	OW_XMLNode node = intrinsicMethod(path, commandName);
	OW_CIMQualifierTypeEnumeration retVal;

	node = node.getChild();
	while (node)
	{
		if (node.getToken() != OW_XMLNode::XML_ELEMENT_QUALIFIER_DECLARATION)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Invalid XML");
		}
		OW_CIMQualifierType cqt(OW_Bool(true));
		node = processQualifierDecl(node, cqt);
		retVal.addElement(cqt);
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMXMLCIMOMHandle::getClass(const OW_CIMObjectPath& path, OW_Bool localOnly,
    OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	static const char* const commandName = "GetClass";
	OW_Array<OW_Param> params;
	OW_String className = path.getObjectName();

	if (className.length() > 0)
	{
		params.push_back(OW_Param(XMLP_CLASSNAME,OW_Param::VALUESET,
			"<CLASSNAME NAME=\""+className+"\"/>"));
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS, "no class given for "
			"GetClass()");
	}

	params.push_back(OW_Param(XMLP_LOCAL, localOnly));
	params.push_back(OW_Param(XMLP_QUAL, includeQualifiers));
	params.push_back(OW_Param(XMLP_ORIGIN, includeClassOrigin));

	OW_String extraStr;
	if(propertyList)
	{
		OW_StringStream extra(1000);
		extra << "<IPARAMVALUE NAME=\"" << XMLP_PROPERTYLIST <<
		"\"><VALUE.ARRAY>";
		for (size_t i = 0;i < propertyList->size(); i++)
		{
			extra << "<VALUE>" << (*propertyList)[i] << "</VALUE>";
		}
		extra << "</VALUE.ARRAY></IPARAMVALUE>";

		extraStr = extra.toString();
	}

	OW_XMLNode node = intrinsicMethod(path, commandName, params, extraStr);

	if (!node)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	node = node.getChild();
	node = node.findElement(OW_XMLNode::XML_ELEMENT_CLASS);
	if (!node)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, format("Class %1 not found",
			className).c_str());
	}
	return OW_XMLCIMFactory::createClass(node);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMXMLCIMOMHandle::getInstance(const OW_CIMObjectPath& path,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	static const char* const commandName = "GetInstance";
	OW_StringStream extra(1000);
	OW_Array<OW_Param> params;

	params.push_back(OW_Param(XMLP_LOCAL, localOnly));
	params.push_back(OW_Param(XMLP_QUAL, includeQualifiers));
	params.push_back(OW_Param(XMLP_ORIGIN, includeClassOrigin));

	extra << instanceNameToKey(path, "InstanceName");

	if (propertyList)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_PROPERTYLIST <<
		"\"><VALUE.ARRAY>";
		for (size_t i = 0;i < propertyList->size(); i++)
		{
			extra << "<VALUE>" << (*propertyList)[i] << "</VALUE>";
		}
		extra << "</VALUE.ARRAY></IPARAMVALUE>";
	}

	OW_XMLNode node = intrinsicMethod(path, commandName, params,
		extra.toString());

	if (!node)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}

	node = node.getChild();
	node = node.findElement(OW_XMLNode::XML_ELEMENT_INSTANCE);
	return OW_XMLCIMFactory::createInstance(node);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMXMLCIMOMHandle::invokeMethod(const OW_CIMObjectPath& name,
											  const OW_String& methodName,
											  const OW_CIMValueArray& inParams,
											  OW_CIMValueArray& outParams)
{
	OW_Reference<std::iostream> iostrRef =
		m_protocol->beginRequest(methodName, name.getNameSpace());
	std::iostream& tfs = *iostrRef;

	OW_CIMClass cc = getClass(name);
	OW_CIMMethod cm = cc.getMethod(methodName);
	OW_CIMParameterArray paramList = cm.getParameters();

	sendXMLHeader(methodName, name, tfs, false);

	size_t paramIdx = 0;

	for (size_t i = 0; i < inParams.size(); ++i)
	{
		for (; paramIdx < paramList.size(); ++paramIdx)
		{
			if (paramList[paramIdx].getQualifier("IN"))
			{
				break;
			}
		}
		if (paramIdx >= paramList.size() ||
			 paramList[paramIdx].getQualifier("OUT"))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
					 "Too many parameters (%1) for method %2", inParams.size(),
					  methodName).c_str());
		}
		if (inParams[i].getType() != paramList[paramIdx].getType().getType())
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
						"Parameter type mismatch for parameter number %1 of "
						"method %2", i + 1, methodName).c_str());
		}
		tfs << "<PARAMVALUE NAME=\"" << paramList[paramIdx].getName() << "\"><VALUE>"
		<< inParams[i] << "</VALUE></PARAMVALUE>";
	}


	sendXMLTrailer(tfs, false);

	OW_XMLNode node = doSendRequest(iostrRef, methodName, name);

	node = node.mustGetChild();

	OW_CIMValue rval;
	if (node.getToken() == OW_XMLNode::XML_ELEMENT_VALUE)
	{
		rval = OW_XMLCIMFactory::createValue(node, cm.getReturnType().toString());
		node = node.getNext();
	}

	for (size_t outParamCount = 0;
		  node && node.getToken() == OW_XMLNode::XML_ELEMENT_PARAMVALUE;
		  node = node.getNext(), ++outParamCount)
	{
		outParams.push_back(OW_XMLCIMFactory::createValue(node,
			OW_CIMDataType(outParams[outParamCount].getType()).toString()));
	}
	if (node)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Invalid OutParam "
			"returned from invokeMethod");
	}

	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_CIMXMLCIMOMHandle::getQualifierType(const OW_CIMObjectPath& path)
{
	static const char* const commandName = "GetQualifier";

	OW_String qualName = path.getObjectName();
	OW_Array<OW_Param> params;

	params.push_back(OW_Param(XMLP_QUALIFIERNAME, qualName));


	OW_XMLNode node = intrinsicMethod(path, commandName, params);
	if (!node)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	node=node.mustChildElement(OW_XMLNode::XML_ELEMENT_QUALIFIER_DECLARATION);

	OW_CIMQualifierType cqt(OW_Bool(true));
	processQualifierDecl(node, cqt);
	return cqt;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::setQualifierType(const OW_CIMObjectPath& path,
													const OW_CIMQualifierType& qt)
{
	static const char* const commandName = "SetQualifier";
	OW_StringStream extra;
	extra << "<IPARAMVALUE NAME=\"QualifierDeclaration\">";
	//qt.toXML(extra);
	OW_CIMtoXML(qt, extra);
	extra << "</IPARAMVALUE>";

	intrinsicMethod(path, commandName, OW_Array<OW_Param>(),
						 extra.toString());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::setClass(const OW_CIMObjectPath& path,
										 const OW_CIMClass& cc)
{
	static const char* const commandName = "ModifyClass";
	OW_String className = path.getObjectName();

	OW_StringStream extra(1024);
	extra << "<IPARAMVALUE NAME=\"" << XMLP_MODIFIED_CLASS << "\">";
	OW_CIMtoXML(cc, extra ,OW_CIMtoXMLFlags::notLocalOnly,
		OW_CIMtoXMLFlags::includeQualifiers, OW_CIMtoXMLFlags::includeClassOrigin,
		OW_StringArray(), false);
	//cc.toXML(extra, true);
	extra << "</IPARAMVALUE>";
	intrinsicMethod(path, commandName, OW_Array<OW_Param>(), extra.toString());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::createClass(const OW_CIMObjectPath& path,
											 const OW_CIMClass& cc)
{
	static const char* const commandName = "CreateClass";

	OW_StringStream ostr;
	ostr << "<IPARAMVALUE NAME=\"NewClass\">";
	OW_CIMtoXML(cc, ostr, OW_CIMtoXMLFlags::notLocalOnly,
		OW_CIMtoXMLFlags::includeQualifiers , OW_CIMtoXMLFlags::includeClassOrigin,
		OW_StringArray(), false);
	//cc.toXML(ostr);
	ostr << "</IPARAMVALUE>";

	intrinsicMethod(path, commandName, OW_Array<OW_Param>(), ostr.toString());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::setInstance(const OW_CIMObjectPath& path,
											 const OW_CIMInstance& ci)
{
	static const char* const commandName = "ModifyInstance";
	OW_String nameSpace = path.getNameSpace();
	OW_String className = path.getObjectName();

	OW_StringStream ostr(1000);
	ostr << "<IPARAMVALUE NAME=\"ModifiedInstance\">";
	ostr << "<VALUE.NAMEDINSTANCE>";
	OW_CIMtoXML(path, ostr, OW_CIMtoXMLFlags::isNotInstanceName);
	//path.toXML(ostr);
	OW_CIMtoXML(ci, ostr, OW_CIMObjectPath(),
		OW_CIMtoXMLFlags::notLocalOnly,
		OW_CIMtoXMLFlags::includeQualifiers,
		OW_CIMtoXMLFlags::includeClassOrigin,
		OW_StringArray());
	//ci.toXML(ostr, OW_CIMObjectPath());
	ostr << "</VALUE.NAMEDINSTANCE></IPARAMVALUE>";
	intrinsicMethod(path, commandName, OW_Array<OW_Param>(), ostr.toString());
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_CIMXMLCIMOMHandle::createInstance(const OW_CIMObjectPath& path,
	const OW_CIMInstance& ci)
{
	static const char* const commandName = "CreateInstance";
	OW_String pathName = path.getObjectName();

	//
	// Check that the path has specified at least one key (can't have
	// an instance without such keys)
	//
	OW_CIMPropertyArray validKeys = path.getKeys();
	if (validKeys.size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format("The "
						   "path (%1) for the instance does not specify any keys",
							 pathName).c_str());
	}

	OW_StringStream ostr;
	ostr << "<IPARAMVALUE NAME=\"NewInstance\">";
	OW_CIMtoXML(ci, ostr, OW_CIMObjectPath(),
		OW_CIMtoXMLFlags::notLocalOnly,
		OW_CIMtoXMLFlags::includeQualifiers,
		OW_CIMtoXMLFlags::includeClassOrigin,
		OW_StringArray());
	//ci.toXML(ostr);

	ostr << "</IPARAMVALUE>";

	OW_XMLNode node = intrinsicMethod(path, commandName, OW_Array<OW_Param>(),
												 ostr.toString());
	if (!node)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}

	node = node.getChild();

	if (node)
	{
		OW_CIMObjectPath cimPath = OW_XMLCIMFactory::createObjectPath(node);
		//
		// We have to set the namespace on the basis of the path that
		// was originally passed since it didn't come back over
		// the wire.
		//
		cimPath.setNameSpace(path.getNameSpace());
		return(cimPath);
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Failed to create instance");
	}
	return OW_CIMObjectPath();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMXMLCIMOMHandle::getProperty(const OW_CIMObjectPath& path,
											 const OW_String& propName)
{
	static const char* const commandName = "GetProperty";
	OW_Array<OW_Param> params;

	params.push_back(OW_Param(XMLP_PROPERTYNAME, propName));


	OW_XMLNode node = intrinsicMethod(path, commandName, params,
												 instanceNameToKey(path,"InstanceName"));
	OW_CIMValue rval;
	if (!node)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}

	node = node.getChild();
	if (!node)
	{
		return rval;
	}
	rval = OW_XMLCIMFactory::createValue(node, "string");
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLCIMOMHandle::setProperty(const OW_CIMObjectPath& path,
											 const OW_String& propName,
											 const OW_CIMValue& cv)
{
	static const char* const commandName = "SetProperty";
	OW_Array<OW_Param> params;
	params.push_back(OW_Param(XMLP_PROPERTYNAME, propName));
	OW_StringStream ostr;
	OW_CIMtoXML(cv, ostr);
	//cv.toXML(ostr);
	params.push_back(OW_Param(XMLP_NewValue,OW_Param::VALUESET, ostr.toString()));

	intrinsicMethod(path, commandName, params,
												 instanceNameToKey(path,"InstanceName"));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMXMLCIMOMHandle::associatorNames(const OW_CIMObjectPath& path,
				const OW_String& assocClass, const OW_String& resultClass,
				const OW_String& role, const OW_String& resultRole)
{
	static const char* const commandName = "AssociatorNames";

	OW_Array<OW_Param> params;
	OW_StringStream extra(1000);
	if (role.length() > 0)
	{
		params.push_back(OW_Param(XMLP_ROLE, role));
	}
	if (resultRole.length() > 0)
	{
		params.push_back(OW_Param(XMLP_RESULTROLE, resultRole));
	}

	if (path.getKeys().size() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">";
		OW_CIMtoXML(path, extra, OW_CIMtoXMLFlags::isNotInstanceName);
		//path.toXML(extra);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">" <<
		"<CLASSNAME NAME=\"" << path.getObjectName() <<
		"\"/></IPARAMVALUE>";
	}

	if (assocClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_ASSOCCLASS << "\">" <<
		"<CLASSNAME NAME=\"" << assocClass <<
		"\"/></IPARAMVALUE>";
	}

	if (resultClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_RESULTCLASS << "\">" <<
		"<CLASSNAME NAME=\"" << resultClass <<
		"\"/></IPARAMVALUE>";
	}

	OW_XMLNode node = intrinsicMethod(path, commandName, params,
												 extra.toString());

	OW_CIMObjectPathEnumeration retVal;
	OW_XMLNode tmp;
	node = node.getChild();

	for (; node; node = node.getNext())
	{
		int token = node.getToken();
		OW_CIMObjectPath cop = OW_XMLCIMFactory::createObjectPath(node);
		switch (token)
		{
			case OW_XMLNode::XML_ELEMENT_CLASSNAME:
				cop.setNameSpace(path.getNameSpace());
				break;
			case OW_XMLNode::XML_ELEMENT_INSTANCENAME:
				cop.setNameSpace(path.getNameSpace());
				break;
			default:
				break;
		} // switch
		retVal.addElement(cop);
	}

	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_CIMXMLCIMOMHandle::associators(const OW_CIMObjectPath& path,
		 	 			 const OW_String& assocClass, const OW_String& resultClass,
		 	 			 const OW_String& role, const OW_String& resultRole,
		 	 			 OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		 				 const OW_StringArray* propertyList)
{

	static const char* const commandName = "Associators";

	OW_Array<OW_Param> params;
	OW_StringStream extra(1000);

	if (role.length() > 0)
	{
		params.push_back(OW_Param(XMLP_ROLE, role));
	}
	if (resultRole.length() > 0)
	{
		params.push_back(OW_Param(XMLP_RESULTROLE, resultRole));
	}
	params.push_back(OW_Param(XMLP_QUAL, includeQualifiers));
	params.push_back(OW_Param(XMLP_ORIGIN, includeClassOrigin));

	if (propertyList)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_PROPERTYLIST <<
		"\"><VALUE.ARRAY>";
		for (size_t i = 0;i < propertyList->size(); i++)
		{
			extra << "<VALUE>" << (*propertyList)[i] << "</VALUE>";
		}
		extra << "</VALUE.ARRAY></IPARAMVALUE>";
	}

	if (path.getKeys().size() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">";
		OW_CIMtoXML(path, extra, OW_CIMtoXMLFlags::isNotInstanceName);
		//path.toXML(extra);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">" <<
		"<CLASSNAME NAME=\"" << path.getObjectName() <<
		"\"/></IPARAMVALUE>";
	}

	if (assocClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_ASSOCCLASS << "\">" <<
		"<CLASSNAME NAME=\"" << assocClass <<
		"\"/></IPARAMVALUE>";
	}

	if (resultClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_RESULTCLASS << "\">" <<
		"<CLASSNAME NAME=\"" << resultClass <<
		"\"/></IPARAMVALUE>";
	}

	OW_XMLNode node = intrinsicMethod(path, commandName, params,
												 extra.toString());

	node = node.getChild();
	OW_CIMInstanceEnumeration retVal;

	for (; node; node = node.getNext())
	{
		OW_CIMInstanceArray cia;
		OW_CIMClassArray cca;
		OW_CIMObjectPath cop = getObjectWithPath(node, cca, cia);
		if (cop)
		{
			if (cia.size() != 1)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED, "getObjectWithPath "
									"returned multiple values.");
			}
			retVal.addElement(cia[0]);
		}
	}

	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMXMLCIMOMHandle::referenceNames(const OW_CIMObjectPath& path,
												 const OW_String& resultClass,
												 const OW_String& role)
{
	static const char* const commandName = "ReferenceNames";
	OW_Array<OW_Param> params;
	OW_StringStream extra(1000);
	if (role.length() > 0)
	{
		params.push_back(OW_Param(XMLP_ROLE, role));
	}

	if (path.getKeys().size() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">";
		OW_CIMtoXML(path, extra, OW_CIMtoXMLFlags::isNotInstanceName);
		//path.toXML(extra);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">" <<
		"<CLASSNAME NAME=\"" << path.getObjectName() <<
		"\"></CLASSNAME></IPARAMVALUE>";
	}

	if (resultClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_RESULTCLASS << "\">" <<
		"<CLASSNAME NAME=\"" << resultClass <<
		"\"></CLASSNAME></IPARAMVALUE>";
	}

	OW_XMLNode node = intrinsicMethod(path, commandName, params,
												 extra.toString());

	OW_CIMObjectPathEnumeration retVal;

	node = node.getChild();
	OW_XMLNode tmp;
	for (; node; node = node.getNext())
	{
		int token = node.getToken();
		OW_CIMObjectPath cop = OW_XMLCIMFactory::createObjectPath(node);
		switch (token)
		{
			case OW_XMLNode::XML_ELEMENT_CLASSNAME:
				cop.setNameSpace(path.getNameSpace());
				break;
			case OW_XMLNode::XML_ELEMENT_INSTANCENAME:
				cop.setNameSpace(path.getNameSpace());
				break;
			default:
				break;
		} // switch
		retVal.addElement(cop);
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_CIMXMLCIMOMHandle::references(const OW_CIMObjectPath& path,
		 						const OW_String& resultClass, const OW_String& role,
		 						OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
								const OW_StringArray* propertyList)
{
	static const char* const commandName = "References";

	OW_Array<OW_Param> params;
	OW_StringStream extra(1000);

	if (role.length() > 0)
	{
		params.push_back(OW_Param(XMLP_ROLE, role));
	}
	params.push_back(OW_Param(XMLP_QUAL, includeQualifiers));
	params.push_back(OW_Param(XMLP_ORIGIN, includeClassOrigin));

	if (propertyList)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_PROPERTYLIST <<
		"\"><VALUE.ARRAY>";
		for (size_t i = 0; i < propertyList->size(); i++)
		{
			extra << "<VALUE>" << (*propertyList)[i] << "</VALUE>";
		}
		extra << "</VALUE.ARRAY></IPARAMVALUE>";

	}

	if (path.getKeys().size() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">";
		OW_CIMtoXML(path, extra, OW_CIMtoXMLFlags::isNotInstanceName);
		//path.toXML(extra);
		extra << "</IPARAMVALUE>";
	}
	else
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_OBJECTNAME << "\">"
		<< "<CLASSNAME NAME=\"" << path.getObjectName()
		<< "\"></CLASSNAME></IPARAMVALUE>";
	}

	if (resultClass.length() > 0)
	{
		extra << "<IPARAMVALUE NAME=\"" << XMLP_RESULTCLASS << "\">"
		<< "<CLASSNAME NAME=\"" << resultClass <<
		"\"></CLASSNAME></IPARAMVALUE>";
	}

	OW_XMLNode node = intrinsicMethod(path, commandName,
												 params, extra.toString());

	node = node.getChild();

	OW_XMLNode tmpNode;
	OW_CIMInstanceEnumeration retVal;

	for (; node; node = node.getNext())
	{
		OW_CIMInstanceArray cia;
		OW_CIMClassArray cca;
		OW_CIMObjectPath cop = getObjectWithPath(node, cca, cia);
		if (cop)
		{
			if (cia.size() != 1)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED, "getObjectWithName "
									"returned multiple values.");
			}
			retVal.addElement(cia[0]);
		}
	}

	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceArray
OW_CIMXMLCIMOMHandle::execQuery(const OW_CIMNameSpace& path,
	const OW_String& query, int wqlLevel)
{
	return execQuery(path, query, OW_String("WQL") + OW_String(wqlLevel));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceArray
OW_CIMXMLCIMOMHandle::execQuery(const OW_CIMNameSpace& path,
	const OW_String& query, const OW_String& queryLanguage)
{
	static const char* const commandName = "ExecQuery";

	OW_Array<OW_Param> params;

	params.push_back(OW_Param(XMLP_QUERYLANGUAGE, OW_XMLEscape(queryLanguage)));
	params.push_back(OW_Param(XMLP_QUERY, OW_XMLEscape(query)));

	OW_CIMObjectPath cop("", path.getNameSpace());
	OW_XMLNode node = intrinsicMethod(cop, commandName, params);
	if (!node)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	OW_CIMInstanceArray retVal;
	node = node.getChild();
	while (node)
	{
		OW_CIMObjectPath iop(OW_XMLCIMFactory::createObjectPath(node));
		node = node.getNext();
		if (node)
		{
			OW_CIMInstance ci = OW_XMLCIMFactory::createInstance(node);
			ci.setKeys(iop.getKeys());
			retVal.push_back(ci);
			node = node.getNext();
		}
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMFeatures
OW_CIMXMLCIMOMHandle::getServerFeatures()
{
	return m_protocol->getFeatures();
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_CIMXMLCIMOMHandle::intrinsicMethod(
	  				 const OW_CIMObjectPath& path, const OW_String& operation,
	  				 const OW_Array<OW_Param>& params, const OW_String& extra)
{
	OW_Reference<std::iostream> iostrRef = m_protocol->beginRequest(operation, path.getNameSpace());
	std::iostream& iostr = *iostrRef;

	sendXMLHeader( operation, path, iostr);
	for (size_t i = 0; i < params.size(); i++)
	{
		iostr << "<IPARAMVALUE NAME=\"" << params[i].getArgName()
		<< "\">" << params[i].getArgValue() << "</IPARAMVALUE>";
	}
	if (extra.length() > 0)
	{
		iostr << extra;
	}
	sendXMLTrailer(iostr);
	return doSendRequest(iostrRef, operation, path);
}


