
/*
 *
 * CmpiEnumeration.h
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

#ifndef _CmpiEnumeration_h_
#define _CmpiEnumeration_h_
#include "OW_config.h"

#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiObject.h"

class CmpiObjectPath;

/** This class acts as a container for holding results from enunmerating
    CmpiBroker services.
*/

class CmpiEnumeration : public CmpiObject {
   friend class CmpiBroker;
  protected:

   /** Protected constructor used by MIDrivers to encapsulate CMPIEnumeration.
   */
   inline CmpiEnumeration(CMPIEnumeration* enc)
      { this->enc=enc; }

   /** getEnc - Gets the encapsulated CMPIEnumeration.
   */
   inline CMPIEnumeration *getEnc() const
     { return (CMPIEnumeration*)enc; }
  private:

   /** Constructor - Should not be called
   */
   CmpiEnumeration() {}
  public:

   /** getNext - Gets next entry.
   */
   CmpiData getNext();

   /** getNext - Check for at least one remaining entry.
   */
   CmpiBoolean hasNext();

   /** toArray - Convert an Enumeration to an Array
    */
   CmpiData toArray();
};

#endif



