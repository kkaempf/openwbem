
/*
 *
 * cmpisrv.cpp
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
 * Description: CMPI internal services header
 *
 */


#ifndef _CMPITHREADCONTEXT_H_
#define _CMPITHREADCONTEXT_H_
#include "OW_config.h"

#include <stdlib.h>
#include <iostream>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpisrv.h"


class CMPI_ThreadContext {
   static ulong hiKey;
   ulong key;
   CMPI_ThreadContext* prev;
   CMPI_Object *CIMfirst,*CIMlast;
   void add(CMPI_Object *o);
   void remove(CMPI_Object *o);
   void setThreadContext();
   void setContext();
   static CMPI_ThreadContext* getContext(ulong *kp=NULL);
  public:
   static void addObject(CMPI_Object*);
   static void remObject(CMPI_Object*);
   static CMPI_ThreadContext* getThreadContext();
   CMPI_ThreadContext();
   ~CMPI_ThreadContext();
};

#endif
