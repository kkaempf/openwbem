
/*
 *
 * cmpiBroker.cpp
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
 * Description: CMPIBroker up-call support
 *
 */

#include <iostream>
#include "cmpisrv.h"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_LocalCIMOMHandle.hpp"

using namespace OW_WBEMFlags;
//extern int traceAdapter;


// mb->hdl is a pointer to OW_ProviderEnvironmentIFCRef
#define CM_CIMOM(mb) \
(* static_cast<OW_ProviderEnvironmentIFCRef *>(mb->hdl))->getCIMOMHandle()
#define CM_LOGGER(mb) \
(* static_cast<OW_ProviderEnvironmentIFCRef *>(mb->hdl))->getLogger()
#define CM_Context(ctx) (((CMPI_Context*)ctx)->ctx)
#define CM_Instance(ci) ((OW_CIMInstance*)ci->hdl)
#define CM_ObjectPath(cop) ((OW_CIMObjectPath*)cop->hdl)
#define CM_LocalOnly(flgs) (OW_Bool)(((flgs) & CMPI_FLAG_LocalOnly)!=0)
#define CM_ClassOrigin(flgs) \
(OW_Bool)(((flgs) & CMPI_FLAG_IncludeClassOrigin)!=0)
#define CM_IncludeQualifiers(flgs) \
(OW_Bool)(((flgs) & CMPI_FLAG_IncludeQualifiers) !=0)
#define CM_DeepInheritance(flgs) (((flgs) & CMPI_FLAG_DeepInheritance)!=0)


class OW_CIMObjectPathArrayBuilder :  public OW_CIMObjectPathResultHandlerIFC
{
public:
        OW_CIMObjectPathArrayBuilder(OW_CIMObjectPathArray& a_)
        : a(a_)
        {
        }
private:
        void doHandle(const OW_CIMObjectPath& x)
	{
		a.push_back(x);
	}
        OW_CIMObjectPathArray a;
};


class OW_CIMInstanceArrayBuilder : public OW_CIMInstanceResultHandlerIFC
{
public:
        OW_CIMInstanceArrayBuilder(OW_CIMInstanceArray& a_)
        : a(a_)
        {
        }
private:
        void doHandle(const OW_CIMInstance& x)
	{
		a.push_back(x);
	}
        OW_CIMInstanceArray a;
};


#define DDD(X) X


OW_CIMPropertyArray getList(char** l) {
	(void) l;
	return OW_CIMPropertyArray();
}


OW_CIMClass* mbGetClass(CMPIBroker *mb, const OW_CIMObjectPath &cop)
{

	CM_LOGGER(mb)->logDebug("CMPIBroker: mbGetClass()");

	CMPI_Broker * xBroker = (CMPI_Broker*)mb;
	OW_String clsId = cop.getNameSpace()+":"+cop.getObjectName();
	OW_CIMClass ccp;
	if (xBroker->clsCache) {
		if (!(ccp = xBroker->clsCache->getFromCache(clsId)))
		return new OW_CIMClass(ccp);
	}
	else xBroker->clsCache=new ClassCache();

	try {
		OW_CIMClass cc=CM_CIMOM(mb)->getClass(
			cop.getNameSpace(),
			cop.getObjectName(),
			E_NOT_LOCAL_ONLY, 
			E_INCLUDE_QUALIFIERS, 
			E_EXCLUDE_CLASS_ORIGIN);

		xBroker->clsCache->addToCache(cc, clsId);
		return new OW_CIMClass(cc);
	}
	catch (OW_CIMException &e) {
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbGetClass code: %1, msg %2",
			e.type(), e.getMessage()));
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbGetClass");
	}
	return NULL;
}

static CMPIInstance* mbGetInstance(CMPIBroker *mb, CMPIContext *ctx,
			CMPIObjectPath *cop, char **properties, CMPIStatus *rc)
{
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbGetInstance()");

	CMPIFlags flgs =
		 ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
	OW_CIMPropertyArray Props = getList(properties);

	OW_CIMObjectPath qop(
			CM_ObjectPath(cop)->getObjectName(),
			Props);
	try {
		OW_CIMInstance ci=CM_CIMOM(mb)->getInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			qop,
			CM_LocalOnly(flgs) ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY,
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			CM_ClassOrigin(flgs) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN
				);
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return (CMPIInstance*)new CMPI_Object(new OW_CIMInstance(ci));
	}
	catch (OW_CIMException &e) {
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbGetInstance code: %1, msg %2",
			e.type(), e.getMessage()));
		if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbGetInstance");
		if (rc) CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
	return NULL;
}

static CMPIObjectPath* mbCreateInstance(CMPIBroker *mb, CMPIContext *ctx,
                CMPIObjectPath *cop, CMPIInstance *ci, CMPIStatus *rc)
{
	(void) ctx;

	if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbCreateInstance()");

	try
	{
		OW_CIMObjectPath ncop=CM_CIMOM(mb)->createInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_Instance(ci));
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return (CMPIObjectPath*)
			new CMPI_Object(new OW_CIMObjectPath(ncop));
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mCreateInstance code: %1, msg %2",
			e.type(), e.getMessage()));
		if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbCreateInstance");
		if (rc) CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
	return NULL;
}

