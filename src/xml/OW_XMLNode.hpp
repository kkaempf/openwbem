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

//------------------------------
// OW_XMLNode.hpp
//------------------------------

#ifndef _OW_XMLNODE_HPP__
#define _OW_XMLNODE_HPP__

#include	"OW_config.h"
#include	"OW_String.hpp"
#include	"OW_Map.hpp"
#include	"OW_CIMException.hpp"
#include	"OW_XMLAttribute.hpp"

class OW_XMLNodeImpl;

typedef OW_Reference<OW_XMLNodeImpl> OW_XMLNodeImplRef;
typedef OW_Map<OW_String, int> XMLElementMap;


/**
 * Base class used to represent an XML tag such as <name> and
 * contains all of the attributes and the value of such an
 * XML tag.
 *
 * @author Calvin R. Gaisford
 * @version 1.0
 * @since 1/26/2001
 */
class OW_XMLNode
{
public:
	
	/**
	 * Constructor taking the XML tag name and the attributes
	 * object from the SAX parser
	 *
	 * @param name     The XML tag name.  If the XML tag were <foo> then name
	 *                 would contain the string "foo"
	 * @param attrArray OW_XMLAttrArray containing the XML attributes that were
	 *                 associated with this node (XML Tag)
	 */
	OW_XMLNode(const OW_String& name, const OW_XMLAttrArray& attrArray);
	
	/**
	 * Constructor taking the XML tag name
	 * object from the SAX parser
	 *
	 * @param name       The XML tag name.  If the XML tag were <foo> then name
	 *                   would contain the string "foo"
	 */
	OW_XMLNode(const OW_String& name);

	/**
	 * Constructs an empty XMLNode.
	 */
	OW_XMLNode();

	/**
	 * Copy constructor
	 */
	OW_XMLNode(const OW_XMLNode& arg);

	/**
	 * Copy constructor for an OW_XMLNode
	 *
	 * @param arg
	 * @return
	 */
	OW_XMLNode& operator= (const OW_XMLNode & arg);


	~OW_XMLNode();

	/**
	 * Sets the current node to NULL and will free any children or siblings this
	 * node may have
	 */
	void setToNULL();

	/**
	 * Gets the node type value.
	 *
	 * @return short set to the type of the current node
	 */
	int getNodeType() const;
	
	/**
	 * Gets the name of the node which came from the XML tag
	 * this node was created from
	 *
	 * @return Returns an OW_String object containing the name of the node
	 */
	OW_String getNodeName() const;
	
	/**
	 * Sets the text value for the XMLNode
	 *
	 * @param text   OW_String containing the value to be set
	 */
	void assignText(const OW_String& text) const;

	/**
	 * Add the given text to the existing text of the node.
	 */
	void appendText(const OW_String& text) const;
	
	/**
	 * Searches the attributes of the current node for the
	 * attribute name and returns the value
	 *
	 * @param name   OW_String containing the attribute name to search for
	 * @return OW_String containing the value of the attribute
	 */
	OW_String getAttribute(const OW_String& name) const;
	
	/**
	 * Same as getAttribute but if the attribute is not found
	 * a OW_CIMException will be thrown.
	 *
	 * @param name   OW_String containing the attribute name to search for
	 * @return OW_String containing the value of the attribute found
	 * @exception OW_CIMException
	 *                   thrown if the attribute name is not found
	 */
	OW_String mustGetAttribute(const OW_String& name) const /*throw (OW_CIMException)*/;
	
	/**
	 * Gets the attribute array for the currrent node
	 *
	 * @return OW_XMLAttrArray holding the attributes of the current
	 *         node
	 */
	OW_XMLAttrArray getAttrs() const;
	
	/**
	 * Returns the XML value of the current node
	 *
	 * @return OW_String holding the XML value of the current node
	 */
	OW_String getText() const;
	
	/**
	 * Returns the token associated with the current node
	 *
	 * @return int set to the value of the token associated with the
	 *         current node
	 */
	int getToken() const;
	
