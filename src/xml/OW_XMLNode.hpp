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
 * @author Calvin R. Gaisford
 * @author Bart Whiteley
 * @author Dan Nuffer
 */


#ifndef OW_XMLNODE_HPP_INCLUDE_GUARD_
#define OW_XMLNODE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_XMLAttribute.hpp"
#include "OW_XMLFwd.hpp"

#include <iosfwd>

namespace OW_NAMESPACE
{

class XMLNodeImpl;

typedef IntrusiveReference<XMLNodeImpl> XMLNodeImplRef;

/**
 * Base class used to represent an XML tag such as <name> and
 * contains all of the attributes and the value of such an
 * XML tag.
 *
 * @author Calvin R. Gaisford
 * @version 1.0
 * @since 1/26/2001
 */
class OW_XML_API XMLNode
{
public:
	
	/**
	 * Constructor taking the XML tag name and the attributes
	 * object from the SAX parser
	 *
	 * @param name     The XML tag name.  If the XML tag were <foo> then name
	 *                 would contain the string "foo"
	 * @param attrArray XMLAttributeArray containing the XML attributes that were
	 *                 associated with this node (XML Tag)
	 */
	XMLNode(const String& name, const XMLAttributeArray& attrArray);
	
	/**
	 * Constructor taking the XML tag name
	 * object from the SAX parser
	 *
	 * @param name       The XML tag name.  If the XML tag were <foo> then name
	 *                   would contain the string "foo"
	 */
	XMLNode(const String& name);

	/**
	 * Constructs an empty XMLNode.
	 */
	XMLNode();

	/**
	 * Copy constructor
	 */
	XMLNode(const XMLNode& arg);

	/**
	 * Copy constructor for an XMLNode
	 *
	 * @param arg
	 * @return
	 */
	XMLNode& operator= (const XMLNode & arg);


	~XMLNode();

	/**
	 * Sets the current node to NULL and will free any children or siblings this
	 * node may have
	 */
	void setToNULL();

	/**
	 * Gets the name of the node which came from the XML tag
	 * this node was created from
	 *
	 * @return Returns an String object containing the name of the node
	 */
	String getName() const;
	
	/**
	 * Sets the text value for the XMLNode
	 *
	 * @param text   String containing the value to be set
	 */
	void assignText(const String& text) const;

	/**
	 * Add the given text to the existing text of the node.
	 */
	void appendText(const String& text) const;
	
	/**
	 * Searches the attributes of the current node for the
	 * attribute name and returns the value
	 *
	 * @param name   String containing the attribute name to search for
	 * @return String containing the value of the attribute
	 */
	String getAttribute(const String& name) const;
	
	/**
	 * Adds the argument attribute to the Node's attribute array.
	 *
	 * @param attr   XMLAttribute to add.
	 */
	void addAttribute(const XMLAttribute& attr) const;
	
	/**
	 * Same as getAttribute but if the attribute is not found
	 * a CIMException will be thrown.
	 *
	 * @param name   String containing the attribute name to search for
	 * @return String containing the value of the attribute found
	 * @exception CIMException
	 *                   thrown if the attribute name is not found
	 */
	String mustGetAttribute(const String& name) const;
	
	/**
	 * Gets the attribute array for the currrent node
	 *
	 * @return XMLAttributeArray holding the attributes of the current
	 *         node
	 */
	XMLAttributeArray getAttrs() const;
	
	/**
	 * Returns the XML value of the current node
	 *
	 * Amoung other things, if the current node is a CDATA, getText
	 * returns the text of the CDATA.
	 *
	 * @return String holding the XML value of the current node
	 */
	String getText() const;
	
	/**
	 * Compares the elementToken to the current node and to all
	 * of the current node's siblings and returns the node that
	 * matches
	 *
	 * @param elementToken
	 *               int holding the value of the elementToken to look for
	 * @return XMLNode which has a token matching elementToken
	 */
	XMLNode findElement(const char* elementName) const;
	
	/**
	 * Same as findElement but will throw an exception if the
	 * elementToken is not matched.
	 * Compares the elementToken to the current node and to all
	 * of the current node's siblings and returns the node that
	 * matches
	 *
	 * @param elementToken
	 *               int holding the value of the elementToken to look for
	 * @return XMLNode which has a token matching elementToken
	 * @exception CIMException
	 *                   Thrown if the elementToken is not matched
	 */
	XMLNode mustFindElement(const char* elementName) const;
	
