
/*
 *
 * CmpiPropertyMI.h
 *
 * (C) Copyright IBM Corp. 2002
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
 * Description: CMPI C++ property provider wrapper
 *
 */

#ifndef _CmpiPropertyMI_h_
#define _CmpiPropertyMI_h_

#include "OW_config.h"

#include <iostream>

#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiBaseMI.h"
#include "CmpiStatus.h"
#include "CmpiObjectPath.h"
#include "CmpiResult.h"
#include "CmpiContext.h"


class CmpiPropertyMI : public CmpiBaseMI {
  protected:
  public:
   virtual ~CmpiPropertyMI() {}
   CmpiPropertyMI(CMPIBroker *mbp, const CmpiContext& ctx)
      : CmpiBaseMI(mbp,ctx) {}

   static CMPIStatus driveSetProperty
      (CMPIPropertyMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, char* name, CMPIData data);
   static CMPIStatus driveGetProperty
      (CMPIPropertyMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, char* name);

   virtual CmpiStatus setProperty
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* name, const CmpiData& data);
   virtual CmpiStatus getProperty
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* name);
};

#endif

