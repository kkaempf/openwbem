
/*
 *
 * CmpiInstanceMI.h
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
 * Description: CMPI C++ instance provider wrapper
 *
 */

#ifndef _CmpiInstanceMI_h_
#define _CmpiInstanceMI_h_

#include "cmpisrv.h"
#include "CmpiString.h"
#include "CmpiData.h"
#include "CmpiObjectPath.h"
#include "CmpiInstance.h"
#include "CmpiResult.h"
#include "CmpiContext.h"
#include "CmpiBroker.h"


extern "C" {
  CMPIrc driveCleanup
      (CMPIInstanceMI*);
  CMPIrc driveEnumInstanceNames
      (CMPIInstanceMI*,CMPIContext*,CMPIResult*,CMPIObjectPath*);
  CMPIrc driveEnumInstances
      (CMPIInstanceMI*,CMPIContext*,CMPIResult*,CMPIObjectPath*);
  CMPIrc driveGetInstance
      (CMPIInstanceMI*,CMPIContext*,CMPIResult*,CMPIObjectPath*);
  CMPIrc driveCreateInstance
      (CMPIInstanceMI*,CMPIContext*,CMPIResult*,CMPIObjectPath*,CMPIInstance*);
  CMPIrc driveSetInstance
      (CMPIInstanceMI*,CMPIContext*,CMPIResult*,CMPIObjectPath*,CMPIInstance*);
  CMPIrc driveDeleteInstance
      (CMPIInstanceMI*,CMPIContext*,CMPIResult*,CMPIObjectPath*);
  CMPIrc driveExecQuery
      (CMPIInstanceMI*,CMPIContext*,CMPIResult*,CMPIObjectPath*,char*,char*);
};


class CmpiInstanceMI {
  protected:
   CMPIInstanceMI *mi;
   CmpiInstanceMI() {}
   CmpiInstanceMI(CmpiInstanceMI&) {}
  private:
  public:
   CmpiBroker *mb;
   virtual ~CmpiInstanceMI() {}
   CmpiInstanceMI(CMPIInstanceMI* mi, CmpiBroker* mb) { this->mi=mi; this->mb=mb; }
   void initialize();
   void cleaup();
   virtual CMPIrc enumInstanceNames
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop)=0;
   virtual CMPIrc enumInstances
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop)=0;
   virtual CMPIrc getInstance
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop)=0;
   virtual CMPIrc createInstance
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop,
               CmpiInstance& inst)=0;
   virtual CMPIrc setInstance
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop,
               CmpiInstance& inst)=0;
   virtual CMPIrc deleteInstance
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop)=0;
   virtual CMPIrc execQuery
              (CmpiContext& ctx, CmpiResult& rslt, CmpiObjectPath& cop,
               char* language, char* query)=0;
};



#endif









