
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
 * 
 * Contributor:   Markus Mueller <sedgewick_de@yahoo.de>
 *
 * Description: CMPIClass support
 *
 */

//#include "OW_config.h"

#include "cmpisrv.h"
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "OW_CIMDateTime.hpp"
#include "OW_DateTime.hpp"
#include "OW_CIMFwd.hpp"

static OpenWBEM::CIMDateTime * makeCIMDateTime(
	time_t inTime, 
	unsigned long usec, 
	CMPIBoolean interval)
{
	if (interval)
	{
		OpenWBEM::UInt64 microsecs = inTime * 1000000;
		microsecs += usec;
		return new OpenWBEM::CIMDateTime(microsecs);
	}

	OpenWBEM::DateTime dt(inTime, OpenWBEM::UInt32(usec));
	return new OpenWBEM::CIMDateTime(dt);
/*
	OpenWBEM::CIMDateTime * dt;
	char strTime[256];
	char utcOffset[20];
	char usTime[32];
	struct tm tmTime;

	localtime_r(&inTime,&tmTime);
	if (strftime(strTime,256,"%Y%m%d%H%M%S.",&tmTime)) 
	{
		snprintf(usTime,32,"%6.6ld",usec);
		strcat(strTime,usTime);
		if (interval) 
			strcpy(utcOffset,":000");
		else 
		{
#if defined (OW_GNU_LINUX)
			snprintf(utcOffset,20,"%+4.3ld",tmTime.tm_gmtoff/60);
#else
			snprintf(utcOffset,20,"%+4.3ld",0L);
#endif
		}
		strncat(strTime,utcOffset,256);
		dt = new OpenWBEM::CIMDateTime(OpenWBEM::String(strTime));
		//cout<<"dt = " <<dt->toString()<<endl;
	}
	return dt;
*/
}

CMPIDateTime *newDateTime() 
{
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv,&tz);
	return (CMPIDateTime*)
		new CMPI_Object(makeCIMDateTime(tv.tv_sec,tv.tv_usec,0));
}

CMPIDateTime *newDateTime(CMPIUint64 tim, CMPIBoolean interval) 
{
	return (CMPIDateTime*)
		new CMPI_Object(makeCIMDateTime(tim/1000000,tim%1000000,interval));
}

CMPIDateTime *newDateTime(char *strTime) 
{
	OpenWBEM::CIMDateTime *dt=new OpenWBEM::CIMDateTime(OpenWBEM::String(strTime));
	return (CMPIDateTime*)new CMPI_Object(dt);
}

static CMPIStatus dtRelease(CMPIDateTime* eDt) 
{
	//cout<<"--- dtRelease()"<<endl;
	OpenWBEM::CIMDateTime* dt=(OpenWBEM::CIMDateTime*)eDt->hdl;
	if (dt) 
	{
		delete dt;
		((CMPI_Object*)eDt)->unlinkAndDelete();
	}
	CMReturn(CMPI_RC_OK);
}

static CMPIDateTime* dtClone(CMPIDateTime* eDt, CMPIStatus* rc) 
{
	OpenWBEM::CIMDateTime * dt=(OpenWBEM::CIMDateTime*)eDt->hdl;
	//OpenWBEM::CIMDateTime * cDt=new OpenWBEM::CIMDateTime(dt->toString());
	OpenWBEM::CIMDateTime * cDt = new OpenWBEM::CIMDateTime(*dt);

	CMPIDateTime* neDt=(CMPIDateTime*)new CMPI_Object(cDt,CMPI_DateTime_Ftab);
	CMSetStatus(rc,CMPI_RC_OK);
	return neDt;
}

static CMPIBoolean dtIsInterval(CMPIDateTime* eDt, CMPIStatus* rc) 
{
	OpenWBEM::CIMDateTime* dt=(OpenWBEM::CIMDateTime*)eDt->hdl;
	CMSetStatus(rc,CMPI_RC_OK);
	return dt->isInterval();
}

static CMPIString *dtGetStringFormat(CMPIDateTime* eDt, CMPIStatus* rc) 
{
	OpenWBEM::CIMDateTime* dt=(OpenWBEM::CIMDateTime*)eDt->hdl;
	CMPIString *str=(CMPIString*)new CMPI_Object(dt->toString());
	CMSetStatus(rc,CMPI_RC_OK);
	return str;
}

static CMPIUint64 dtGetBinaryFormat(CMPIDateTime* eDt, CMPIStatus* rc) 
{
	OpenWBEM::CIMDateTime* dt = (OpenWBEM::CIMDateTime*)eDt->hdl;
	CMPIUint64 days,hours,mins,secs,usecs,utc,lTime;
	struct tm tm,tmt;
	//CString tStr=dt->toString().getCString();
	//const char * tStr=dt->toString().c_str();
	//char *cStr=strdup((const char*)tStr);

	if (dt->isInterval()) 
	{
		usecs = dt->getMicroSeconds();
		secs = dt->getSeconds();
		mins = dt->getMinutes();
		hours = dt->getHours();
		days = dt->getDays();

		lTime = (days*(OpenWBEM::UInt64)(86400000000LL))+
			(hours*(OpenWBEM::UInt64)(3600000000LL))+
			(mins*60000000)+(secs*1000000)+usecs;
	}
	else 
	{
		time_t tt = time(NULL);
		localtime_r(&tt, &tmt);
		memset(&tm, 0, sizeof(tm));
		tm.tm_isdst = tmt.tm_isdst;
		utc = dt->getUtc();
		usecs = dt->getMicroSeconds();
		tm.tm_sec = dt->getSeconds();
		tm.tm_min = dt->getMinutes();
		tm.tm_hour = dt->getHours();
		tm.tm_mday = dt->getDays();
		tm.tm_mon = dt->getMonth();
		tm.tm_year = dt->getYear() - 1900;

		lTime = mktime(&tm);
		lTime *= 1000000;
		lTime += usecs;
	}

	return lTime;
}

static CMPIDateTimeFT dateTime_FT = 
{
	CMPICurrentVersion,
	dtRelease,
	dtClone,
	dtGetBinaryFormat,
	dtGetStringFormat,
	dtIsInterval,
};

CMPIDateTimeFT *CMPI_DateTime_Ftab=&dateTime_FT;

