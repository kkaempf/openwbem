
/*
 *
 * cmpiObject.cpp
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
 * Description: Internal CMPI_Object support
 *              CMPI_Object is the encapsualted type structure
 *
 */

#include "cmpisrv.h"
#include <string.h>

CMPI_Object::CMPI_Object(CMPI_Object *obj)
	: hdl(obj->hdl)
	, ftab(obj->ftab)
	, priv(0)
{
}

CMPI_Object::CMPI_Object(void* obj, void *fTab)
{
   hdl=obj;
   ftab=fTab;
   priv = 0;
}

CMPI_Object::CMPI_Object(OpenWBEM::CIMInstance* ci) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)ci;
   ftab=CMPI_Instance_Ftab;
   priv = 0;
}

CMPI_Object::CMPI_Object(OpenWBEM::CIMObjectPath* cop) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)cop;
   ftab=CMPI_ObjectPath_Ftab;
   priv = 0;
}

CMPI_Object::CMPI_Object(OpenWBEM::CIMDateTime* cdt) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)cdt;
   ftab=CMPI_DateTime_Ftab;
   priv = 0;
}

CMPI_Object::CMPI_Object(const blocxx::String& str) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)strdup(str.c_str());
   ftab=CMPI_String_Ftab;
   priv = 0;
}

CMPI_Object::CMPI_Object(char *str) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)strdup((const char*)str);
   ftab=CMPI_String_Ftab;
   priv = 0;
}

CMPI_Object::CMPI_Object(OpenWBEM::CIMParamValueArray *args) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)args;
   ftab=CMPI_Args_Ftab;
   priv = 0;
}

CMPI_Object::CMPI_Object(CMPIData *dta) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)dta;
   ftab=CMPI_Array_Ftab;
   priv = 0;
}

CMPI_Object::~CMPI_Object() {
}

void CMPI_Object::unlinkAndDelete() {
   CMPI_ThreadContext::remObject(this);
   delete this;
}
