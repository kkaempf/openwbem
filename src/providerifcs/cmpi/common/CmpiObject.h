
/*
 *
 * CmpiObject.h
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
 * Description: CMPI bas class for all Cmpi classes
 *
 */

#ifndef _CmpiObject_h_
#define _CmpiObject_h_
#include "OW_config.h"

#include "cmpidt.h"
#include "cmpift.h"

#ifndef CmpiBoolean
#define CmpiBoolean CMPIBoolean
#define CmpiRc      CMPIrc
#endif

#include "CmpiBaseMI.h"
#include "CmpiString.h"

class CmpiBroker;
//class CmpiString;

/** Abstract base class for all Cmpi classes.
*/

class CmpiObject {
   friend class CmpiBroker;
   friend class CmpiObjectPath;
   friend class CmpiInstance;
  protected:
   /** Protected pointer to encapsulated CMPI instance
   */
    void *enc;
   /** Constructor - Do nothing
   */
   CmpiObject() {}
   /** Constructor - Normal base class constructor
   */
   inline CmpiObject(const void* enc)
      { this->enc=(void*)enc; }
   CmpiString doToString(CMPIBroker *mb);
   CmpiBoolean doIsA(CMPIBroker *mb, const char *typeName);
  private:
  public:
   /** isNull - Test for valid encapsualtion pointer
   */
   inline CmpiBoolean isNull() const
      { return (enc==NULL); }
   /** toString - Produc;s CIMOM specific string representation of object
   */
   inline CmpiString toString() {
      return doToString(CmpiProviderBase::getBroker());
   }
   /** isA - checks this objects type   ;
   */
   inline CmpiBoolean isA(const char *typeName) {
      return doIsA(CmpiProviderBase::getBroker(),typeName);
   }
};

#endif

