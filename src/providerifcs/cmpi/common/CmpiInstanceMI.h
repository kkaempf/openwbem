
/*
 *
 * CmpiInstanceMI.h
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

#ifndef _CmpiInstanceMI_h_
#define _CmpiInstanceMI_h_

#include <iostream>

#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiBaseMI.h"
#include "CmpiStatus.h"
#include "CmpiString.h"
#include "CmpiData.h"
#include "CmpiObjectPath.h"
#include "CmpiInstance.h"
#include "CmpiResult.h"
#include "CmpiContext.h"
#include "CmpiBroker.h"


class CmpiInstanceMI : public CmpiBaseMI {
  protected:
  public:
   virtual ~CmpiInstanceMI() {}
   CmpiInstanceMI(CMPIBroker *mbp, const CmpiContext& ctx)
      : CmpiBaseMI(mbp,ctx) {}
   virtual CmpiStatus enumInstanceNames
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop);
   virtual CmpiStatus enumInstances
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop,
	       char* *properties);
   virtual CmpiStatus getInstance
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop,
	       char* *properties);
   virtual CmpiStatus createInstance
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop,
               CmpiInstance& inst);
   virtual CmpiStatus setInstance
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop,
               CmpiInstance& inst, char* *properties);
   virtual CmpiStatus deleteInstance
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop);
   virtual CmpiStatus execQuery
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop,
               char* language, char* query);
};

#endif









