
/*
 *
 * CmpiBroker.h
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
 * Description: CMPI C++ magement broker wrapper
 *
 */

#ifndef _CmpiBroker_h_
#define _CmpiBroker_h_

#include "cmpisrv.h"

#include "CmpiString.h"
#include "CmpiData.h"
#include "CmpiObjectPath.h"
#include "CmpiInstance.h"
#include "CmpiContext.h"

class CmpiBroker {
  protected:
   CMPIBroker *enc;
  private:
   CmpiBroker() {}
  public:
   CmpiBroker(CMPIBroker* b) { enc=b; }
   CmpiInstance newInstance(CmpiObjectPath cop) {
      CMPIrc rc;
      CMPIInstance *inst=enc->eft->newInstance(enc,cop.enc,&rc);
      if (rc!=CMPI_RC_OK) throw rc;
      return CmpiInstance(inst);
   }
};

#endif



