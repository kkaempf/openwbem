
/*
 *
 * CmpiString.h
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
 * Description: CMPI string provider wrapper
 */

#ifndef _CmpiString_h_
#define _CmpiString_h_

#include "cmpisrv.h"

class CmpiContext;
class CmpiResult;
class CmpiObjectPath;
class CmpiInstance;
class CmpiBroker;
class CmpiString;
class CmpiResult;
class CmpiContext;

class CmpiString {
  friend class CmpiBroker;
  friend class CmpiData;
  protected:
   CMPIString *enc;
  private:
  public:
   void makeGlobal(const CmpiString) {}
   CmpiString(const CmpiString& s) { enc=s.enc->ft->clone(s.enc,NULL); }
   CmpiString()              { enc=NULL; }
   CmpiString(CMPIString* c) { enc=c; }
   char* charPtr()           { if (enc) return (char*)enc->hdl; else return NULL; }
};

#endif


