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
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef OW_METAREPOSITORY_HPP_
#define OW_METAREPOSITORY_HPP_

#include "OW_config.h"
#include "OW_GenericHDBRepository.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMException.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_Cache.hpp"


class OW_MetaRepository : public OW_GenericHDBRepository
{
public:

	/**
	 * Create a new OW_MetaRepository object.
	 */
	OW_MetaRepository(OW_ServiceEnvironmentIFCRef env);

	~OW_MetaRepository();

	/**
	 * Open this OW_MetaRepository.
	 * @param the Fully qalified path to the database (minus extension)
	 */
	void open(const OW_String& path);

	/**
	 * Get an existing qualifier type from the repository.
	 * @param ns			The namespace for the qualifier name.
	 * @param qualName	The name of the qualifier type to retrieve.
	 * @return A valid OW_CIMQauliferType type on success. Otherwise a NULL
	 * OW_CIMQualifierType.
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	OW_CIMQualifierType getQualifierType(const OW_String& ns,
		const OW_String& qualName, OW_HDBHandle* hdl=0);

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Delete an existing qualifier from the repository
	 * @param ns	The namespace for the qualifier name.
	 * @param qualName	The name of the qualifier to delete
	 * @return true if the delete operation was successful. false indicates
	 * the qualifier was not found.
	 * @exception OW_CIMException
	 */
	OW_Bool deleteQualifierType(const OW_String& ns, const OW_String& qualName);


	/**
	 * Update a qualifier type in the repository
	 * @param ns	The namespace for the qualifier
	 * @param qt	The qualifier type to update
	 * @exception OW_CIMException
	 */
	void setQualifierType(const OW_String& ns, const OW_CIMQualifierType& qt);

	/**
	 * Enumerator the qualifiers in a given namespace
	 * @param ns	The namespace to get the qualifiers from
	 * @return An enumeration of the qualifier types in the namespace.
	 */
	void enumQualifierTypes(const OW_String& ns,
		OW_CIMQualifierTypeResultHandlerIFC& result);
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

	/**
	 * Gets an existing class from the store
	 * @param ns	The namespace for the class
	 * @param className	The name of the class to retrieve
	 * @param cc the class to be retrieved.
	 * @return 0 on success.  Otherwise a value from OW_CIMException.
	 * @exception OW_CIMException
	 * @exception OW_HDBException An error occurred in the database.
	 * @exception OW_IOException Couldn't read class object from file.
	 */
	OW_CIMException::ErrNoType getCIMClass(const OW_String& ns, 
		const OW_String& className, OW_Bool localOnly,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList,
		OW_CIMClass& cc);

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Delete an existing class from the store
	 * @param ns			The namespace for the class
	 * @param className	The name of the class to retrieve
	 * @return true if the class was deleted. false if class was not found.
	 * @exception CIMException if class does not exist
	 */
	OW_Bool deleteClass(const OW_String& ns, const OW_String& className);

	/**
	 * creates a class in the store
	 *
	 * @param ns			The namespace for the class
	 * @param cimClass 	The class to create
	 * @exception 	CIMException if the class already exists, or parent class
	 *					is not yet on file.
	 * @exception OW_HDBException An error occurred in the database.
	 * @exception OW_IOException Couldn't write class object to file.
	 */
	void createClass(const OW_String& ns, OW_CIMClass& cimClass);

	/**
	 * set a class in the store - note children are not affected
	 *
	 * @param ns		 The namespace for the class
	 * @param cimClass The class to update
	 * @exception CIMException if the class already exists
	 */
	void modifyClass(const OW_String& ns, const OW_CIMClass& cimClass);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

	/**
	 * Enumerates the class specified by the OW_CIMObjectPath.
	 * @param ns The namespace of the class
	 * @param className	The name of the class to enumerate
	 * @param deep If set to OW_CIMClient::DEEP, the enumeration returned will
	 *		contain the names of all classes derived from the enumerated class.
	 *		If set to OW_CIMClient::SHALLOW the enumermation will return only
	 *		the names of the first level children of the enumerated class.
	 * @param localOnly If true, then only CIM elements (properties, methods,
	 *		qualifiers) overriden within the definition are returned.
	 * @param includeQualifiers If true, then all qualifiers for the class
	 *		(including properties, methods, and method parameters) are returned.
	 * @param includeClassOrigin If true, then the class origin attribute will
	 *		be returned on all appropriate components.
	 * @exception OW_CIMException  	If the specified CIMObjectPath object
	 *											cannot be foundl
	 */
	void enumClass(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin);

