
/*
 *
 * CmpiInstance.h
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
 * Description: CMPI C++ Instance wrapper
 *
 */

#ifndef _CmpiInstance_h_
#define _CmpiInstance_h_

#include "cmpisrv.h"

#include "CmpiObjectPath.h"

class CmpiInstance {
   friend class CmpiBroker;
   friend class CmpiResult;
 protected:
  CMPIInstance *enc;
  private:
   CmpiInstance() {}
  public:
   void makeGlobal() {}
   CmpiInstance(CMPIInstance* enc)  { this->enc=enc; }
   CmpiData getProperty(char* name) {
      CmpiData d;
      CMPIrc rc;
      d.data=enc->ft->getProperty(enc,name,&d.type,&rc);
      if (rc!=CMPI_RC_OK) throw rc;
      return d;
   }
   CmpiData getProperty(int pos, CmpiString *name=NULL) {
      CmpiData d;
      CMPIrc rc;
      CMPIString *s;
      d.data=enc->ft->getPropertyAt(enc,pos,&s,&d.type,&rc);
      if (rc!=CMPI_RC_OK) throw rc;
      if (name) *name=*(new CmpiString(s));
      return d;
   };
   unsigned int getPropertyCount() {
      CMPIrc rc;
      unsigned int c=enc->ft->getPropertyCount(enc,&rc);
      if (rc!=CMPI_RC_OK) throw rc;
      return c;
   }
   void setProperty(char* name, CmpiData data) {
      CMPIrc rc=enc->ft->setProperty(enc,name,&data.data,data.type,data.count);
      if (rc!=CMPI_RC_OK) throw rc;
   }
   CmpiObjectPath getObjectPath() {
      CMPIrc rc;
      CmpiObjectPath cop(enc->ft->getObjectPath(enc,&rc));
      if (rc!=CMPI_RC_OK) throw rc;
      return cop;
   }
};

#endif



