
/*
 *
 * cmpiDateTime.cpp
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
 * Description: CMPIClass support
 *
 */

#include "cmpisrv.h"
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "OW_CIMDateTime.hpp"
#include "OW_CIMFwd.hpp"

static OW_CIMDateTime * makeCIMDateTime(time_t inTime, ulong usec, CMPIBoolean interval)
{
   OW_CIMDateTime * dt;
   char strTime[256];
   char utcOffset[20];
   char usTime[32];
   struct tm tmTime;

   localtime_r(&inTime,&tmTime);
   if (strftime(strTime,256,"%Y%m%d%H%M%S.",&tmTime)) {
      snprintf(usTime,32,"%6.6ld",usec);
      strcat(strTime,usTime);
      if (interval) strcpy(utcOffset,":000");
      else {
//#if defined (PEGASUS_PLATFORM_LINUX_IX86_GNU)
      snprintf(utcOffset,20,"%+4.3ld",tmTime.tm_gmtoff/60);
//#else
//      snprintf(utcOffset,20,"%+4.3ld",0);
//#endif
      }
      strncat(strTime,utcOffset,256);
      dt = new OW_CIMDateTime(OW_String(strTime));
      //cout<<"dt = " <<dt->toString()<<endl;
   }
   return dt;
}

CMPIDateTime *newDateTime() {
   struct timeval tv;
   struct timezone tz;
   gettimeofday(&tv,&tz);
   return (CMPIDateTime*)new CMPI_Object(makeCIMDateTime(tv.tv_sec,tv.tv_usec,0));
}

CMPIDateTime *newDateTime(CMPIUint64 tim, CMPIBoolean interval) {
   return (CMPIDateTime*)new CMPI_Object(makeCIMDateTime(tim/1000000,tim%1000000,interval));
}

CMPIDateTime *newDateTime(char *strTime) {
   //OW_CIMDateTime *dt=new OW_CIMDateTime();
   //*dt=String(strTime);
   OW_CIMDateTime *dt=new OW_CIMDateTime(OW_String(strTime));
   return (CMPIDateTime*)new CMPI_Object(dt);
}

static CMPIStatus dtRelease(CMPIDateTime* eDt) {
   //cout<<"--- dtRelease()"<<endl;
   OW_CIMDateTime* dt=(OW_CIMDateTime*)eDt->hdl;
   if (dt) {
      delete dt;
      ((CMPI_Object*)eDt)->unlinkAndDelete();
   }
   CMReturn(CMPI_RC_OK);
}

static CMPIDateTime* dtClone(CMPIDateTime* eDt, CMPIStatus* rc) {
   OW_CIMDateTime * dt=(OW_CIMDateTime*)eDt->hdl;
   //OW_CIMDateTime * cDt=new OW_CIMDateTime(dt->toString());
   OW_CIMDateTime * cDt = new OW_CIMDateTime(*dt);

   CMPIDateTime* neDt=(CMPIDateTime*)new CMPI_Object(cDt,CMPI_DateTime_Ftab);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return neDt;
}

static CMPIBoolean dtIsInterval(CMPIDateTime* eDt, CMPIStatus* rc) {
   OW_CIMDateTime* dt=(OW_CIMDateTime*)eDt->hdl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return dt->isInterval();
}

static CMPIString *dtGetStringFormat(CMPIDateTime* eDt, CMPIStatus* rc) {
   OW_CIMDateTime* dt=(OW_CIMDateTime*)eDt->hdl;
   CMPIString *str=(CMPIString*)new CMPI_Object(dt->toString());
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return str;
}

static CMPIUint64 dtGetBinaryFormat(CMPIDateTime* eDt, CMPIStatus* rc) {
   (void) rc;
   OW_CIMDateTime* dt=(OW_CIMDateTime*)eDt->hdl;
   CMPIUint64 days,hours,mins,secs,usecs,utc,lTime;
   struct tm tm,tmt;
   //CString tStr=dt->toString().getCString();
   //const char * tStr=dt->toString().c_str();
   //char *cStr=strdup((const char*)tStr);
   char *cStr=strdup(dt->toString().c_str());

   if (dt->isInterval()) {
      cStr[21]=0;
      usecs=atoi(cStr+15);
      cStr[15]=0;
      secs=atoi(cStr+12);
      cStr[12]=0;
      mins=atoi(cStr+10);
      cStr[10]=0;
      hours=atoi(cStr+8);
      cStr[8]=0;
      days=atoi(cStr);
      lTime=(days*(OW_UInt64)(86400000000LL))+
            (hours*(OW_UInt64)(3600000000LL))+
            (mins*60000000)+(secs*1000000)+usecs;
      //lTime=(days*PEGASUS_UINT64_LITERAL(86400000000))+
      //      (hours*PEGASUS_UINT64_LITERAL(3600000000))+
      //      (mins*60000000)+(secs*1000000)+usecs;
   }

   else {
      time_t tt=time(NULL);
      localtime_r(&tt,&tmt);
      memset(&tm,0,sizeof(tm));
      tm.tm_isdst=tmt.tm_isdst;
      utc=atoi(cStr+21);
      cStr[21]=0;
      usecs=atoi(cStr+15);
      cStr[15]=0;
      tm.tm_sec=atoi(cStr+12);
      cStr[12]=0;
      tm.tm_min=atoi(cStr+10);
      cStr[10]=0;
      tm.tm_hour=atoi(cStr+8);
      cStr[8]=0;
      tm.tm_mday=atoi(cStr+6);
      cStr[6]=0;
      tm.tm_mon=(atoi(cStr+4)-1);
      cStr[4]=0;
      tm.tm_year=(atoi(cStr)-1900);

      lTime=mktime(&tm);
      lTime*=1000000;
      lTime+=usecs;
   }

   return lTime;
}

static CMPIDateTimeFT dateTime_FT={
     CMPICurrentVersion,
     dtRelease,
     dtClone,
     NULL,
     dtGetBinaryFormat,
     dtGetStringFormat,
     dtIsInterval,
};

CMPIDateTimeFT *CMPI_DateTime_Ftab=&dateTime_FT;