	/**
	 * Gets the children of a class
	 *
	 * @param ns		 The namespace for the class
	 * @param className The class for whose children are to be retrieved
	 * @return An array of the class names of the children
	 * @exception CIMException if class does not exist
	 */
	OW_StringArray getClassChildren(const OW_String& ns,
		const OW_String& className);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	/**
	 * Get all top level classes that are associations
	 * @param ns	The namespace to enumerate
	 * @return An enumeration of classes that are associations.
	 */
	void getTopLevelAssociations(const OW_String& ns,
		OW_CIMClassResultHandlerIFC& result);
#endif

	/**
	 * Delete the given namespace and all object contained within it.
	 * @param nsName	The name of the namespace
	 */
	void deleteNameSpace(const OW_String& nsName);

	/**
	 * Create the necessary containers to make a valid path. Fail if the
	 * last container already exists.
	 * @param ns	The name of the namespace
	 * @return 0 on success. Otherwise -1 if the bottom most container already
	 * exists.
	 */
	virtual int createNameSpace(OW_String ns);

private:

	// unimplemented
	OW_MetaRepository(const OW_MetaRepository& x);
	OW_MetaRepository& operator=(const OW_MetaRepository& x);

	/**
	 * Get the node for the qualifier container. Create the node if it
	 * doesn't already exist.
	 * @param hdl	The OW_HDBHandleLock to use in the operation.
	 * @param ns	The namespace
	 * @return The OW_HDBNode for the qualifier container.
	 */
	OW_HDBNode _getQualContainer(OW_HDBHandleLock& hdl, const OW_String& ns);

	/**
	 * Make a path to a qualifier.
	 * @param ns			The namespace for the qualifier.
	 * @param qualName	The name of the qualifier
	 * @return A string that can be used to locate the qualifier.
	 */
	OW_String _makeQualPath(const OW_String& ns, const OW_String& qualName);

	/**
	 * Make a path to a CIM class.
	 * @param ns			The namespace for the class.
	 * @param className	The name of the class.
	 * @return A string that can be used to locate the class.
	 */
	OW_String _makeClassPath(const OW_String& ns, const OW_String& className);

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Add a qualifier type to the repository
	 * @param ns	The namespace for the qualifier type
	 * @param qt	The qualifier type to add to the repository
	 * @exception OW_CIMException
	 */
	void _addQualifierType(const OW_String& ns, const OW_CIMQualifierType& qt,
		OW_HDBHandle* phdl=0);
#endif

	void _getClassNodes(OW_CIMClassResultHandlerIFC& result, OW_HDBNode node,
		OW_HDBHandle hdl, OW_Bool deep, OW_Bool localOnly=false,
		OW_Bool includeQualifiers=true, OW_Bool includeClassOrigin=true);

	void _getClassChildNames(OW_StringArray& ra, OW_HDBNode node,
		OW_HDBHandle hdl);

	void _resolveClass(OW_CIMClass& cls, OW_HDBNode& node, OW_HDBHandle& hdl,
		const OW_String& ns);

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	OW_HDBNode adjustClass(const OW_String& ns, OW_CIMClass& childClass,
		OW_HDBHandle hdl);

	void _resolveQualifiers(const OW_String& ns, OW_CIMQualifierArray& quals,
		OW_HDBHandle hdl);
#endif

	OW_CIMClass _getClassFromNode(OW_HDBNode& node, OW_HDBHandle hdl,
		const OW_String& ns=OW_String());

	//void _throwIfBadClass(const OW_CIMClass& cc, const OW_CIMClass& parentClass);

    OW_Cache<OW_CIMClass> m_classCache;
    OW_Cache<OW_CIMQualifierType> m_qualCache;
};

#endif

