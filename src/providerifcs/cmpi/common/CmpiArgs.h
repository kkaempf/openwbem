
/*
 *
 * CmpiInstance.h
 *
 * (C) Copyright IBM Corp. 2003
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author:        Adrian Schuur <schuur@de.ibm.com>
 * Contributors:
 *
 * Description: CMPI C++ Instance wrapper
 *
 */

#ifndef _CmpiArgs_h_
#define _CmpiArgs_h_

#include "OW_config.h"
#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiBaseMI.h"
#include "CmpiObject.h"
#include "CmpiStatus.h"
#include "CmpiObjectPath.h"
#include "CmpiBroker.h"
#include "CmpiBaseMI.h"

/** This class wraps a CIMOM specific input and output arguments
    passed to methodInvocation functions.
*/

class CmpiArgs : public CmpiObject {
   friend class CmpiBroker;
   friend class CmpiMethodMI;
  protected:

   /** Protected constructor used by MIDrivers to encapsulate CMPIArgs.
   */
   inline CmpiArgs(CMPIArgs* enc)
      { this->enc=enc; }

   /** Gets the encapsulated CMPIArgs.
   */
   inline CMPIArgs *getEnc() const
      { return (CMPIArgs*)enc; }
   void *makeArgs(CMPIBroker* mb);
  private:
  public:

   /** Constructor - Empty argument container.
   */
   inline CmpiArgs() {
      enc=makeArgs(CmpiProviderBase::getBroker());
   }

    /**	getArgCount - Gets the number of arguments
	defined for this argument container.
	@return	Number of arguments in this container.
    */
   unsigned int getArgCount();

    /**	getArg - Gets the CmpiData object representing the value
        associated with the argument  name
	@param name argument name.
	@return CmpiData value object associated with the argument.
    */
   CmpiData getArg(const char* name);

    /**	getArgument - Gets the CmpiData object defined
	by the input index parameter.
	@param index Index into the argument array.
	@param name Optional output parameter returning the argument name.
	@return CmpiData value object corresponding to the index.
    */
   CmpiData getArg(const int index, CmpiString *name=NULL);

    /**	setArg - adds/replaces an argument value defined by the input
	parameter to the container.
	@param name Property name.
	@param data Type and Value to be added.
    */
   void setArg(const char* name, CmpiData& data);
};

#endif
