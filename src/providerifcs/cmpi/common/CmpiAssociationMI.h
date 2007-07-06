
/*
 *
 * CmpiAssociationMI.h
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
 * Description: CMPI C++ association provider wrapper
 *
 */

#ifndef _CmpiAssociationMI_h_
#define _CmpiAssociationMI_h_

#include "OW_config.h"

#include <iostream>

#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiBaseMI.h"
#include "CmpiStatus.h"

class CmpiObjectPath;
class CmpiResult;
class CmpiContext;

class CmpiAssociationMI : public CmpiBaseMI {
  protected:
  public:
   virtual ~CmpiAssociationMI() {}
   CmpiAssociationMI(CMPIBroker *mbp, const CmpiContext& ctx)
      : CmpiBaseMI(mbp,ctx) {}

   static CMPIStatus driveAssociators
      (CMPIAssociationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, char* asscClass, char* resultClass,
       char* role, char* resultRole, const char** properties);
   static CMPIStatus driveAssociatorNames
      (CMPIAssociationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, char* assocClass, char* resultClass,
       char* role, char* resultRole);
   static CMPIStatus driveReferences
      (CMPIAssociationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, char* resultClass, char* role ,
       const char** properties);
   static CMPIStatus driveReferenceNames
      (CMPIAssociationMI*, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, char* resultClass, char* role);

   virtual CmpiStatus associators
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* asscClass, const char* resultClass,
       const char* role, const char* resultRole, const char** properties);
   virtual CmpiStatus associatorNames
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* assocClass, const char* resultClass,
       const char* role, const char* resultRole);
   virtual CmpiStatus references
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* resultClass, const char* role ,
       const char** properties);
   virtual CmpiStatus referenceNames
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* resultClass, const char* role);
};

#endif

