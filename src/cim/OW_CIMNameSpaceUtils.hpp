/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

#ifndef OW_CIM_NAME_SPACE_UTILS_HPP_INCLUDE_GUARD_
#define OW_CIM_NAME_SPACE_UTILS_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_ConfigOpts.hpp" // for OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE

namespace OpenWBEM
{

namespace CIMNameSpaceUtils
{
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Create a cim namespace by creating an instance of the CIM_Namespace 
	 * class.
	 * @param hdl The cimom handle that will be used.
	 * @param ns 	The namespace name to be created.
	 * @param classInfo     [Required, Write, Description (
	 *   "Enumeration indicating the organization/schema of the "
	 *   "Namespace's objects. For example, they may be instances "
	 *   "of classes of a specific CIM version."),
	 *   ValueMap {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	 *             "200", "201", "202"},
	 *   Values {"Unknown", "Other", "CIM 1.0", "CIM 2.0",
	 *           "CIM 2.1", "CIM 2.2", "CIM 2.3", "CIM 2.4", "CIM 2.5",
	 *           "CIM 2.6", "DMI Recast", "SNMP Recast", "CMIP Recast"},
	 *   ModelCorrespondence {"CIM_Namespace.DescriptionOfClassInfo"} ]
	 * @param descriptionOfClassInfo [Write, Description (
	 *   "A string providing more detail (beyond the general "
	 *   "classification in ClassInfo) for the object hierarchy of "
	 *   "the Namespace."),
	 *   ModelCorrespondence {"CIM_Namespace.ClassInfo"} ]
	 * @param interopNS The interop namespace which contains the CIM_Namespace 
	 *  class & instances.  The default is root because the CIM Ops. spec says: 
	 *  "It is the common practice proposed by this document that instances of
	 *  CIM_Namespace be created in root unless there is a specific reason to
	 *  define them in another namespace."
	 * @exception CIMException If the namespace already exists.
	 */
	void createCIM_Namespace(CIMOMHandleIFC& hdl, const String& ns, 
		UInt16 classInfo = 0, const String& descriptionOfClassInfo = "", const String& interopNs = OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);
	/**
	 * Delete a specified namespace by deleting an instance of the 
	 * CIM_Namespace class. The namespace must be empty of all classes and
	 * instances before it can be deleted.
	 * @param hdl The cimom handle that will be used.
	 * @param ns	The namespace to delete.
	 * @param interopNS The interop namespace which contains the CIM_Namespace 
	 *  class & instances.  The default is root because the CIM Ops. spec says: 
	 *  "It is the common practice proposed by this document that instances of
	 *  CIM_Namespace be created in root unless there is a specific reason to
	 *  define them in another namespace."
	 * @exception CIMException If the namespace does not exist.
	 */
	void deleteCIM_Namespace(CIMOMHandleIFC& hdl, const String& ns, const String& interopNs = OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Gets a list of the namespaces by enumerating the instances of the
	 * CIM_Namespace class.
	 * @param hdl The cimom handle that will be used.
	 * @param interopNS The interop namespace which contains the CIM_Namespace 
	 *  class & instances.  The default is root because the CIM Ops. spec says: 
	 *  "It is the common practice proposed by this document that instances of
	 *  CIM_Namespace be created in root unless there is a specific reason to
	 *  define them in another namespace."
	 * @return An Array of namespace names as strings.
	 * @exception CIMException If the namespace does not exist or the object
	 *		cannot be found in the specified namespace.
	 */
	StringArray enumCIM_Namespace(CIMOMHandleIFC& hdl, const String& interopNs = OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);
	/**
	 * Gets a list of the namespaces by enumerating the instances of the
	 * CIM_Namespace class.
	 * @param hdl The cimom handle that will be used.
	 * @param result result.handle will be called once for each enumerated
	 *  namespace.
	 * @param interopNS The interop namespace which contains the CIM_Namespace 
	 *  class & instances.  The default is root because the CIM Ops. spec says: 
	 *  "It is the common practice proposed by this document that instances of
	 *  CIM_Namespace be created in root unless there is a specific reason to
	 *  define them in another namespace."
	 * @exception CIMException If the namespace does not exist or the object
	 *		cannot be found in the specified namespace.
	 */
	void enumCIM_Namespace(CIMOMHandleIFC& hdl,
		StringResultHandlerIFC& result, const String& interopNs = OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Create a cim namespace by creating an instance of the __Namespace 
	 * class.  This method has been deprecated in the CIM Ops spec.
	 * This implementation assumes that __Namespace presents a hierarcial view
	 * of the namespaces.
	 * @param hdl The cimom handle that will be used.
	 * @param ns 	The namespace name to be created.
	 * @exception CIMException If the namespace already exists.
	 */
	void create__Namespace(CIMOMHandleIFC& hdl, const String& ns);
	/**
	 * Delete a specified namespace by deleting an instance of the 
	 * __Namespace class.  This method has been deprecated in the CIM Ops spec.
	 * This implementation assumes that __Namespace presents a hierarcial view
	 * of the namespaces.
	 * @param hdl The cimom handle that will be used.
	 * @param ns	The namespace to delete.
	 * @exception CIMException If the namespace does not exist.
	 */
	void delete__Namespace(CIMOMHandleIFC& hdl, const String& ns);
#endif
	/**
	 * Gets a list of the namespaces by enumerating the instances of the
	 * __Namespace class.  This method has been deprecated in the CIM Ops spec.
	 * This implementation assumes that __Namespace presents a hierarcial view
	 * of the namespaces.
	 * @param hdl The cimom handle that will be used.
	 * @param ns The parent namespace to enumerate
	 * @param deep If set to DEEP, the enumeration returned will contain the
	 *		entire hierarchy of namespaces present under the enumerated
	 *		namespace. If set to SHALLOW  the enuermation will return only the
	 *		first level children of the enumerated namespace.
	 * @return An Array of namespace names as strings.
	 * @exception CIMException If the namespace does not exist or the object
	 *		cannot be found in the specified namespace.
	 */
	StringArray enum__Namespace(CIMOMHandleIFC& hdl, const String& ns, WBEMFlags::EDeepFlag deep=WBEMFlags::E_DEEP);
	/**
	 * Gets a list of the namespaces by enumerating the instances of the
	 * __Namespace class.  This method has been deprecated in the CIM Ops spec.
	 * This implementation assumes that __Namespace presents a hierarcial view
	 * of the namespaces.
	 * @param hdl The cimom handle that will be used.
	 * @param ns The parent namespace to enumerate
	 * @param result result.handle will be called once for each enumerated
	 *  namespace.
	 * @param deep If set to DEEP, the enumeration returned will contain the
	 *		entire hierarchy of namespaces present under the enumerated
	 *		namespace. If set to SHALLOW  the enuermation will return only the
	 *		first level children of the enumerated namespace.
	 * @exception CIMException If the namespace does not exist or the object
	 *		cannot be found in the specified namespace.
	 */
	void enum__Namespace(CIMOMHandleIFC& hdl, const String& ns,
		StringResultHandlerIFC& result, WBEMFlags::EDeepFlag deep=WBEMFlags::E_DEEP);
	/**
	 * This function creates a "normalized" namespace.  Because of differing 
	 * WBEM implementations, it's expected that the following all refer to the
	 * same namespace: root, /root, and \root.  As far as the openwbem cimom
	 * is concerned, they are all different, so this function translates all
	 * backslashes to forward slashes and also removes all beginning slashes.
	 * So /root and \root yield just root.
	 * @param ns The namespace name.
	 * @return ns The normalized version of ns.
	 */
	String prepareNamespace(const String& ns);
}

} // end namespace OpenWBEM

#endif
