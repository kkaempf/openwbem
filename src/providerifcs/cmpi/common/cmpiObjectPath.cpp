
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
 *
 * Contributor:   Markus Mueller <sedgewick_de@yahoo.de>
 *
 * Description: CMPIObjectPath support
 *
 */


#include <iostream>
#include "cmpisrv.h"

static CMPIStatus refRelease(CMPIObjectPath* eRef)
{

	OpenWBEM::CIMObjectPath* ref=(OpenWBEM::CIMObjectPath*)eRef->hdl;
	if(ref)
	{
		delete ref;
		((CMPI_Object*)eRef)->unlinkAndDelete();
	}
	CMReturn(CMPI_RC_OK);
}

static CMPIStatus refReleaseNop(CMPIObjectPath* eRef)
{
	(void) eRef;
	CMReturn(CMPI_RC_OK);
}

static CMPIObjectPath* refClone(CMPIObjectPath* eRef, CMPIStatus* rc)
{
	OpenWBEM::CIMObjectPath *ref=(OpenWBEM::CIMObjectPath*)eRef->hdl;
	//OpenWBEM::CIMObjectPath *nRef=new OpenWBEM::CIMObjectPath(ref->getObjectName(),
	//                                       ref->getNameSpace());
	//nRef->setHost(ref->getHost());
	//nRef->setKeys(ref->getKeys());
	OpenWBEM::CIMObjectPath *nRef = new OpenWBEM::CIMObjectPath(* ref);
	CMPIObjectPath* neRef=(CMPIObjectPath*)new CMPI_Object(nRef,CMPI_ObjectPath_Ftab);

	CMSetStatus(rc,CMPI_RC_OK);
	return neRef;
}

static CMPIStatus refSetNameSpace(CMPIObjectPath* eRef, char* ns)
{
	OpenWBEM::CIMObjectPath* ref=(OpenWBEM::CIMObjectPath*)eRef->hdl;
	ref->setNameSpace(OpenWBEM::String(ns));
	CMReturn(CMPI_RC_OK);
}

static CMPIString* refGetNameSpace(CMPIObjectPath* eRef, CMPIStatus* rc)
{
	OpenWBEM::CIMObjectPath* ref=(OpenWBEM::CIMObjectPath*)eRef->hdl;
	const OpenWBEM::String &ns=ref->getNameSpace();
	CMPIString *eNs=string2CMPIString(ns);
	CMSetStatus(rc,CMPI_RC_OK);
	return eNs;
}

static CMPIStatus refSetClassName(CMPIObjectPath * eRef,char * cl)
{
	OpenWBEM::CIMObjectPath* ref=(OpenWBEM::CIMObjectPath*)eRef->hdl;
	ref->setObjectName(OpenWBEM::String(cl));
	CMReturn(CMPI_RC_OK);
}

static CMPIString* refGetClassName(CMPIObjectPath* eRef, CMPIStatus* rc)
{
	OpenWBEM::CIMObjectPath * ref=(OpenWBEM::CIMObjectPath*)eRef->hdl;
	const OpenWBEM::String &cn=ref->getObjectName();
	CMPIString* eCn=string2CMPIString(cn);
	CMSetStatus(rc,CMPI_RC_OK);
	return eCn;
}


static long locateKey(const OpenWBEM::CIMPropertyArray &kb, const OpenWBEM::String& eName)
{
	for(unsigned long i=0,s=kb.size(); i<s; i++)
	{
		const OpenWBEM::String &n=kb[i].getName();
		if(n.equalsIgnoreCase(eName))
		{
			return i;
		}
	}
	return -1;
}