	/**
	 * Compares the elementToken to the current node and to all
	 * of the current node's siblings and returns the node that
	 * matches
	 *
	 * @param elementToken
	 *               int holding the value of the elementToken to look for
	 * @return OW_XMLNode which has a token matching elementToken
	 */
	OW_XMLNode findElement(int elementToken) const;
	
	/**
	 * Same as findElement but will throw an exception if the
	 * elementToken is not matched.
	 * Compares the elementToken to the current node and to all
	 * of the current node's siblings and returns the node that
	 * matches
	 *
	 * @param elementToken
	 *               int holding the value of the elementToken to look for
	 * @return OW_XMLNode which has a token matching elementToken
	 * @exception OW_CIMException
	 *                   Thrown if the elementToken is not matched
	 */
	OW_XMLNode mustFindElement(int elementToken) const  /*throw (OW_CIMException)*/;
	
	/**
	 * Gets the next OW_XMLNode unless it does not exist or the
	 * elementToken does not match.
	 *
	 * @param elementToken
	 *               int value of the token to match
	 * @return OW_XMLNode of the next node if it not NULL and matches
	 *         elementToken
	 */
	OW_XMLNode nextElement(int elementToken);
	
	/**
	 * Same as nextElement but it throws and exception if either
	 * the next node is NULL or it does not match elementToken.
	 *
	 * @param elementToken
	 *               int value of the token to match
	 * @return OW_XMLNode of the next node if it not NULL and matches
	 *         elementToken
	 * @exception OW_CIMException
	 *                   Thrown if either the next node is NULL or the elementToken
	 *                   does not match
	 */
	OW_XMLNode mustNextElement(int elementToken) const /*throw (OW_CIMException)*/;
	
	/**
	 * Matches elementToken with the current node or throws an
	 * exception
	 *
	 * @param elementToken
	 *               int set to the elementToken to match
	 * @exception OW_CIMException
	 */
	void mustElement(int elementToken) const /*throw (OW_CIMException)*/;
	
	/**
	 * Same as mustElement but returns the child of the current
	 * node after matching elementToken.  Throws and exception if
	 * the elementToken does not match the current node.
	 *
	 * @param elementToken
	 *               int set to the elementToken value to match
	 * @return OW_XMLNode child of the current node
	 * @exception OW_CIMException
	 *                   Thrown if either the elementToken did not match the current
	 *                   node or the child node is NULL.
	 */
	OW_XMLNode mustElementChild(int elementToken) const /*throw (OW_CIMException)*/;
	
	/**
	 * Returns the child node of the current node if the child
	 * node matches the elementToken
	 *
	 * @param elementToken
	 *               int set to the elementToken value to match
	 * @return OW_XMLNode child of the current node
	 * @exception OW_CIMException
	 *                   Thrown if any of the following happens:
	 *                   the current node is NULL,
	 *                   the child of the current node is null,
	 *                   the child of the current node does not match the elementToken
	 */
	OW_XMLNode mustChildElement(int elementToken) const /*throw (OW_CIMException)*/;
	
	/**
	 * Returns the current node's child's child if the current
	 * node's child matches the elementToken
	 *
	 * @param elementToken
	 *               int set to the value of the elementToken to match
	 * @return OW_XMLNode current node's child's child
	 * @exception OW_CIMException
	 *                   Thrown if one of the following happens:
	 *                   The current node is NULL
	 *                   The child of the current node is NULL
	 *                   The child of the current node does not match elementToken
	 *                   The child of the child of the current node is NULL
	 */
	OW_XMLNode mustChildElementChild(int elementToken) const /*throw (OW_CIMException)*/;
	
	/**
	 * Gets the child of the current node and search the child
	 * and the child's siblings to match the elementToken
	 *
	 * @param elementToken
	 *               int set to the value of the elementToken to match
	 * @return OW_XMLNode from the current node's child and siblings that
	 *         matched the elementToken
	 * @exception OW_CIMException
	 *                   Thrown if one of the following happens:
	 *                   The current node is NULL
	 *                   the child of the current node is NULL
	 *                   the elementToken is not matched
	 */
	OW_XMLNode mustChildFindElement(int elementToken) const /*throw (OW_CIMException)*/;
	
