
/*
 *
 * cmpiString.cpp
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
 * Description: CMPIString function support
 *
 */


#include "cmpisrv.h"
#include "OW_String.hpp"

CMPIString* string2CMPIString(const OpenWBEM::String &s)
{
	return (CMPIString*) new CMPI_Object(s);
}

static CMPIStatus stringRelease(CMPIString *eStr)
{
	//std::cout << "****** stringRelease()" << std::endl;

	if (eStr->hdl)
	{
		::free(eStr->hdl);
		((CMPI_Object*)eStr)->unlinkAndDelete();
	}

	CMReturn(CMPI_RC_OK);
}

static CMPIString* stringClone(const CMPIString *eStr, CMPIStatus* rc)
{
	CMSetStatus(rc,CMPI_RC_OK);
	return (CMPIString*) new CMPI_Object((char*) eStr->hdl);
}

static char* stringGetCharPtr(const CMPIString *eStr, CMPIStatus* rc)
{
	char* ptr = (char*) eStr->hdl;
	CMSetStatus(rc,CMPI_RC_OK);
	return ptr;
}

static CMPIStringFT string_FT =
{
	CMPICurrentVersion,
	stringRelease,
	stringClone,
	stringGetCharPtr,
};

CMPIStringFT *CMPI_String_Ftab=&string_FT;