	/**
	 * Gets the next XMLNode unless it does not exist or the
	 * elementToken does not match.
	 *
	 * @param elementToken
	 *               int value of the token to match
	 * @return XMLNode of the next node if it not NULL and matches
	 *         elementToken
	 */
	XMLNode nextElement(const char* elementName);
	
	/**
	 * Same as nextElement but it throws and exception if either
	 * the next node is NULL or it does not match elementToken.
	 *
	 * @param elementToken
	 *               int value of the token to match
	 * @return XMLNode of the next node if it not NULL and matches
	 *         elementToken
	 * @exception CIMException
	 *                   Thrown if either the next node is NULL or the elementToken
	 *                   does not match
	 */
	XMLNode mustNextElement(const char* elementName) const;
	
	/**
	 * Matches elementToken with the current node or throws an
	 * exception
	 *
	 * @param elementToken
	 *               int set to the elementToken to match
	 * @exception CIMException
	 */
	void mustElement(const char* elementName) const;
	
	/**
	 * Same as mustElement but returns the child of the current
	 * node after matching elementToken.  Throws and exception if
	 * the elementToken does not match the current node.
	 *
	 * @param elementToken
	 *               int set to the elementToken value to match
	 * @return XMLNode child of the current node
	 * @exception CIMException
	 *                   Thrown if either the elementToken did not match the current
	 *                   node or the child node is NULL.
	 */
	XMLNode mustElementChild(const char* elementName) const;
	
	/**
	 * Returns the child node of the current node if the child
	 * node matches the elementToken
	 *
	 * @param elementToken
	 *               int set to the elementToken value to match
	 * @return XMLNode child of the current node
	 * @exception CIMException
	 *                   Thrown if any of the following happens:
	 *                   the current node is NULL,
	 *                   the child of the current node is null,
	 *                   the child of the current node does not match the elementToken
	 */
	XMLNode mustChildElement(const char* elementName) const;
	
	/**
	 * Returns the current node's child's child if the current
	 * node's child matches the elementToken
	 *
	 * @param elementToken
	 *               int set to the value of the elementToken to match
	 * @return XMLNode current node's child's child
	 * @exception CIMException
	 *                   Thrown if one of the following happens:
	 *                   The current node is NULL
	 *                   The child of the current node is NULL
	 *                   The child of the current node does not match elementToken
	 *                   The child of the child of the current node is NULL
	 */
	XMLNode mustChildElementChild(const char* elementName) const;
	
	/**
	 * Gets the child of the current node and search the child
	 * and the child's siblings to match the elementToken
	 *
	 * @param elementToken
	 *               int set to the value of the elementToken to match
	 * @return XMLNode from the current node's child and siblings that
	 *         matched the elementToken
	 * @exception CIMException
	 *                   Thrown if one of the following happens:
	 *                   The current node is NULL
	 *                   the child of the current node is NULL
	 *                   the elementToken is not matched
	 */
	XMLNode mustChildFindElement(const char* elementName) const;
	
	/**
	 * Same as findElement but it returns the child of the
	 * matched node
	 *
	 * @param elementToken
	 *               int set to the value of the elementToken to match
	 * @return XMLNode which is the child of the matched node
	 */
	XMLNode findElementChild(const char* elementName) const;
	
	/**
	 * Same as findElementChild but it throws an exception if
	 * anything goes wrong
	 *
	 * @param elementToken
	 *               int set to the value of the elementToken to match
	 * @return XMLNode which is the child of the matched node
	 * @exception CIMException
	 *                   Thrown if the XMLNode is not going to be one that
	 *                   matched the elementToken
	 */
	XMLNode mustFindElementChild(const char* elementName) const;
	
	/**
	 * Same as mustFindElementChild but it starts the search
	 * at the child of current node
	 *
	 * @param elementToken
	 *               int set to the value of the elementToken to match
	 * @return XMLNode which is the child of the matched node
	 * @exception CIMException
	 *                   Thrown if the XMLNode is not going to be one that
	 *                   matched the elementToken
	 */
	XMLNode mustChildFindElementChild(const char* elementName) const;
	
