
/*
 *
 * CmpiObjectPath.h
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
 * Description: CMPI C++ ObjectPath wrapper
 *
 */

#ifndef _CmpiObjectPath_h_
#define _CmpiObjectPath_h_

#include "cmpisrv.h"

#include "CmpiData.h"

class CmpiObjectPath {
    friend class CmpiBroker;
    friend class CmpiResult;
   protected:
   CMPIObjectPath *enc;
  private:
   CmpiObjectPath() {}
  public:
   void makeGlobal() {};
   CmpiObjectPath(CMPIObjectPath* c) { enc=c; }
   CmpiString getNameSpace() {
      CMPIrc rc;
      CMPIString *s=enc->ft->getNameSpace(enc,&rc);
      if (rc!=CMPI_RC_OK) throw rc;
      return *new CmpiString(s);
   }
   void setNameSpace(CmpiString ns) { setNameSpace(ns.charPtr()); }
   void setNameSpace(char* ns) {
      CMPIrc rc=enc->ft->setNameSpace(enc,ns);
      if (rc!=CMPI_RC_OK) throw rc;
   }
   CmpiString getClassName() {
      CMPIrc rc;
      CMPIString *s=enc->ft->getClassName(enc,&rc);
      if (rc!=CMPI_RC_OK) throw rc;
      return *new CmpiString(s);
   }
   CmpiData getKey(char* name) {
      CmpiData d;
      CMPIrc rc;
      d.data=enc->ft->getKey(enc,name,&d.type,&rc);
      if (rc!=CMPI_RC_OK) throw rc;
      return d;
   }
   unsigned int getKeyCount() {
      CMPIrc rc;
      unsigned int c=enc->ft->getKeyCount(enc,&rc);
      if (rc!=CMPI_RC_OK) throw rc;
      return c;
   }
   CmpiData getKey(int pos, CmpiString *name=NULL) {
      CmpiData d;
      CMPIrc rc;
      CMPIString *s;
      d.data=enc->ft->getKeyAt(enc,pos,&s,&d.type,&rc);
      if (rc!=CMPI_RC_OK) throw rc;
      if (name) *name=*(new CmpiString(s));
      return d;
   };
   void addKey(char* name, CmpiData& data) {
      CMPIrc rc=enc->ft->addKey(enc,name,&data.data,data.type,data.count);
      if (rc!=CMPI_RC_OK) throw rc;
   }
};

#endif


