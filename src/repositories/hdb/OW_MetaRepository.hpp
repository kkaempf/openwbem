/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_METAREPOSITORY_HPP_
#define OW_METAREPOSITORY_HPP_
#include "OW_config.h"
#include "OW_GenericHDBRepository.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMException.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_Cache.hpp"
#include "OW_WBEMFlags.hpp"

namespace OpenWBEM
{

class OW_HDB_API MetaRepository : public GenericHDBRepository
{
public:
	/**
	 * Create a new MetaRepository object.
	 */
	MetaRepository(const ServiceEnvironmentIFCRef& env);
	~MetaRepository();
	/**
	 * Open this MetaRepository.
	 * @param the Fully qalified path to the database (minus extension)
	 */
	void open(const String& path);
	/**
	 * Get an existing qualifier type from the repository.
	 * @param ns			The namespace for the qualifier name.
	 * @param qualName	The name of the qualifier type to retrieve.
	 * @return A valid CIMQauliferType type on success. Otherwise a NULL
	 * CIMQualifierType.
	 * @exception CIMException
	 * @exception IOException
	 */
	CIMQualifierType getQualifierType(const String& ns,
		const CIMName& qualName, HDBHandle* hdl=0);
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Delete an existing qualifier from the repository
	 * @param ns	The namespace for the qualifier name.
	 * @param qualName	The name of the qualifier to delete
	 * @return true if the delete operation was successful. false indicates
	 * the qualifier was not found.
	 * @exception CIMException
	 */
	bool deleteQualifierType(const String& ns, const CIMName& qualName);
	/**
	 * Update a qualifier type in the repository
	 * @param ns	The namespace for the qualifier
	 * @param qt	The qualifier type to update
	 * @exception CIMException
	 */
	void setQualifierType(const String& ns, const CIMQualifierType& qt);
	/**
	 * Enumerator the qualifiers in a given namespace
	 * @param ns	The namespace to get the qualifiers from
	 * @return An enumeration of the qualifier types in the namespace.
	 */
	void enumQualifierTypes(const String& ns,
		CIMQualifierTypeResultHandlerIFC& result);
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Gets an existing class from the store
	 * @param ns	The namespace for the class
	 * @param className	The name of the class to retrieve
	 * @param cc the class to be retrieved.
	 * @return 0 on success.  Otherwise a value from CIMException.
	 * @exception CIMException
	 * @exception HDBException An error occurred in the database.
	 * @exception IOException Couldn't read class object from file.
	 */
	CIMException::ErrNoType getCIMClass(const String& ns,
		const CIMName& className, WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		CIMClass& cc);
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Delete an existing class from the store
	 * @param ns			The namespace for the class
	 * @param className	The name of the class to retrieve
	 * @return true if the class was deleted. false if class was not found.
	 * @exception CIMException if class does not exist
	 */
	bool deleteClass(const String& ns, const CIMName& className);
	/**
	 * creates a class in the store
	 *
	 * @param ns			The namespace for the class
	 * @param cimClass 	The class to create
	 * @exception 	CIMException if the class already exists, or parent class
	 *					is not yet on file.
	 * @exception HDBException An error occurred in the database.
	 * @exception IOException Couldn't write class object to file.
	 */
	void createClass(const String& ns, CIMClass& cimClass);
	/**
	 * set a class in the store - note children are not affected
	 *
	 * @param ns		 The namespace for the class
	 * @param cimClass The class to update
	 * @exception CIMException if the class already exists
	 */
	void modifyClass(const String& ns, const CIMClass& cimClass);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Enumerates the class specified by className.
	 * @param ns The namespace of the class
	 * @param className	The name of the class to enumerate
	 * @param deep If set to CIMClient::DEEP, the enumeration returned will
	 *		contain the names of all classes derived from the enumerated class.
	 *		If set to CIMClient::SHALLOW the enumermation will return only
	 *		the names of the first level children of the enumerated class.
	 * @param localOnly If true, then only CIM elements (properties, methods,
	 *		qualifiers) overriden within the definition are returned.
	 * @param includeQualifiers If true, then all qualifiers for the class
	 *		(including properties, methods, and method parameters) are returned.
	 * @param includeClassOrigin If true, then the class origin attribute will
	 *		be returned on all appropriate components.
	 * @exception CIMException  	If the specified CIMObjectPath object
	 *		cannot be found.
	 */
	void enumClass(const String& ns,
		const CIMName& className,
		CIMClassResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin);
	/**
	 * Enumerates the class names specified by className.
	 * @param ns The namespace of the class
	 * @param className	The name of the class to enumerate
	 * @param deep If set to CIMClient::DEEP, the enumeration returned will
	 *		contain the names of all classes derived from the enumerated class.
	 *		If set to CIMClient::SHALLOW the enumermation will return only
	 *		the names of the first level children of the enumerated class.
	 * @exception CIMException  	If the specified CIMObjectPath object
	 *		cannot be found.
	 */
	void enumClassNames(const String& ns,
		const CIMName& className,
		StringResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	/**
	 * Get all top level classes that are associations
	 * @param ns	The namespace to enumerate
	 * @return An enumeration of classes that are associations.
	 */
	void getTopLevelAssociations(const String& ns,
		CIMClassResultHandlerIFC& result);
#endif
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
	/**
	 * Delete the given namespace and all object contained within it.
	 * @param nsName	The name of the namespace
	 */
	void deleteNameSpace(const String& nsName);
	/**
	 * Create the necessary containers to make a valid path. Fail if the
	 * last container already exists.
	 * @param ns	The name of the namespace
	 * @return 0 on success. Otherwise -1 if the bottom most container already
	 * exists.
	 */
	virtual int createNameSpace(const String& ns);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
private:
	// unimplemented
	MetaRepository(const MetaRepository& x);
	MetaRepository& operator=(const MetaRepository& x);
	/**
	 * Get the node for the qualifier container. Create the node if it
	 * doesn't already exist.
	 * @param hdl	The HDBHandleLock to use in the operation.
	 * @param ns	The namespace
	 * @return The HDBNode for the qualifier container.
	 */
	HDBNode _getQualContainer(HDBHandleLock& hdl, const String& ns);
	/**
	 * Make a path to a qualifier.
	 * @param ns			The namespace for the qualifier.
	 * @param qualName	The name of the qualifier
	 * @return A string that can be used to locate the qualifier.
	 */
	String _makeQualPath(const String& ns, const CIMName& qualName);
	/**
	 * Make a path to a CIM class.
	 * @param ns			The namespace for the class.
	 * @param className	The name of the class.
	 * @return A string that can be used to locate the class.
	 */
	String _makeClassPath(const String& ns, const CIMName& className);
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Add a qualifier type to the repository
	 * @param ns	The namespace for the qualifier type
	 * @param qt	The qualifier type to add to the repository
	 * @exception CIMException
	 */
	void _addQualifierType(const String& ns, const CIMQualifierType& qt,
		HDBHandle* phdl=0);
#endif
	void _getClassNodes(const String& ns, CIMClassResultHandlerIFC& result, HDBNode node,
		HDBHandle hdl,
		WBEMFlags::EDeepFlag deep,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_INCLUDE_CLASS_ORIGIN);
	void _getClassNameNodes(StringResultHandlerIFC& result, HDBNode node,
		HDBHandle hdl, WBEMFlags::EDeepFlag deep);
//	void _getClassChildNames(StringArray& ra, HDBNode node,
//		HDBHandle hdl);
	void _resolveClass(CIMClass& cls, HDBNode& node, HDBHandle& hdl,
		const String& ns);
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	HDBNode adjustClass(const String& ns, CIMClass& childClass,
		HDBHandle hdl);
	void _resolveQualifiers(const String& ns, CIMQualifierArray& quals,
		HDBHandle hdl);
#endif
	CIMClass _getClassFromNode(HDBNode& node, HDBHandle hdl,
		const String& ns);
	CIMName _getClassNameFromNode(HDBNode& node);
	//void _throwIfBadClass(const CIMClass& cc, const CIMClass& parentClass);

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	Cache<CIMClass> m_classCache;
	Cache<CIMQualifierType> m_qualCache;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

};

} // end namespace OpenWBEM

#endif
