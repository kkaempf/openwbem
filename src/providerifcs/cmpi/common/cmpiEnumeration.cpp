
/*
 *
 * cmpiEnumeration.cpp
 *
 * (C) Copyright IBM Corp. 2003
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
 * Description: CMPIEnumeration support
 *
 */

#include "cmpisrv.h"

static CMPIStatus enumRelease(CMPIEnumeration* eEnum)
{
	//cout<<"--- enumRelease()"<<endl;
	OpenWBEM::CIMInstance* enm=(OpenWBEM::CIMInstance*)eEnum->hdl;
	if (enm)
	{
		delete enm;
		((CMPI_Object*)eEnum)->unlinkAndDelete();
	}
	CMReturn(CMPI_RC_OK);
}

//static CMPIStatus enumReleaseNop(CMPIEnumeration* eEnum) {
//   CMReturn(CMPI_RC_OK);
//}

static CMPIEnumeration* enumClone(CMPIEnumeration* eEnum, CMPIStatus* rc)
{
	(void)eEnum; (void)rc;
//   CIMInstance* enm=(CIMInstance*)eEnum->hdl;
//   CIMInstance* cInst=new CIMInstance(enum->clone());
//   CMPIEnumeration* neEnum=(CMPIEnumeration*)new CMPI_Object(cInst,CMPI_Instance_Ftab);
//   if (rc) CMSetStatus(rc,CMPI_RC_OK);
//   return neEnum;
	return NULL;
}

CMPIData enumGetNext(CMPIEnumeration* eEnum, CMPIStatus* rc)
{
	CMPIData data={0,0,{0}};
	if ((void*)eEnum->ft==(void*)CMPI_ObjEnumeration_Ftab)
	{
		CMPI_ObjEnumeration* ie=(CMPI_ObjEnumeration*)eEnum;
		data.type=CMPI_instance;
		OpenWBEM::Array<OpenWBEM::CIMInstance>* ia=(OpenWBEM::Array<OpenWBEM::CIMInstance>*)ie->hdl;
		if (ie->cursor<ie->max)
		{
			data.value.inst=(CMPIInstance*)
			new CMPI_Object(new OpenWBEM::CIMInstance((*ia)[ie->cursor++]));
			if (rc)	CMSetStatus(rc,CMPI_RC_OK);
		}
		else if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	}

	else if ((void*)eEnum->ft==(void*)CMPI_InstEnumeration_Ftab)
	{
		CMPI_InstEnumeration* ie=(CMPI_InstEnumeration*)eEnum;
		data.type=CMPI_instance;
		OpenWBEM::Array<OpenWBEM::CIMInstance>* ia=(OpenWBEM::Array<OpenWBEM::CIMInstance>*)ie->hdl;
		if (ie->cursor<ie->max)
		{
			data.value.inst=(CMPIInstance*)
			new CMPI_Object(new OpenWBEM::CIMInstance((*ia)[ie->cursor++]));
			if (rc)	CMSetStatus(rc,CMPI_RC_OK);
		}
		else if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	}

	else
	{
		CMPI_OpEnumeration* oe=(CMPI_OpEnumeration*)eEnum;
		data.type=CMPI_ref;
		OpenWBEM::Array<OpenWBEM::CIMObjectPath>* opa=(OpenWBEM::Array<OpenWBEM::CIMObjectPath>*)oe->hdl;
		if (oe->cursor<oe->max)
		{
			data.value.ref=(CMPIObjectPath*)
			new CMPI_Object(new OpenWBEM::CIMObjectPath((*opa)[oe->cursor++]));
			if (rc)	CMSetStatus(rc,CMPI_RC_OK);
		}
		else if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	}
	return data;
}

CMPIBoolean enumHasNext(CMPIEnumeration* eEnum, CMPIStatus* rc)
{
	if (rc)	CMSetStatus(rc,CMPI_RC_OK);
	if ((void*)eEnum->ft==(void*)CMPI_ObjEnumeration_Ftab)
	{
		CMPI_ObjEnumeration* ie=(CMPI_ObjEnumeration*)eEnum;
		if (ie->cursor<ie->max)	return true;
	}
	else if ((void*)eEnum->ft==(void*)CMPI_InstEnumeration_Ftab)
	{
		CMPI_InstEnumeration* ie=(CMPI_InstEnumeration*)eEnum;
		if (ie->cursor<ie->max)	return true;
	}
	else
	{
		CMPI_OpEnumeration* oe=(CMPI_OpEnumeration*)eEnum;
		if (oe->cursor<oe->max)	return true;
	}
	return false;
}

CMPIArray* enumToArray(CMPIEnumeration* eEnum, CMPIStatus* rc)
{
	(void)eEnum; (void)rc;
	return NULL;
}


static CMPIEnumerationFT objEnumeration_FT={
	CMPICurrentVersion,
	enumRelease,
	enumClone,
	enumGetNext,
	enumHasNext,
	enumToArray,
};

static CMPIEnumerationFT instEnumeration_FT={
	CMPICurrentVersion,
	enumRelease,
	enumClone,
	enumGetNext,
	enumHasNext,
	enumToArray,
};

static CMPIEnumerationFT opEnumeration_FT={
	CMPICurrentVersion,
	enumRelease,
	enumClone,
	enumGetNext,
	enumHasNext,
	enumToArray,
};

CMPIEnumerationFT *CMPI_ObjEnumeration_Ftab=&objEnumeration_FT;
CMPIEnumerationFT *CMPI_InstEnumeration_Ftab=&instEnumeration_FT;
CMPIEnumerationFT *CMPI_OpEnumeration_Ftab=&opEnumeration_FT;

CMPI_ObjEnumeration::CMPI_ObjEnumeration(OpenWBEM::Array<OpenWBEM::CIMInstance>* oa)
{
	cursor=0;
	max=oa->size();
	hdl=(void*)oa;
	ft=CMPI_ObjEnumeration_Ftab;
}
CMPI_InstEnumeration::CMPI_InstEnumeration(OpenWBEM::Array<OpenWBEM::CIMInstance>* ia)
{
	cursor=0;
	max=ia->size();
	hdl=(void*)ia;
	ft=CMPI_InstEnumeration_Ftab;
}

CMPI_OpEnumeration::CMPI_OpEnumeration(OpenWBEM::Array<OpenWBEM::CIMObjectPath>* opa)
{
	cursor=0;
	max=opa->size();
	hdl=(void*)opa;
	ft=CMPI_OpEnumeration_Ftab;
}



