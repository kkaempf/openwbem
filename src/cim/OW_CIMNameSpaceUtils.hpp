/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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

#ifndef OW_CIM_NAME_SPACE_UTILS_HPP_INCLUDE_GUARD_
#define OW_CIM_NAME_SPACE_UTILS_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_Bool.hpp"

namespace OW_CIMNameSpaceUtils
{
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
	 * @exception OW_CIMException If the namespace already exists.
	 */
	void createCIM_Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns, 
        OW_UInt16 classInfo = 0, const OW_String& descriptionOfClassInfo = "", const OW_String& interopNs = "root");

	/**
	 * Delete a specified namespace by deleting an instance of the 
     * CIM_Namespace class.
     * @param hdl The cimom handle that will be used.
 	 * @param ns	The namespace to delete.
     * @param interopNS The interop namespace which contains the CIM_Namespace 
     *  class & instances.  The default is root because the CIM Ops. spec says: 
     *  "It is the common practice proposed by this document that instances of
     *  CIM_Namespace be created in root unless there is a specific reason to
     *  define them in another namespace."
	 * @exception OW_CIMException If the namespace does not exist.
	 */
	void deleteCIM_Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns, const OW_String& interopNs = "root");

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
	 * @exception OW_CIMException If the namespace does not exist or the object
	 *		cannot be found in the specified namespace.
	 */
	OW_StringArray enumCIM_Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& interopNs = "root");

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
	 * @exception OW_CIMException If the namespace does not exist or the object
	 *		cannot be found in the specified namespace.
	 */
	void enumCIM_Namespace(const OW_CIMOMHandleIFCRef& hdl,
		OW_StringResultHandlerIFC& result, const OW_String& interopNs = "root");

	/**
	 * Create a cim namespace by creating an instance of the __Namespace 
     * class.  This method has been deprecated in the CIM Ops spec.
     * This implementation assumes that __Namespace presents a hierarcial view
     * of the namespaces.
     * @param hdl The cimom handle that will be used.
	 * @param ns 	The namespace name to be created.
	 * @exception OW_CIMException If the namespace already exists.
	 */
	void create__Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns);

	/**
	 * Delete a specified namespace by deleting an instance of the 
     * __Namespace class.  This method has been deprecated in the CIM Ops spec.
     * This implementation assumes that __Namespace presents a hierarcial view
     * of the namespaces.
     * @param hdl The cimom handle that will be used.
 	 * @param ns	The namespace to delete.
	 * @exception OW_CIMException If the namespace does not exist.
	 */
	void delete__Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns);

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
	 * @exception OW_CIMException If the namespace does not exist or the object
	 *		cannot be found in the specified namespace.
	 */
	OW_StringArray enum__Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns, OW_Bool deep=OW_CIMOMHandleIFC::DEEP);

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
	 * @exception OW_CIMException If the namespace does not exist or the object
	 *		cannot be found in the specified namespace.
	 */
	void enum__Namespace(const OW_CIMOMHandleIFCRef& hdl, const OW_String& ns,
		OW_StringResultHandlerIFC& result, OW_Bool deep=OW_CIMOMHandleIFC::DEEP);


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
    OW_String prepareNamespace(OW_String ns);

}

#endif


