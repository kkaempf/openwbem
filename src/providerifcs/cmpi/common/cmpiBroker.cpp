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

using namespace OpenWBEM::WBEMFlags;
//extern int traceAdapter;


// mb->hdl is a pointer to OpenWBEM::ProviderEnvironmentIFCRef
#define CM_CIMOM(mb) \
(* static_cast<OpenWBEM::ProviderEnvironmentIFCRef *>(mb->hdl))->getCIMOMHandle()
#define CM_LOGGER(mb) \
(* static_cast<OpenWBEM::ProviderEnvironmentIFCRef *>(mb->hdl))->getLogger()
#define CM_Context(ctx) (((CMPI_Context*)ctx)->ctx)
#define CM_Instance(ci) ((OpenWBEM::CIMInstance*)ci->hdl)
#define CM_ObjectPath(cop) ((OpenWBEM::CIMObjectPath*)cop->hdl)
#define CM_LocalOnly(flgs) (((flgs) & CMPI_FLAG_LocalOnly)!=0)
#define CM_ClassOrigin(flgs) \
(((flgs) & CMPI_FLAG_IncludeClassOrigin)!=0)
#define CM_IncludeQualifiers(flgs) \
(((flgs) & CMPI_FLAG_IncludeQualifiers) !=0)
#define CM_DeepInheritance(flgs) (((flgs) & CMPI_FLAG_DeepInheritance)!=0)


// TODO: put these in a common header
namespace OpenWBEM
{

class CIMObjectPathArrayBuilder :  public OpenWBEM::CIMObjectPathResultHandlerIFC
{
public:
		CIMObjectPathArrayBuilder(OpenWBEM::CIMObjectPathArray& a_)
		: a(a_)
		{
		}
private:
		void doHandle(const OpenWBEM::CIMObjectPath& x)
	{
		a.push_back(x);
	}
		OpenWBEM::CIMObjectPathArray a;
};


class CIMInstanceArrayBuilder : public OpenWBEM::CIMInstanceResultHandlerIFC
{
public:
		CIMInstanceArrayBuilder(OpenWBEM::CIMInstanceArray& a_)
		: a(a_)
		{
		}
private:
		void doHandle(const OpenWBEM::CIMInstance& x)
	{
		a.push_back(x);
	}
		OpenWBEM::CIMInstanceArray a;
};

}


#define DDD(X) X


OpenWBEM::CIMPropertyArray getList(char** l) {
	(void) l;
	return OpenWBEM::CIMPropertyArray();
}


OpenWBEM::CIMClass* mbGetClass(CMPIBroker *mb, const OpenWBEM::CIMObjectPath &cop)
{

	CM_LOGGER(mb)->logDebug("CMPIBroker: mbGetClass()");

	CMPI_Broker * xBroker = (CMPI_Broker*)mb;
	OpenWBEM::String clsId = cop.getNameSpace()+":"+cop.getObjectName();
	OpenWBEM::CIMClass ccp;
	if (xBroker->clsCache) {
		if ((ccp = xBroker->clsCache->getFromCache(clsId)))
		{
		    return new OpenWBEM::CIMClass(ccp);
		}
	}
	else
	{
	     xBroker->clsCache=new ClassCache();
	}

	try {
		OpenWBEM::CIMClass cc=CM_CIMOM(mb)->getClass(
			cop.getNameSpace(),
			cop.getObjectName(),
			E_NOT_LOCAL_ONLY, 
			E_INCLUDE_QUALIFIERS, 
			E_EXCLUDE_CLASS_ORIGIN);

		xBroker->clsCache->addToCache(cc, clsId);
		return new OpenWBEM::CIMClass(cc);
	}
	catch (OpenWBEM::CIMException &e) {
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
	OpenWBEM::CIMPropertyArray Props = getList(properties);

	OpenWBEM::CIMObjectPath qop(
			CM_ObjectPath(cop)->getObjectName(),
			Props);
	try {
		OpenWBEM::CIMInstance ci=CM_CIMOM(mb)->getInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			qop,
			CM_LocalOnly(flgs) ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY,
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			CM_ClassOrigin(flgs) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN
				);
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return (CMPIInstance*)new CMPI_Object(new OpenWBEM::CIMInstance(ci));
	}
	catch (OpenWBEM::CIMException &e) {
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

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	try
	{
		OpenWBEM::CIMObjectPath ncop=CM_CIMOM(mb)->createInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_Instance(ci));
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return (CMPIObjectPath*)
			new CMPI_Object(new OpenWBEM::CIMObjectPath(ncop));
	}
	catch (OpenWBEM::CIMException &e)
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
#endif
	return NULL;
}

