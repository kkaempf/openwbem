
/*
 *
 * CmpiContext.h
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
 * Description: CMPI C++ context provider wrapper
 *
 */

#ifndef _CmpiContext_h_
#define _CmpiContext_h_

#include "cmpisrv.h"

#include "CmpiString.h"
#include "CmpiData.h"
#include "CmpiObjectPath.h"
#include "CmpiInstance.h"

class CmpiContext {
  protected:
   CMPIContext *enc;
  private:
   CmpiContext() {}
  public:
   void makeGlobal() {}
   CmpiContext(CMPIContext* r) { enc=r; }
};

#endif