	/**
	 * Same as findElement but it returns the child of the
	 * matched node
	 *
	 * @param elementToken
	 *               int set to the value of the elementToken to match
	 * @return OW_XMLNode which is the child of the matched node
	 */
	OW_XMLNode findElementChild(int elementToken) const;
	
	/**
	 * Same as mustFindElementChild but it throws an exception if
	 * anything goes wrong
	 *
	 * @param elementToken
	 *               int set to the value of the elementToken to match
	 * @return OW_XMLNode which is the child of the matched node
	 * @exception OW_CIMException
	 *                   Thrown if the OW_XMLNode is not going to be one that
	 *                   matched the elementToken
	 */
	OW_XMLNode mustFindElementChild(int elementToken) const /*throw (OW_CIMException)*/;
	
	/**
	 * Same as mustFindElementChild but it starts the search
	 * at the child of current node
	 *
	 * @param elementToken
	 *               int set to the value of the elementToken to match
	 * @return OW_XMLNode which is the child of the matched node
	 * @exception OW_CIMException
	 *                   Thrown if the OW_XMLNode is not going to be one that
	 *                   matched the elementToken
	 */
	OW_XMLNode mustChildFindElementChild(int elementToken) const /*throw (OW_CIMException)*/;
	
	/**
	 * Sets the current node's next sibling to node
	 *
	 * @param node   OW_XMLNode to be set as the current node's next sibling.
	 *               Any node current set as the current node's next sibling
	 *               will be removed and deleted and replaced by node.
	 */
	void setNext(const OW_XMLNode& node) const;
	
	/**
	 * Gets the current node's next sibling
	 *
	 * @return OS_XMLNode holding the next sibling of the current node
	 */
	OW_XMLNode getNext() const;
	
	/**
	 * Adds the node as a child of the current node
	 *
	 * @param node   OW_XMLNode to be set as the current node's first child.
	 *               Any child set as the current child node will be set as the
	 *				 next sibling to the node added
	 */
	void addChild(const OW_XMLNode& node) const;
	
	/**
	 * Gets the current node's child
	 *
	 * @return OW_XMLNode which is the child of the current node
	 */
	OW_XMLNode getChild() const;

	/**
	 * Gets the current node's child and throws an exception if
	 * the child is NULL
	 *
	 * @return OW_XMLNode which is the child of the current node
	 * @exception OW_CIMException
	 *                   Thrown if the child of the current node is NULL.
	 */
	OW_XMLNode mustGetChild() const /*throw (OW_CIMException)*/;
	
	/**
	 * This operator allows a comparison of an OW_XMLNode to be
	 * made with 0 or 1.  If an empty OW_XMLNode is created, it
	 * will compare true to 0.  When an OW_XMLNode is valid and
	 * contains valid data, it compares true to 1.
	 *
	 * @return bool 1 or 0
	 */
	operator void*() const
	{
		return (void*)(!m_impl.isNull());
	}


