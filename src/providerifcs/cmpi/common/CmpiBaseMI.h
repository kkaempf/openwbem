/*
 *
 * CmpiBaseMI.h
 *
 * Copyright (c) 2003, International Business Machines
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

#include <iostream>

#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiStatus.h"
#include "CmpiBroker.h"

class CmpiContext;

extern "C" CMPIInstanceMIFT CMPICppInstMIFT;
extern "C" CMPIMethodMIFT CMPICppMethMIFT;

class CmpiBaseMI {
  protected:
   static CMPIBroker *broker;
  public:
   static inline CMPIBroker *getBroker()
      { return broker; }
   virtual ~CmpiBaseMI() {}
   CmpiBaseMI(CMPIBroker *mbp, const CmpiContext& ctx)
      { broker=mbp; }
   virtual CmpiStatus initialize(const CmpiContext& ctx)
      { return CmpiStatus(CMPI_RC_OK); }
   virtual CmpiStatus cleanup(CmpiContext& ctx)
      { return CmpiStatus(CMPI_RC_OK); }
};

#endif
