/*
 * mlogsup.c
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author: Adrian Schuur <schuur@de.ibm.com>
 * Contributors:
 *
 * Description: Functions to log and free malloc'ed areas.
 */



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "mlogsup.h"

#define HDRS 64
#define IDXS 32

char debug=1;

static MLogHeader *freeHdrs=NULL;
static MLogIndex *lindex=NULL;
static pthread_mutex_t mutex;

static void newIndex()
{
   MLogIndex *ix=NULL;
   int i,l=sizeof(MLogIndex)+((IDXS-1)*sizeof(void*));

   if (debug) printf("--- newIndex()\n");
   ix=(MLogIndex*)malloc(l);
   for (i=0; i<IDXS; i++) ix->entry.nextFree[i]=i+1;
   ix->entry.nextFree[IDXS-1]=IDXS+2;
   ix->free=0;
   ix->max=IDXS;
   lindex=ix;
}

static int growMLogIndex()
{
   MLogIndex *ix=NULL;
   int i,ol=sizeof(MLogIndex)+((lindex->max-1)*sizeof(MLogHeader*));
   int nl=sizeof(MLogIndex)+(((lindex->max*2)-1)*sizeof(MLogHeader*));

   ix=(MLogIndex*)malloc(nl);
   memcpy(ix,lindex,ol);
   ix->max=lindex->max*2;
   for (i=lindex->max; i<ix->max; i++) ix->entry.nextFree[i]=i+1;
   ix->entry.nextFree[ix->max-1]=IDXS+2;
   ix->entry.nextFree[ix->free]=lindex->max;
   free(lindex);
   lindex=ix;
   return lindex->free;
}

static MLogHeader *newMLogHeader(int s)
{
   MLogHeader *ah;
   int l;

   if (lindex==NULL) newIndex();

   if (freeHdrs) {
      ah=freeHdrs;
      freeHdrs=ah->area[0];
      ah->cur=0;
      if (debug) printf("--- newMLogHeader() reuse: %p\n",ah);
   }
   else {
      l=sizeof(MLogHeader)+((s-1)*sizeof(void*));
      ah=(MLogHeader*)malloc(l);
      memset(ah,0,l);
      ah->max=s;
      if (debug) if (debug) printf("--- newMLogHeader() new: %p\n",ah);
   }
   ah->cur=0;
   return ah;
}

static MLogHeader *growMLogHeader(MLogHeader *ah)
{
   MLogHeader *an;
   int nl=sizeof(MLogHeader)+(((ah->max*2)-1)*sizeof(void*));

   an=(MLogHeader*)realloc(ah,nl);
   an->max*=2;
   if (debug) printf("--- growMLogHeader old: %p new: %p\n",ah,an);
   return an;
}

static void freeMLogHeader(MLogHeader *ah)
{
   ah->area[0]=freeHdrs;
   freeHdrs=ah;
}

int makeMLog()
{
   int idx;
   MLogHeader *ah=newMLogHeader(HDRS);

   idx=lindex->free;
   if (idx==IDXS+2) idx=growMLogIndex();
   lindex->free=lindex->entry.nextFree[idx];
   lindex->entry.hdr[idx]=ah;
   if (debug) printf("--- makeMLog() id: %d\n",idx);
   return idx;
}

void *addToMLog(int idx, void *area)
{
   MLogHeader *ah;

   pthread_mutex_lock(&mutex);

   ah=lindex->entry.hdr[idx];
   if (ah->cur>=ah->max)
      ah=lindex->entry.hdr[idx]=growMLogHeader(ah);
   if (debug) printf("--- addToMLog() id: %d area: %p\n",idx,area);
   ah->area[ah->cur++]=area;

   pthread_mutex_unlock(&mutex);

   return area;
}

void freeMLog(int idx)
{
   int i;
   MLogHeader *ah;

   pthread_mutex_lock(&mutex);

   ah=lindex->entry.hdr[idx];
   if (debug) printf("--- freeMLog() id: %d cur: %d max: %d\n",idx,ah->cur,ah->max);
   for (i=ah->cur-1; i>=0; i--) {
      if (debug) if (debug) printf("--- freeing %p\n",ah->area[i]);
      free(ah->area[i]);
   }
   lindex->entry.nextFree[idx]=lindex->free;
   lindex->free=idx;
   freeMLogHeader(ah);

   pthread_mutex_unlock(&mutex);
}

void freeMLogAll()
{
   int i;

   pthread_mutex_lock(&mutex);

   if (debug) printf("--- freeMLogALL()\n");
   if (lindex==NULL) return;

   for (i=0; i<IDXS; i++) {
 //     if (debug) printf("--- trying %d - %d\n",i,index->entry.nextFree[i]);
      if (lindex->entry.nextFree[i]>IDXS+2) freeMLog(i);
   }

   free(lindex);
   lindex=NULL;

   pthread_mutex_unlock(&mutex);
}


/*   simple usage scenario
int main()
{
   int a,b;
   void *p;

   a=makeMLog();                // make a malloc logger a
   freeMLogAll();               // free all loggers and cleanup index
   a=makeMLog();                // make a malloc logger a
   addToMLog(a,p=malloc(32));   // add malloc'ed areas ...
   addToMLog(a,p=malloc(32));
   addToMLog(a,p=malloc(32));

   b=makeMLog();                // make an additonal logger b
   addToMLog(b,p=malloc(32));   // add malloc'ed areas ...
   addToMLog(b,p=malloc(32));
   addToMLog(b,p=malloc(32));
   freeMLog(b);                 // free malloc'ed areas logged in b and logger b

   b=makeMLog();                // make a new logger
   addToMLog(b,p=malloc(32));   // add malloc'ed areas
   addToMLog(b,p=malloc(32));
   addToMLog(b,p=malloc(32));

   freeMLog(a);                 // free all malloc'ed areas in a and logger a

   freeMLogAll();               // free all malloc'ed areas, loggers and index
                                // be carefull in using this....
}
*/
