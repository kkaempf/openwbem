
/*
 *
 * CmpiProviderBase.h
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
 * Description: CMPI C++ Provider Base class
 *              This class must be instatiated once per C++ CMPI style provider.
 *              It maintains the broker and oneTime variables on a per provider
 *              basis and allows CmpiImpl to be used as a DLL.
 *              Use the CMProviderBase macro instantiate this class and the 
 *              static variables.
 *
 */

#ifndef _CmpiProviderBase_h_
#define _CmpiProviderBase_h_

#include "OW_config.h"

#include "cmpidt.h"

class CmpiProviderBase;

class CmpiProviderBase {
   static CmpiProviderBase *base;
   CMPIBroker *broker;
   int oneTime;
  public:
   static CMPIBroker *getBroker() {
      return base->broker;
   }
   static void setBroker(CMPIBroker *mb) {
      base->broker=mb;
   }
   static int testAndSetOneTime(int t) {
      if (base->oneTime&t) return 0;
      base->oneTime|=t;
      return 1;
   }
   CmpiProviderBase() {
      base=this;
      broker=NULL;
      oneTime=0;
   }
};

#endif


