
/*
 *
 * cmpiThreadContext.cpp
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
 * Description: CMPI Thread Context support
 *
 */

#include "cmpisrv.h"
#include <pthread.h>
#include <limits.h>


ulong CMPI_ThreadContext::hiKey=PTHREAD_KEYS_MAX+1;

CMPI_ThreadContext* CMPI_ThreadContext::getContext(ulong *kp) {
   int i;
   ulong k;
   for (i=hiKey,k=0; i>=0 && ((k)&0xffffc00)!=0xffffc00; i--)
       k=(ulong)pthread_getspecific(i);
   if (i<0) return NULL;
   k&=0x3ff;
   if (kp) *kp=k;
   return (CMPI_ThreadContext*)pthread_getspecific(k);
}

void CMPI_ThreadContext::setContext() {
   pthread_key_t k;
   pthread_key_create(&k,NULL);
   pthread_key_create((pthread_key_t*)&key,NULL);
   pthread_setspecific(k,this);
   pthread_setspecific((pthread_key_t)key,(void*)(0xfffffc00|k));
}


void CMPI_ThreadContext::setThreadContext() {
   ulong k;
   CMPI_ThreadContext* prevCtx;

   if (hiKey>PTHREAD_KEYS_MAX) {
      prev=NULL;
      setContext();
      hiKey=key;
      // cout<<"--- setThreadContext(1) key: "<<key<<endl;
      return;
   }
   if ((prevCtx=getContext(&k))!=NULL) {
      prev=prevCtx;
      pthread_setspecific(k,this);
      return;
   }
   setContext();
   prev=NULL;
   if (hiKey<key) hiKey=key;
   //cout<<"--- setThreadContext(2) key: "<<key<<endl;
   return;
}

void CMPI_ThreadContext::add(CMPI_Object *o) {
   ENQ_TOP_LIST(o,CIMfirst,CIMlast,next,prev);
}

void CMPI_ThreadContext::addObject(CMPI_Object* o) {
   CMPI_ThreadContext* ctx=getContext();
   ctx->add(o);
}

void CMPI_ThreadContext::remove(CMPI_Object *o) {
   DEQ_FROM_LIST(o,CIMfirst,CIMlast,next,prev);
}

void CMPI_ThreadContext::remObject(CMPI_Object* o) {
   CMPI_ThreadContext* ctx=getContext();
   ctx->remove(o);
}

CMPI_ThreadContext* CMPI_ThreadContext::getThreadContext() {
   return NULL;
}

CMPI_ThreadContext::CMPI_ThreadContext() {
   CIMfirst=CIMlast=NULL;
   setThreadContext();
}

CMPI_ThreadContext::~CMPI_ThreadContext() {

   for (CMPI_Object *nxt,*cur=CIMfirst; cur; cur=nxt) {
      nxt=cur->next;
      ((CMPIInstance*)cur)->ft->release((CMPIInstance*)cur);
   }

   ulong k=(ulong)pthread_getspecific(key);
   if (prev!=NULL) {
      pthread_setspecific(k&0x3ff,prev);
      return;
   }
   pthread_key_delete(key);
   pthread_key_delete(k&0x3ff);
}