	/**
	 * Sets the current node's next sibling to node
	 *
	 * @param node   XMLNode to be set as the current node's next sibling.
	 *               Any node current set as the current node's next sibling
	 *               will be removed and deleted and replaced by node.
	 */
	void setNext(const XMLNode& node) const;
	
	/**
	 * Gets the current node's next sibling
	 *
	 * @return XMLNode holding the next sibling of the current node
	 */
	XMLNode getNext() const;
	
	/**
	 * Adds the node as a child of the current node
	 *
	 * @param node   XMLNode to be set as the current node's first child.
	 *               Any child set as the current child node will be set as the
	 *				 next sibling to the node added
	 */
	void addChild(const XMLNode& node) const;
	
	/**
	 * Gets the current node's child
	 *
	 * @return XMLNode which is the child of the current node
	 */
	XMLNode getChild() const;

	/**
	 * Gets the current node's child and throws an exception if
	 * the child is NULL
	 *
	 * @return XMLNode which is the child of the current node
	 * @exception CIMException
	 *                   Thrown if the child of the current node is NULL.
	 */
	XMLNode mustGetChild() const;

	/**
	 * Gets the current node's children.
	 *
	 * @return XMLNodeArray containing child nodes
	 */
	XMLNodeArray getChildren() const;

	/**
	 * Prints the node in UTF8 format
	 *
	 * @param iostream& to print to
	 *
	 */
	void printNode( std::ostream& ostr ) const;

	/**
	 * Returns formatted xml representation of node in String
	 *
	 * @param XMLNode node to format
	 *
	 * @returns String containing formatted xml
	 */
	String toString() const;

	typedef XMLNodeImplRef XMLNode::*safe_bool;
	operator safe_bool () const
		{  return (m_impl) ? &XMLNode::m_impl : 0; }
	bool operator!() const
		{  return !m_impl; }


private:

	XMLNode(const XMLNodeImplRef& ref);

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	XMLNodeImplRef m_impl;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	friend class XMLNodeImpl;
};

OW_XML_API std::ostream& operator<<(std::ostream& ostr, const XMLNode& node);

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

class OW_XML_API XMLNodeImpl : public IntrusiveCountableBase
{
public:
	XMLNodeImpl(const String& name, const XMLAttributeArray& attrArray);
	XMLNodeImpl(const String& name);
	XMLNodeImpl();
	String getNodeName() const;
	void assignText(const String& text);
	void appendText(const String& text);
	String getAttribute(const String& name, bool throwException = false) const;
	void addAttribute(const XMLAttribute& attr);
	XMLAttributeArray getAttrs() const;
	String getText() const;
	String getName() const;
	
	void mustElement(const char* elementName) const;
	XMLNodeImplRef findElement(const char* elementName, bool throwException = false) const;
	XMLNodeImplRef nextElement(const char* elementName, bool throwException = false) const;
	XMLNodeImplRef findElementChild(const char* elementName, bool throwException = false) const;
	XMLNodeImplRef mustNextElement(const char* elementName) const;
	XMLNodeImplRef mustElementChild(const char* elementName) const;
	XMLNodeImplRef mustChildElement(const char* elementName) const;
	XMLNodeImplRef mustChildElementChild(const char* elementName) const;
	XMLNodeImplRef mustChildFindElement(const char* elementName) const;
	XMLNodeImplRef mustChildFindElementChild(const char* elementName) const;
	
	void setNext(const XMLNodeImplRef& node);

	XMLNodeImplRef getNext() const;
	void addChild(const XMLNodeImplRef& node);

	XMLNodeImplRef mustGetChild() const;
	XMLNodeImplRef getChild() const;
	XMLNodeArray getChildren() const;
	void printNode( std::ostream& ostr ) const;
	String toString() const;

protected:
	XMLNodeImplRef m_nextNode;
	XMLNodeImplRef m_childNode;
	XMLNodeImplRef m_lastChildNode;

	XMLAttributeArray m_XMLAttributeArray;
	String m_strName;
	String m_strText;
};

#ifdef OW_WIN32
#pragma warning (pop)
#endif

} // end namespace OW_NAMESPACE

#endif

