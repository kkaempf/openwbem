
/*
 *
 * CmpiString.h
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
 * Description: CMPI string provider wrapper
 */

#ifndef _CmpiString_h_
#define _CmpiString_h_
#include "OW_config.h"

#include "cmpidt.h"
#include "cmpift.h"
#include <string.h>

#ifndef CmpiBoolean
#define CmpiBoolean CMPIBoolean
#define CmpiRc      CMPIrc
#endif

/** This class wraps a CIMOM specific string representation.
*/

class CmpiString { /*: public CmpiObject { */
   friend class CmpiBroker;
   friend class CmpiData;
   friend class CmpiObjectPath;
   friend class CmpiInstance;
   friend class CmpiObject;
   friend class CmpiArgs;
   friend class CmpiArrayIdx;
  private:
  protected:
   void *enc;
   /** Constructor - Internal use only
   */
   inline CmpiString(CMPIString* c) {
      enc=c;
   }

   /** Gets the encapsulated CMPIString.
   */
   inline CMPIString *getEnc() const
     { return (CMPIString*)enc; }
  public:

   /** Constructor - Empty string
   */
   CmpiString()
     { enc=NULL; }

   /** Constructor - Copy consrtuctor
   */
   CmpiString(const CmpiString& s);

   /** charPtr - get pointer to char* representation
   */
   inline const char* charPtr() const
     { if (getEnc()) return (const char*)getEnc()->hdl;
       else return NULL; }

   /** charPtr - Case sensitive compare
   */
   inline CmpiBoolean equals(const char *str) const
     { return (strcmp(charPtr(),str)==0); }
   inline CmpiBoolean equals(const CmpiString& str) const
     { return (strcmp(charPtr(),str.charPtr())==0); }

   /** charPtr - Case insensitive compare
   */
   inline CmpiBoolean equalsIgnoreCase(const char *str) const
     { return (strcasecmp(charPtr(),str)==0); }
   inline CmpiBoolean equalsIgnoreCase(const CmpiString& str) const
     { return (strcasecmp(charPtr(),str.charPtr())==0); }
};

#endif


