/*
 *
 * cmpiContextArgs.cpp
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
 * Description: Combined CMPIContext CMPIArgs support
 *
 */

#include "cmpisrv.h"
#include "OW_COWReference.hpp"
#include "OW_CIMFwd.hpp"

// CMPIArgs section

static CMPIStatus argsRelease(CMPIArgs* eArg) {
   (void) eArg;
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus argsReleaseNop(CMPIArgs* eArg) {
   (void) eArg;
   CMReturn(CMPI_RC_OK);
}

static CMPIArgs* argsClone(CMPIArgs* eArg, CMPIStatus* rc) {
   OW_CIMParamValueArray * arg = (OW_CIMParamValueArray *)eArg->hdl;
   OW_CIMParamValueArray * cArg = new OW_CIMParamValueArray();
   for (long i=0,s=arg->size(); i<s; i++) {
      //const OW_CIMParamValue &v=(*arg)[i];
      //OW_CIMParamValue * v=OW_COWReferenceClone(&(*arg)[i]);
      //cArg->append(v.clone());
      OW_String name = (*arg)[i].getName();
      OW_CIMValue value = (*arg)[i].getValue();
      OW_CIMParamValue pv(name,value);
      cArg->append(pv);
   }
   CMPIArgs* neArg=(CMPIArgs*)new CMPI_Object(cArg,CMPI_ObjectPath_Ftab);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return neArg;
}

static long locateArg(const OW_CIMParamValueArray &a, const OW_String &eName) {
   for (long i=0,s=a.size(); i<s; i++) {
      const OW_String &n=a[i].getName();
      if (n.compareToIgnoreCase(eName)) return i;
   }
   return -1;
}

static CMPIStatus argsAddArg(CMPIArgs* eArg, char* name, CMPIValue* data, CMPIType type) {
   OW_CIMParamValueArray * arg=(OW_CIMParamValueArray *)eArg->hdl;
   CMPIrc rc;
   OW_CIMValue v = value2CIMValue(data,type,&rc);
   OW_String sName(name);

   long i=locateArg(*arg,sName);
   if (i>=0) arg->remove(i);

   arg->append(OW_CIMParamValue(sName,v));
   CMReturn(CMPI_RC_OK);
}

static CMPIData argsGetArgAt(CMPIArgs* eArg, CMPICount pos, CMPIString** name,
               CMPIStatus* rc) {
   OW_CIMParamValueArray * arg=(OW_CIMParamValueArray *)eArg->hdl;
   CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue() };

   if (pos > arg->size()) {
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
      return data;
   }

   OW_CIMValue v=(*arg)[pos].getValue();
   OW_CIMDataType pType=v.getType();
   CMPIType t=type2CMPIType(pType,v.isArray());

   value2CMPIData(v,t,&data);

   if (name) {
      OW_String n=(*arg)[pos].getName();
      *name=string2CMPIString(n);
   }

   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return data;
}

static CMPIData argsGetArg(CMPIArgs* eArg, char* name, CMPIStatus* rc) {
   OW_CIMParamValueArray * arg=(OW_CIMParamValueArray *)eArg->hdl;
   OW_String eName(name);

   long i=locateArg(*arg,eName);
   if (i>=0) return argsGetArgAt(eArg,i,NULL,rc);

   CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue() };
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
   return data;
}

static CMPICount argsGetArgCount(CMPIArgs* eArg, CMPIStatus* rc) {
   OW_CIMParamValueArray * arg=(OW_CIMParamValueArray *)eArg->hdl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return arg->size();
}


static CMPIArgsFT args_FT={
     CMPICurrentVersion,
     argsRelease,
     argsClone,
     argsAddArg,
     argsGetArg,
     argsGetArgAt,
     argsGetArgCount,
};

CMPIArgsFT *CMPI_Args_Ftab=&args_FT;

static CMPIArgsFT argsOnStack_FT={
     CMPICurrentVersion,
     argsReleaseNop,
     argsClone,
     argsAddArg,
     argsGetArg,
     argsGetArgAt,
     argsGetArgCount,
};

CMPIArgsFT *CMPI_ArgsOnStack_Ftab=&argsOnStack_FT;



// CMPIContext Session

static CMPIStatus contextReleaseNop(CMPIContext* eCtx) {
   (void) eCtx;
   CMReturn(CMPI_RC_OK);
}

static CMPIData contextGetEntry(CMPIContext* eCtx, char* name, CMPIStatus* rc) {
   return argsGetArg((CMPIArgs*)eCtx,name,rc);
}

CMPIData contextGetEntryAt(CMPIContext* eCtx, CMPICount pos,
            CMPIString** name, CMPIStatus* rc) {
   return argsGetArgAt((CMPIArgs*)eCtx,pos,name,rc);
}

static CMPICount contextGetEntryCount(CMPIContext* eCtx, CMPIStatus* rc) {
   return argsGetArgCount((CMPIArgs*)eCtx,rc);
}

static CMPIStatus contextAddEntry(CMPIContext* eCtx, char* name,
            CMPIValue* data, CMPIType type) {
   return argsAddArg((CMPIArgs*)eCtx,name,data,type);
}


static CMPIContextFT context_FT={
     CMPICurrentVersion,
     contextReleaseNop,
     NULL,		// clone
     contextGetEntry,
     contextGetEntryAt,
     contextGetEntryCount,
     contextAddEntry,
};

CMPIContextFT *CMPI_Context_Ftab=&context_FT;

static CMPIContextFT contextOnStack_FT={
     CMPICurrentVersion,
     contextReleaseNop,
     NULL,		// clone
     contextGetEntry,
     contextGetEntryAt,
     contextGetEntryCount,
     contextAddEntry,
};

CMPIContextFT *CMPI_ContextOnStack_Ftab=&contextOnStack_FT;


CMPI_Context::CMPI_Context(const OperationContext& ct) {
      ctx=(OperationContext*)&ct;
      hdl=(void*)new OW_CIMParamValueArray();
      ft=CMPI_Context_Ftab;
   }

CMPI_ContextOnStack::CMPI_ContextOnStack(const OperationContext& ct) {
      ctx=(OperationContext*)&ct;
      hdl=(void*)new OW_CIMParamValueArray();
      ft=CMPI_ContextOnStack_Ftab;
   }

CMPI_ContextOnStack::~CMPI_ContextOnStack() {
      //cout<<"--- ~CMPI_ContextOnStack()"<<endl;
      delete (OW_CIMParamValueArray *)hdl;
   }

CMPI_ArgsOnStack::CMPI_ArgsOnStack(const OW_CIMParamValueArray& args) {
      hdl=(void*)&args;
      ft=CMPI_ArgsOnStack_Ftab;
   }
