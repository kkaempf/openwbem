
/*
 *
 * CmpiContext.h
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
 * Description: CMPI C++ context provider wrapper
 *
 */

#ifndef _CmpiContext_h_
#define _CmpiContext_h_
#include "OW_config.h"

#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiObject.h"
#include "CmpiData.h"

/** This class acts as a container to holding provider functions context information.
*/

class CmpiContext : public CmpiObject {
   friend class CmpiBroker;
   friend class CmpiInstanceMI;
   friend class CmpiMethodMI;
   friend class CmpiAssociationMI;
   friend class CmpiPropertyMI;
   friend class CmpiIndicationMI;
  private:
  protected:

   /** Constructor - Should not be called
   */
   CmpiContext() {}

   /** getEnc - Gets the encapsulated CMPIContext.
   */
   inline CMPIContext *getEnc() const
      { return (CMPIContext*)enc; }
  public:

   /** Constructor used by MIDrivers to encapsulate CMPIContext.
   */
   inline CmpiContext(CMPIContext* c)
      : CmpiObject((void*)c) {}

   /** invocationFlags - InvocationFlags entry name.
   */
   static const char* invocationFlags;

   /** getEntry - Gets a named context entry.
   */
  CmpiData getEntry(const char* name);
};

#endif