static CMPIStatus refAddKey(CMPIObjectPath* eRef, char* name,
	CMPIValue* data, CMPIType type)
{
	OpenWBEM::CIMObjectPath* ref=(OpenWBEM::CIMObjectPath*)eRef->hdl;
	OpenWBEM::CIMPropertyArray keyBindings=ref->getKeys();
	OpenWBEM::String key(name);
	CMPIrc rc;

	long i = locateKey(keyBindings, key);
	if(i >= 0)
	{
		keyBindings.remove(i);
		ref->setKeys(keyBindings);
	}

	OpenWBEM::CIMValue val = value2CIMValue(data,type,&rc);
	ref->setKeyValue(key, val);
	CMReturn(CMPI_RC_OK);
}

static CMPIData refGetKey(CMPIObjectPath* eRef, char* name, CMPIStatus* rc)
{
	OpenWBEM::CIMObjectPath* ref=(OpenWBEM::CIMObjectPath*)eRef->hdl;
	const OpenWBEM::String eName(name);
	OpenWBEM::CIMProperty cpr = ref->getKey(eName);
	CMPIData data = {(CMPIType) 0, CMPI_nullValue, CMPIValue()};

	CMSetStatus(rc,CMPI_RC_OK);

	if(cpr)
	{
		OpenWBEM::CIMValue cv = cpr.getValue();
		value2CMPIData(cv, type2CMPIType(cv.getType(), cv.isArray()), &data);
		return data;
	}

	CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
	return data;
}

static CMPIData refGetKeyAt(CMPIObjectPath* eRef, unsigned pos, CMPIString** name,
	CMPIStatus* rc)
{
	OpenWBEM::CIMObjectPath* ref=(OpenWBEM::CIMObjectPath*)eRef->hdl;
	const OpenWBEM::CIMPropertyArray &akb=ref->getKeys();
	CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue()};
	CMSetStatus(rc,CMPI_RC_OK);

	if(pos >= akb.size())
	{
		CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
		return data;
	}

	OpenWBEM::CIMValue cv = akb[pos].getValue();
	value2CMPIData(cv, type2CMPIType(cv.getType(),cv.isArray()) ,&data);

	if(name)
	{
		const OpenWBEM::String &n=akb[pos].getName();
		*name=string2CMPIString(n);
	}
	return data;
}

static CMPICount refGetKeyCount(CMPIObjectPath* eRef, CMPIStatus* rc)
{
	OpenWBEM::CIMObjectPath* ref=(OpenWBEM::CIMObjectPath*)eRef->hdl;
	const OpenWBEM::CIMPropertyArray &akb=ref->getKeys();
	CMSetStatus(rc,CMPI_RC_OK);
	return akb.size();
}

static CMPIStatus refSetNameSpaceFromObjectPath(CMPIObjectPath* eRef,
	CMPIObjectPath* eSrc)
{
	OpenWBEM::CIMObjectPath* ref=(OpenWBEM::CIMObjectPath*)eRef->hdl;
	OpenWBEM::CIMObjectPath* src=(OpenWBEM::CIMObjectPath*)eSrc->hdl;
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
	NULL,	   // setHostName
	NULL,	   // getHostName
	refSetClassName,
	refGetClassName,
	refAddKey,
	refGetKey,
	refGetKeyAt,
	refGetKeyCount,
	refSetNameSpaceFromObjectPath,
	NULL,	   //refSetHostAndNameSpaceFromObjectPath,
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
	NULL,	   // setHostName
	NULL,	   // getHostName
	refSetClassName,
	refGetClassName,
	refAddKey,
	refGetKey,
	refGetKeyAt,
	refGetKeyCount,
	refSetNameSpaceFromObjectPath,
	NULL,	   //refSetHostAndNameSpaceFromObjectPath,
	//refClassPathIsA,
	// no qualifier support yet
	NULL,
	NULL,
	NULL,
	NULL
};

CMPIObjectPathFT *CMPI_ObjectPathOnStack_Ftab=&objectPathOnStack_FT;


CMPI_ObjectPathOnStack::CMPI_ObjectPathOnStack(const OpenWBEM::CIMObjectPath& cop)
{
	hdl=(void*)&cop;
	ft=CMPI_ObjectPathOnStack_Ftab;
}
