
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
#include <pthread.h>
#include <iostream>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpisrv.h"


class CMPI_ThreadContext {
   static pthread_key_t theKey;
   CMPI_ThreadContext* m_prev;
   CMPI_Object *CIMfirst,*CIMlast;

   CMPIBroker * broker;
   CMPIContext * context;

   void add(CMPI_Object *o);
   void remove(CMPI_Object *o);
   void setThreadContext();
   void setContext();

  public:
   static void addObject(CMPI_Object*);
   static void remObject(CMPI_Object*);

   static CMPI_ThreadContext* getThreadContext();
   static CMPIBroker* getBroker();
   static CMPIContext* getContext();
   CMPI_ThreadContext();
   CMPI_ThreadContext(CMPIBroker*,CMPIContext*);
   ~CMPI_ThreadContext();

   friend void initializeTheKey()
   {
	   pthread_key_create(&CMPI_ThreadContext::theKey,NULL);
   }

};

#endif