static CMPIStatus mbSetInstance(CMPIBroker *mb, CMPIContext *ctx,
		CMPIObjectPath *cop, CMPIInstance *ci)
{
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbSetInstance()");

	CMPIFlags flgs =
		ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
	OW_StringArray sProps;
	try
	{

		CM_CIMOM(mb)->modifyInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_Instance(ci),
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			&sProps);
		CMReturn(CMPI_RC_OK);
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbSetInstance code: %1, msg %2",
			e.type(), e.getMessage()));
		CMReturn((CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbSetInstance");
		CMReturn(CMPI_RC_ERROR_SYSTEM);
	}
	CMReturn(CMPI_RC_ERR_FAILED);
}

static CMPIStatus mbDeleteInstance (CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop)
{
	(void) ctx;

	CM_LOGGER(mb)->logDebug("CMPIBroker: mbDeleteInstance()");

	OW_CIMObjectPath qop( CM_ObjectPath(cop)->getObjectName(),
		     CM_ObjectPath(cop)->getKeys());

	try
	{
		CM_CIMOM(mb)->deleteInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			qop);
		CMReturn(CMPI_RC_OK);
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbDeleteInstance code: %1, msg %2",
			e.type(), e.getMessage()));
		CMReturn((CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbDeleteInstance");
		CMReturn(CMPI_RC_ERROR_SYSTEM);
	}
	CMReturn(CMPI_RC_ERROR);
}

static CMPIEnumeration* mbExecQuery(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *query, char *lang, CMPIStatus *rc)
{
	(void) ctx;
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbExecQuery()");

	try
	{
		OW_CIMInstanceArray cia;
		OW_CIMInstanceArrayBuilder result(cia);

		CM_CIMOM(mb)->execQuery(
		CM_ObjectPath(cop)->getNameSpace(),
		result,
		OW_String(query),
		OW_String(lang));

		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_ObjEnumeration(
			new OW_Array<OW_CIMInstance>(cia));
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbExecQuery code: %1, msg %2",
			e.type(), e.getMessage()));
		if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbExecQuery");
		if (rc) CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
	return NULL;
}

static CMPIEnumeration* mbEnumInstances(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char **properties, CMPIStatus *rc)
{
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbEnumInstances()");

	CMPIFlags flgs = ctx->ft->getEntry(
			ctx,CMPIInvocationFlags,NULL).value.uint32;
	OW_CIMPropertyArray Props = getList(properties);

	OW_CIMInstanceArray cia;
	OW_CIMInstanceArrayBuilder result(cia);

	try
	{
		CM_CIMOM(mb)->enumInstances(
			CM_ObjectPath(cop)->getNameSpace(),
			CM_ObjectPath(cop)->getObjectName(),
			result,
			CM_DeepInheritance(flgs) ? E_DEEP : E_SHALLOW,
			CM_LocalOnly(flgs) ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY,
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			CM_ClassOrigin(flgs) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN
			);
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_InstEnumeration( new OW_CIMInstanceArray(cia));
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbEnumInstances code: %1, msg %2",
			e.type(), e.getMessage()));
		if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbEnumInstances");
		if (rc) CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
	if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbEnumInstanceNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, CMPIStatus *rc)
{
	(void) ctx;

	CM_LOGGER(mb)->logDebug("CMPIBroker: mbEnumInstanceNames()");

	OW_CIMObjectPathArray cia;
	OW_CIMObjectPathArrayBuilder result(cia);

	try
	{
		CM_CIMOM(mb)->enumInstanceNames(
			CM_ObjectPath(cop)->getNameSpace(),
			CM_ObjectPath(cop)->getClassName(),
			result);
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_OpEnumeration( new OW_CIMObjectPathArray(cia));
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbEnumInstanceNames code: %1, msg %2",
			e.type(), e.getMessage()));
		if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbEnumInstanceNames");
		if (rc) CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   return NULL;
}

