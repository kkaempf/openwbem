
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
 * Contributors:
 *
 * Description: CMPIString function support
 *
 */


#include "cmpisrv.h"
#include "OW_String.hpp"

CMPI_String* string2CMPIString(const OW_String &s) {
  char *cp=strdup(s.c_str());
  CMPI_Object *obj= new CMPI_Object((void*)cp,CMPI_String_Ftab);
//  CMPIRefs::localRefs().addRef(obj,CMPIRefs::TypeString);
  return (CMPI_String*)obj;
}

static CMPIStatus stringRelease(CMPIString *eStr) {
   (void) eStr;
   std::cout << "--- stringRelease()" << std::endl;
   CMReturn(CMPI_RC_OK);
}

static CMPIString* stringClone(CMPIString *eStr, CMPIStatus* rc) {
   char* str=(char*)eStr->hdl;
   char* newstr=::strdup(str);
   CMPI_Object * obj=new CMPI_Object(newstr,CMPI_String_Ftab);
//   CMPIRefs::localRefs().addRef(obj,CMPIRefs::TypeString);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return (CMPIString*)obj;
}

static char * stringGetCharPtr(CMPIString *eStr, CMPIStatus* rc) {
   char* ptr=(char*)eStr->hdl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return ptr;
}

static CMPIStringFT string_FT={
     CMPICurrentVersion,
     stringRelease,
     stringClone,
     NULL,
     stringGetCharPtr,
};

CMPIStringFT *CMPI_String_Ftab=&string_FT;
