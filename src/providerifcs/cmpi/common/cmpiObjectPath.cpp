
/*
 *
 * cmpiObjectPath.cpp
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
 * Description: CMPIObjectPath support
 *
 */


#include <iostream>
#include "cmpisrv.h"


static CMPIStatus refRelease(CMPIObjectPath* eRef) {

   std::cout << "--- refRelease()" << std::endl;
   OW_CIMObjectPath* ref=(OW_CIMObjectPath*)eRef->hdl;
   if (ref) {
      delete ref;
      ((CMPI_Object*)eRef)->unlinkAndDelete();
   }
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus refReleaseNop(CMPIObjectPath* eRef) {
   (void) eRef;
   CMReturn(CMPI_RC_OK);
}

static CMPIObjectPath* refClone(CMPIObjectPath* eRef, CMPIStatus* rc) {
   std::cout << "--- refClone()" << std::endl;
   OW_CIMObjectPath *ref=(OW_CIMObjectPath*)eRef->hdl;
   //OW_CIMObjectPath *nRef=new OW_CIMObjectPath(ref->getObjectName(),
   //                                       ref->getNameSpace());
   //nRef->setHost(ref->getHost());
   //nRef->setKeys(ref->getKeys());
   OW_CIMObjectPath *nRef = new OW_CIMObjectPath(* ref);
   CMPIObjectPath* neRef=(CMPIObjectPath*)new CMPI_Object(nRef,CMPI_ObjectPath_Ftab);
   CMSetStatus(rc,CMPI_RC_OK);
   return neRef;
}

static CMPIStatus refSetNameSpace(CMPIObjectPath* eRef, char* ns) {
   OW_CIMObjectPath* ref=(OW_CIMObjectPath*)eRef->hdl;
   ref->setNameSpace(OW_String(ns));
   CMReturn(CMPI_RC_OK);
}

static CMPIString* refGetNameSpace(CMPIObjectPath* eRef, CMPIStatus* rc) {
   OW_CIMObjectPath* ref=(OW_CIMObjectPath*)eRef->hdl;
   const OW_String &ns=ref->getNameSpace();
   CMPIString *eNs=string2CMPIString(ns);
   CMSetStatus(rc,CMPI_RC_OK);
   return eNs;
}

static CMPIStatus refSetClassName(CMPIObjectPath * eRef,char * cl) {
   OW_CIMObjectPath* ref=(OW_CIMObjectPath*)eRef->hdl;
   ref->setObjectName(OW_String(cl));
   CMReturn(CMPI_RC_OK);
}

static CMPIString* refGetClassName(CMPIObjectPath* eRef, CMPIStatus* rc) {
   OW_CIMObjectPath * ref=(OW_CIMObjectPath*)eRef->hdl;
   const OW_String &cn=ref->getObjectName();
	std::cout << "cop classname " << cn << std::endl;
   CMPIString* eCn=string2CMPIString(cn);
   CMSetStatus(rc,CMPI_RC_OK);
   return eCn;
}


static long locateKey(const OW_CIMPropertyArray &kb, const OW_String& eName) {
   for (ulong i=0,s=kb.size(); i<s; i++) {
      const OW_String &n=kb[i].getName();
      if (n.compareToIgnoreCase(eName)) return i;
   }
   return -1;
}

static CMPIStatus refAddKey(CMPIObjectPath* eRef, char* name,
          CMPIValue* data, CMPIType type) {
   OW_CIMObjectPath* ref=(OW_CIMObjectPath*)eRef->hdl;
   OW_CIMPropertyArray keyBindings=ref->getKeys();
   OW_String key(name);
   CMPIrc rc;

   long i=locateKey(keyBindings,key);
   if (i>=0) 
   {
	keyBindings.remove(i);
   	ref->setKeys(keyBindings);
   }
   OW_CIMValue val=value2CIMValue(data,type,&rc);
   ref->addKey(key, val);
   CMReturn(CMPI_RC_OK);
}

static CMPIData refGetKey(CMPIObjectPath* eRef, char* name, CMPIStatus* rc) {
   OW_CIMObjectPath* ref=(OW_CIMObjectPath*)eRef->hdl;
   const OW_String eName(name);
   OW_CIMProperty cpr = ref->getKey(eName);
   if (cpr) std::cout << " cop key " << cpr.toMOF() << std::endl;

   CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue() };
   CMSetStatus(rc,CMPI_RC_OK);

   if (cpr)
   {
      OW_CIMValue cv = cpr.getValue();
      value2CMPIData(cv, type2CMPIType(cv.getType(),cv.isArray()) ,&data);
      return data;
   }

   CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
   return data;
}

