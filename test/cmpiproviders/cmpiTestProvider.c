/*
 *
 * cmpiTestPrivider.c
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
 * Description: CMPI test provider
 *
 */


#define CMStringCharPtr(s) ((char*)((s)->hdl))

#include <time.h>
#include <string.h>
#include <pthread.h>
#include "cmpidt.h"
#include "cmpift.h"

int dataNext=0;
typedef struct data {
   CMPIString *key;
   CMPIString *data;
   CMPIArray *array;
} Data;

Data store[64];

CMPIBroker* broker;

int addToStore(CMPIString *k, CMPIString *d, CMPIArray *a)
{
   int i;
   CMPIStatus rc;
   for (i=0; i<dataNext; i++)
      if (strcmp(CMStringCharPtr(k),CMStringCharPtr(store[i].key))==0) return 0;
   store[dataNext].key=k->ft->clone(k,&rc);
   store[dataNext].data=d->ft->clone(d,&rc);
   store[dataNext++].array=a->ft->clone(a,&rc);
   return 1;
}

struct data* getFromStore(CMPIString *k)
{
   int i;
   for (i=0; i<dataNext; i++)
      if (strcmp(CMStringCharPtr(k),CMStringCharPtr(store[i].key))==0) return &store[i];
   return NULL;
}

int remFromStore(CMPIString *k)
{
   int i;
   for (i=0; i<dataNext; i++)
     if (strcmp(CMStringCharPtr(k),CMStringCharPtr(store[i].key))==0) {
	for (; i<dataNext; i++) {
	  store[i].key=store[i+1].key;
	  store[i].data=store[i+1].data;
	  store[i].array=store[i+1].array;
	}
	--dataNext;
	return 1;
     }
   return 0;
}



