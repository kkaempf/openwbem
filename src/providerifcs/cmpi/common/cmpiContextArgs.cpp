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
 *
 * Contributor:   Markus Mueller <sedgewick_de@yahoo.de>
 *
 * Description: Combined CMPIContext CMPIArgs support
 *
 */

#include "cmpisrv.h"
#include "OW_COWReference.hpp"
#include "OW_CIMFwd.hpp"

// CMPIArgs section

static CMPIStatus argsRelease(CMPIArgs* eArg)
{
	(void) eArg;
	CMReturn(CMPI_RC_OK);
}

static CMPIStatus argsReleaseNop(CMPIArgs* eArg)
{
	(void) eArg;
	CMReturn(CMPI_RC_OK);
}

static CMPIArgs* argsClone(CMPIArgs* eArg, CMPIStatus* rc)
{
	OpenWBEM::CIMParamValueArray * arg = (OpenWBEM::CIMParamValueArray *)eArg->hdl;
	OpenWBEM::CIMParamValueArray * cArg = new OpenWBEM::CIMParamValueArray();
	for (long i=0,s=arg->size(); i<s; i++)
	{
		OpenWBEM::String name = (*arg)[i].getName();
		OpenWBEM::CIMValue value = (*arg)[i].getValue();
		OpenWBEM::CIMParamValue pv(name,value);
		cArg->append(pv);
	}
	CMPIArgs* neArg=(CMPIArgs*)new CMPI_Object(cArg,CMPI_ObjectPath_Ftab);
	CMSetStatus(rc,CMPI_RC_OK);
	return neArg;
}

static long locateArg(const OpenWBEM::CIMParamValueArray &a, const OpenWBEM::String &eName)
{
	for (long i = 0, s = a.size(); i < s; i++)
	{
		const OpenWBEM::String &n = a[i].getName();
		if (n.compareToIgnoreCase(eName) == 0)
		{
			return i;
		}
	}

	return -1;
}

static CMPIStatus argsAddArg(CMPIArgs* eArg, char* name,
				 CMPIValue* data, CMPIType type)
{
	OpenWBEM::CIMParamValueArray* arg = (OpenWBEM::CIMParamValueArray *)eArg->hdl;
	CMPIrc rc;
	OpenWBEM::CIMValue v = value2CIMValue(data, type, &rc);
	OpenWBEM::String sName(name);

	long i = locateArg(*arg, sName);
	if (i >= 0)
	{
		arg->remove(i);
	}

	arg->append(OpenWBEM::CIMParamValue(sName, v));
	CMReturn(CMPI_RC_OK);
}

static CMPIData argsGetArgAt(CMPIArgs* eArg, CMPICount pos, CMPIString** name,
			CMPIStatus* rc)
{
	OpenWBEM::CIMParamValueArray * arg=(OpenWBEM::CIMParamValueArray *)eArg->hdl;
	CMPIData data={(CMPIType) 0, CMPI_nullValue, {0} };

	if (pos > arg->size())
	{
		CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
		return data;
	}

	OpenWBEM::CIMValue v=(*arg)[pos].getValue();
	OpenWBEM::CIMDataType pType=v.getType();
	CMPIType t=type2CMPIType(pType,v.isArray());

	value2CMPIData(v,t,&data);

	if (name)
	{
		OpenWBEM::String n=(*arg)[pos].getName();
		*name=string2CMPIString(n);
	}

	CMSetStatus(rc,CMPI_RC_OK);
	return data;
}

static CMPIData argsGetArg(CMPIArgs* eArg, char* name, CMPIStatus* rc)
{
	OpenWBEM::CIMParamValueArray *arg = (OpenWBEM::CIMParamValueArray *)eArg->hdl;
	OpenWBEM::String eName(name);
	long i = locateArg(*arg, eName);

	if (i >= 0)
	{
		return argsGetArgAt(eArg, i, NULL, rc);
	}

	CMPIData data={(CMPIType) 0, CMPI_nullValue, {0} };
	CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
	return data;
}

static CMPICount argsGetArgCount(CMPIArgs* eArg, CMPIStatus* rc)
{
	OpenWBEM::CIMParamValueArray * arg=(OpenWBEM::CIMParamValueArray *)eArg->hdl;
	CMSetStatus(rc,CMPI_RC_OK);
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

static CMPIStatus contextReleaseNop(CMPIContext* eCtx)
{
	(void) eCtx;
	CMReturn(CMPI_RC_OK);
}

static CMPIData contextGetEntry(CMPIContext* eCtx, char* name, CMPIStatus* rc)
{
	return argsGetArg((CMPIArgs*)eCtx,name,rc);
}

CMPIData contextGetEntryAt(CMPIContext* eCtx, CMPICount pos,
				CMPIString** name, CMPIStatus* rc)
{
	return argsGetArgAt((CMPIArgs*)eCtx,pos,name,rc);
}

static CMPICount contextGetEntryCount(CMPIContext* eCtx, CMPIStatus* rc)
{
	return argsGetArgCount((CMPIArgs*)eCtx,rc);
}

static CMPIStatus contextAddEntry(CMPIContext* eCtx, char* name,
					CMPIValue* data, CMPIType type)
{
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


CMPI_Context::CMPI_Context(const ::CMPIOperationContext& ct)
{
	ctx=(::CMPIOperationContext*)&ct;
	hdl=(void*)new OpenWBEM::CIMParamValueArray();
	ft=CMPI_Context_Ftab;
}

CMPI_ContextOnStack::CMPI_ContextOnStack(const ::CMPIOperationContext& ct)
{
	ctx=(::CMPIOperationContext*)&ct;
	hdl=(void*)new OpenWBEM::CIMParamValueArray();
	ft=CMPI_ContextOnStack_Ftab;
}

CMPI_ContextOnStack::~CMPI_ContextOnStack()
{
	delete (OpenWBEM::CIMParamValueArray *)hdl;
}

CMPI_ArgsOnStack::CMPI_ArgsOnStack(const OpenWBEM::CIMParamValueArray& args)
{
	hdl=(void*)&args;
	ft=CMPI_ArgsOnStack_Ftab;
}