static CMPIStatus mbSetInstance(CMPIBroker *mb, CMPIContext *ctx,
		CMPIObjectPath *cop, CMPIInstance *ci)
{
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbSetInstance()");

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	CMPIFlags flgs =
		ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
	OpenWBEM::StringArray sProps;
	try
	{

		CM_CIMOM(mb)->modifyInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_Instance(ci),
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			&sProps);
		CMReturn(CMPI_RC_OK);
	}
	catch (OpenWBEM::CIMException &e)
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
#endif
	CMReturn(CMPI_RC_ERR_FAILED);
}

static CMPIStatus mbDeleteInstance (CMPIBroker *mb, CMPIContext *ctx,
				 CMPIObjectPath *cop)
{
	(void) ctx;

	CM_LOGGER(mb)->logDebug("CMPIBroker: mbDeleteInstance()");

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	OpenWBEM::CIMObjectPath qop( CM_ObjectPath(cop)->getObjectName(),
			 CM_ObjectPath(cop)->getKeys());

	try
	{
		CM_CIMOM(mb)->deleteInstance(
			CM_ObjectPath(cop)->getNameSpace(),
			qop);
		CMReturn(CMPI_RC_OK);
	}
	catch (OpenWBEM::CIMException &e)
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
#endif
	CMReturn(CMPI_RC_ERROR);
}

static CMPIEnumeration* mbExecQuery(CMPIBroker *mb, CMPIContext *ctx,
				 CMPIObjectPath *cop, char *query, char *lang, CMPIStatus *rc)
{
	(void) ctx;
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbExecQuery()");

	try
	{
		OpenWBEM::CIMInstanceArray cia;
		OpenWBEM::CIMInstanceArrayBuilder result(cia);

		CM_CIMOM(mb)->execQuery(
		CM_ObjectPath(cop)->getNameSpace(),
		result,
		OpenWBEM::String(query),
		OpenWBEM::String(lang));

		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_ObjEnumeration(
			new OpenWBEM::Array<OpenWBEM::CIMInstance>(cia));
	}
	catch (OpenWBEM::CIMException &e)
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
	OpenWBEM::CIMPropertyArray Props = getList(properties);

	OpenWBEM::CIMInstanceArray cia;
	OpenWBEM::CIMInstanceArrayBuilder result(cia);

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
		return new CMPI_InstEnumeration( new OpenWBEM::CIMInstanceArray(cia));
	}
	catch (OpenWBEM::CIMException &e)
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

	OpenWBEM::CIMObjectPathArray cia;
	OpenWBEM::CIMObjectPathArrayBuilder result(cia);

	try
	{
		CM_CIMOM(mb)->enumInstanceNames(
			CM_ObjectPath(cop)->getNameSpace(),
			CM_ObjectPath(cop)->getClassName(),
			result);
		if (rc)
		{
		    CMSetStatus(rc,CMPI_RC_OK);
		}

		return new CMPI_OpEnumeration( new OpenWBEM::CIMObjectPathArray(cia));
	}
	catch (OpenWBEM::CIMException &e)
	{
		CM_LOGGER(mb)->logDebug(format("CMPIBroker Exception in "
			"mbEnumInstanceNames code: %1, msg %2",
			e.type(), e.getMessage()));
                if (rc)
		{
		    CMSetStatus(rc,(CMPIrc)e.getErrNo());
		}
	}
	catch (...)
	{
		CM_LOGGER(mb)->logDebug("CMPIBroker Exception in mbEnumInstanceNames");
		if (rc)
		{
		    CMSetStatus(rc,CMPI_RC_ERROR_SYSTEM);
		}
	}
   if (rc)
   {
       CMSetStatus(rc,CMPI_RC_ERR_FAILED);
   }

   return NULL;
}