	enum tokenId
	{
		XML_ELEMENT_UNKNOWN							= 0,
		XML_ELEMENT_CIM								= 1,
		XML_ELEMENT_MESSAGE							= 2,
		XML_ELEMENT_SIMPLEREQ						= 3,
		XML_ELEMENT_IMETHODCALL						= 4,
		XML_ELEMENT_LOCALNAMESPACEPATH			= 5,
		XML_ELEMENT_NAMESPACE						= 6,
		XML_ELEMENT_PARAMVALUE						= 7,
		XML_ELEMENT_IMETHODRESPONSE				= 8,
		XML_ELEMENT_IRETURNVALUE					= 10,
		XML_ELEMENT_CLASS								= 12,
		XML_ELEMENT_QUALIFIER						= 13,
		XML_ELEMENT_SIMPLERSP						= 15,
		XML_ELEMENT_PROPERTY							= 16,
		XML_ELEMENT_PROPERTY_ARRAY					= 17,
		XML_ELEMENT_PARAMETER_REFERENCE			= 19,
		XML_ELEMENT_PARAMETER_ARRAY				= 20,
		XML_ELEMENT_PARAMETER_REFARRAY			= 21,
		XML_ELEMENT_PARAMETER						= 22,
		XML_ELEMENT_METHOD							= 23,
		XML_ELEMENT_INSTANCE							= 24,
		XML_ELEMENT_INSTANCENAME					= 25,
		XML_ELEMENT_KEYBINDING						= 26,
		XML_ELEMENT_KEYVALUE							= 27,
		XML_ELEMENT_IMPLICITKEY						= 30,
		XML_ELEMENT_METHODCALL						= 33,
		XML_ELEMENT_RETURNVALUE						= 34,
		XML_ELEMENT_METHODRESPONSE					= 35,
		XML_ELEMENT_PROPERTY_REF					= 36,
		XML_ELEMENT_ERROR								= 37,
		XML_ELEMENT_QUALIFIER_DECLARATION		= 38,
		XML_ELEMENT_SCOPE								= 39,
		XML_ELEMENT_DECLARATION						= 40,
		XML_ELEMENT_IPARAMVALUE						= 42,

		XML_ELEMENT_CLASSPATH						= 1000,
		XML_ELEMENT_LOCALCLASSPATH					= 1001,
		XML_ELEMENT_CLASSNAME						= 1002,
		XML_ELEMENT_INSTANCEPATH					= 1003,
		XML_ELEMENT_LOCALINSTANCEPATH				= 1004,
		XML_ELEMENT_HOST								= 1005,
		XML_ELEMENT_NAMESPACEPATH					= 1006,
		XML_ELEMENT_OBJECTPATH						= 1007,

		XML_ELEMENT_DECLGROUP						= 2000,
		XML_ELEMENT_DECLGROUP_WITHNAME			= 2001,

		XML_ELEMENT_VALUE								= 4000,
		XML_ELEMENT_VALUE_ARRAY						= 4001,
		XML_ELEMENT_VALUE_NAMEDOBJECT				= 4002,
		XML_ELEMENT_VALUE_REFERENCE				= 4003,
		XML_ELEMENT_VALUE_REFARRAY					= 4004,
		XML_ELEMENT_VALUE_OBJECT					= 4005,
		XML_ELEMENT_VALUE_OBJECTWITHLOCALPATH	= 4006,
		XML_ELEMENT_VALUE_OBJECTWITHPATH			= 4007,
		XML_ELEMENT_VALUE_NAMEDINSTANCE			= 4008,

		XML_ELEMENT_MULTIREQ							= 5000,
		XML_ELEMENT_MULTIRSP							= 5001,

		XML_ELEMENT_SIMPLEEXPREQ 					= 6000,
		XML_ELEMENT_SIMPLEEXPRSP					= 6001,
		XML_ELEMENT_MULTIEXPREQ 					= 6002,
		XML_ELEMENT_MULTIEXPRSP						= 6003,
		XML_ELEMENT_EXPMETHODCALL 					= 6004,
		XML_ELEMENT_EXPMETHODRESPONSE				= 6005,

		XML_ELEMENT_DECLGROUP_WITHPATH			= 10000
	};



	OW_StringArray extractParameterStringArray(const OW_String& value,
		OW_Bool& found);

	OW_String extractParameterValueAttr(const OW_String& iparamName,
			tokenId tokenid, const OW_String& attrName)
		/*throw (OW_CIMException)*/;

	OW_Bool extractParameterValue(const OW_String& value, OW_Bool defaultValue);

	OW_String extractParameterValue(const OW_String& value,
			const OW_String& defaultValue) /*throw (OW_CIMException)*/;
	OW_XMLNode findElementAndParameter(const OW_String& nameOfParameter)
		/*throw (OW_CIMException)*/;