static CMPIEnumeration* mbAssociators(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *assocClass, char *resultClass,
                 char *role, char *resultRole, char **properties, CMPIStatus *rc) {
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbAssociators()");

	CMPIFlags flgs = ctx->ft->getEntry(
		ctx,CMPIInvocationFlags,NULL).value.uint32;

	OW_CIMPropertyArray Props =getList(properties);
	OW_StringArray sProps;

	OW_CIMInstanceArray cia;
	OW_CIMInstanceArrayBuilder result(cia);

	try
	{
		CM_CIMOM(mb)->associators(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OW_String(assocClass),
			OW_String(resultClass),
			OW_String(role),
			OW_String(resultRole),
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			CM_ClassOrigin(flgs) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			&sProps);

      		if (rc) CMSetStatus(rc,CMPI_RC_OK);
      		return new CMPI_ObjEnumeration( new OW_CIMInstanceArray(cia));
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbAssociators code: %1, msg %2",
			e.type(), e.getMessage()));
		if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbAssociators");
		if (rc) CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
	if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbAssociatorNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *assocClass, char *resultClass,
		 char *role, char *resultRole, CMPIStatus *rc)
{
	(void) ctx;
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbAssociatorNames()");

	OW_CIMObjectPathArray cia;
	OW_CIMObjectPathArrayBuilder result(cia);

	try
	{
		CM_CIMOM(mb)->associatorNames(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OW_String(assocClass),
			OW_String(resultClass),
			OW_String(role),
			OW_String(resultRole));
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_OpEnumeration(new OW_CIMObjectPathArray(cia));
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbAssociatorNames code: %1, msg %2",
			e.type(), e.getMessage()));
		if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbAssociatorNames");
		if (rc) CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
	if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbReferences(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop,  char *resultClass, char *role ,
		 char **properties, CMPIStatus *rc)
{
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbReferences()");

	CMPIFlags flgs = ctx->ft->getEntry(
			ctx,CMPIInvocationFlags,NULL).value.uint32;
	OW_CIMPropertyArray Props = getList(properties);

	OW_StringArray sProps;

	OW_CIMInstanceArray cia;
	OW_CIMInstanceArrayBuilder result(cia);

	try
	{
		CM_CIMOM(mb)->references(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OW_String(resultClass),
			OW_String(role),
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			CM_ClassOrigin(flgs) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			&sProps
		);
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_ObjEnumeration(new OW_CIMInstanceArray(cia));
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbReferences code: %1, msg %2",
			e.type(), e.getMessage()));
		if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbReferences");
		if (rc) CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
	if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbReferenceNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *resultClass, char *role,
                 CMPIStatus *rc) {
	(void) ctx;

	CM_LOGGER(mb)->logDebug("CMPIBroker: mbReferenceNames()");

	OW_CIMObjectPathArray cia;
	OW_CIMObjectPathArrayBuilder result(cia);

	try
	{
		CM_CIMOM(mb)->referenceNames(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OW_String(resultClass),
			OW_String(role));
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_OpEnumeration(new OW_CIMObjectPathArray(cia));
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbReferenceNames code: %1, msg %2",
			e.type(), e.getMessage()));
		if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbReferenceNames");
		if (rc) CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
	if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIData mbInvokeMethod(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *method, CMPIArgs *in, CMPIArgs *out,
		 CMPIStatus *rc) {
	(void) ctx;
	 (void) mb;
	 (void) cop;
	 (void) method;
	 (void) in;
	 (void) out;

	CM_LOGGER(mb)->logDebug("CMPIBroker: mbInvokeMethod()");

	CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue()};
	if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
	return data;
}

static CMPIStatus mbSetProperty(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *name, CMPIValue *val,
                 CMPIType type)
{
	(void) ctx;

	CM_LOGGER(mb)->logDebug("CMPIBroker: mbSetProperty()");

	CMPIrc rc;
	OW_CIMValue v=value2CIMValue(val,type,&rc);

	try
	{
		CM_CIMOM(mb)->setProperty(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			OW_String(name),
			v);
		CMReturn(CMPI_RC_OK);
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbSetProperty code: %1, msg %2",
			e.type(), e.getMessage()));
		CMReturn((CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbSetProperty");
		CMReturn(CMPI_RC_ERROR_SYSTEM);
	}
	CMReturn(CMPI_RC_ERR_FAILED);
}

static CMPIData mbGetProperty(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop,char *name, CMPIStatus *rc)
{
	(void) ctx;

	CM_LOGGER(mb)->logDebug("CMPIBroker: mbGetProperty()");

	CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue()};
	try
	{
		OW_CIMValue v = CM_CIMOM(mb)->getProperty(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			OW_String(name));

		OW_CIMDataType vType=v.getType();
		CMPIType t=type2CMPIType(vType,v.isArray());
		value2CMPIData(v,t,&data);
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return data;
	}
	catch (OW_CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbGetProperty code: %1, msg %2",
			e.type(), e.getMessage()));
		if (rc) CMSetStatus(rc,(CMPIrc)e.getErrNo());
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbGetProperty");
		if (rc) CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
	}
	return data;
}





static CMPIBrokerFT broker_FT={
     0, // brokerClassification;
     CMPICurrentVersion,
     "OpenWBEM",
     NULL,
     NULL,
     NULL,
     NULL,
     mbEnumInstanceNames,
     mbGetInstance,
     mbCreateInstance,
     mbSetInstance,
     mbDeleteInstance,
     mbExecQuery,
     mbEnumInstances,
     mbAssociators,
     mbAssociatorNames,
     mbReferences,
     mbReferenceNames,
     mbInvokeMethod,
     mbSetProperty,
     mbGetProperty,
};

CMPIBrokerFT *CMPI_Broker_Ftab=& broker_FT;

CMPIBroker *CMPI_Broker::staticBroker=NULL;
