
/*
 *
 * CmpiArray.h
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
 * Description: CMPI C++ helper class
 *
 */

#ifndef _CmpiArray_h_
#define _CmpiArray_h_

#include "OW_config.h"
#include "cmpidt.h"
#include "CmpiProviderBase.h"
#include "CmpiObject.h"

class CmpiString;

class CmpiArrayIdx {
  friend class CmpiArray;
   CmpiArray &ar;
   CMPICount idx;
   CmpiArrayIdx(CmpiArray &a, CMPICount i)
     : ar(a), idx(i) {}
  public:
   void operator=(const CmpiString);
   void operator=(const char*);

   void operator=(const CMPISint8);
   void operator=(const CMPISint16);
   void operator=(const CMPISint32);
   void operator=(const int);
   void operator=(const CMPISint64);

   void operator=(const CMPIUint8);
   void operator=(const CMPIUint16);
   void operator=(const CMPIUint32);
   void operator=(const unsigned int);
   void operator=(const CMPIUint64);

   void operator>>(CmpiString&);

   void operator>>(CMPISint8&);
   void operator>>(CMPISint16&);
   void operator>>(CMPISint32&);
   void operator>>(int&);
   void operator>>(CMPISint64&);

   void operator>>(CMPIUint8&);
   void operator>>(CMPIUint16&);
   void operator>>(CMPIUint32&);
   void operator>>(unsigned int&);
   void operator>>(CMPIUint64&);
};


/** This class wraps a CMPIData value array.
      Index operations use the [] operator.
      Data extraction uses the >> operator.
      Extraction operations can be appended to an array indexing operation
      like this:

        CmpiString state;

	CmpiArray states;

        ci.getProperty("States")>>states;

	states[3]>>state;

	Assignment statements use array indexing operations as well:

	states[5]="offline";

       Type mismatches will be signalled by exceptions.
*/
class CmpiArray : public CmpiObject {
  friend class CmpiArrayIdx;
   void operator=(int x) {}

   /** Gets the encapsulated CMPIArray.
   */
   inline CMPIArray *getEnc() const
      { return (CMPIArray*)enc; }
   void *makeArray(CMPIBroker *mb,CMPICount max, CMPIType type);
  public:
   inline CmpiArray(CMPICount max, CMPIType type) {
      enc=makeArray(CmpiProviderBase::getBroker(),max,type);
   }
   CmpiArray() {}
   CMPICount size();
   inline CmpiArrayIdx operator[](int idx) {
      return CmpiArrayIdx(*this,idx);
   }
};

#endif