	OW_String mustExtractParameterValue(const OW_String& value);
	//throw(OW_CIMException);

private:
	void setNodeType(OW_String nodeName);

	OW_XMLNode(OW_XMLNodeImplRef ref);
	OW_XMLNodeImplRef m_impl;

	friend class OW_XMLNodeImpl;
};

typedef OW_Array<OW_XMLNode> OW_XMLNodeArray;

class OW_XMLNodeImpl
{
public:
	OW_XMLNodeImpl(const OW_String& name, const OW_XMLAttrArray& attrArray);
	OW_XMLNodeImpl(const OW_String& name);
	OW_XMLNodeImpl(const OW_XMLNodeImpl& arg);
	OW_XMLNodeImpl();
	~OW_XMLNodeImpl();
	OW_XMLNodeImpl& operator= (const OW_XMLNodeImpl & arg);
	int getNodeType();
	OW_String getNodeName();
	void assignText(const OW_String& text);
	void appendText(const OW_String& text);
	OW_String getAttribute(const OW_String& name, OW_Bool throwException = false) /*throw (OW_CIMException)*/;
	OW_XMLAttrArray getAttrs();
	OW_String getText();
	int getToken();
	
	void mustElement(int elementToken) /*throw (OW_CIMException)*/;
	OW_XMLNodeImplRef findElement(int elementToken, OW_Bool throwException = false) /*throw (OW_CIMException)*/;
	OW_XMLNodeImplRef nextElement(int elementToken, OW_Bool throwException = false) /*throw (OW_CIMException)*/;
	OW_XMLNodeImplRef findElementChild(int elementToken, OW_Bool throwException = false) /*throw (OW_CIMException)*/;
	OW_XMLNodeImplRef mustNextElement(int elementToken) /*throw (OW_CIMException)*/;
	OW_XMLNodeImplRef mustElementChild(int elementToken) /*throw (OW_CIMException)*/;
	OW_XMLNodeImplRef mustChildElement(int elementToken) /*throw (OW_CIMException)*/;
	OW_XMLNodeImplRef mustChildElementChild(int elementToken) /*throw (OW_CIMException)*/;
	OW_XMLNodeImplRef mustChildFindElement(int elementToken) /*throw (OW_CIMException)*/;
	OW_XMLNodeImplRef mustChildFindElementChild(int elementToken) /*throw (OW_CIMException)*/;
	
	void setNext(OW_XMLNodeImplRef node);
	OW_XMLNodeImplRef getNext();
	void addChild(const OW_XMLNodeImplRef& node);

	OW_XMLNodeImplRef mustGetChild() /*throw (OW_CIMException)*/;
	OW_XMLNodeImplRef getChild();

	void setNodeType(int nodeType);

	OW_StringArray extractParameterStringArray(const OW_String& value,
		OW_Bool& found);

	OW_String extractParameterValueAttr(const OW_String& iparamName,
			OW_XMLNode::tokenId tokenid, const OW_String& attrName)
		/*throw (OW_CIMException)*/;
	OW_Bool extractParameterValue(const OW_String& value, OW_Bool defaultValue)
		/*throw (OW_CIMException)*/;
	OW_String extractParameterValue(const OW_String& value,
			const OW_String& defaultValue) /*throw (OW_CIMException)*/;

	OW_XMLNodeImplRef findElementAndParameter(const OW_String& nameOfParameter);
		//throw(OW_CIMException);
	OW_String mustExtractParameterValue(const OW_String& value);
	//throw(OW_CIMException);


protected:
	OW_XMLNodeImplRef m_nextNode;
	OW_XMLNodeImplRef m_childNode;
	OW_XMLNodeImplRef m_lastChildNode;
	int m_iToken;
	int m_iType;

	OW_XMLAttrArray m_XMLAttrArray;
	OW_String m_strName;
	OW_String m_strText;
};

#endif  // _OW_XMLNODE_HPP__