CMPIStatus testProvCleanup
                (CMPIInstanceMI* cThis, CMPIContext *ctx) {
   (void) cThis;
   (void) ctx;

   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvEnumInstanceNames
                (CMPIInstanceMI* cThis, CMPIContext* ctx,CMPIResult* rslt,
                 CMPIObjectPath* ref) {
   int i;
   CMPIStatus rc;
   CMPIObjectPath *cop, *copClone;
   CMPIString *k;
   (void) cThis;
   (void) ctx;

   fprintf(stderr,"+++ testProvEnumInstanceNames()\n");

   cop=broker->eft->newObjectPath(broker,
	 CMStringCharPtr(ref->ft->getClassName(ref,&rc)),
	 CMStringCharPtr(ref->ft->getNameSpace(ref,&rc)),
	 &rc);

   for (i=0; i<dataNext; i++) {
      copClone=cop->ft->clone(cop,&rc);
      k=store[i].key;
      copClone->ft->addKey(copClone,"Identifier",(CMPIValue*)&k,CMPI_string);
      rslt->ft->returnObjectPath(rslt,copClone);
   }
   rslt->ft->returnDone(rslt);

   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvEnumInstances
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* ref, char** props) {
   int i;
   CMPIStatus rc;
   CMPIObjectPath *cop, *copClone;
   CMPIInstance * inst;
   (void) cThis;
   (void) ctx;
   (void) props;

   fprintf(stderr,"+++ testProvEnumInstances()\n");
   cop=broker->eft->newObjectPath(broker,
	CMStringCharPtr(ref->ft->getClassName(ref,&rc)),
	CMStringCharPtr(ref->ft->getNameSpace(ref,&rc)),
	&rc);
   for (i=0; i<dataNext; i++) {
	  CMPIString * csh = 0;
      copClone=cop->ft->clone(cop,&rc);
      copClone->ft->addKey(copClone,"Identifier", (CMPIValue*)&store[i].key,CMPI_string);
      csh = copClone->ft->getClassName(cop,&rc);
      csh = (CMPIString *)NULL;
      inst=broker->eft->newInstance(broker,copClone,&rc);
      inst->ft->setProperty(inst,"Identifier",(CMPIValue*)&store[i].key,CMPI_string);
      inst->ft->setProperty(inst,"data",(CMPIValue*)&store[i].data,CMPI_string);
      rslt->ft->returnInstance(rslt,inst);
   }
   rslt->ft->returnDone(rslt);
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvGetInstance
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* cop, char** props) {
   CMPIString *k;
   struct data *dat;
   CMPIStatus rc;
   CMPIInstance *inst;

   (void) cThis;
   (void) ctx;
   (void) props;

   k=cop->ft->getKey(cop,"Identifier",&rc).value.string;
   if (!(k->hdl))
   fprintf(stderr,"+++ testProvGetInstance() %s as %d\n",CMStringCharPtr(k),
      (int)(void*)pthread_self());

   if ((dat=getFromStore(k))!=NULL) {
      inst=broker->eft->newInstance(broker,cop,&rc);
      inst->ft->setProperty(inst,"Identifier",(CMPIValue*)&k,CMPI_string);
      inst->ft->setProperty(inst,"data",(CMPIValue*)&dat->data,CMPI_string);
      inst->ft->setProperty(inst,"array",(CMPIValue*)&dat->array,CMPI_stringA);
      rslt->ft->returnInstance(rslt,inst);
   }
   else CMReturn(CMPI_RC_ERR_NOT_FOUND);

   rslt->ft->returnDone(rslt);
   fprintf(stderr,"+++ testProvGetInstance() done\n");
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvCreateInstance
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* cop, CMPIInstance* inst) {
   CMPIString *k,*d;
   CMPIArray *a;
   CMPIStatus rc;
   //CMPIData data;
   void * _hdl;

   (void) cThis;
   (void) ctx;

   fprintf(stderr,"+++ testProvCreateInstance()\n");

   _hdl = inst->hdl;
   k=inst->ft->getProperty(inst,"Identifier",&rc).value.string;
   d=inst->ft->getProperty(inst,"data",&rc).value.string;
   a=inst->ft->getProperty(inst,"array",&rc).value.array;

   if (addToStore(k,d,a)==0){
      fprintf(stderr,"+++ testProvCreateInstance() already exists\n");
      CMReturn(CMPI_RC_ERR_ALREADY_EXISTS);
   }

   rslt->ft->returnObjectPath(rslt,cop);
   rslt->ft->returnDone(rslt);
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvSetInstance
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* cop, CMPIInstance* inst, char ** msg) {
   (void) cThis;
   (void) ctx;
   (void) rslt;
   (void) cop;
   (void) inst;
   (void) msg;

   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvDeleteInstance
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* cop) {
   CMPIString *k;
   CMPIStatus rc;
   //CMPIObjectPath *nx;
   CMPIInstance *inst;

   (void) cThis;
   (void) rslt;

   fprintf(stderr,"+++ testProvDeleteInstance()\n");

   fprintf(stderr,"--- trying 1 ...as %d\n",(int)(void*)pthread_self());
//   nx=broker->eft->newObjectPath(broker,"xyz",
//      CMStringCharPtr(cop->ft->getNameSpace(cop,&rc)),&rc);
//   printf("--- trying 2 ...\n");
   inst=broker->bft->getInstance(broker,ctx,cop,NULL,&rc);
   fprintf(stderr,"--- thru ...\n");

   k=cop->ft->getKey(cop,"Identifier",&rc).value.string;
   if (remFromStore(k)) { CMSetStatus(&rc,CMPI_RC_OK); }
   else CMSetStatus(&rc,CMPI_RC_ERR_NOT_FOUND);
   k->ft->release(k);
   return rc;
}

CMPIStatus testProvExecQuery
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* cop, char* lang, char* query) {
   (void) cThis;
   (void) ctx;
   (void) rslt;
   (void) cop;
   (void) lang;
   (void) query;

   CMReturn(CMPI_RC_OK);
}



//----------------------------------------------------------
//---
//	Provider Factory
//---
//----------------------------------------------------------


static CMPIInstanceMIFT instMIFT={
   CMPICurrentVersion,
   CMPICurrentVersion,
   "instanceTestProvider",
   testProvCleanup,
   testProvEnumInstanceNames,
   testProvEnumInstances,
   testProvGetInstance,
   testProvCreateInstance,
   testProvSetInstance,
   testProvDeleteInstance,
   testProvExecQuery,
};

CMPIInstanceMI* cmpiTestProvider_Create_InstanceMI(CMPIBroker* brkr, CMPIContext *ctx) {
   static CMPIInstanceMI mi={
      NULL,
      &instMIFT
      //,NULL
   };
   (void) ctx;
   broker=brkr;
   fprintf(stderr,"+++ TestProvider_Create_InstanceMI(): called\n");
   return &mi;
}

int preloadTestProvider()
{
   return 5;
}




