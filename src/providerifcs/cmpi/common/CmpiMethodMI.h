
/*
 *
 * CmpiMethodMI.h
 *
 * Copyright (c) 2002, International Business Machines
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
 * Description: CMPI C++ method provider wrapper
 *
 */

#ifndef _CmpiMethodMI_h_
#define _CmpiMethodMI_h_

#include <iostream>

#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiBaseMI.h"
#include "CmpiStatus.h"
#include "CmpiObjectPath.h"
#include "CmpiResult.h"
#include "CmpiContext.h"
#include "CmpiBroker.h"
#include "CmpiArgs.h"


class CmpiMethodMI : public CmpiBaseMI {
  protected:
  public:
   virtual ~CmpiMethodMI() {}
   CmpiMethodMI(CMPIBroker *mbp, const CmpiContext& ctx)
      : CmpiBaseMI(mbp,ctx) {}
   virtual CmpiStatus invokeMethod
              (const CmpiContext& ctx, CmpiResult& rslt,
	       const CmpiObjectPath& ref, const char* methodName,
	       const CmpiArgs& in, CmpiArgs& out);
};

#endif