static CMPIData refGetKeyAt(CMPIObjectPath* eRef, unsigned pos, CMPIString** name,
          CMPIStatus* rc) {
   OW_CIMObjectPath* ref=(OW_CIMObjectPath*)eRef->hdl;
   const OW_CIMPropertyArray &akb=ref->getKeys();
   CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue() };
   CMSetStatus(rc,CMPI_RC_OK);

   if (pos >= akb.size()) {
      CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
      return data;
   }

   OW_CIMValue cv = akb[pos].getValue();
   value2CMPIData(cv, type2CMPIType(cv.getType(),cv.isArray()) ,&data);

   if (name) {
      const OW_String &n=akb[pos].getName();
      *name=string2CMPIString(n);
   }
   return data;
}

static CMPICount refGetKeyCount(CMPIObjectPath* eRef, CMPIStatus* rc) {
   OW_CIMObjectPath* ref=(OW_CIMObjectPath*)eRef->hdl;
   const OW_CIMPropertyArray &akb=ref->getKeys();
   CMSetStatus(rc,CMPI_RC_OK);
   return akb.size();
}

static CMPIStatus refSetNameSpaceFromObjectPath(CMPIObjectPath* eRef,
          CMPIObjectPath* eSrc) {
   OW_CIMObjectPath* ref=(OW_CIMObjectPath*)eRef->hdl;
   OW_CIMObjectPath* src=(OW_CIMObjectPath*)eSrc->hdl;
   ref->setNameSpace(src->getNameSpace());
   CMReturn(CMPI_RC_OK);
}

#if 0
static CMPIBoolean refClassPathIsA(CMPIObjectPath *eRef,
		char * classname, CMPIStatus * rc)
{
   (void) eRef;
   (void) classname;
   (void) rc;
   return false;
}
#endif

CMPIObjectPathFT objectPath_FT={
     CMPICurrentVersion,
     refRelease,
     refClone,
     refSetNameSpace,
     refGetNameSpace,
     NULL,		// setHostName
     NULL,		// getHostName
     refSetClassName,
     refGetClassName,
     refAddKey,
     refGetKey,
     refGetKeyAt,
     refGetKeyCount,
     refSetNameSpaceFromObjectPath,
     NULL,		//refSetHostAndNameSpaceFromObjectPath,
     //refClassPathIsA,
     // no qualifier support yet
     NULL,
     NULL,
     NULL,
     NULL
};

CMPIObjectPathFT *CMPI_ObjectPath_Ftab=&objectPath_FT;

CMPIObjectPathFT objectPathOnStack_FT={
     CMPICurrentVersion,
     refReleaseNop,
     refClone,
     refSetNameSpace,
     refGetNameSpace,
     NULL,		// setHostName
     NULL,		// getHostName
     refSetClassName,
     refGetClassName,
     refAddKey,
     refGetKey,
     refGetKeyAt,
     refGetKeyCount,
     refSetNameSpaceFromObjectPath,
     NULL,		//refSetHostAndNameSpaceFromObjectPath,
     //refClassPathIsA,
     // no qualifier support yet
     NULL,
     NULL,
     NULL,
     NULL
};

CMPIObjectPathFT *CMPI_ObjectPathOnStack_Ftab=&objectPathOnStack_FT;


CMPI_ObjectPathOnStack::CMPI_ObjectPathOnStack(const OW_CIMObjectPath& cop) {
      hdl=(void*)&cop;
      ft=CMPI_ObjectPathOnStack_Ftab;
   }
