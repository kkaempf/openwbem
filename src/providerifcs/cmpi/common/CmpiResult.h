
/*
 *
 * CmpiResult.h
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
 * Description: CMPI C++ result wrapper
 *
 */

#ifndef _CmpiResult_h_
#define _CmpiResult_h_

#include "cmpisrv.h"

#include "CmpiData.h"
#include "CmpiInstance.h"
#include "CmpiObjectPath.h"

class CmpiResult {
  protected:
   CMPIResult *enc;
  private:
   CmpiResult() {}
  public:
   void makeGlobal() {}
   CmpiResult(CMPIResult* r) { enc=r; }
   void returnData(CmpiData d) {
      CMPIrc rc=enc->ft->returnData(enc,&d.data,d.type,d.count);
      if (rc!=CMPI_RC_OK) throw rc;
   }
   void returnData(CmpiInstance d) {
      CMPIrc rc=enc->ft->returnInstance(enc,d.enc);
      if (rc!=CMPI_RC_OK) throw rc;
   }
   void returnData(CmpiObjectPath d) {
      CMPIrc rc=enc->ft->returnObjectPath(enc,d.enc);
      if (rc!=CMPI_RC_OK) throw rc;
   }
   void returnDone() {
      CMPIrc rc=enc->ft->returnDone(enc);
      if (rc!=CMPI_RC_OK) throw rc;
   }
};

#endif