static CMPIEnumeration* mbAssociators(CMPIBroker *mb, CMPIContext *ctx,
				 CMPIObjectPath *cop, char *assocClass, char *resultClass,
				 char *role, char *resultRole, char **properties, CMPIStatus *rc) {
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbAssociators()");

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	CMPIFlags flgs = ctx->ft->getEntry(
		ctx,CMPIInvocationFlags,NULL).value.uint32;

	OpenWBEM::CIMPropertyArray Props =getList(properties);
	OpenWBEM::StringArray sProps;

	OpenWBEM::CIMInstanceArray cia;
	OpenWBEM::CIMInstanceArrayBuilder result(cia);

	try
	{
		CM_CIMOM(mb)->associators(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OpenWBEM::String(assocClass),
			OpenWBEM::String(resultClass),
			OpenWBEM::String(role),
			OpenWBEM::String(resultRole),
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			CM_ClassOrigin(flgs) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			&sProps);

			if (rc) CMSetStatus(rc,CMPI_RC_OK);
			return new CMPI_ObjEnumeration( new OpenWBEM::CIMInstanceArray(cia));
	}
	catch (OpenWBEM::CIMException &e)
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
#endif
	if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbAssociatorNames(CMPIBroker *mb, CMPIContext *ctx,
				 CMPIObjectPath *cop, char *assocClass, char *resultClass,
		 char *role, char *resultRole, CMPIStatus *rc)
{
	(void) ctx;
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbAssociatorNames()");

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	OpenWBEM::CIMObjectPathArray cia;
	OpenWBEM::CIMObjectPathArrayBuilder result(cia);

	try
	{
		CM_CIMOM(mb)->associatorNames(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OpenWBEM::String(assocClass),
			OpenWBEM::String(resultClass),
			OpenWBEM::String(role),
			OpenWBEM::String(resultRole));
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_OpEnumeration(new OpenWBEM::CIMObjectPathArray(cia));
	}
	catch (OpenWBEM::CIMException &e)
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
#endif
	if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbReferences(CMPIBroker *mb, CMPIContext *ctx,
				 CMPIObjectPath *cop,  char *resultClass, char *role ,
		 char **properties, CMPIStatus *rc)
{
	CM_LOGGER(mb)->logDebug("CMPIBroker: mbReferences()");

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	CMPIFlags flgs = ctx->ft->getEntry(
			ctx,CMPIInvocationFlags,NULL).value.uint32;
	OpenWBEM::CIMPropertyArray Props = getList(properties);

	OpenWBEM::StringArray sProps;

	OpenWBEM::CIMInstanceArray cia;
	OpenWBEM::CIMInstanceArrayBuilder result(cia);

	try
	{
		CM_CIMOM(mb)->references(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OpenWBEM::String(resultClass),
			OpenWBEM::String(role),
			CM_IncludeQualifiers(flgs) ? E_INCLUDE_QUALIFIERS : E_EXCLUDE_QUALIFIERS,
			CM_ClassOrigin(flgs) ? E_INCLUDE_CLASS_ORIGIN : E_EXCLUDE_CLASS_ORIGIN,
			&sProps
		);
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_ObjEnumeration(new OpenWBEM::CIMInstanceArray(cia));
	}
	catch (OpenWBEM::CIMException &e)
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
#endif
	if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
	return NULL;
}

static CMPIEnumeration* mbReferenceNames(CMPIBroker *mb, CMPIContext *ctx,
				 CMPIObjectPath *cop, char *resultClass, char *role,
				 CMPIStatus *rc) {
	(void) ctx;

	CM_LOGGER(mb)->logDebug("CMPIBroker: mbReferenceNames()");

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	OpenWBEM::CIMObjectPathArray cia;
	OpenWBEM::CIMObjectPathArrayBuilder result(cia);

	try
	{
		CM_CIMOM(mb)->referenceNames(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			result,
			OpenWBEM::String(resultClass),
			OpenWBEM::String(role));
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return new CMPI_OpEnumeration(new OpenWBEM::CIMObjectPathArray(cia));
	}
	catch (OpenWBEM::CIMException &e)
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
#endif
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

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	CMPIrc rc;
	OpenWBEM::CIMValue v=value2CIMValue(val,type,&rc);

	try
	{
		CM_CIMOM(mb)->setProperty(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			OpenWBEM::String(name),
			v);
		CMReturn(CMPI_RC_OK);
	}
	catch (OpenWBEM::CIMException &e)
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
#endif
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
		OpenWBEM::CIMValue v = CM_CIMOM(mb)->getProperty(
			CM_ObjectPath(cop)->getNameSpace(),
			*CM_ObjectPath(cop),
			OpenWBEM::String(name));

		OpenWBEM::CIMDataType vType=v.getType();
		CMPIType t=type2CMPIType(vType,v.isArray());
		value2CMPIData(v,t,&data);
		if (rc) CMSetStatus(rc,CMPI_RC_OK);
		return data;
	}
	catch (OpenWBEM::CIMException &e)
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
