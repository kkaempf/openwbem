
/*
 *
 * CmpiData.h
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
 * Description: CMPI C++ helper class
 *
 */

#ifndef _CmpiData_h_
#define _CmpiData_h_

#include "cmpidt.h"
#include "CmpiString.h"

class CmpiData {
  public:
   CMPIValue  data;
   CMPIType  type;
   CMPICount count;
   CmpiData() {}
   CmpiData(CmpiString& d) {data.chars=d.charPtr(); type=CMPI_chars; count=-1;}
   CmpiData(char* d)      {data.chars=d;  type=CMPI_chars;  count=-1;}
   CmpiData(CMPISint8 d)  {data.sint8=d;  type=CMPI_sint8;  count=-1;}
   CmpiData(CMPISint16 d) {data.sint16=d; type=CMPI_sint16; count=-1;}
   CmpiData(CMPISint32 d) {data.sint32=d; type=CMPI_sint32; count=-1;}
   CmpiData(CMPISint64 d) {data.sint64=d; type=CMPI_sint64; count=-1;}
   CmpiData(CMPIUint8 d)  {data.sint8=d;  type=CMPI_sint8;  count=-1;}
   CmpiData(CMPIUint16 d) {data.sint16=d; type=CMPI_sint16; count=-1;}
   CmpiData(CMPIUint32 d) {data.sint32=d; type=CMPI_sint32; count=-1;}
   CmpiData(CMPIUint64 d) {data.sint64=d; type=CMPI_sint64; count=-1;}
};

#endif



