/*
 * mlogsup.h
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


#ifdef __cplusplus
extern "C" {
#endif

typedef struct mLogHeader {
   int max,cur;
   void* area[1];
} MLogHeader;


typedef struct mLogIndex {
   int max,free;
   union {
      MLogHeader* hdr[1];
      unsigned int nextFree[1];
   } entry;
} MLogIndex;


extern int makeMLog();
extern void* addToMLog(int idx,void* area);
extern void freeMLog(int idx);
extern void freeMLogAll();

#ifdef __cplusplus
}
#endif



