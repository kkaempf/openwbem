/*
 *
 * CmpiBaseMI.h
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
 * Description: CMPI C++ instance provider wrapper
 *
 */

#ifndef _CmpiBaseMI_h_
#define _CmpiBaseMI_h_

#include "OW_config.h"

#include <iostream>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "CmpiProviderBase.h"

extern "C" CMPIInstanceMIFT CMPICppInstMIFT;
extern "C" CMPIMethodMIFT CMPICppMethMIFT;
class CmpiContext;
class CmpiBroker;
class CmpiStatus;

class CmpiBaseMI {
  protected:
   CMPIBroker *broker;
  public:
   virtual ~CmpiBaseMI() {}
   inline CmpiBaseMI(CMPIBroker *mbp, const CmpiContext& ctx) {
      broker=mbp;
   }

  static CMPIStatus doDriveBaseCleanup
      (void* mi, CMPIContext* eCtx);
  inline static CMPIStatus driveBaseCleanup
      (CMPIInstanceMI* mi, CMPIContext* eCtx) {
         if (CmpiProviderBase::testAndSetOneTime(1))
	    return doDriveBaseCleanup(mi,eCtx);
	 CMReturn(CMPI_RC_OK);
   }

   virtual CmpiStatus initialize(const CmpiContext& ctx);
   virtual CmpiStatus cleanup(CmpiContext& ctx);
};

#endif
