
/*
 *
 * cmpiInstance.cpp
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
 * Description: CMPIInstance support
 *
 */

#include "cmpisrv.h"
#include "OW_CIMInstance.hpp"

using namespace OpenWBEM::WBEMFlags;

static CMPIStatus instRelease(CMPIInstance* eInst) {
   //cout<<"--- instRelease()"<<endl;
   OpenWBEM::CIMInstance* inst=(OpenWBEM::CIMInstance*)eInst->hdl;
   if (inst) {
	  delete inst;
	  ((CMPI_Object*)eInst)->unlinkAndDelete();
   }
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus instReleaseNop(CMPIInstance* eInst) {
   (void) eInst;
   CMReturn(CMPI_RC_OK);
}

static CMPIInstance* instClone(CMPIInstance* eInst, CMPIStatus* rc) {
   OpenWBEM::CIMInstance* inst=(OpenWBEM::CIMInstance*)eInst->hdl;
   OpenWBEM::CIMInstance* cInst=new OpenWBEM::CIMInstance(inst->clone(E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN));
   CMPIInstance* neInst=(CMPIInstance*)new CMPI_Object(cInst,CMPI_Instance_Ftab);
   CMSetStatus(rc,CMPI_RC_OK);
   return neInst;
}

static CMPIData instGetPropertyAt(CMPIInstance* eInst, CMPICount pos, CMPIString** name,
							CMPIStatus* rc) {
   OpenWBEM::CIMInstance* inst=(OpenWBEM::CIMInstance*)eInst->hdl;
   CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue() };

   const OpenWBEM::CIMPropertyArray& p=inst->getProperties();

   if (pos >= p.size()) {
	 CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
	  return data;
   }

   const OpenWBEM::CIMValue& v=p[pos].getValue();
   OpenWBEM::CIMDataType pType=v.getType();

   CMPIType t=type2CMPIType(pType,v.isArray());

   CMPIrc rrc = value2CMPIData(v,t,&data);

   if (name) {
	  OpenWBEM::String str=p[pos].getName();
	  *name=string2CMPIString(str);
   }

   CMSetStatus(rc, rrc);
   return data;
}

static CMPIData instGetProperty(CMPIInstance* eInst, char* name, CMPIStatus* rc) {
   OpenWBEM::CIMInstance* inst=(OpenWBEM::CIMInstance*)eInst->hdl;
   CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue() };

   const OpenWBEM::CIMProperty& p = inst->getProperty(OpenWBEM::String(name));

   if (p) {
	  const OpenWBEM::CIMValue& v=p.getValue();
	  OpenWBEM::CIMDataType pType=v.getType();
	  CMPIType t=type2CMPIType(pType,v.isArray());

	  CMPIrc rrc=value2CMPIData(v,t,&data);
	  CMSetStatus(rc,rrc);
	  return data;
   }
   CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
   return data;
}


static CMPICount instGetPropertyCount(CMPIInstance* eInst, CMPIStatus* rc) {
   OpenWBEM::CIMInstance* inst=(OpenWBEM::CIMInstance*)eInst->hdl;
   const OpenWBEM::CIMPropertyArray& p=inst->getProperties();
   CMSetStatus(rc,CMPI_RC_OK);
   return p.size();
}

static CMPIStatus instSetProperty(CMPIInstance* eInst, char* name,
						  CMPIValue* data, CMPIType type) {
   OpenWBEM::CIMInstance *inst=(OpenWBEM::CIMInstance*)eInst->hdl;
   char **list=(char**)((CMPI_Object*)eInst)->priv;
   CMPIrc rc;

   if (list) {
	  while (*list) {
		 if (strcasecmp(name,*list)==0) goto ok;
		 list++;
	  }
	  CMReturn(CMPI_RC_OK);
   }

ok:

   OpenWBEM::CIMValue v=value2CIMValue(data,type,&rc);
   OpenWBEM::String sName(name);
   inst->setProperty(sName, v);
   CMReturn(CMPI_RC_OK);
}

static CMPIObjectPath* instGetObjectPath(CMPIInstance* eInst, CMPIStatus* rc) {
   OpenWBEM::CIMInstance* inst=(OpenWBEM::CIMInstance*)eInst->hdl;
   OpenWBEM::CIMObjectPath ref("", *inst);
   CMPIObjectPath *cop=(CMPIObjectPath*)new CMPI_Object(new OpenWBEM::CIMObjectPath(ref));
   CMSetStatus(rc,CMPI_RC_OK);
   return cop;
}

static CMPIStatus instSetPropertyFilter(CMPIInstance* eInst,
			char** propertyList, char **keys){
   CMPI_Object *inst=(CMPI_Object*)eInst;
   char **list=(char**)inst->priv;    // Thank you Warren !
   int i,s;

   if (inst->priv) {
	  while (*list) {
		 free (*list);
		 list++;
	  }
	  free(inst->priv);
   }
   inst->priv=NULL;

   if (propertyList==NULL) CMReturn(CMPI_RC_OK);
   if (keys==NULL) CMReturn(CMPI_RC_ERR_FAILED);

   for (s=0,i=0; propertyList[i]; i++,s++);
   for (i=0; keys[i]; i++,s++);
   list=(char**)malloc((s+2)*sizeof(char*));
   for (s=0,i=0; propertyList[i]; i++,s++) list[s]=strdup(propertyList[i]);
   for (i=0; keys[i]; i++,s++) list[s]=strdup(keys[i]);
   list[s]=NULL;
   inst->priv=(void*)list;

   CMReturn(CMPI_RC_OK);
}

static CMPIStatus instSetPropertyFilterIgnore(CMPIInstance* eInst,
			char** propertyList, char **keys){
	(void)eInst;
	(void)propertyList;
	(void)keys;
   CMReturn(CMPI_RC_OK);
}



static CMPIInstanceFT instance_FT={
	 CMPICurrentVersion,
	 instRelease,
	 instClone,
	 instGetProperty,
	 instGetPropertyAt,
	 instGetPropertyCount,
	 instSetProperty,
	 instGetObjectPath,
	 instSetPropertyFilter,
};

static CMPIInstanceFT instanceOnStack_FT={
	 CMPICurrentVersion,
	 instReleaseNop,
	 instClone,
	 instGetProperty,
	 instGetPropertyAt,
	 instGetPropertyCount,
	 instSetProperty,
	 instGetObjectPath,
	 instSetPropertyFilterIgnore,
};

CMPIInstanceFT *CMPI_Instance_Ftab=&instance_FT;
CMPIInstanceFT *CMPI_InstanceOnStack_Ftab=&instanceOnStack_FT;


CMPI_InstanceOnStack::CMPI_InstanceOnStack(const OpenWBEM::CIMInstance& ci) {
	  hdl=(void*)&ci;
	  ft=CMPI_InstanceOnStack_Ftab;
   }
